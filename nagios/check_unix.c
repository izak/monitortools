/*
 * $Id$
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>

#define NAGIOS_OK 0
#define NAGIOS_WARN 1
#define NAGIOS_FAIL 2
#define NAGIOS_UNKNOWN 3

int main(int argc, const char **argv){
    int s, n;
    struct sockaddr_un unixsocket;
    int flags;
    fd_set wset, rset;
    struct timeval tv;

    /* Check params */
    if(argc<2){
        fprintf(stderr, "Usage: $0 path-to-socket\n");
        return NAGIOS_UNKNOWN;
    }

    /* Create endpoint */
    if((s=socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket");
        return NAGIOS_UNKNOWN;
    }

    /* connect endpoint to filesystem node */
    unixsocket.sun_family= AF_UNIX;
    snprintf(unixsocket.sun_path, sizeof(unixsocket.sun_path), "%s", argv[1]);

    /* Set non-blocking */
    flags= fcntl(s, F_GETFL, 0);
    flags |= O_NONBLOCK;
    if(fcntl(s, F_SETFL, flags) < 0) {
        perror("fcntl");
        return NAGIOS_UNKNOWN;
    }

    /* Attempt a connection */
    if(connect(s, (struct sockaddr *)&unixsocket, sizeof(unixsocket))<0){
        if(errno != EINPROGRESS){
            printf("FAILED\n");
            return NAGIOS_FAIL;
        }
    }
    FD_ZERO(&rset);
    FD_SET(s, &rset);
    wset = rset;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if((n=select(s+1, &rset, &wset, NULL, &tv))<0){
        close(s);
        perror("select");
        return NAGIOS_UNKNOWN;
    } else if(n==0){
        close(s);
        printf("Connection timed out\n");
        return NAGIOS_FAIL;
    }
    if(FD_ISSET(s, &rset) || FD_ISSET(s, &wset)){
        int err;
        unsigned int len = sizeof(err);
        if(getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &len) == 0){
            printf("OK\n");
            return NAGIOS_OK;
        }
    }
    return NAGIOS_FAIL;

}

/* vim: set ts=4 sw=4 expandtab: */
