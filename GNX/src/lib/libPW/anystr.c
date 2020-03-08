/*
 * anystr.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)anystr.c	1.1 (NSC) 11/2/82";
# endif

# include	"sys/types.h"
# include	"macros.h"

/*
	This routine returns the position of the first character
	in s1 that matches any character in s2.
*/

anystr(s1, s2)
char *s1, *s2;
{
	register int i = -1;
	register int c;

	while (c = s1[++i])
		if (any(c, s2))
			return(i);
	return(-1);
}
