/*
 * userexit.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)userexit.c	1.1 (NSC) 11/2/82";
# endif

/*
	Default userexit routine for fatal and setsig.
	User supplied userexit routines can be used for logging.
*/

userexit(code)
{
	return(code);
}
