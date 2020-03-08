/*
 * setbuf.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)setbuf.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

setbuf(iop, buf)
register struct _iobuf *iop;
char *buf;
{
	if (iop->_base != NULL && iop->_flag&_IOMYBUF)
		free(iop->_base);
	iop->_flag &= ~(_IOMYBUF|_IONBF|_IOLBF);
	if ((iop->_base = buf) == NULL)
		iop->_flag |= _IONBF;
	else
		iop->_ptr = iop->_base;
	iop->_cnt = 0;
}
