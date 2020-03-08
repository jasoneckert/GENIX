/*
 * echo.c: version 1.2 of 9/2/82
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)echo.c	1.2 (NSC) 9/2/82";
# endif

#include <stdio.h>

main(argc, argv)
int argc;
char *argv[];
{
	register int i, nflg;

	nflg = 0;
	if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'n') {
		nflg++;
		argc--;
		argv++;
	}
	for(i=1; i<argc; i++) {
		fputs(argv[i], stdout);
		if (i < argc-1)
			putchar(' ');
	}
	if(nflg == 0)
		putchar('\n');
	exit(0);
}
