/* $Id$ */

#include <stdio.h>
#include <dirent.h>
#include <getopt.h>
#include "../common.h"
#include "../cpu.h"

void printusage(const char *name){
    fprintf(stderr, "Usage: %s [-O | --owner=username] [-T | --tag=tag] [-V | --target=tagname]\n", name);
    fprintf(stderr, " [-W | --warning=percentage] [-C | --critical=percentage]\n");
    fprintf(stderr, " [-S | --cache=dir] [-H | --help]\n");
}

void printmessage(const char *name, const char *stat, int u){
    printf("%s cpu %s: %d\n", name, stat, u);
}

int main(int argc, char **argv){
    DIR *d;
    struct dirent *de;
    char *owner;
    char *tag;
    char *cache;
    char *target;
    int warning  = 0;
    int critical = 0;
    int status = 3; /* UNKNOWN */
    int c;
    unsigned long int usage[2];
    unsigned long int uptime[2];

    while(1){
        int option_index = 0;
        static struct option long_options[] = {
            {"owner",    1, NULL, 'O'},
            {"tag",      1, NULL, 'T'},
            {"target",   1, NULL, 'V'},
            {"warning",  1, NULL, 'W'},
            {"critical", 1, NULL, 'C'},
            {"cache",    1, NULL, 'S'},
            {"help",     1, NULL, 'H'},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "O:T:V:W:C:S:H", long_options, &option_index);
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
                warning = atoi(optarg);
                break;
            case 'C':
                critical = atoi(optarg);
                break;
            case 'S':
                cache = optarg;
                break;
            case 'H':
            default:
                printusage(argv[0]);
                return 3; /* UNKNOWN */
        }
    }

    if( (tag==NULL) || (owner==NULL) || (target==NULL) || (cache==NULL) || (warning==0) || (critical==0)){
        printusage(argv[0]);
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
            unsigned long int ticks, span;
            snprintf(p, sizeof(p), "/proc/%s", de->d_name);
            if(monitored(p, owner, tag, NULL, name, sizeof(name))){
                char cachefile[1024];
                snprintf(cachefile, sizeof(cachefile), "%s/%s", cache, name);
                readcache(cachefile, &uptime[0], &usage[0]);
                usage[1] = cpuusage(atoi(de->d_name));
                uptime[1] = getuptime();
                writecache(cachefile, uptime[1], usage[1]);
                /* If the process is restarted, the system is rebooted, or
                 * the jiffy counter wraps (once every 1.5 years) just
                 * don't report anything */
                if((usage[1] >= usage[0]) && (uptime[1] > uptime[0])){
                    int cpu = (100*(usage[1]-usage[0]))/(uptime[1]-uptime[0]);
                    if(cpu > critical){
                        printmessage(name, "critical", cpu);
                        status = 2; /* CRITICAL */
                    } else if(cpu > warning){
                        printmessage(name, "warning", cpu);
                        status = 1; /* WARNING */
                    } else {
                        printmessage(name, "ok", cpu);
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
