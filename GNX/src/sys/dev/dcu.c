/*
 * dcu.c: version 3.26 of 11/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)dcu.c	3.26 (NSC) 11/8/83";
# endif


/*	dcu.c	Mesa 16000 DCU driver for disks		82/04/24	*/
/*							David I. Bell	*/

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/disk.h"
#include "../h/dcu.h"
#include "../h/vm.h"
#include "../h/devvm.h"

#ifndef	NPORT
#define	NPORT		1	/* number of ports allowed */
#endif
#define	NDCU		8	/* number of disk units allowed */
#define	RETRIES		10	/* number of retries before giving up */
#define	MAXTRANSFER	128	/* maximum sectors allowed in one IOCB */
#define	ERRORNONE	0	/* no error occurred */
#define	ERRORRETRY	1	/* error in progress, retry the operation */
#define	ERRORFAILED	2	/* error recovery done with failure */
#define	ERRORFIXED	3	/* error recovery done successfully */
#define	ERRORECCFIXED	4	/* error recovery done by using ECC */
#define	BURSTVALUE	0x007	/* interleave of 0, burst value of 7 */
#define	GETB(d,i) (((unsigned char *)(&d))[i])	/* to reference data as chars */
#define INT(d)	((int)(d))	/* make something an integer */
#define	STAT(p,s) (((s) >> (p)->ph_statshift) & DCUSTATMASK)	/* get status */
#define b_cylin	b_resid		/* cylinder number for doing IO to */
				/* shares an entry in the buf struct */

/*
 * The structure of a disk drive.
 */
struct	disk {
	struct	disk	*dcu_forw;		/* next drive in active queue */
	struct	port	*dcu_port;		/* port disk is connected o */
	dcdata	dcu_dd;				/* device configuration data */
	unsigned char	dcu_unit;		/* unit number to the system */
	unsigned char	dcu_exists;		/* nonzero if disk exists */
	struct	buf	dcu_buffers;		/* head of queue for disk */
};



/*
 * The status of a disk controller, which keeps track of whether or not
 * the port is active, and which drives want to do IO.
 */
struct	port {
	struct	disk *ph_actf;			/* head of queue to transfer */
	struct	disk *ph_actl;			/* tail of queue to transfer */
	unsigned char	*ph_cmdadr;		/* command address */
	unsigned char	*ph_ackadr;		/* acknowledge address */
	char	ph_isact;			/* set if channel is active */
	char	ph_conn;			/* number of units connected */
	char	ph_statshift;			/* shift to extract status */
	char	ph_cmdbit;			/* command bit value */
	char	ph_retries;			/* current error retry count */
	int	ph_errstat;			/* last error status */
	int	ph_errsector;			/* last error sector */
	struct	dcu_iocb ph_iocb;		/* IOCB for channel */
	long	ph_pagtab[MAXTRANSFER];		/* page table for channel */
};


extern	long	dummy;			/* to keep optimizer happy */
struct	port	dcuports[NPORT];	/* table of ports */
struct	disk	dcudrives[NDCU];	/* table of units */
struct	buf	rdcubuf[NDCU];		/* buffers for raw IO */
long	dc_xfer[NDCU];			/* count of sectors transfered */
char	dcuinited;			/* dcu initialized */
int	minphys();			/* routine for physio */
int	nulldev();			/* force optimizer to keep references */
int	printf();			/* printf routines */
int	lprintf();

/* Read/write routine for a buffer.  Finds the proper unit, range checks
 * arguments, and schedules the transfer.  Does not wait for the transfer
 * to complete.  Multi-page transfers are supported.  All I/O requests must
 * be page aligned, and be a multiple of a page in length.
 */
