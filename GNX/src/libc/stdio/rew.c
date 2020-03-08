/*
 * rew.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)rew.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

rewind(iop)
register struct _iobuf *iop;
{
	fflush(iop);
	lseek(fileno(iop), 0L, 0);
	iop->_cnt = 0;
	iop->_ptr = iop->_base;
	iop->_flag &= ~(_IOERR|_IOEOF);
	if (iop->_flag & _IORW)
		iop->_flag &= ~(_IOREAD|_IOWRT);
}
