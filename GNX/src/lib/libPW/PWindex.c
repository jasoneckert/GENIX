/*
 * PWindex.c: version 1.2 of 11/6/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)PWindex.c	1.2 (NSC) 11/6/82";
# endif

/*
	If `s2' is a substring of `s1' return the offset of the first
	occurrence of `s2' in `s1',
	else return -1.

	NOTE: this routine has been renamed PWindex() from index()
	to avoid naming conflicts with the 4.1 routine.
*/

PWindex (as1, as2)
	char *as1, *as2;
{
	register char *s1,*s2,c;
	int offset;

	s1 = as1;
	s2 = as2;
	c = *s2;

	while (*s1)
		if (*s1++ == c) {
			offset = s1 - as1 - 1;
			s2++;
			while ((c = *s2++) == *s1++ && c) ;
			if (c == 0)
				return(offset);
			s1 = offset + as1 + 1;
			s2 = as2;
			c = *s2;
		}
	 return(-1);
}
