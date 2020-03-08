/*
 * userdir.c: version 1.2 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)userdir.c	1.2 (NSC) 11/2/82";
# endif

/*
	Gets user's login directory.
	The argument must be an integer.
	Returns pointer to login directory on success,
	0 on failure.
        Remembers user ID and login directory for subsequent calls.

	This version has been modified to be 4.1bsd-compatible.
*/

# include <pwd.h>

char *
userdir(uid)
	register int uid;
{
	extern struct passwd	*getpwuid ();
	register struct passwd	*pwp;

	static int ouid;
	static char ldir[33];
	register int i;
	register char *cp;

	if (ouid!=uid || ouid==0) {
		if ((pwp = getpwuid (uid)) == (struct passwd *) 0)
			return((char *)0);
		cp = pwp->pw_dir;
		for (i=0; i<32; i++) {
			if ((ldir[i] = *cp)=='\0' || *cp==':') break;
			cp++;
		}
		ldir[i] = '\0';
	}
	return(ldir);
}
