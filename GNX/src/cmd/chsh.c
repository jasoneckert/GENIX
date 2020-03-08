/*
 * chsh.c: version 1.2 of 8/5/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)chsh.c	1.2 (NSC) 8/5/83";
# endif

/*
 * chsh
 */

#include <stdio.h>
#include <signal.h>
#include <pwd.h>

char	passwd[] = "/etc/passwd";
char	temp[]	 = "/etc/ptmp";
struct	passwd *pwd;
struct	passwd *getpwent();
int	endpwent();
char	*crypt();
char    *getpass();
char	*pw;
char	pwbuf[10];
char	buf[BUFSIZ];

/*
 * List of `approved' shells:
 */
char    *goodshells[] =
{       "/bin/sh",
	"/bin/csh",
	0
};

main(argc, argv)
char *argv[];
{
	char *p;
	int i;
	char saltc[2];
	long salt;
	int u,fi,fo;
	int insist;
	int ok, flags;
	int c;
	int pwlen;
	register FILE *tf;
	register int sind;

	insist = 0;
	if(argc < 2 || argc > 3) {
		usage ();
		/* NOTREACHED */
	}

	if (argc == 2)
		argv[2] = "";
	else {
		/* Check for shell acceptability: */
		register int gotit = 0;

		for (sind = 0; goodshells[sind] != 0; sind++) {
			if (strcmp (argv[2], goodshells[sind]) == 0) {
				gotit++;
				break;
			}
		}
		if ( ! gotit ) {
			usage ();
			/* NOTREACHED */
		}
	}

	while((pwd=getpwent()) != NULL){
		if(strcmp(pwd->pw_name,argv[1]) == 0){
			u = getuid();
			if(u!=0 && u != pwd->pw_uid){
				printf("Permission denied.\n");
				goto bex;
				}
			break;
			}
		}
	endpwent();
	signal(SIGHUP, 1);
	signal(SIGINT, 1);
	signal(SIGQUIT, 1);
	signal(SIGTSTP, 1);

	if(access(temp, 0) >= 0) {
		printf("Temporary file busy -- try again\n");
		goto bex;
	}
	if((tf=fopen(temp,"w")) == NULL) {
		printf("Cannot create temporary file\n");
		goto bex;
	}

/*
 *	copy passwd to temp, replacing matching lines
 *	with new shell.
 */

	while((pwd=getpwent()) != NULL) {
		if(strcmp(pwd->pw_name,argv[1]) == 0) {
			u = getuid();
			if(u != 0 && u != pwd->pw_uid) {
				printf("Permission denied.\n");
				goto out;
			}
			pwd->pw_shell = argv[2];
		}
		fprintf(tf,"%s:%s:%d:%d:%s:%s:%s\n",
			pwd->pw_name,
			pwd->pw_passwd,
			pwd->pw_uid,
			pwd->pw_gid,
			pwd->pw_gecos,
			pwd->pw_dir,
			pwd->pw_shell);
	}
	endpwent();
	fclose(tf);

/*
 *	copy temp back to passwd file
 */

	if((fi=open(temp,0)) < 0) {
		printf("Temp file disappeared!\n");
		goto out;
	}
	if((fo=creat(passwd, 0644)) < 0) {
		printf("Cannot recreate passwd file.\n");
		goto out;
	}
	while((u=read(fi,buf,sizeof(buf))) > 0) write(fo,buf,u);

out:
	unlink(temp);

bex:
	exit(1);
}

/*
 * Explain how to use the stupid thing.
 */
usage ()
{
	register int sind;

	printf("Usage: chsh user [ shellname ]\n");
	printf("where shellname is one of:\n");
	for (sind = 0; goodshells[sind] != 0; sind++)
		printf ("    %s\n", goodshells[sind]);
	exit (1);
}
