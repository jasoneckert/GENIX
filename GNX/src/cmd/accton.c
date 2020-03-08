/*
 * accton.c: version 1.1 of 2/10/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)accton.c	1.1 (NSC) 2/10/83";
# endif

main(argc, argv)
char **argv;
{
	extern errno;
	if (argc > 1)
		acct(argv[1]);
	else
		acct((char *)0);
	if (errno) {
		perror("accton");
		exit(1);
	}
	exit(0);
}
