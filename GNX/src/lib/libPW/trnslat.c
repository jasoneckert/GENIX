/*
 * trnslat.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)trnslat.c	1.1 (NSC) 11/2/82";
# endif

/*
	Copy `str' to `result' replacing any character found
	in both `str' and `old' with the corresponding character from `new'.
	Return `result'.
*/

char *trnslat(str,old,new,result)
register char *str;
char *old, *new, *result;
{
	register char *r, *o;

	for (r = result; *r = *str++; r++)
		for (o = old; *o; )
			if (*r == *o++) {
				*r = new[o - old -1];
				break;
			}
	return(result);
}
