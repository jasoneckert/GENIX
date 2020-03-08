/*
 * tty.c: version 1.1 of 9/23/82
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)tty.c	1.1 (NSC) 9/23/82";
# endif

/*
 * Type tty name
 */

char	*ttyname();

main(argc, argv)
char **argv;
{
	register char *p;

	p = ttyname(0);
	if(argc==2 && !strcmp(argv[1], "-s"))
		;
	else
		printf("%s\n", (p? p: "not a tty"));
	exit(p? 0: 1);
}
