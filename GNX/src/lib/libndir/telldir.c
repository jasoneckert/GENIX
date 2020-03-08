/*
 * telldir.c: version 1.1 of 2/9/83
 * Libndir Source
 */
# ifdef SCCS
static char *sccsid = "@(#)telldir.c	1.1 (NSC) 2/9/83";
# endif

#include <sys/types.h>
#include <dir.h>

/*
 * return a pointer into a directory
 */
long
telldir(dirp)
	DIR *dirp;
{
	return (lseek(dirp->dd_fd, 0L, 1) - dirp->dd_size + dirp->dd_loc);
}
