/*
 * strcmp.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)strcmp.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */

strcmp(s1, s2)
register char *s1, *s2;
{

	while (*s1 == *s2++)
		if (*s1++=='\0')
			return(0);
	return(*s1 - *--s2);
}
