/*
 * getchar.c: version 1.3 of 9/3/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getchar.c	1.3 (NSC) 9/3/82";
# endif

/*
 * A subroutine version of the macro getchar.
 */
#include <stdio.h>

#undef getchar

getchar()
{
	return(getc(stdin));
}
