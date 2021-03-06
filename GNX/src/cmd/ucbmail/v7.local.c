#
/*
 * v7.local.c: version 1.1 of 2/15/83
 * Mesa Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)v7.local.c	1.1 (NSC) 2/15/83";
# endif


/*
 * Mail -- a mail program
 *
 * Version 7
 *
 * Local routines that are installation dependent.
 */

#include "rcv.h"

/*
 * Locate the user's mailbox file (ie, the place where new, unread
 * mail is queued).  In Version 7, it is in /usr/spool/mail/name.
 */

findmail()
{
	register char *cp;

	cp = copy("/usr/spool/mail/", mailname);
	copy(myname, cp);
	if (isdir(mailname)) {
		stradd(mailname, '/');
		strcat(mailname, myname);
	}
}

/*
 * Get rid of the queued mail.
 */

demail()
{

	if (remove(mailname) >= 0)
		return;
	close(creat(mailname, 0666));
	alter(mailname);
}

/*
 * Discover user login name.
 */

username(uid, namebuf)
	char namebuf[];
{
	register char *np;

	if (uid == getuid() && (np = getenv("USER")) != NOSTR) {
		strncpy(namebuf, np, 9);
		return(0);
	}
	return(getname(uid, namebuf));
}
