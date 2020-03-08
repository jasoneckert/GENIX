/*
 * flsbuf.c: version 3.18 of 5/18/83
 * 
 */

# ifdef SCCS
static char *sccsid = "@(#)flsbuf.c	3.18 (NSC) 5/18/83";
# endif

/*
 * Copyright (C) 1982 by National Semiconductor Corporation
 *
 *	National Semiconductor Corporation
 *	2900 Semiconductor Drive
 *	Santa Clara, California 95051
 *
 *	All Rights Reserved
 *
 * This software is furnished under a license and may be used and copied only
 * in accordance with the terms of such license and with the inclusion of the
 * above copyright notice.  This software or any other copies thereof may not
 * be provided or otherwise made available to any other person.  No title to
 * or ownership of the software is hereby transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by National Semiconductor
 * Corporation.
 *
 * National Semiconductor Corporation assumes no responsibility for the use
 * or reliability of its software on equipment configurations that are not
 * supported by National Semiconductor Corporation.
 */


#include	<stdio.h>

char	*malloc();

_flsbuf(c, iop)
register FILE *iop;
{
	register char *base;
	register n, rn;
	char c1;
	extern char _sobuf[];

	if (iop->_flag & _IORW) {
		iop->_flag |= _IOWRT;
		iop->_flag &= ~_IOEOF;
	}

	if ((iop->_flag&_IOWRT)==0)
		return(EOF);
tryagain:
	if (iop->_flag&_IOLBF) {
		base = iop->_base;
		*iop->_ptr++ = c;
		if (iop->_ptr >= base+BUFSIZ || c == '\n') {
			n = write(fileno(iop), base, rn = iop->_ptr - base);
			iop->_ptr = base;
		} else
			rn = n = 0;
		iop->_cnt = 0;
	} else if (iop->_flag&_IONBF) {
		c1 = c;
		rn = 1;
		n = write(fileno(iop), &c1, rn);
		iop->_cnt = 0;
	} else {
		if ((base=iop->_base)==NULL) {
			if (iop==stdout) {
				if (isatty(fileno(stdout)))
					iop->_flag |= _IOLBF;
				iop->_base = _sobuf;
				iop->_ptr = _sobuf;
				goto tryagain;
			}
			if ((iop->_base=base=malloc(BUFSIZ)) == NULL) {
				iop->_flag |= _IONBF;
				goto tryagain;
			}
			iop->_flag |= _IOMYBUF;
			rn = n = 0;
		} else if ((rn = n = iop->_ptr - base) > 0) {
			iop->_ptr = base;
			n = write(fileno(iop), base, n);
		}
		iop->_cnt = BUFSIZ-1;
		*base++ = c;
		iop->_ptr = base;
	}
	if (rn != n) {
		iop->_flag |= _IOERR;
		return(EOF);
	}
	return(c);
}

fflush(iop)
register struct _iobuf *iop;
{
	register char *base;
	register n;

	if ((iop->_flag&(_IONBF|_IOWRT))==_IOWRT
	 && (base=iop->_base)!=NULL && (n=iop->_ptr-base)>0) {
		iop->_ptr = base;
		iop->_cnt = (iop->_flag&(_IOLBF|_IONBF)) ? 0 : BUFSIZ;
		if (write(fileno(iop), base, n)!=n) {
			iop->_flag |= _IOERR;
			return(EOF);
		}
	}
	return(0);
}

/*
 * Flush buffers on exit
 */

_cleanup()
{
	register struct _iobuf *iop;
	extern struct _iobuf *_lastbuf;

	for (iop = _iob; iop < _lastbuf; iop++)
		fclose(iop);
}

fclose(iop)
register struct _iobuf *iop;
{
	register r;

	r = EOF;
	if (iop->_flag&(_IOREAD|_IOWRT|_IORW) && (iop->_flag&_IOSTRG)==0) {
		r = fflush(iop);
		if (close(fileno(iop)) < 0)
			r = EOF;
		if (iop->_flag&_IOMYBUF)
			free(iop->_base);
		if (iop->_flag&(_IOMYBUF|_IONBF|_IOLBF))
			iop->_base = NULL;
	}
	iop->_flag &= ~(_IOREAD|_IOWRT|_IOLBF|_IONBF|_IOMYBUF|_IOERR|_IOEOF|_IOSTRG|_IORW);
	iop->_cnt = 0;
	iop->_ptr = iop->_base;		/* clear the buffer, please !!! */
	return(r);
}