dcustrategy(bp)
	register struct buf *bp;	/* IO operation to perform */
{
	register struct disk *du;	/* disk unit to do the IO */
	register dcdata *dd;		/* device configuration data */
	long nblocks, cyloff, blknum, blkcnt;
	int unit, xunit;

	unit = dkunit(bp);
	xunit = minor(bp->b_dev) & 7;
	if ((unit >= NDCU) || (bp->b_blkno < 0) || (bp->b_bcount & PGOFSET))
		goto bad;
	if ((INT(bp->b_un.b_addr) & PGOFSET) && (bp->b_flags & B_PHYSMAP == 0))
		goto bad;
	du = &dcudrives[unit];
	if ((du->dcu_exists) == 0) goto bad;
	dd = &du->dcu_dd;
	blknum = dkblock(bp);
	blkcnt = (bp->b_bcount) >> PGSHIFT;
	nblocks = dd->dd_partition[xunit].nblocks;
	cyloff = dd->dd_partition[xunit].cyloff;
	if (blknum + blkcnt > nblocks) goto bad;
	bp->b_cylin = (blknum / dd->d_secpercyl) + cyloff;
	(void) spl5();
	disksort(&du->dcu_buffers, bp);
	dcuqueueio(du);			/* start IO if drive is idle */
	(void) spl0();
	return;

bad:	bp->b_flags |= B_ERROR;
	iodone(bp);
}

/* Routine to queue a read or write command to a port.  The request is
 * linked into the active list for one of the ports.  If the port is idle,
 * the transfer is started.
 */
dcuqueueio(du)
	register struct disk *du;		/* disk to start IO for */
{
	register struct buf *bp;		/* IO transfer to perform */
	register struct port *ph;		/* port header structure */
	register struct	port *phsrc;		/* for port search */

	if (du->dcu_buffers.b_active)
		return;
	bp = du->dcu_buffers.b_actf;		/* get next transfer if any */
	if (bp == NULL)
		return;	
	ph = du->dcu_port;
	if (ph == NULL) {			/* assign most unused port */
		ph = dcuports;
		for (phsrc = &dcuports[NPORT-1]; phsrc > ph; phsrc--) {
			if (phsrc->ph_conn < ph->ph_conn) ph = phsrc;
		}
		ph->ph_conn++;
		du->dcu_port = ph;
	}
	du->dcu_forw = NULL;
	if (ph->ph_actf  == NULL)		/* link unit into active list */
		ph->ph_actf = du;
	else
		ph->ph_actl->dcu_forw = du;
	ph->ph_actl = du;
	du->dcu_buffers.b_active = 1;		/* mark the drive as busy */
	if (ph->ph_isact == 0) dcustart(ph->ph_actf, 0);  /* start IO if idle */
}

/* Channel startup routine for the DCU.  This builds an IOCB, and starts
 * a multiple-sector read or write operation.  The skip parameter is used
 * to start the transfer after a given number of pages, so that error
 * recovery can restart at the sector which was in error.  Transfers cannot
 * cross cylinder boundaries, or be too large in other ways, so this routine
 * doesn't always start IO for the whole transfer.  It is up to the interrupt
 * routine to notice this case and restart the IO for the remaining sectors.
 * RESTRICTIONS:
 * 1.	The transfer length must be an exact multiple of the page size.
 * 2.	The transfer must begin on a page boundary.
 */

