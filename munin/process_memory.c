/* $Id$ */

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../common.h"
#include "../memory.h"

/*
 * Find memory usage for processes that:
 * 1. Has effective uid of the specified user (/proc/PID owned by user)
 * 2. Has the specified tag in /proc/PID/environ
 */

int main(int argc, const char **argv){
    DIR *d;
    struct dirent *de;
    char *owner;
    char *tag;
    int config = 0;

    /* Check env */
    owner = getenv("OWNER");
    tag   = getenv("TAG");

    if((tag==NULL) || (owner==NULL)){
        fprintf(stderr, "OWNER and/or TAG environment variable needs to be set\n");
        return 1;
    }

    /* Handle config request */
    if(argc>1){
        if(strcmp(argv[1], "config")==0){
            config = 1;
            printf("graph_title Process Memory Usage\n");
            printf("graph_args --base 1024\n");
            printf("graph.vlabel bytes\n");
        }
    }


    d = opendir("/proc");
    if(!d){ return 1; }

    de = readdir(d);
    while(de){
        if((de->d_type == DT_DIR) && (atoi(de->d_name)>0)){
            char p[256+6];
            char name[1024];
            snprintf(p, sizeof(p), "/proc/%s", de->d_name);
            if(monitored(p, owner, tag, name, sizeof(name))){
                if(config){
                    printf("%s.label %s\n", name, name);
                } else {
                    printf("%s.value %ld\n", name, memusage(p));
                }
            }
        }
        de = readdir(d);
    }
}

/* vim: set ts=4 sw=4 expandtab: */
