/* $Id$ */
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "memory.h"

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