dcustart(du, skip)
	register struct disk *du;	/* disk unit for IO */
{
	register struct dcu_iocb *io;	/* pointer to IOCB being used */
	register struct buf *bp;	/* buffer IO is for */
	register long *pt;		/* pointer to table of page addresses */
	register struct	port *ph;	/* port header */
	long	opcode, blknum, blkcnt, cylin, head, sector;
	long	secpertrk, secpercyl, addr, i, isphys, isphysmap;

	ph = du->dcu_port;
	if ((ph == NULL) || (ph->ph_isact)) panic("dcustart");
	io = &ph->ph_iocb;
	pt = ph->ph_pagtab;
	bp = du->dcu_buffers.b_actf;
	isphys = bp->b_flags & (B_PHYS | B_PHYSMAP);
	isphysmap = bp->b_flags & B_PHYSMAP;
	if (bp->b_flags & B_READ) opcode = DCUC_READ; else opcode = DCUC_WRITE;
	secpertrk = du->dcu_dd.d_nsectors;
	secpercyl = du->dcu_dd.d_secpercyl;
	blknum = bp->b_blkno + skip;
	cylin = bp->b_cylin + (blknum / secpercyl) - (bp->b_blkno / secpercyl);
	head = (blknum % secpercyl) / secpertrk;
	sector = blknum % secpertrk;
	blkcnt = ((bp->b_bcount) >> PGSHIFT) - skip;
	if (blkcnt <= 0) panic("dcubadblkcnt");
	if (ph->ph_retries) blkcnt = 1;		/* doing error recovery */
	if (blkcnt != 1) {
		if (blkcnt > MAXTRANSFER) blkcnt = MAXTRANSFER;
		if ((blknum + blkcnt - 1) / secpercyl != blknum / secpercyl)
			blkcnt = secpercyl - (blknum % secpercyl);
				/* keep transfer within current cylinder */
	}

	addr = INT(bp->b_un.b_addr) + (skip << PGSHIFT);
	for (i = 0; i < blkcnt; i++) {	/* fill in addresses of memory pages */
		if (isphys == 0) {	/* normal buffer IO */
			pt[i] = addr;
			addr += NBPG;
			continue;
		}
		if (isphysmap) {	/* page map style IO for swapping */
			pt[i] = ((long *)bp->b_un.b_addr)[skip + i];
			continue;
		}
					/* normal physio call for raw IO */
		pt[i] = physaddr(bp->b_spti, addr);
		if (pt[i] < 0) panic("dcubadmem");
		addr += NBPG;
	}
	while ((*DCUA_STATUS & DCUS_READY) == 0) nulldev();/* check status */
	if (STAT(ph,*DCUA_STATUS) != DCUSTS_IDLE) panic("dcunotidle");
	io->dcuio_opcode = opcode;	/* set read or write opcode */
	io->dcuio_count = blkcnt;	/* set sector count */
	io->dcuio_resid = blkcnt;	/* here also in case sick port */
	io->dcuio_cylinder = cylin;	/* set cylinder */
	io->dcuio_drhdsec = DCUD_DHS(du->dcu_unit, head, sector);
					/* set drive, head, and sector */
	io->dcuio_buffer = INT(pt);	/* set page table address */
	io->dcuio_status = 0;		/* clear status */
	io->dcuio_sense = 0;		/* clear sense bytes */
	io->dcuio_ecc = 0;		/* clear ecc data */
	io->dcuio_chain = 0;		/* set no chaining */
	io->dcuio_skip = skip;		/* remember skip count */
	ph->ph_isact = 1;		/* mark channel active */

/* The following offset by 2 kludge exists because the dcu_iocb structure
 * begins with two bytes of data not used by the controller.  The placement
 * of that data was necessary so that C would not misalign the offset of the
 * fields.  Such a `feature'!!
 */
	dcucommand(ph, DCUC_START, INT(io)+2);	/* start IO */

	/* Now the DCU will interrupt will finished. */
}

/* Interrupt routine for the ports.  Acknowledge the interrupt, check for
 * errors on the current operation, mark it done if necessary, and start
 * the next request.  Also check for a partially done transfer, and
 * continue with the next chunk if so.
 */
dcuintr()
{
	register struct port *ph;	/* port header */
	register struct	disk *du;	/* active disk unit */
	register char	code;		/* status code */
	register char	status;		/* status of port */

	if (!dcuinited) return;		/* if initializing, ignore interrupt */
	while (1) {			/* wait till steady state */
		status = *DCUA_STATUS;
		if (status & DCUS_READY) break;
		nulldev();		/* to preserve references */
	}
	/*
	 * Loop over all ports
	 */
	for (ph = dcuports; ph < &dcuports[NPORT]; ph++) {
		code = STAT(ph,status);			/* extract code */
		if (code == DCUSTS_BUSY) continue;	/* busy, continue */
		du = ph->ph_actf;
		if (code == DCUSTS_IDLE) {
			if (du == NULL) continue;	/* obscure, ignore it */
			panic("dcuidle");
		}
		*ph->ph_ackadr = '\0';		/* acknowledge the interrupt */
		if (du == NULL) panic("dcunullunit");
		dcuintrunit(du, (code != DCUSTS_DONE));
	}
}

