/*
 * closedir.c: version 1.1 of 2/9/83
 * Libndir Source
 */
# ifdef SCCS
static char *sccsid = "@(#)closedir.c	1.1 (NSC) 2/9/83";
# endif

#include <sys/types.h>
#include <dir.h>

/*
 * close a directory.
 */
void
closedir(dirp)
	register DIR *dirp;
{
	close(dirp->dd_fd);
	dirp->dd_fd = -1;
	dirp->dd_loc = 0;
	free(dirp);
}
