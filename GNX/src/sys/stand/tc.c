/*
 * tc.c: version 1.11 of 10/10/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)tc.c	1.11 (NSC) 10/10/83";
# endif

/*  stand alone device driver for streaming tape */

#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/saio.h>

#define TCREAD 0
#define TCWRITE 1
#define MAXSIZE 127		/* maximum pages in one IO */

/* MUST MATCH DEFINITION IN TCUSAIO.C */
#define	NOERROR		0		/* no error occurred */
#define	ARGERROR	1		/* bad arguments */
#define	CMMDERROR	2		/* illegal command sequence */
#define	SENSEERROR	3		/* failed to read sense bytes */
#define	EOFERROR	4		/* end of file error (file mark) */
#define	EOTERROR	5		/* end of tape error */
#define	TCUERROR	6		/* other errors (see sense bytes) */
/* these should be put into an include file.... */

extern long tcerror;		/* tc error value */

/*
 * tcstrategy - Attempt to do more efficient physical io directly into the
 * user buffer. We can do this if the io is a multiple of pages, and
 * begins on a page boundry.
 */
#undef DEBUG
tcstrategy(io,func)
register struct iob *io;
{
	register int pages,cc,np;
	register char *ma;

	cc = io->i_cc;
	ma = io->i_ma;
#ifdef DEBUG
	printf("tcio: %d -> %d\n",cc,ma);
#endif
	if ((cc % PGSIZE) == 0 && ((int)ma & PGOFSET) == 0) {
		pages = cc / PGSIZE;
		np = dotapeio(func==READ?TCREAD:TCWRITE, pages, ma);
		if (np != pages) {
			cc = np * PGSIZE;
			if (tcerror != EOTERROR && tcerror != EOFERROR){
				printf("tcstrategy: error %d\n",tcerror);
				cc = -1;
			}
		}
	} else {
		return(tcbufio(io,func));
	}
	return(cc);
}

/*
 * 	tcbufio - user buffer not page aligned or a mulitple of pages,
 *	so we must do buffered io.
 */

tcbufio(io,func)
struct iob *io;
{
	char *bufptr;		/* pointer into buffer area for tape IO */
	register char *curptr;	/* pointer into user's area */
	register int totalpages;/* total number of pages to transfer */
	register int npages;	/* number of pages to do IO for this loop */
	register int np;	/* number of pages done for this loop */
	register int cc;	/* nbytes done for this loop */
	char tapebuf[PGSIZE*MAXSIZE+PGOFSET]; /* data area for tape IO */


	totalpages = (io->i_cc + PGOFSET) / PGSIZE;
	curptr = io->i_ma;
	/* page align the buffer pointer */
	bufptr = (char *)((((int)tapebuf) + PGOFSET) & ~PGOFSET);

#ifdef DEBUG
	{
	char *f;
	f = (func==READ)?"READ":"WRITE";
	printf("TC: buffered %s %d bytes\n",f,io->i_cc);
	}
#endif

	switch(func) {

	case READ:
		while (totalpages > 0) {
			npages = totalpages;
			if (npages > MAXSIZE) npages = MAXSIZE;
      			np = dotapeio(TCREAD, npages, bufptr);
			totalpages -= np;
			cc = np * PGSIZE;
			if (totalpages == 0) {
				/* last transfer, may not be a page amount */
				cc -= (PGSIZE - (io->i_cc % PGSIZE));
			}
			iomove(curptr,bufptr,cc);
			curptr += cc;
			if (np != npages){
			    totalpages = 0;
			    if (tcerror != EOTERROR && tcerror != EOFERROR){
				    printf("tcstrategy: error %d\n",tcerror);
				    return(-1);
			    }
			}
		}
		break;
	case WRITE:
		printf("tcstrat: writing %d\n",totalpages);
		if (io->i_cc & PGOFSET)
			_stop("tcstrategy - not writing even page amount");
		while (totalpages > 0) {
			npages = totalpages;
			if (npages > MAXSIZE) npages = MAXSIZE;
			cc = npages * PGSIZE;
			iomove(bufptr, curptr, cc);
      			np = dotapeio(TCWRITE, npages, bufptr);
			totalpages -= np;
			curptr += np * PGSIZE;
			if (np != npages) {
				totalpages = 0;
			}
		}
		break;
	default:
		_stop("tcstrategy - func not READ or WRITE");
	}
	return(curptr - io->i_ma);
}

tcopen(io)
register struct iob *io;
{
	register int error;

	if (io->i_unit)
   		_stop("No such tape unit");	/* Only have one unit */
	if (error = inittcunit()){
   		printf("tcopen -- inittcunit returned error %d\n",error);
   		_stop(0);
	}
	dorewind();
	while (io->i_boff-- > 0) {
   		if ((error = doskipfile()) != NOERROR) {
   			if ((error == EOTERROR) && io->i_boff)
				_stop("Premature end of tape");
   			if (error != EOTERROR)
				_stop("Error skipping file");
		}
	}
}

tcclose(io)
struct iob *io;
{
	return(dorewind());
}
