/*
 * repeat.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)repeat.c	1.1 (NSC) 11/2/82";
# endif

/*
	Set `result' equal to `str' repeated `repfac' times.
	Return `result'.
*/

char *repeat(result,str,repfac)
char *result, *str;
register unsigned repfac;
{
	register char *r, *s;

	r = result;
	for (++repfac; --repfac > 0; --r)
		for (s=str; *r++ = *s++; );
	*r = '\0';
	return(result);
}
