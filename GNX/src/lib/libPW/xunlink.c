/*
 * xunlink.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)xunlink.c	1.1 (NSC) 11/2/82";
# endif

/*
	Interface to unlink(II) which handles all error conditions.
	Returns 0 on success,
	fatal() on failure.
*/

xunlink(f)
{
	if (unlink(f))
		return(xmsg(f,"xunlink"));
	return(0);
}
