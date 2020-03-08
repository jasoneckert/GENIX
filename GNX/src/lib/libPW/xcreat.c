/*
 * xcreat.c: version 1.2 of 11/15/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)xcreat.c	1.2 (NSC) 11/15/82";
# endif

# include	"sys/types.h"
# include	"macros.h"

/*
	"Sensible" creat: write permission in directory is required in
	all cases, and created file is guaranteed to have specified mode
	and be owned by effective user.
	(It does this by first unlinking the file to be created.)
	Returns file descriptor on success,
	fatal() on failure.
*/

xcreat(name,mode)
char *name;
int mode;
{
	register int fd;
	register char *d, *svd;

	svd = d = (char *) malloc (size (name));
	copy(name,d);
	if (!exists(dname(d))) {
		sprintf(Error,"directory `%s' nonexistent (ut1)",d);
		fatal(Error);
	}
	unlink(name);
	if ((fd = creat (name, mode)) >= 0) {
		free (svd);
		return(fd);
	}
	free (svd);
	return(xmsg(name,"xcreat"));
}
