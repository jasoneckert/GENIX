/*
 * walkdir.c: version 1.6 of 8/23/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)walkdir.c	1.6 (NSC) 8/23/83";
# endif

/* The walkdir routine provides a way of recursively examining all files
 * downward from a starting path specification.  For each file that is found,
 * a procedure supplied by the caller is called with the following arguments:
 *
 *	routine(path, statbuf, error)
 *
 * where path is the pathname to the current file, statbuf is the address of
 * a stat buffer for the file (or zero if the data was not available), and
 * error is nonzero if the current file could not be accessed.  If the path
 * name and status buffer are to be preserved, they should be copied, since
 * the path name lives on the stack, and the stat buffer is reused.  The
 * routine returns zero to continue looking at files, or nonzero to indicate
 * that the current directory is to be aborted.
 *
 * Walkdir accepts the starting path specification, the routine to be called
 * as described above, and flags describing how to carry out the search.
 * Walkdir returns zero if all files were accessable, -1 if the initial path
 * specification was not accessable, or a positive number giving the number
 * of files which were not accessable.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dir.h>
#include <walkdir.h>

#define	READ	0		/* read function */
#define	MAXNAME	1000		/* maximum length of directory spec */
#define	MAXREAD	128		/* number of directory entries to read */

static	struct	stat	statbuf;	/* status buffer */
static	int	(*routine)();		/* routine to call */
static	int	flags;			/* flags */
static	int	errcount;		/* error count */
static	char	*path;			/* pointer to current path */
extern	int	alphasort();		/* sort routine for scandir */


walkdir(rootpath, userproc, opflags)
	char	*rootpath;		/* starting path */
	int	(*userproc)();		/* procedure to call */
	int	opflags;		/* flags (defined in walkdir.h) */
{
	char	pathdata[MAXNAME];	/* path specification */

	strcpy(pathdata, rootpath);
	routine = userproc;
	path = pathdata;
	flags = opflags;
	errcount = 0;
	if (stat(path, &statbuf)) {
		doerror(0);
		return(-1);
	}
	if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
		dodir(path + strlen(path));
	else
		if (flags & SD_RETFILE) (*routine)(path, &statbuf, 0);
	return(errcount);
}


/* Recursively read a directory and handle its files */
static dodir(endpath)
	char	*endpath;		/* end of current path */
{
	register struct	direct	**dp;	/* directory array pointer */
	register struct	direct	**enddp;/* end of directory data */
	register struct	direct	*de;	/* current directory entry */
	register char	*cp;		/* pointer to char */
	struct	direct	**namelist;	/* list of files returned by scandir */
	int	ndir;			/* number of files in directory */
	struct	stat	dirstat;	/* saved status of directory */

	if (flags & SD_RETDIRF) {
		if ((*routine)(path, &statbuf, 0)) return;
	}
	ndir = scandir(path, &namelist, 0, alphasort);
	if (ndir < 0) {
		doerror(0);
		return;
	}
	dirstat = statbuf;
	enddp = namelist + ndir;
	for (dp = namelist; dp < enddp; dp++) {
		de = *dp;
		if ((de->d_ino == 0) ||
			((de->d_name[0] == '.') &&
			((de->d_name[1] == '\0') ||
				((de->d_name[1] == '.') &&
				(de->d_name[2] == '\0'))))) continue;
		cp = endpath;
		if (cp[-1] != '/') *cp++ = '/';
		strcpy(cp, de->d_name);
		if (stat(path, &statbuf)) {
			doerror(0);
			continue;
		}
		if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
			dodir(cp + strlen(cp));
			continue;
		}
		if (flags & SD_RETFILE) {
			if ((*routine)(path, &statbuf, 0)) break;
		}
	}
	for (dp = namelist; dp < enddp; dp++) free(*dp);
	free(*namelist);
	*endpath = '\0';
	if (flags & SD_RETDIRL) (*routine)(path, &dirstat, 0);
}


/* Here on an error */
static doerror(statbuf)
	struct	stat	*statbuf;	/* stat for file in error */
{
	if (flags & SD_TYPERR) perror(path);
	if (flags & SD_RETERR) (*routine)(path, statbuf, 1);
	errcount++;
}
