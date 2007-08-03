/*
 * $Id$
 */

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>



int main(int argc, const char **argv){
	/* Check command line */
	if(argc < 3){
		fprintf(stderr, "Usage: %s user cmd\n", argv[0]);
		return 1;
	}
	
	struct passwd *pw;
	pw = getpwnam(argv[1]);

	if(pw==NULL){
		fprintf(stderr, "User not found\n");
		return 1;
	}

	if(initgroups(argv[1], pw->pw_gid)==-1){
		perror("initgroups");
		return 1;
	}

	if (setregid(pw->pw_gid, pw->pw_gid)==0 && setreuid(pw->pw_uid, pw->pw_uid)==0){
		argv+=2;
                execvp(argv[0], argv);
		perror("exec");
		return 1;
	}
	perror("setre[gu]id");
	return 1;
}
