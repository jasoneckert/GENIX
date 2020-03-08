/*
 * sysacct.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)sysacct.c	1.1 (NSC) 5/4/83";
# endif

#include <sys/types.h>


/*******
 *	sysacct(bytes, time)	output accounting info
 *	time_t time;
 *	long bytes;
 */

sysacct(bytes, time)
time_t time;
long bytes;
{
	return;
}
