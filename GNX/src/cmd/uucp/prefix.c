/*
 * prefix.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)prefix.c	1.1 (NSC) 5/4/83";
# endif

/*******
 *	prefix(s1, s2)	check s2 for prefix s1
 *	char *s1, *s2;
 *
 *	return 0 - !=
 *	return 1 - == 
 */

prefix(s1, s2)
char *s1, *s2;
{
	char c;

	while ((c = *s1++) == *s2++)
		if (c == '\0')
			return(1);
	return(c == '\0');
}
