/*
 * strout.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)strout.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

_strout(count, string, adjust, file, fillch)
register char *string;
register count;
int adjust;
register struct _iobuf *file;
{
	while (adjust < 0) {
		if (*string=='-' && fillch=='0') {
			putc(*string++, file);
			count--;
		}
		putc(fillch, file);
		adjust++;
	}
	while (--count>=0)
		putc(*string++, file);
	while (adjust) {
		putc(fillch, file);
		adjust--;
	}
}
