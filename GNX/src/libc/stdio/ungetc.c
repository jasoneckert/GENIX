/*
 * ungetc.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)ungetc.c	1.1 (NSC) 9/2/82";
# endif

#include <stdio.h>

ungetc(c, iop)
register FILE *iop;
{
	if (c == EOF)
		return(-1);
	if ((iop->_flag&_IOREAD)==0 || iop->_ptr <= iop->_base)
		if (iop->_ptr == iop->_base && iop->_cnt==0)
			*iop->_ptr++;
		else
			return(-1);
	iop->_cnt++;
	*--iop->_ptr = c;
	return(0);
}
