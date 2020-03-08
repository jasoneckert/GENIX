/*
 * strcat.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)strcat.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Concatenate s2 on the end of s1.  S1's space must be large enough.
 * Return s1.
 */

char *
strcat(s1, s2)
register char *s1, *s2;
{
	register char *os1;

	os1 = s1;
	while (*s1++)
		;
	--s1;
	while (*s1++ = *s2++)
		;
	return(os1);
}
