/*
 * strlen.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)strlen.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Returns the number of
 * non-NULL bytes in string argument.
 */

strlen(s)
register char *s;
{
	register n;

	n = 0;
	while (*s++)
		n++;
	return(n);
}
