/*
 * clrerr.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)clrerr.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

clearerr(iop)
register struct _iobuf *iop;
{
	iop->_flag &= ~(_IOERR|_IOEOF);
}
