/*
 * strcmpn.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)strcmpn.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Compare strings (at most n bytes):  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */

strcmpn(s1, s2, n)
register char *s1, *s2;
register n;
{

	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return(n<0 ? 0 : *s1 - *--s2);
}
