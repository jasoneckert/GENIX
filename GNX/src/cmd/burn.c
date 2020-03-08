/*
 * burn.c: version 1.3 of 1/27/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)burn.c	1.3 (NSC) 1/27/83";
# endif

/* Burn - program to run for specified elapsed time */

#include <signal.h>

int	quit();

main(argc, argv)
	char	**argv;
{
	register int	val;		/* seconds to run */
	register char	*ptr;		/* pointer to number */

	if (argc > 1) {
		++argv;
		ptr = *argv;
		val = 0;
		while (*ptr) {
			if ((*ptr < '0') || (*ptr > '9')) {
				printf("Bad number\n");
			}
			val = (val * 10) + *ptr - '0';
			ptr++;
		}
		if (val == 0) exit(0);
		signal(SIGALRM, quit);
		alarm(val);
	}
	while (1);
}


quit()
{
	exit(0);
}
