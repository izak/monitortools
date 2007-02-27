/* $Id$ */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "common.h"

int readfile(const char *filename, char *buf, size_t count){
    int i = 0;
    int j;
    int fd;
    bzero(buf, count);
    fd = open(filename, O_RDONLY);
    while((j = read(fd, buf+i, count-i))>0){
        i+=j;
    }
    close(fd);
    return i;
}

const char* scanbuf(const char *buf, size_t count, const char *needle){
    int j;
    const char *ptr = buf;

    while(ptr < buf + count){
        if(strstr(ptr, needle)){
            return ptr;
        }
        while((*ptr!='\0') && (ptr < buf+count)) { ptr++; }
        ptr++; /* point to next string in buffer */
    }
    return NULL;
}

/* vim: set ts=4 sw=4 expandtab: */
