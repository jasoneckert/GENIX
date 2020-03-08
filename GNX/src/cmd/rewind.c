/*
 * rewind.c: version 1.2 of 1/18/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)rewind.c	1.2 (NSC) 1/18/83";
# endif

/* rewinds mag tape drive */
main(argc,argv) char**argv; {
	char *f;
	int fd;

	if (argc > 1) f = argv[1];
		else  f = "/dev/tc0";

	fd = open(f,0);
	if (fd < 0) printf("Can't open %s\n",f);
		else close(fd);
}
