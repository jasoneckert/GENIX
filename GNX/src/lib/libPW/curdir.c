/*
 * curdir.c: version 1.2 of 10/17/83
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)curdir.c	1.2 (NSC) 10/17/83";
# endif

/*
 *  curdir - get current working directory  (algorithm from /bin/pwd)
 *
 *  Remarks:
 *
 *  The name of the current working directory is copied into
 *  the supplied string `wdir'.  The current working directory
 *  is changed during the execution of the routine and restored
 *  at the end by a chdir(wdir).  If an error occurs the current
 *  working directory is undefined.
 *
 *  Returns 0 on success, < 0 on failure.
 *
 *  This routine is a complete replacement of the original System III
 *  version.  It is adapted from code obtained over the USENET net.sources.
 *  It uses the 4.2bsd directory interface.  If run on a previous system,
 *  the libndir compatability routines must be loaded as well.
 */

# include <sys/types.h>
# include <dir.h>
# include <sys/stat.h>

# define MaxPathNameLen 1024	/* System maximum path name length */

curdir (wdir)
	char	*wdir;
{
#   define isbad(xx)	(xx) == NULL		/* used on opendir() */
#   define readit()	(dirp = readdir (fd))
#   define skipit()	0

    struct direct	*dirp;
    register DIR	*fd;

    char		temp[MaxPathNameLen];
    struct stat		sb,
			sbc,
			root;
    register int	found;

    /* Initially root */
    strcpy (wdir, "/");
    stat ("/", &root);

    for ( ;; ) {
	if (isbad (fd = opendir ("..")))
	    return (-1);
	if (stat (".", &sbc) < 0 || stat ("..", &sb) < 0)
	    goto out;
	if (sbc.st_ino == root.st_ino && sbc.st_dev == root.st_dev) {
	    closedir (fd);
	/* Elide trailing '/' (left over from initial dir): */
	if (strlen (wdir) > 1)
		wdir [strlen (wdir) - 1] = '\0';
	    return (chdir (wdir));
	}

	if (sbc.st_ino == sb.st_ino && sbc.st_dev == sb.st_dev) {
	    closedir (fd);
	    chdir    ("/");
	    if (isbad (fd = opendir (".")))
		return (-1);
	    if (stat (".", &sb) < 0)
		goto out;

	    /*  scan the root directory for directory with same device  */
	    if (sbc.st_dev != sb.st_dev) {
		while (readit ()) {
		    if (skipit ())
			continue;
		    if (stat (dirp->d_name, &sb) < 0)
			goto out;
		    if (sbc.st_dev == sb.st_dev) {
			sprintf  (temp, "%s%s", dirp->d_name, wdir);
			strcpy   (wdir + 1, temp);
			closedir (fd);
			/* Elide trailing '/' (left over from initial dir): */
			if (strlen (wdir) > 1)
				wdir [strlen (wdir) - 1] = '\0';
			return (chdir (wdir));
		    }
		}
	    }
	    else {
		closedir (fd);
		return   (chdir (wdir));
	    }
	}

	/*  scan parent directory for file with inode of current directory  */
	found = 0;
	while (readit ()) {
	    if (skipit ())
		continue;
	    sprintf (temp, "../%s", dirp->d_name);
	    if (stat (temp, &sb) >= 0
		    && sb.st_ino == sbc.st_ino
		    && sb.st_dev == sbc.st_dev) {
		closedir (fd);
		found++;
		chdir   ("..");
		sprintf (temp, "%s%s", dirp->d_name, wdir);
		strcpy  (wdir + 1, temp);
		break;
	    }
	}
	if (!found)
	    goto out;
    }

out: 
    closedir (fd);
    return   (-1);
}
