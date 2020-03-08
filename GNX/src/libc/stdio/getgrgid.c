/*
 * getgrgid.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getgrgid.c	1.1 (NSC) 9/2/82";
# endif

#include <grp.h>

struct group *
getgrgid(gid)
register gid;
{
	register struct group *p;
	struct group *getgrent();

	setgrent();
	while( (p = getgrent()) && p->gr_gid != gid );
	endgrent();
	return(p);
}
