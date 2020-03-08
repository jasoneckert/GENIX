/*
 * zeropad.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)zeropad.c	1.1 (NSC) 11/2/82";
# endif

/*
	Replace initial blanks with '0's in `str'.
	Return `str'.
*/

char *zeropad(str)
char *str;
{
	register char *s;

	for (s=str; *s == ' '; s++)
		*s = '0';
	return(str);
}
