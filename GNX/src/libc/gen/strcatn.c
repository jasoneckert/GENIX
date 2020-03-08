/*
 * strcatn.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)strcatn.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Concatenate s2 on the end of s1.  S1's space must be large enough.
 * At most n characters are moved.
 * Return s1.
 */

char *
strcatn(s1, s2, n)
register char *s1, *s2;
register n;
{
	register char *os1;

	os1 = s1;
	while (*s1++)
		;
	--s1;
	while (*s1++ = *s2++)
		if (--n < 0) {
			*--s1 = '\0';
			break;
		}
	return(os1);
}
