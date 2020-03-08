/*
 * yes.c: version 1.2 of 9/2/82
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)yes.c	1.2 (NSC) 9/2/82";
# endif

main(argc, argv)
char **argv;
{
	for (;;)
		printf("%s\n", argc>1? argv[1]: "y");
}
