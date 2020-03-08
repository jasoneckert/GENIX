/*
 * any.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)any.c	1.1 (NSC) 11/2/82";
# endif

/*
	If any character of `s' is `c', return 1
	else return 0.
*/

any(c,s)
register char c, *s;
{
	while (*s)
		if (*s++ == c)
			return(1);
	return(0);
}
