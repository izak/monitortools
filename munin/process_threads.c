/* $Id$ */

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../common.h"

unsigned int threadcount(const char *pidpath){
    char procpidtask[1024];
    DIR *d;
    struct dirent *e;
    unsigned int count;

    /* Count threads */
    snprintf(procpidtask, sizeof(procpidtask), "%s/task", pidpath);
    d = opendir(procpidtask);
    if(!d){ return 0; }

    count = 0;
    e = readdir(d);
    while(e){
        if((e->d_type == DT_DIR) && (atoi(e->d_name)>0)){
            count++;
        }
        e = readdir(d);
    }
    return count;
}

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
            printf("graph_title Process Threads\n");
            printf("graph.vlabel threads\n");
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
                    printf("%s.value %u\n", name, threadcount(p));
                }
            }
        }
        de = readdir(d);
    }
}

/* vim: set ts=4 sw=4 expandtab: */
