/*
 * tmpnam.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)tmpnam.c	1.1 (NSC) 9/2/82";
# endif

char *tmpnam(s)
char *s;
{
	static seed;

	sprintf(s, "temp.%d.%d", getpid(), seed++);
	return(s);
}
