/*
 * strend.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)strend.c	1.1 (NSC) 11/2/82";
# endif

char *strend(p)
register char *p;
{
	while (*p++)
		;
	return(--p);
}
