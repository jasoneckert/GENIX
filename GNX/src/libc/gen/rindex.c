/*
 * rindex.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)rindex.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Return the ptr in sp at which the character c last
 * appears; NULL if not found
*/

#define NULL 0

char *
rindex(sp, c)
register char *sp, c;
{
	register char *r;

	r = NULL;
	do {
		if (*sp == c)
			r = sp;
	} while (*sp++);
	return(r);
}
