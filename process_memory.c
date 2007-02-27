/* $Id$ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"

/*
 * Find memory usage for processes that:
 * 1. Has effective uid of the specified user (/proc/PID owned by user)
 * 2. Has the specified tag in /proc/PID/environ
 */

int getzopeuid(const char *username){
    struct passwd *pw;
    pw = getpwnam(username);
    if(!pw) return -1;
    return pw->pw_uid;
}

const char* monitored(const char *pidpath, const char *zopeuser, const char *tag){
    struct stat s_procpid;
    uid_t zopeuid = getzopeuid(zopeuser);

    if(stat(pidpath, &s_procpid)==-1){
        return 0;
    }
    if(s_procpid.st_uid == zopeuid){
        char procpidenv[1024];
        char env[4096];
        const char *ptr;
        int i;
        snprintf(procpidenv, sizeof(procpidenv), "%s/environ", pidpath);
        i = readfile(procpidenv, env, sizeof(env));
        if(ptr=scanbuf(env, i, tag, '\0')){
            if(ptr = strchr(ptr, '=')){
                return ++ptr;
            }
        }
    }
    return NULL;
}

unsigned long int memusage(const char *pidpath){
    char procpidstatm[1024];
    char buf[1024];
    int i;
    unsigned long int vsz;

    /* Read statm */
    snprintf(procpidstatm, sizeof(procpidstatm), "%s/statm", pidpath);
    bzero(buf, sizeof(buf));
    i = readfile(procpidstatm, buf, sizeof(buf));
    /* statm has 7 fields, but we only care about the first */
    sscanf(buf, "%ld %*ld %*ld %*ld %*ld %*ld %*ld", &vsz);
    return getpagesize()*vsz;
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
            const char *name;
            snprintf(p, sizeof(p), "/proc/%s", de->d_name);
            if(name = monitored(p, owner, tag)){
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
