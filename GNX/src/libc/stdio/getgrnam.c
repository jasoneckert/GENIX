/*
 * getgrnam.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getgrnam.c	1.1 (NSC) 9/2/82";
# endif

#include <grp.h>

struct group *
getgrnam(name)
register char *name;
{
	register struct group *p;
	struct group *getgrent();

	setgrent();
	while( (p = getgrent()) && strcmp(p->gr_name,name) );
	endgrent();
	return(p);
}
