/*
 * cat.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)cat.c	1.1 (NSC) 11/2/82";
# endif

/*
	Concatenate strings.
 
	cat(destination,source1,source2,...,sourcen,0);
 
	returns destination.
*/

char *cat(dest,source)
char *dest, *source;
{
	register char *d, *s, **sp;

	d = dest;
	for (sp = &source; s = *sp; sp++) {
		while (*d++ = *s++) ;
		d--;
	}
	return(dest);
}
