/* $Id$ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <asm/param.h>
#include <stdlib.h>
#include <dirent.h>
#include "common.h"

void splitbuf(char *buf, size_t count, char separator){
    char *ptr = buf;
    while((ptr < buf + count) && (ptr = strchr(buf, separator))){
        *ptr = '\0';
        ptr++;
    }
}

unsigned long int getuptime(){
    char buf[1024];
    const char *ptr;
    unsigned long int total;
    readfile("/proc/stat", buf, sizeof(buf));
    splitbuf(buf, sizeof(buf), '\n');
    ptr = scanbuf(buf, sizeof(buf), "cpu ");

    total = 0;
    while(ptr = strchr(ptr, ' ')){
        ptr++;
        if((*ptr != '\0') && (*ptr!=' ')){
            total+=atoi(ptr);
        }
    }

    return total;
}

unsigned long int cpuusage(pid_t pid){
    char procpidstat[1024];
    char buf[4096];
    unsigned long int utime, stime;
    snprintf(procpidstat, sizeof(procpidstat), "/proc/%d/stat", pid);
    readfile(procpidstat, buf, sizeof(buf));
    sscanf(buf,
            "%*d "
            "%*s "
            "%*c "
            "%*d %*d %*d %*d %*d "
            "%*lu %*lu %*lu %*lu %*lu %lu %lu "
            "%*ld %*ld %*ld %*ld %*ld %*ld "
            "%*lu %*lu "
            "%*ld "
            "%*lu %*lu %*lu %*lu %*lu %*lu "
            "%*u %*u %*u %*u "
            "%*lu %*lu %*lu",
            &utime, &stime);
    return utime+stime;
}

void writecache(const char *cachefile, int uptime, int usage){
    int fd = open(cachefile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd!=-1){
        char buf[1024];
        int n, i;
        n = snprintf(buf, sizeof(buf), "%lu %lu\n", uptime, usage);
        while(n>0){
            if((i = write(fd, buf, n))==-1){
                perror("write");
            }
            n -= i;
        }
        close(fd);
    } else {
        perror("open");
    }
}

void readcache(const char *cachefile, unsigned long int *uptime, unsigned long int *usage){
    char buf[1024];
    *uptime = *usage = 0;
    if(readfile(cachefile, buf, sizeof(buf))!=-1){
        sscanf(buf, "%lu %lu", uptime, usage);
    }
}

int main(int argc, const char **argv){
    DIR *d;
    struct dirent *de;
    char *owner;
    char *tag;
    char *cache;
    unsigned long int usage[2];
    unsigned long int uptime[2];
    int config = 0;

    /* Check env */
    owner = getenv("OWNER");
    tag   = getenv("TAG");
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
            snprintf(p, sizeof(p), "/proc/%s", de->d_name);
            if(monitored(p, owner, tag, name, sizeof(name))){
                if(config){
                    printf("%s.label %s\n", name, name);
                } else {
                    char cachefile[1024];
                    snprintf(cachefile, sizeof(cachefile), "%s/%s", cache, name);
                    readcache(cachefile, &uptime[0], &usage[0]);
                    usage[1] = cpuusage(atoi(de->d_name));
                    uptime[1] = getuptime();
                    writecache(cachefile, uptime[1], usage[1]);
                    printf("%s.value %lu\n", name, (100*(usage[1]-usage[0]))/(uptime[1]-uptime[0]));
                }
            }
        }
        de = readdir(d);
    }
}

/* vim: set ts=4 sw=4 expandtab: */
