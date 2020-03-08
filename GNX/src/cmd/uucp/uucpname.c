/*
 * uucpname.c: version 1.2 of 5/11/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)uucpname.c	1.2 (NSC) 5/11/83";
# endif

#include "uucp.h"

#ifdef UNAME
#include <sys/utsname.h>
#endif

/*******
 *	uucpname(name)		get the uucp name
 *
 *	return code - none
 */

uucpname(name)
char *name;
{
	register char *s, *d;

#ifdef UNAME
	struct utsname utsn;

	uname(&utsn);
	s = utsn.nodename;
#endif UNAME

#ifdef HOSTNAME
	char	namebuf [256];
	int	nlen = sizeof namebuf;

	s = namebuf;
	gethostname (s, &nlen);
#endif HOSTNAME

#ifndef UNAME
#ifndef HOSTNAME
	s = MYNAME;
#endif  HOSTNAME
#endif  UNAME

	d = name;
	while ((*d = *s++) && d < name + 7)
		d++;
	*(name + 7) = '\0';
	return;
}
