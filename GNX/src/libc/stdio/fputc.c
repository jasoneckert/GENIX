/*
 * fputc.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)fputc.c	1.1 (NSC) 9/2/82";
# endif

#include <stdio.h>

fputc(c, fp)
FILE *fp;
{
	return(putc(c, fp));
}
