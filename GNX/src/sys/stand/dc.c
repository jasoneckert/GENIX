/*
 * dc.c: version 2.17 of 10/10/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)dc.c	2.17 (NSC) 10/10/83";
# endif

/*  device driver for winchester disk  */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/disk.h>
#include <sys/dcusaio.h>
#include <sys/saio.h>

#define	DCREAD 0
#define	DCWRITE 1
#define	MAXSIZE 16		/* maximum sectors in one io */

extern long dcerror;

/*
 * dcstrategy - Attempt to do more efficient physical io directly into the
 * user buffer. We can do this if the io is a multiple of pages, and
 * begins on a page boundry.
 */

#undef DEBUG
dcstrategy(io,func)
register struct iob *io;
{
	register int pages,np,cc;
	register char *ma;

	cc = io->i_cc;
	ma = io->i_ma;
#ifdef DEBUG
	{
	char f;
	f = (func==READ)?'R':'W';
	printf("dc%c:%d: %d -> %d",f,cc,ma,io->i_bn);
	}
#endif
	pages = (cc + PGOFSET) / PGSIZE;
	if (io->i_bn < io->i_boff || (io->i_bn + pages) > io->i_cyloff){
		printf("bk# %d+%d, lo %d hi %d\n",io->i_bn,
		pages, io->i_boff, io->i_cyloff);
   		_stop("dcstrategy - I/O out of filesystem boundaries");
	}
	if ((cc % PGSIZE) == 0 && ((int)ma & PGOFSET) == 0) {
#ifdef DEBUG
		printf("\n");
#endif
		np = dodiskio(func==READ?DCREAD:DCWRITE, io->i_unit,
			io->i_bn, pages, ma);
		if (np != pages) {
			printf("dcstrategy: error %d\n",dcerror);
			cc = -1;
		}
	} else {
#ifdef DEBUG
		printf("*\n");
#endif
		return(dcbufio(io,func));
	}
	return(cc);
}

dcbufio(io,func)
register struct iob *io;
{
	char *bufptr;		/* pointer into buffer area for disk IO */
	char *curptr;		/* pointer into user's area */
	register int npages;	/* number of sectors to do IO for this loop */
	register int np;	/* number of sectors done this loop */
	register int cc;	/* number of bytes done this loop */
	register int sectorsdone; /* number of sectors completed */
	int totalsectors;	/* number of sectors to transfer */
	char diskbuf[PGSIZE*MAXSIZE+PGOFSET]; /* data area for disk IO */

	totalsectors = (io->i_cc + PGOFSET) / PGSIZE;
	sectorsdone = 0;
	curptr = io->i_ma;
	/* page align the buffer pointer */
	bufptr = (char *)((((int)diskbuf) + PGOFSET) & ~PGOFSET);

	switch(func) {

	case READ:
		while (sectorsdone < totalsectors) {
			npages = totalsectors - sectorsdone;
			if (npages > MAXSIZE) npages = MAXSIZE;
			np = dodiskio(DCREAD, io->i_unit,
				io->i_bn + sectorsdone, npages, bufptr);
			sectorsdone += np;
			cc = np * PGSIZE;
			if (sectorsdone == totalsectors) {
				/* last transfer, may not be a page amount */
				cc -= (PGSIZE - (io->i_cc % PGSIZE));
			}
			iomove(curptr,bufptr,cc);
			curptr += cc;
			if (np != npages) {
			    printf("dcstrategy - error %d, sectorsdone = %d\n",
				    dcerror,sectorsdone);
			    return(-1);
			}
		}
		break;
	case WRITE:
		if (io->i_cc & PGOFSET)
			_stop("dcstrategy - not writing even page amount");
		while (sectorsdone < totalsectors) {
			npages = totalsectors - sectorsdone;
			if (npages > MAXSIZE) npages = MAXSIZE;
			iomove(bufptr, curptr, npages * PGSIZE);
			np = dodiskio(DCWRITE, io->i_unit,
				     io->i_bn + sectorsdone, npages, bufptr);
			if (np != npages) return(-1);
			curptr += np * PGSIZE;
			sectorsdone += np;
		}
		break;
	default:
		_stop("dcstrategy - func not READ or WRITE");
	}
	return(curptr - io->i_ma);
}


dcopen(io)
register struct iob *io;
{
	register struct disktab *dt;
	register int error,retries;

	if (io->i_boff < 0 || io->i_boff >= MAXUNITS)
		_stop("Bad offset");
	retries = 0;
	while ((error = initunit(io->i_unit)) == DCUERROR) {
		if ((++retries % 10) == 0) {
			printf("Can't find unit %d\n",io->i_unit);
			break;
		}
	}
	if (error){
		printf("dcopen - initunit returned error %d\n",error);
		_stop(0);
	}
	/* Compute this filesystem boundries */
	dt = unittable[io->i_unit];
	io->i_cyloff = dt->dt_partition[io->i_boff].nblocks;
	io->i_boff = dt->dt_partition[io->i_boff].cyloff;
	io->i_boff *= dt->dt_secpercyl;		/* lower block boundry */
	io->i_cyloff += io->i_boff;		/* upper block boundry */
}
