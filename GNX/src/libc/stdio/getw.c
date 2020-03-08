/*
 * getw.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getw.c	1.1 (NSC) 9/2/82";
# endif

#include <stdio.h>

getw(iop)
register FILE *iop;
{
	register i;
	register char *p;
	int w;

	p = (char *)&w;
	for (i=sizeof(int); --i>=0;)
		*p++ = getc(iop);
	if (feof(iop))
		return(EOF);
	return(w);
}
