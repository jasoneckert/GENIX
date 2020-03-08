/*
 * fprintf.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)fprintf.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

fprintf(iop, fmt, args)
FILE *iop;
char *fmt;
{
	_doprnt(fmt, &args, iop);
	return(ferror(iop)? EOF: 0);
}
