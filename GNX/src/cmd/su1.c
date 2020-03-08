/*
 * su1.c: version 1.8 of 2/24/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)su1.c	1.8 (NSC) 2/24/83";
# endif

/* Program to execute a single command as another user (typically root).
 * Requires that the program be run with effective id of root.
 */

#include <stdio.h>
#include <pwd.h>
#define	ROOT	0

struct	passwd	*getpwnam();	/* password finding routine */
char	*getpass();		/* password reading routine */


main(argc, argv)
	register char	**argv;
{
	register char	*str;		/* password string */
	register char	*user;		/* user name */
	register struct	passwd	*pwd;	/* password entry */
	register FILE	*console;	/* console file */

	argv++;
	user = "root";
	if (**argv == '-') {
		user = &argv[0][1];
		argv++;
		argc--;
	}
	if (argc < 2) {
		printf("Usage: su1 [-user] command arg ...\n");
		exit(1);
	}
	if (geteuid() != ROOT) {
		printf("Not setuid as root\n");
		exit(1);
	}
	pwd = getpwnam(user);
	if (pwd == NULL) {
		printf("Cannot find password entry\n");
		exit(1);
	}
	if ((getuid() != ROOT) && (pwd->pw_passwd[0] != '\0')) {
		str = getpass("Password:");
		if (strcmp(pwd->pw_passwd, crypt(str, pwd->pw_passwd))) {
			printf("Sorry\n");
			exit(1);
		}
	}
	endpwent();
	if (pwd->pw_uid == ROOT) {
		console = fopen("/dev/console", "w");
		if (console != NULL) {
			fprintf(console, "SU1: %s %s %s\r\n",
				getlogin(), ttyname(2), *argv);
			fclose(console);
		}
	}
	setgid(pwd->pw_gid);
	setuid(pwd->pw_uid);
	execvp(*argv, argv);
	perror(*argv);
	exit(1);
}