/* Here for an interrupt for the active unit of a port.  Err is nonzero
 * if the operation terminated with an error.
 */
dcuintrunit(du, err)
	register struct	disk *du;	/* unit which got interrupt */
{
	register struct buf *bp;	/* active transfer */
	register struct port *ph;	/* port header */
	register struct dcu_iocb *io;	/* IOCB block */
	register int	errcode;	/* error status */
	int	blkcnt;			/* sectors successfully transfered */
	int	seccyl;			/* sectors per cylinder */
	int	(*pr)();		/* print routine */

	ph = du->dcu_port;
	if (ph == NULL) panic("dcuintrunit");
	io = &ph->ph_iocb;
	bp = du->dcu_buffers.b_actf;
	blkcnt = io->dcuio_count;
	errcode = ERRORFIXED;
	if (ph->ph_retries == 0) errcode = ERRORNONE;
	if (err) {
		blkcnt -= io->dcuio_resid;	/* reduce by sectors left */
		if (blkcnt > 0) blkcnt--;	/* 1 more due to ambiguity */
		seccyl = du->dcu_dd.d_secpercyl;
		ph->ph_errstat = io->dcuio_sense;	/* save error status */
		ph->ph_errsector = (bp->b_cylin * seccyl) +
			(bp->b_blkno % seccyl) + blkcnt;
		errcode = dcuhandleerror(du);
		switch (errcode) {
			case ERRORRETRY:
				if (ph->ph_retries++ <= RETRIES) break;
				errcode = ERRORFAILED;
				/* fall into failure case if no more retries */

			case ERRORFAILED:
				bp->b_flags |= B_ERROR;	/* flag the error */
				blkcnt++;		/* count bad sector */
				break;

			case ERRORECCFIXED:
				blkcnt = io->dcuio_count - io->dcuio_resid;
				/* For ECCs, the residue was decremented */
				/* fall into the fixed case */

			case ERRORFIXED:
				break;
		}
	}

/* This piece of the transfer is complete (maybe with an error).  If there
 * was no error and there is more to be transfered, start the next piece.
 * Otherwise, terminate IO for this request and try to start a new one.
 * Hard errors are logged and described on the console.  Soft errors are
 * only logged.
 */

	ph->ph_isact = 0;
	io->dcuio_skip += blkcnt;	/* add to successful sectors */
	dc_xfer[du->dcu_unit] += blkcnt;
	if ((errcode != ERRORNONE) && (errcode != ERRORRETRY)) {
		pr = lprintf;
		if (errcode == ERRORFAILED) pr = printf;
		pr("%s disk %s error: unit %d sector %d status %x retries %d\n",
			(errcode == ERRORFAILED) ? "Hard" : "Soft",
			(bp->b_flags & B_READ) ? "read" : "write",
			du->dcu_unit, ph->ph_errsector,
			ph->ph_errstat, ph->ph_retries);
	}
	if (errcode != ERRORFAILED) {		/* see if more to transfer */
		if (errcode != ERRORRETRY) ph->ph_retries = 0;
		if (io->dcuio_skip < (bp->b_bcount >> PGSHIFT)) {
			dcustart(du, io->dcuio_skip);
			return;		/* next chunk is started */
		}
	}

	/* All of this buffer is now complete, with or without errors. */

	ph->ph_actf = du->dcu_forw;
	ph->ph_conn--;
	ph->ph_retries = 0;
	du->dcu_port = NULL;
	du->dcu_buffers.b_active = 0;
	du->dcu_buffers.b_actf = bp->av_forw;
	du->dcu_buffers.b_errcnt = 0;
	bp->b_resid = 0;
	iodone(bp);			/* THIS MIGHT START SWAPPING I/O */
	if (du->dcu_buffers.b_actf) {
		dcuqueueio(du);		/* requeue disk if more io to do */
		return;
	}
	if (ph->ph_isact)		/* done if swapint already started io */
		return;

	while (du = ph->ph_actf) {	/* look at requests from other drives */
		if (du->dcu_buffers.b_actf) {
			dcustart(du, 0);	/* start IO on drive */
			return;
		}
		ph->ph_actf = du->dcu_forw;	/* skip null request */
	}
}

