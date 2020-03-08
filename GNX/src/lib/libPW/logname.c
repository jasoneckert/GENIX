/*
 * logname.c: version 1.2 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)logname.c	1.2 (NSC) 11/2/82";
# endif

char *
logname()
{
	return((char *)getenv("USER"));
}
