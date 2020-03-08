/*
 * strcpy.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)strcpy.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Copy string s2 to s1.  s1 must be large enough.
 * return s1
 */

char *
strcpy(s1, s2)
register char *s1, *s2;
{
	register char *os1;

	os1 = s1;
	while (*s1++ = *s2++)
		;
	return(os1);
}
