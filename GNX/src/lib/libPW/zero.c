/*
 * zero.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)zero.c	1.1 (NSC) 11/2/82";
# endif

/*
	Zero `cnt' bytes starting at the address `ptr'.
	Return `ptr'.
*/

char	*zero(p,n)
register char *p;
register int n;
{
	char *op = p;
	while (--n >= 0)
		*p++ = 0;
	return(op);
}
