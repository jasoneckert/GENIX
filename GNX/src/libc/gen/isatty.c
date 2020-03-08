/*
 * isatty.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)isatty.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Returns 1 iff file is a tty
 */

#include <sgtty.h>

isatty(f)
{
	struct sgttyb ttyb;

	if (gtty(f, &ttyb) < 0)
		return(0);
	return(1);
}
