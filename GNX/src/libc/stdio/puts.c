/*
 * puts.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)puts.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

puts(s)
register char *s;
{
	register c;

	while (c = *s++)
		putchar(c);
	return(putchar('\n'));
}
