/* $Id$ */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"
#include "cpu.h"

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
