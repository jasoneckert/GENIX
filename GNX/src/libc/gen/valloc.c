/*
 * valloc.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)valloc.c	1.1 (NSC) 9/5/82";
# endif

#include <valign.h>

char	*malloc();

char *
valloc(i)
	int i;
{
	char *cp = malloc(i + (VALSIZ-1));
	int j;

	j = ((int)cp + (VALSIZ-1)) &~ (VALSIZ-1);
	return ((char *)j);
}
