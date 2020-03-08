/*
 * getpwuid.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getpwuid.c	1.1 (NSC) 9/2/82";
# endif

#include <pwd.h>

struct passwd *
getpwuid(uid)
register uid;
{
	register struct passwd *p;
	struct passwd *getpwent();

	setpwent();
	while( (p = getpwent()) && p->pw_uid != uid );
	endpwent();
	return(p);
}
