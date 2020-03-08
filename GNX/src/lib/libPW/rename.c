/*
 * rename.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)rename.c	1.1 (NSC) 11/2/82";
# endif

# include "errno.h"
# include "fatal.h"

/*
	rename (unlink/link)
	Calls xlink() and xunlink().
*/

rename(oldname,newname)
char *oldname, *newname;
{
	extern int errno;

	if (unlink(newname) < 0 && errno != ENOENT)
		return(xunlink(newname));

	if (xlink(oldname,newname) == Fvalue)
		return(-1);
	return(xunlink(oldname));
}
