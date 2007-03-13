/* $Id$ */

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "../common.h"
#include "../memory.h"

void usage(const char *name){
    fprintf(stderr, "Usage: %s [-O | --owner=username] [-T | --tag=tag] [-V | --target=tagname]\n", name);
    fprintf(stderr, "          [-W | --warning=integer] [-C | --critical=integer] [-H | --help]\n");
}

void printmessage(const char *name, const char *stat, unsigned long int u){
    int i = 0;
    char *units[] = {"b", "K", "M"};
    while((i < 2) && (u > 1024)){
        u /= 1024;
        i++;
    }
    printf("%s memory %s: %lu %s\n", name, stat, u, units[i]);
}

int main(int argc, char **argv){
    DIR *d;
    struct dirent *de;
    char *owner;
    char *tag;
    char *target;
    unsigned long int warning  = 0;
    unsigned long int critical = 0;
    int status = 3; /* UNKNOWN */
    int c;

    /* Parse options */
    while(1){
        int option_index = 0;
        static struct option long_options[] = {
            {"owner",    1, NULL, 'O'},
            {"tag",      1, NULL, 'T'},
            {"target",   1, NULL, 'V'},
            {"warning",  1, NULL, 'W'},
            {"critical", 1, NULL, 'C'},
            {"help",     1, NULL, 'H'},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "O:T:V:W:C:H", long_options, &option_index);
        if(c==-1){
            break;
        }
        switch(c){
            case 'O':
                owner = optarg;
                break;
            case 'T':
                tag = optarg;
                break;
            case 'V':
                target = optarg;
                break;
            case 'W':
                warning = atol(optarg)*(strchr(optarg, 'k')?1024:(strchr(optarg, 'm')?1048576:1));
                break;
            case 'C':
                critical = atol(optarg)*(strchr(optarg, 'k')?1024:(strchr(optarg, 'm')?1048576:1));
                break;
            case 'H':
            default:
                usage(argv[0]);
                return 3; /* UNKNOWN */
        }
    }

    if( (tag==NULL) || (owner==NULL) || (target==NULL) || (warning==0) || (critical==0)){
        usage(argv[0]);
        return 3; /* UNKNOWN */
    }
    if(warning > critical){
        fprintf(stderr, "The Warning value (%lu) must be lower than the Critical value (%lu)\n", warning, critical);
        return 3; /* UNKNOWN */
    }

    d = opendir("/proc");
    if(!d){ return 3; /* UNKNOWN */ }

    de = readdir(d);
    while(de){
        if((de->d_type == DT_DIR) && (atoi(de->d_name)>0)){
            char p[256+6];
            char name[1024];
            snprintf(p, sizeof(p), "/proc/%s", de->d_name);
            if(monitored(p, owner, tag, name, sizeof(name))){
                if(strncmp(name, target, strlen(target))==0){
                    unsigned long int mem = memusage(p);
                    if(mem > critical){
                        printmessage(name, "critical", mem);
                        status = 2; /* CRITICAL */
                    } else if(mem > warning){
                        printmessage(name, "warning", mem);
                        status = 1; /* WARNING */
                    } else {
                        printmessage(name, "ok", mem);
                        status = 0; /* OK */
                    }
                    break;
                }
            }
        }
        de = readdir(d);
    }
    closedir(d);
    return status;
}

/* vim: set ts=4 sw=4 expandtab: */
