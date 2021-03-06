/*
 * filbuf.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)filbuf.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>
char	*malloc();

_filbuf(iop)
register FILE *iop;
{
	static char smallbuf[_NFILE];

	if (iop->_flag & _IORW)
		iop->_flag |= _IOREAD;

	if ((iop->_flag&_IOREAD) == 0)
		return(EOF);
	if (iop->_flag&_IOSTRG)
		return(EOF);
tryagain:
	if (iop->_base==NULL) {
		if (iop->_flag&_IONBF) {
			iop->_base = &smallbuf[fileno(iop)];
			goto tryagain;
		}
		if ((iop->_base = malloc(BUFSIZ)) == NULL) {
			iop->_flag |= _IONBF;
			goto tryagain;
		}
		iop->_flag |= _IOMYBUF;
	}
	if (iop == stdin && (stdout->_flag&_IOLBF))
		fflush(stdout);
	iop->_cnt = read(fileno(iop), iop->_base, iop->_flag&_IONBF?1:BUFSIZ);
	iop->_ptr = iop->_base;
	if (--iop->_cnt < 0) {
		if (iop->_cnt == -1) {
			iop->_flag |= _IOEOF;
			if (iop->_flag & _IORW)
				iop->_flag &= ~_IOREAD;
		} else
			iop->_flag |= _IOERR;
		iop->_cnt = 0;
		return(-1);
	}
	return(*iop->_ptr++&0377);
}