/* Error handling routine for the disk.  Deciphers the error, and indicates
 * the appropriate action.  If the error was an ECC, it is corrected.
 * Returns one of ERRORRETRY, ERRORFIXED, ERRORECCFIXED, or ERRORFAILED.
 */

dcuhandleerror(du)
	register struct	disk *du;	/* active disk unit */
{
	register struct buf *bp;	/* buffer in error */
	register struct	dcu_iocb *io;	/* IOCB for transfer */
	register unsigned long sb;	/* sense bytes 0 - 3 */
	long eccadr;			/* memory address of ecc corrections */

	bp = du->dcu_buffers.b_actf;
	io = &du->dcu_port->ph_iocb;
	sb = io->dcuio_sense;		/* sense bytes stored by DCU */

	if (sb & DCUSB_NOTREADY) {
		return (ERRORFAILED);	/* offline */
	}

	if (sb & (DCUSB_BUSFAULT | DCUSB_BUSERROR)) {
		return (ERRORRETRY);	/* random bus type errors, retry */
	}

	if (sb & (DCUSB_ILLCMMD | DCUSB_ILLPARAM)) panic("dcubadcmd1");

	if ((sb & DCUSB_RWFAULT) && (sb & DCUSB_REJECT)) {
		printf("Write locked drive %d\n", du->dcu_unit);
		return (ERRORFAILED);	/* drive is write locked */
	}

	if (sb & (DCUSB_SEQERR | DCUSB_SEEKERR | DCUSB_RWFAULT |
		DCUSB_POWER | DCUSB_VIOLATION | DCUSB_SPEEDERR |
		DCUSB_REJECT | DCUSB_OTHERERR | DCUSB_VENDORERR)) {
		return (ERRORRETRY);	/* random type drive errors */
	}

	switch (CTLFAULTCODE(sb)) {		/* get drive fault code */

		case DCUCF_ECCDATA:
			eccadr = io->dcuio_count - io->dcuio_resid - 1;
			if (eccadr < 0) return (ERRORRETRY);
			eccadr = ((long *) io->dcuio_buffer)[eccadr] &~ PGOFSET;
			if ((bp->b_flags & B_READ) == 0 ||
				dcuecc(io->dcuio_ecc, eccadr)) {
					return (ERRORRETRY);	/* ECC failed */
			}
			return (ERRORECCFIXED);	/* successful ECC fix!! */
		case DCUCF_OCCUPIED:
		case DCUCF_NOSECTOR:
		case DCUCF_ECCHEADER:
		case DCUCF_MEMERROR:
		case DCUCF_SYNCERROR:
			return (ERRORRETRY);	/* random errors */
		case DCUCF_DRIVEERR:
			return (ERRORFAILED);	/* nonexistent drive */
		case DCUCF_ILLPARAM:
		case DCUCF_ILLCMMD:
			panic("dcubadcmd2");
	}
	return (ERRORRETRY);			/* retry unknown errors */
}

/* ECC routine for the disk.  The following routine takes the four ECC bytes
 * from the sense bytes, and uses them to generate a mask and offset, and
 * performs an XOR to correct the data in the buffer. The algorithm is
 * from the article "Error Correction Method for Disk" (September 26, 1980),
 * Volume 1, page 12", by Data Systems Technology Corp, Broomfield, Col.
 */

#define RECIP	0xa8a06080		/* reciprocal polynomial bits */
#define	R1	0xff000000		/* syndrome bits x0 - x7 */
#define	R2	0x00ff0000		/* syndrome bits x8 - x15 */
#define	R3	0x0000ff00		/* syndrome bits x16 - 23 */
#define	R4	0x000000ff		/* syndrome bits x23 - x31 */
#define	R3LOW5	0x00001f00		/* low order 5 bits of R3 */
#define R4LOW1	0x00000001		/* low order bit of R4 */


