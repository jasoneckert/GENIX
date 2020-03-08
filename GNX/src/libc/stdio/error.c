/*
 * error.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)error.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

_error(s)
register char *s;
{
	static reentered;

	if (reentered++)
		_exit(0177);
	write(2, s, strlen(s));
	exit(0176);
}
