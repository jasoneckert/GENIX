/*
 * putchar.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)putchar.c	1.1 (NSC) 9/2/82";
# endif

/*
 * A subroutine version of the macro putchar
 */
#include <stdio.h>

#undef putchar

putchar(c)
register c;
{
	putc(c, stdout);
}
