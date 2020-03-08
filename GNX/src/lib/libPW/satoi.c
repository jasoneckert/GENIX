/*
 * satoi.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)satoi.c	1.1 (NSC) 11/2/82";
# endif

# include	"sys/types.h"
# include	"macros.h"

char *satoi(p,ip)
register char *p;
register int *ip;
{
	register int sum;

	sum = 0;
	while (numeric(*p))
		sum = sum * 10 + (*p++ - '0');
	*ip = sum;
	return(p);
}
