/*
 * giveup.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)giveup.c	1.1 (NSC) 11/2/82";
# endif

/*
	Chdir to "/" if argument is 0.
	Set IOT signal to system default.
	Call abort(III).
	(Shouldn't produce a core when called with a 0 argument.)
*/

# include "signal.h"

giveup(dump)
{
	if (!dump)
		chdir("/");
	signal(SIGIOT,0);
	abort();
}
