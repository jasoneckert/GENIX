/*
 * putw.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)putw.c	1.1 (NSC) 9/2/82";
# endif

#include <stdio.h>

putw(w, iop)
register FILE *iop;
{
	register char *p;
	register i;

	p = (char *)&w;
	for (i=sizeof(int); --i>=0;)
		putc(*p++, iop);
	return(ferror(iop));
}
