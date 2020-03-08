/*
 * xpipe.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)xpipe.c	1.1 (NSC) 11/2/82";
# endif

/*
	Interface to pipe(II) which handles all error conditions.
	Returns 0 on success,
	fatal() on failure.
*/

xpipe(t)
int *t;
{
	static char p[]="pipe";

	if (pipe(t) == 0)
		return(0);
	return(xmsg(p,p));
}
