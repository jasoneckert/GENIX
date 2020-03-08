/*
 * index.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)index.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

#define	NULL	0

char *
index(sp, c)
register char *sp, c;
{
	do {
		if (*sp == c)
			return(sp);
	} while (*sp++);
	return(NULL);
}
