/*
 * strcpyn.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)strcpyn.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Copy s2 to s1, truncating or null-padding to always copy n bytes
 * return s1
 */

char *
strcpyn(s1, s2, n)
register char *s1, *s2;
{
	register i;
	register char *os1;

	os1 = s1;
	for (i = 0; i < n; i++)
		if ((*s1++ = *s2++) == '\0') {
			while (++i < n)
				*s1++ = '\0';
			return(os1);
		}
	return(os1);
}