dcuecc(ecc, data)
	register unsigned char *data;	/* pointer to data buffer */
	register unsigned long ecc;	/* ecc data for correction */
{
	register unsigned char *addr;	/* address of byte to ECC correct */
	register j = 517 * 8 - 25;	/* bit counter for loop */
					/* 517 = 1 sync + 4 ecc + 512 data */
	register af = 0;		/* alignment flag */

	if (ecc == 0) return(1);	/* zero ECC should not occur */

	ecc = ((ecc&R1)>>24) | ((ecc&R2)>>8) | ((ecc&R3)<<8) | ((ecc&R4)<<24);
					/* reverse order of the bytes */

	while ((ecc & R1) == 0) {	/* normalize so R1 is nonzero */
		j += 8;
		ecc <<= 8;
	}

	do {				/* loop over all bit positions */
		if (ecc & R4LOW1) {		/* shift R1-R4 right 1 bit */
			ecc >>= 1;
			ecc ^= RECIP;		/* XOR with reciprocal poly */
		} else ecc >>= 1;		/* bypass XOR if R4low is 0 */
		if (ecc & R1) continue;		/* test R1 */
		if (af == 0) {			/* test align flag */
			if (ecc & (R4|R3LOW5))	/* test R4, 5 low order of R3 */
				continue;
			af = 1;			/* set align flag = 1 */
		}
		if ((j & 7) == 0) goto goodecc;	/* test j modulo 8 */
	} while (j-- > 0);			/* test j, set j = j - 1 */
	return(1);				/* failed */

/* Here if the data is correctable.  J contains the bit offset in the data
 * buffer of where to apply the ECC fix, and the mask to XOR with is in
 * R2, R3, and R4.  (Also, j is always a multiple of 8, and R2 is the lowest
 * address byte).  The data record referenced by the ECC data consists of
 * one sync byte, 512 data bytes, and 4 ECC bytes, so therefore the data
 * in memory begins at byte offset 1 and ends with byte offset 512.  The
 * XORs to be performed must therefore be range checked before use.
 */

goodecc:
	j >>= 3;				/* get byte offset in sector */
	addr = data + j - 1;			/* point to memory address */
	if ((j > 0) && (addr < data + NBPG))
		*addr ^= ((ecc & R2) >> 16);	/* fix byte using R2 */
	addr++;
	if (addr < data + NBPG)
		*addr ^= ((ecc & R3) >> 8);	/* fix byte using R3 */
	addr++;
	if ((j > 0) && (addr < data + NBPG))
		*addr ^= (ecc & R4);		/* fix byte using r4 */
	return(0);
}

/* Routine to write a command to a port.  The command is four bytes long,
 * made up of three bytes of data and one byte of command.  On the 16000,
 * the command must be written out a single byte at a time, because the disk
 * port is too stupid to handle word or double word references.  The routine
 * waits for the port to become ready before writing to it (which supposedly
 * occurs quickly, even if a transfer operation is started).
 */
dcucommand(ph, opcode, data)
	struct	port	*ph;		/* port to write to */
{
	register unsigned char *cp;	/* command bytes addr */

	cp = ph->ph_cmdadr;
	opcode |= ph->ph_cmdbit;
	while ((*DCUA_STATUS & DCUS_READY) == 0) nulldev();/* wait till ready */
	cp[0] = GETB(data,0);		/* write data byte 0 to port */
	cp[2] = GETB(data,1);		/* write data byte 1 */
	cp[4] = GETB(data,2);		/* write data byte 2 */
	cp[6] = opcode;			/* write command byte */
}



/* Wait until the dcu has completed a command, and return whether or not
 * the operation was successful.  This routine depends on the fact that
 * dcuintr ignores interrupts while the dcuinited flag is false, since
 * an interrupt is generated when the operation completes.
 */
