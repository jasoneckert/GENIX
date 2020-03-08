/*
 * setbrk.c: version 1.2 of 9/9/82
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)setbrk.c	1.2 (NSC) 9/9/82";
# endif

/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Bell Telephone Laboratories
 *
 */

#include	"defs.h"

setbrk(incr)
{
	REG BYTPTR	a=sbrk(incr);
	brkend=a+incr;
	return(a);
}
