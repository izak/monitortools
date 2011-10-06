/* $Id$ */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <pwd.h>
#include "common.h"

int readfile(const char *filename, char *buf, size_t count){
    int i = 0;
    int j;
    int fd;
    bzero(buf, count);
    fd = open(filename, O_RDONLY);
    if(fd!=-1){
        while((j = read(fd, buf+i, count-i))>0){
            i+=j;
        }
        close(fd);
        return i;
    }
    return -1;
}

const char* scanbuf(const char *buf, size_t count, const char *needle){
    int j;
    const char *ptr = buf;

    while(ptr < buf + count){
        if(strncmp(ptr, needle, strlen(needle))==0){
            return ptr;
        }
        while((*ptr!='\0') && (ptr < buf+count)) { ptr++; }
        ptr++; /* point to next string in buffer */
    }
    return NULL;
}

int getzopeuid(const char *username){
    struct passwd *pw;
    pw = getpwnam(username);
    if(!pw) return -1;
    return pw->pw_uid;
}

int check_string(const char* haystack, const char* needle){
    if(needle[0]=='!'){
        return strstr(haystack, needle+1) == NULL;
    }
    return strstr(haystack, needle) != NULL;
}

int check_cmdline(const char* pidpath, const char* cmdline){
    if(cmdline != NULL){
        char procpidcmdline[1024];
        char buf[1024];
        int i, j;
        const char *ptr;
        const char *ptr2;
        snprintf(procpidcmdline, sizeof(procpidcmdline), "%s/cmdline", pidpath);
        i = readfile(procpidcmdline, buf, sizeof(buf));
        for (j=0; j<i-1; j++){
            if(buf[j]=='\0'){
                buf[j]=' ';
            }
        }
        /* cmdline may be a comma seperated list  of elements to be matched,
           and if an item is prefixed with ! the matching will be inverted */
        ptr = cmdline;
        ptr2 = strchr(ptr, ',');
        if(ptr2){
            int ok = 1;
            char s[1024];
            int len = 0;
            while(ptr2){
                len = ptr2 - ptr;
                strncpy(s, ptr, len);
                s[len] = '\0';
                ok = ok && check_string(buf, s);
                ptr = ptr2+1;
                ptr2 = strchr(ptr, ',');
            }
            ok = ok && check_string(buf, ptr);
            return ok;
        } else {
            return check_string(buf, cmdline);
        }
    }
    return 1;
}

int monitored(const char *pidpath, const char *zopeuser, const char *tag, const char* cmdline, char *name, size_t nlen){
    if(check_cmdline(pidpath, cmdline)){
        struct stat s_procpid;
        uid_t zopeuid = getzopeuid(zopeuser);

        if(stat(pidpath, &s_procpid)==-1){
            return 0;
        }
        if(s_procpid.st_uid == zopeuid){
            char procpidenv[1024];
            char env[65535];
            const char *ptr;
            int i;
            snprintf(procpidenv, sizeof(procpidenv), "%s/environ", pidpath);
            i = readfile(procpidenv, env, sizeof(env));
            if(ptr=scanbuf(env, i, tag)){
                if(ptr = strchr(ptr, '=')){
                    ptr++;
                    strncpy(name, ptr, nlen);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* vim: set ts=4 sw=4 expandtab: */