dcuwait(ph)
	register struct	port	*ph;	/* port to wait for */
{
	register char	stat;		/* current status */
	register char	code;		/* current code */
	register int	count;		/* timeout counter */

	count = 1000000;
	while (1) {
		stat = *DCUA_STATUS;
		code = STAT(ph, stat);
		if ((stat & DCUS_READY) && (code != DCUSTS_BUSY)) break;
		if (count-- <= 0) panic("dcutimeout");
	}
	if (code != DCUSTS_IDLE) *ph->ph_ackadr = '\0'; /* ack the command */
	return (code != DCUSTS_DONE);	/* return error value */
}

/* Initialize the DCU, and find out what units are available.  This routine
 * allows interrupts while running, and depends on dcuintr to ignore the
 * disk interrupts because dcuinited is zero.
 */
dcuopen(dev, rw)
	dev_t	dev;
	int	rw;
{
	register struct	port	*ph;		/* pointer to port */
	register struct	disk	*du;		/* pointer to unit */
	register struct dcu_iocb *io;		/* pointer to iocb */
	register struct disktab *dt;		/* pointer to buffer */
	register int	unit;				/* unit number */
	int	dcucount;			/* number of units found */

	if (dcuinited) return;			/* only do this once */

	/* Initialize the ports */
#if (NPORT > 1)
	ph = &dcuports[1];
	ph->ph_cmdadr = DCUA_CHAN1;
	ph->ph_ackadr = DCUA_ACK1;
	ph->ph_statshift = DCUSTATSH1;
	ph->ph_cmdbit = 1;
#endif
	ph = dcuports;
	ph->ph_cmdadr = DCUA_CHAN0;
	ph->ph_ackadr = DCUA_ACK0;
	ph->ph_statshift = DCUSTATSH0;
	ph->ph_cmdbit = 0;
	io = &ph->ph_iocb;
	dummy += *DCUA_SETDAFF;			/* read DAFF to allow writes */
	dcuwait(ph);
	dcucommand(ph, DCUC_SETBURST, BURSTVALUE); /* set the burst value */
	if (dcuwait(ph)) panic("dcunoburst");

	/* Loop over all possible units, seeing which ones respond to
	 * the write-enable command.  Those units which fail this are
	 * assumed to be nonexistant.  A 1K page is allocated and locked,
	 * and block 0 of the disk is read to determine drive parameters
	 * and partition information.
	 */

	dt = (struct disktab *)kallocpage(1);	/* allocate block 0 buffer */
	ph->ph_pagtab[0] = physaddr(SYSSPTI,dt);
	ph->ph_actf = NULL;
	dcucount = 0;
	for (unit=0; unit<NDCU; unit++) {
		dcucommand(ph, DCUC_ASSDRIVE, DCUD_DRIVE(unit));
		if (dcuwait(ph)) continue;
		dcucommand(ph, DCUC_SPECIFIC, DCUD_ENABLE(1));
		if (dcuwait(ph)) continue;	/* failed, must not exist */
				/* Read block 0 off the drive */
		du = &dcudrives[unit];
		io->dcuio_opcode = DCUC_READ;	/* set read opcode */
		io->dcuio_count = 1;		/* set sector count */
		io->dcuio_cylinder = 0;		/* set cylinder */
						/* set drive, head, sector */
		io->dcuio_drhdsec = DCUD_DHS(unit, 0, 0);
		io->dcuio_buffer = INT(ph->ph_pagtab);
		io->dcuio_chain = 0;		/* set no chaining */

		dcucommand(ph, DCUC_START, INT(io)+2);	/* start IO */
		if (dcuwait(ph)) {
			printf("Header read error on unit %d\n", unit);
			continue;
		}
		if (dt->dt_magic != DMAGIC) {
			printf("Unknown disk type on unit %d\n", unit);
			continue;
		}
		dt->dt_magic = 0;		/* clear the magic # */
		du->dcu_dd = dt->dt_dd;		/* Init device data */
		du->dcu_exists = 1;
		du->dcu_unit = unit;
		du->dcu_buffers.b_active = 0;
		du->dcu_buffers.b_actf = NULL;
		dcucount++;
	}
	kfreepage(dt);
	if (dcucount == 0) panic("dcunounits");		/* no drives! */
	dcuinited = 1;			/* all done */
}

/* Routines to do raw IO for a unit.
 */
