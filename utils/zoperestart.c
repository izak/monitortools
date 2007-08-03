/*
 * $Id$ 
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define TMOUT 60

int restart(const char *d){
    struct stat zopedirstat;
    FILE *log;
    char buf[1024];
    char zopectl[1024];
    char zopelog[1024];
    int ticks;

    /* Check that directory exists */
    if(stat(d, &zopedirstat)==-1){
        perror("stat");
        return 1;
    }

    /* Set up required strings */
    snprintf(zopectl, sizeof(zopectl), "%s/bin/zopectl restart", d);
    snprintf(zopelog, sizeof(zopelog), "%s/log/event.log", d);

    /* Open log file and seek to the end */
    log = fopen(zopelog, "r");
    fseek(log, 0, SEEK_END);

    /* Restart the client */
    fprintf(stderr, "Calling %s\n", zopectl);
    system(zopectl);

    /* tail the logfile and wait for startup to complete */
    ticks = 0;
    while(ticks<TMOUT){
        /* Read log */
        if(fgets(buf, sizeof(buf), log)){
            /* Check the line */
            if(strstr(buf, "Zope Ready to handle requests")){
                fprintf(stderr, " success\n");
                return 0;
            }
        } else {
            sleep(1);
            ticks++;
            fprintf(stderr, ".");
        }
    }
    fprintf(stderr, " alarm\n");
    return 1;
}

int main(int argc, char **argv){
    int i;
    int x;

    if(argc<2){
        fprintf(stderr, "Usage: %s zopeclientdir [...]\n", argv[0]);
        return 3;
    }

    for(i=1; i<argc; i++){
        if((x=restart(argv[i]))!=0){
            return x;
        }
    }
    return 0;
}

/* vim: set ts=4 sw=4 expandtab: */
