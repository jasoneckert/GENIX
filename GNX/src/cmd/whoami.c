/*
 * whoami.c: version 1.2 of 9/2/82
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)whoami.c	1.2 (NSC) 9/2/82";
# endif

#include <pwd.h>
/*
 * whoami
 */
struct	passwd *getpwuid();

main()
{
	register struct passwd *pp;

	pp = getpwuid(getuid());
	if (pp == 0) {
		printf("Intruder alert.\n");
		exit(1);
	}
	printf("%s\n", pp->pw_name);
	exit(0);
}