dcuread(dev)			/* character read routine */
	dev_t dev;
{
	int unit = minor(dev) >> 3;

	if (unit >= NDCU)
		u.u_error = ENXIO;
	else
		physio(dcustrategy, &rdcubuf[unit], dev, B_READ, minphys);
}


dcuwrite(dev)			/* character write routine */
	dev_t dev;
{
	register int unit = minor(dev) >> 3;

	if (unit >= NDCU)
		u.u_error = ENXIO;
	else
		physio(dcustrategy, &rdcubuf[unit], dev, B_WRITE, minphys);
}


dcudump(dev)			/* dump core after a system crash */
	dev_t dev;
{
	register struct dcu_iocb *io;	/* pointer to IOCB being used */
	register long *pt;		/* pointer to table of page addresses */
	register struct	port *ph;	/* port header */
	register struct disk *du;	/* disk unit to do the IO */
	register dcdata	*dd;		/* pointer to device data */
	long num;			/* number of sectors to write */
	long start;
	int unit, xunit;
	long cyloff, blknum, blkcnt;
	long	cylin, head, sector;
	long	secpertrk, secpercyl, addr, i;
	char	stat;		/* current status */
	char	code;		/* current code */
	extern int dumplo, totalpages;

	start = 0;
	num = totalpages * MCPAGES;
	unit = minor(dev) >> 3;
	xunit = minor(dev) & 7;
	if (unit >= NDCU) return(ENXIO);
	du = &dcudrives[unit];
	if ((du->dcu_exists) == 0) return(ENXIO);
	dd = &du->dcu_dd;
	if ((dumplo < 0) || ((dumplo + num) >= dd->dd_partition[xunit].nblocks))
		return(EINVAL);
	ph = dcuports;
	if (ph->ph_isact) return(EFAULT);
	io = &ph->ph_iocb;
	pt = ph->ph_pagtab;
	secpertrk = dd->d_nsectors;
	secpercyl = dd->d_secpercyl;
	cyloff = dd->dd_partition[xunit].cyloff;
	ph->ph_isact = 1;		/* mark channel active */
	while (num > 0) {
		blknum = dumplo + (start >> PGSHIFT);
		cylin = (blknum / secpercyl) + cyloff;
		head = (blknum % secpercyl) / secpertrk;
		sector = blknum % secpertrk;
		blkcnt = num;
		if (blkcnt > MAXTRANSFER) blkcnt = MAXTRANSFER;
		if ((blknum + blkcnt - 1) / secpercyl != blknum / secpercyl)
			blkcnt = secpercyl - (blknum % secpercyl);
			    /* keep transfer within current cylinder */
		addr = start;
		for (i = 0; i < blkcnt; i++) {
			pt[i] = addr;
			addr += NBPG;
		}
		while ((*DCUA_STATUS & DCUS_READY) == 0) nulldev();
		if (STAT(ph,*DCUA_STATUS) != DCUSTS_IDLE) return(EIO);
		io->dcuio_opcode = DCUC_WRITE;	/* set write opcode */
		io->dcuio_count = blkcnt;	/* set sector count */
		io->dcuio_cylinder = cylin;	/* set cylinder */
		io->dcuio_drhdsec = DCUD_DHS(du->dcu_unit, head, sector);
						/* set drive, head, sector */
		io->dcuio_buffer = INT(pt);	/* set page table address */
		io->dcuio_chain = 0;		/* set no chaining */
		dcucommand(ph, DCUC_START, INT(io)+2);	/* start IO */
		i = 1000000;
		while (1) {
			stat = *DCUA_STATUS;
			code = STAT(ph, stat);
			if ((stat & DCUS_READY) && (code != DCUSTS_BUSY)) break;
			if (i-- <= 0) return(EIO);
			nulldev();		/* to preserve references */
		}
		if (code != DCUSTS_IDLE) *ph->ph_ackadr = '\0';
							/* ack the write */
		if (code != DCUSTS_DONE) return(EIO);	/* return error value */
		num -= blkcnt;
		start += blkcnt * NBPG;
	}
	return(0);
}
