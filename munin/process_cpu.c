/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "../common.h"
#include "../cpu.h"


int main(int argc, const char **argv){
    DIR *d;
    struct dirent *de;
    char *owner;
    char *tag;
    char *cmdline;
    char *cache;
    unsigned long int usage[2];
    unsigned long int uptime[2];
    int config = 0;

    /* Check env */
    owner = getenv("OWNER");
    tag   = getenv("TAG");
    cmdline = getenv("CMDLINE");
    cache = getenv("CACHE");

    if((tag==NULL) || (owner==NULL) || (cache==NULL)){
        fprintf(stderr, "OWNER, TAG and/or CACHE environment variable needs to be set\n");
        return 1;
    }

    /* Handle config request */
    if(argc>1){
        if(strcmp(argv[1], "config")==0){
            config = 1;
            printf("graph_title Process CPU Usage\n");
            printf("graph.vlabel percentage\n");
        }
    }

    d = opendir("/proc");
    if(!d){ return 1; }

    de = readdir(d);
    while(de){
        if((de->d_type == DT_DIR) && (atoi(de->d_name)>0)){
            char p[256+6];
            char name[1024];
            unsigned long int ticks, span;
            snprintf(p, sizeof(p), "/proc/%s", de->d_name);
            if(monitored(p, owner, tag, cmdline, name, sizeof(name))){
                if(config){
                    printf("%s.label %s\n", name, name);
                } else {
                    char cachefile[1024];
                    snprintf(cachefile, sizeof(cachefile), "%s/%s", cache, name);
                    readcache(cachefile, &uptime[0], &usage[0]);
                    usage[1] = cpuusage(atoi(de->d_name));
                    uptime[1] = getuptime();
                    writecache(cachefile, uptime[1], usage[1]);
                    /* If the process is restarted, the system is rebooted, or
                     * the jiffy counter wraps (once every 1.5 years) just
                     * don't report anything */
                    if((usage[1] >= usage[0]) && (uptime[1] > uptime[0])){
                        printf("%s.value %lu\n", name, (100*(usage[1]-usage[0]))/(uptime[1]-uptime[0]));
                    }
                }
            }
        }
        de = readdir(d);
    }
}

/* vim: set ts=4 sw=4 expandtab: */
