/* $Id$ */

#ifndef CPU_H
#define CPU_H
#include <stdlib.h>

void splitbuf(char*, size_t, char);
unsigned long int getuptime();
unsigned long int cpuusage(pid_t);
void writecache(const char*, int, int);
void readcache(const char*, unsigned long int*, unsigned long int*);

#endif
