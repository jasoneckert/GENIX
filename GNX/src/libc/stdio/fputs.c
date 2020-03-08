/*
 * fputs.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)fputs.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

fputs(s, iop)
register char *s;
register FILE *iop;
{
	register r;
	register c;

	while (c = *s++)
		r = putc(c, iop);
	return(r);
}
