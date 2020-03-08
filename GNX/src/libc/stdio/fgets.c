/*
 * fgets.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)fgets.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

char *
fgets(s, n, iop)
char *s;
register FILE *iop;
{
	register c;
	register char *cs;

	cs = s;
	while (--n>0 && (c = getc(iop))>=0) {
		*cs++ = c;
		if (c=='\n')
			break;
	}
	if (c<0 && cs==s)
		return(NULL);
	*cs++ = '\0';
	return(s);
}
