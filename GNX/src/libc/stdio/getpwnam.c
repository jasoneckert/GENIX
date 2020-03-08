/*
 * getpwnam.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getpwnam.c	1.1 (NSC) 9/2/82";
# endif

#include <pwd.h>

struct passwd *
getpwnam(name)
char *name;
{
	register struct passwd *p;
	struct passwd *getpwent();

	setpwent();
	while( (p = getpwent()) && strcmp(name,p->pw_name) );
	endpwent();
	return(p);
}
