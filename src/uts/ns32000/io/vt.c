/*
 * vt.c: version 1.3 of 3/21/85
 */
#ifdef SCCS
static char *sccsid = "@(#)vt.c	1.3";
#endif

/*
 * virtual tape driver
 * copyright (c) 1984  American Information Systems Corporation
 *
 * minor device classes:
 * bits 0,1: unit select
 * bit 2 off: rewind on close; on: position after first TM
 * bits 3 and 4: density,  both off: 1600 bpi
 *			   bit 3 on:  800 bpi
 *			   bit 4 on: 6250 bpi
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/buf.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/page.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/iobuf.h"

#include "ais/viostatus.h"	/* vios definitions */
#include "ais/osiopkt.h"
#include "ais/viocmds.h"

#include "ais/vinfo.h"
#include "ais/vtape.h"

/*   #include "../h/mtio.h" MAY WANT SOMETHING LIKE THIS   */

/* special redefines for for buffer header of tape control operation */
#define	b_repcnt	b_start	/* repeat count for command */
#define b_command	b_error	/* b_error will hold command till done */

#define UNITMASK 0x03		/* bits of the minor device that specify unit */
				/* maximum of 4 tape units allowed */
#define	VTUNIT(dev) (minor(dev) & UNITMASK)

#define	T_NOREWIND	0x04		/* don't rewind tape unit flag */

/* density defaults to 1600 */
#define T_BPI1600	00		/*  density = 1600 bpi */
#define T_BPI800	10		/*  density = 800 bpi */
#define T_BPI6250	20		/*  density = 6250 bpi */


/* Software status codes for the tape.  Used to properly sequence commands. */
#define	TS_INIT	0			/* drive needs initing */
#define	TS_BOT	1			/* drive is at beginning of tape */
#define	TS_READ	2			/* drive reading data */
#define	TS_READMARK	3		/* drive looking for file mark */
#define	TS_WRITE 4			/* drive writing data */
#define	TS_WRITE_EOF	5		/* drive writing end of file mark */
#define	TS_EOT	6			/* drive is at end of tape */


#define TRUE 1
#define FALSE 0

/* to do spl's right on interrupt ack */
extern	short	intflg;

/* structures allocated in conf.c */
extern short vt_cnt;
extern struct vtdev vt_dev[];


char hostname[] = "HOST:MT?:";
char vtname[] = "MT?";
char numbers[] = "01234567";

char wrtprot[] = "tape write protected";
char drvoffl[] = "drive is offline";
char pcreate[] = "createpd";
char vcreate[] = "createvd";
char dev_info[] = "dev info";
char assign[] = "assign";



#define NVTD 2
struct	buf	rvtbuf[NVTD];		/* buffers for raw IO */



int	vtintr();			/* interrupt routine for tape device */



/* Open a tape drive.  Tapes are unique open devices, so we refuse it if
 * it is already open.  Verify that the drive is available and can be
 * written if so desired.
 */
vtopen(dev, flag)
	register dev_t	dev;		/* device number */
	int	flag;			/* nonzero if open for writes */
{
	register struct	vtdev *tu;	/* pointer to unit */
	register PKT_IOSTATUS *st;	/* pointer to vios returned status */
	register int	unit;		/* unit number */
	char	*errstr, *xerrstr;	/* error strings */
	int nxtvdd, tppdd;		/* vios descriptors */


	unit = VTUNIT(dev);		/* "minor(dev) & 0x03" */
	tu = &vt_dev[unit];
	tppdd = 0;			/* tape physical device descriptor */

	if (tu->vt_openf) {		/* tape already opened? */

#ifdef DBGMSG
		printf("tape(%x) already opened, flag=%x\n",
			unit, tu->vt_openf);	/* DEBUG */
#endif DBGMSG

		u.u_error = ENXIO;
		return;
	}

	if (unit >= vt_cnt) {		/* bad unit number? */
		u.u_error = ENXIO;
		return;
	}

	/* fix unit number in host device string */
	hostname[7] = numbers[unit];

	st = &tu->vt_stat;
	/* VIOS CALL */
	vios_iorequest( FCODE(PCTRL_FUNCTION, CREATEPD,
			    (LOOKUP_PD | (flag ? WRITE_ACCESS : READ_ONLY))),
			NULL,0,0,0,
			hostname, REC_SEQ,0,0,0,
			st,NULL,0,0);
	while(st->code == 0) nulldev();
	if (st->code < 0) {
		errstr = pcreate;
		goto fail;
	}
	tppdd = st->aux_stat2;	/* save physical device */

	/* fix unit number in virtual tape name string */
	vtname[2] = numbers[unit];

	/* VIOS CALL */
	vios_iorequest(FCODE(VCTRL_FUNCTION,CREATEVD,0),
			NULL, 0, 0, 0,
			vtname, REC_SEQ,0 ,0 ,0,
			st, NULL, 0, 0);
	while(st->code == 0) nulldev();
	if (st->code < 0) {
		errstr = vcreate;
		goto fail;
	}
	tu->vt_VDD = st->aux_stat2;

	/* VIOS CALL */
	vios_iorequest( FCODE(VCTRL_FUNCTION, ASSIGNVD, (PRI_INP | PRI_OUT)),
			tu->vt_VDD, 0, 0, 0,
			NULL, tppdd, 0, 0, 0,
			st,NULL, 0, 0);
	while(st->code == 0) nulldev();
	if (st->code < 0) {
		errstr = assign;
		goto fail;
	}

	tu->vt_openf = 1;		/* device is now open */
	tu->vt_errflg = 0;
	return;

fail:

#ifdef DBGMSG
	switch(st->code) {
	case V_NO_WRITE:
	    xerrstr = wrtprot;
	    break;
	case V_NOT_READY:
	    xerrstr = drvoffl;
	    break;
	default:
	    xerrstr = " ";
	    break;
	}
	printf("tape err: %s, %d %s\n", errstr, st->code, xerrstr);
#endif DBGMSG

	u.u_error = EIO;

	vtdealloc( tu, tppdd );		/* deallocate devices */
	tu->vt_VDD = 0;
	tu->vt_openf = 0;
}


/* Close the tape drive.  If tape was open for writing or last operation
 * was a write, then write a tape mark.  Unless this is a non-rewinding
 * special file, rewind the tape.
 */
vtclose(dev, flag)
	register dev_t	dev;		/* device number */
	register int	flag;		/* write flag */
{
	register struct	vtdev	*tu;	/* tape unit */
	register PKT_IOSTATUS	*st;


	tu = &vt_dev[VTUNIT(dev)];
	st = (PKT_IOSTATUS *)tu;

	if (flag & FWRITE) {
		/*
		 * write file mark, 
		 * position after first of two tape marks
		 */
		vtcommand(dev, WRITE_EOT, 1);
	}
	if ((dev & T_NOREWIND) == 0) {
		/*
		 * 1 count means hang waiting for rewind complete
		 * the vios will block any further opens to the device
		 * until the rewind completes
		 */
		vtcommand(dev, REWIND, 1);	/* rewind tape */
	}

	vtdealloc( tu, 0 );

	/* VIOS CALL */
	vios_iorequest( FCODE(VCTRL_FUNCTION, REMOVEVD, 0),
			tu->vt_VDD, 0, 0, 0,
			NULL, 0, 0, 0, 0,
			st,NULL, 0, 0);
	while(st->code == 0) nulldev();

	tu->vt_tab.b_active = TS_INIT;
	tu->vt_errflg = 0;
	tu->vt_VDD = 0;
	tu->vt_openf = 0;
}



/* Execute a command on the tape drive a specified number of times.  This
 * should only be called for non-data transfer operations.  If count is zero,
 * then the operation will not be waited for, and the user will wait when
 * the drive is next opened.  Returns nonzero if the operation failed.
 */
vtcommand(dev, com, count)
	dev_t	dev;
{
	register struct	vtdev	*tu;		/* tape unit */
	register struct	buf	*bp;		/* buffer used for command */
	int	err;


	tu = &vt_dev[VTUNIT(dev)];
	bp = &tu->vt_ctlbuf;

	(void) spl5();
	while (bp->b_flags & B_BUSY) {
		/*
		 * This special check is because B_BUSY never
		 * gets cleared in the non-waiting rewind case.
		 */
		 if ((bp->b_repcnt == 0) && (bp->b_flags&B_DONE))
			break;
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PRIBIO);
	}
	bp->b_flags = B_BUSY|B_READ;
	(void) spl0();
	bp->b_dev = dev;
	bp->b_repcnt = count;
	bp->b_command = com;
	bp->b_blkno = 0;
	vtstrategy(bp);

	/*
	 * If count is zero, don't wait.
	 */
	if (count == 0)
		return;

	iowait(bp);

	if (bp->b_flags & B_WANTED)
		wakeup((caddr_t)bp);
	bp->b_flags = 0;
	return;
}


/* Strategy routine for a buffer.  Finds the proper unit, range checks
 * arguments, and schedules the operation.  This is called for both data
 * transfers and for tape controlling operations (rewinds, etc).
 */
vtstrategy(bp)
	register struct buf	*bp;	/* IO operation to perform */
{
	register struct	vtdev	*tu;	/* tape unit */

	tu = &vt_dev[VTUNIT(bp->b_dev)];
	bp->av_forw = NULL;

	bp->b_resid = bp->b_bcount;

	/* if we got an error, abort further operations */
	if ( (bp != &tu->vt_ctlbuf) && tu->vt_errflg) {
		bp->b_flags |= B_ERROR;
		bp->b_error = ENXIO;
		iodone(bp);
		return;
	}

	(void) spl5();
	if (tu->vt_tab.b_actf == NULL) {
		tu->vt_tab.b_actf = bp;
	} else
		tu->vt_tab.b_actl->av_forw = bp;
	tu->vt_tab.b_actl = bp;
	if (tu->vt_tab.b_active == 0)
		vtstart(tu);
	(void) spl0();
}


/* Start a data transfer for the tape controller, or start a controlling
 * tape operation.  The choice is made by noticing which buffer was been
 * passed to us.
 * Set state of drive (in vt_tab.b_active) to the current operation.
 */
vtstart(tu)
	register struct	vtdev	*tu;	/* virtual tape device stucture */
{
	register struct	buf	*bp;	/* buffer for IO */
	char	isread;			/* nonzero if reading */
	char	isphys;			/* nonzero if physio */

	bp = tu->vt_tab.b_actf;

	if (bp == NULL)
		return;

	if (bp == &tu->vt_ctlbuf) {
		switch (bp->b_command) {
#ifdef notdef
			case TCUC_TAPEERASE:
#endif
			case UNLOAD_MEDIA:
			case REWIND:
				bp->b_repcnt = 0;	/* clear count */
				tu->vt_tab.b_active = TS_BOT;
				binval(bp->b_dev);	/* toss old data */
				break;
			case WRITE_EOT:
			case WRITE_EOF:
				tu->vt_tab.b_active = TS_WRITE_EOF;
				break;

			case SKIP_RECORDS:
			case SKIP_FILES:
				tu->vt_tab.b_active = TS_READMARK;
				break;

			default: 
				printf("bad vtcommand %x\n",bp->b_command);
		}

		/* active command, don't try to start anything else */
		tu->vt_tab.b_active = bp->b_command;

		/* VIOS CALL */
		vios_iorequest( 
		    FCODE(DEVICE_FUNCTION,VDEV_CONTROL,bp->b_command),
		    tu->vt_VDD,0,0,0,
		    NULL,bp->b_repcnt,0,0,0,
		    &tu->vt_stat,vtintr,0,0);

		return;
	}


	/* Here if this is a real live data transfer */
	if (bp->b_flags & B_READ) isread = 1;
	else isread = 0;

	tu->vt_tab.b_active = (isread ? TS_READ : TS_WRITE);

	if ( bp->b_flags & B_PHYS ) {		/* physio */
		/* 
		 * call the vios with enough information
		 * to do physio itself, i.e. a user virtual
		 * address, a size, and the user page table 
		 * to convert the virtual address to physical
		 */

		/* user virtual address for transfer */
		tu->vt_info.i[0] = (int)bp->b_un.b_addr;

		/* physical address of level 0 page table of user */
		tu->vt_info.i[1] = (int)bp->b_seg;

		/* VIOS CALL */
		tu->vt_kill = vios_iorequest(
		    FCODE( ( isread ? READ_FUNCTION : WRITE_FUNCTION ),
			   ( isread ? READ_DATA : WRITE_DATA ),
			   0),
		    tu->vt_VDD, tu->vt_info.i[0], bp->b_bcount,
		    tu->vt_info.i[1], NULL, bp->b_blkno, 0, 0, 0,
		    &(tu->vt_stat), vtintr, 20, 0);

	} else {				/* normal i/o */
	    /* VIOS CALL */
	    tu->vt_kill = vios_iorequest(
		FCODE(isread ? READ_FUNCTION : WRITE_FUNCTION,
		      isread ? READ_DATA : WRITE_DATA,0),
		tu->vt_VDD,(svtopte(bp->b_un.b_addr))->pgi.pg_pte&PFNMASK,
		bp->b_bcount, PHYS_BUFFER, NULL, 0, 0 , 0, 0,
		&tu->vt_stat, vtintr, 0, 0);
	}
}


/* Interrupt routine for the port.  Acknowledge the interrupt, check for
 * errors on the current operation, restart it if there was a repeat count,
 * and otherwise mark it done and start the next request.
 */
vtintr(st)
	register PKT_IOSTATUS *st;	/* pointer to vios return status */
{
	register struct	vtdev	*tu;	/* tape unit */
	register struct	buf	*bp;	/* current buffer */


	intflg = 4;		/* doing interrupt routine work */
	tu = (struct vtdev *) st;
	bp = tu->vt_tab.b_actf;
	if (bp == NULL) {
		panic("vtintr: nobuf");
	}

	/* Check for errors */
	if (st->code != V_SUCCESS) {
	    if (st->code != V_EOF) {
		bp->b_flags |= B_ERROR;
		tu->vt_errflg = TRUE;

#ifdef DBGMSG
		printf("vtintr: (err) ");
		switch(st->code) {
		case V_NO_WRITE:
			printf("%s\n", wrtprot);
			break;
		case V_NOT_READY:
			printf("%s\n", drvoffl);
			break;
		default:
			if(st->code < 0) { 
				printf(" code=%d\n",st->code);
			}
			break;
		}
#endif DBGMSG

	    }
	}

	tu->vt_tab.b_active = 0;

	/*
	 * blkcount = (st->bcount >> BPCSHIFT);
	 * if (blkcount < 0) blkcount = 0;
	 *
	 *
	 * accumulate blocks read or written
	 *
	 * switch (bp->b_command) {
	 *	case TS_READ:
	 *		tu->vt_mtget.mt_reads += blkcount;
	 *		break;
	 *	case TS_WRITE:
	 *		tu->vt_mtget.mt_writes += blkcount;
	 *		break;
	 *	case TS_WRITE_EOF:
	 *		if ((bp->b_flags & B_ERROR) == 0)
	 *			tu->vt_mtget.mt_writes++;
	 *		break;
	 *	case TS_READMARK:
	 *		if ((bp->b_flags & B_ERROR) == 0) {
	 *			tu->vt_mtget.mt_skips += bp->b_repcnt;
	 *		}
	 * }
	 */

	bp->b_repcnt = 0;
	bp->b_resid -= st->bcount;
	tu->vt_tab.b_errcnt = 0;
	tu->vt_tab.b_actf = bp->av_forw;
	iodone(bp);
	if (tu->vt_errflg) 		/* abort operation */
		goto bad;
	if (tu->vt_tab.b_actf) {	/* start new transfer, if any */
		vtstart(tu);	
	}
bad:
	intflg = 0;	/* finished interrupt routine work */
}


/* Routines to do raw IO for a unit.
 */
vtread(dev)			/* character read routine */
	dev_t dev;
{
	int unit = VTUNIT(dev);

	if (unit >= vt_cnt)
		u.u_error = ENXIO;
	else
		physio(vtstrategy, &rvtbuf[unit], dev, B_READ);
}


vtwrite(dev)			/* character write routine */
	dev_t dev;
{
	register int unit = VTUNIT(dev);

	if (unit >= vt_cnt)
		u.u_error = ENXIO;
	else
		physio(vtstrategy, &rvtbuf[unit], dev, B_WRITE);
}



/* Perform IOCTL functions (rewinds, skips, write file marks, etc).
 */
vtioctl(dev, cmd, addr, flag)
	dev_t	dev;		/* unit number */
	caddr_t	addr;		/* address */
{
#ifdef VTIOCTL
	register int	func;		/* function code to execute */
	register int	count;		/* repeat counter */
	register struct	buf	*bp;	/* buffer pointer */
	register struct	vtdev	*tu;	/* tape unit data */
	struct	mtop	mtop;		/* block for reading user data */


	/* DEBUG */
#ifdef DBGMSG
	printf("vtioctl: dev %x cmd %x addr %x flag %x\n",dev,cmd,addr,flag);
#endif DBGMSG


	tu = &vt_dev[VTUNIT(dev)];
	switch (cmd) {
	case MTIOCTOP:	/* tape operation */
		if (copyin((caddr_t)addr, (caddr_t)&mtop, sizeof(mtop))) {
			u.u_error = EFAULT;
			return;
		}
		count = 1;
		switch(mtop.mt_op) {
		case MTWEOF:
			func = WRITE_EOF;
			count = mtop.mt_count;
			break;
		case MTBSF:
			func = SKIP_FILES;
			count = -(mtop.mt_count);
			break;
		case MTFSF:
			func = SKIP_FILES;
			count = mtop.mt_count;
			break;
		case MTBSR:
			func = SKIP_RECORDS;
			count = -(mtop.mt_count);
			break;
		case MTFSR:
			func = SKIP_RECORDS;
			count = mtop.mt_count;
			break;
		case MTREW:
			func = REWIND;
			break;
		case MTOFFL:
			func = UNLOAD_MEDIA;
			break;
		case MTNOP:
			return;

#ifdef notdef
		case MTRETEN:
			func = TCUC_RETENSION;
			break;
		case MTERASE:
			func = TCUC_TAPEERASE;
			break;
#endif

		default:
			u.u_error = ENXIO;
			return;
		}
		if ((flag & FWRITE) != 0) {
			u.u_error = EBADF;	/* writing when read-only */
			return;
		}
		if ( vtcommand(dev, func, count) ) {
				u.u_error = EIO;
				return;
		}

#ifdef notdef
		if (func == TCUC_TAPEERASE) {	/* needed to "fix up" state */
			vtcommand(dev, REWIND, 1);
		}
#endif

		return;

	case MTIOCGET:	/* return tape status */
		if (copyout(&tu->vt_mtget, addr, sizeof(struct mtget))) {
			u.u_error = EFAULT;
		}
		return;

	case MTIOCZERO:	/* clear tape status (non-standard function) */
		tu->vt_mtget.mt_dsreg = 0;
		tu->vt_mtget.mt_erreg = 0;
		tu->vt_mtget.mt_resid = 0;
		tu->vt_mtget.mt_skips = 0;
		tu->vt_mtget.mt_reads = 0;
		tu->vt_mtget.mt_writes = 0;
		tu->vt_mtget.mt_readerrors = 0;
		tu->vt_mtget.mt_writeerrors = 0;
		tu->vt_mtget.mt_retries = 0;
		return;

	default:
		u.u_error = ENXIO;
	}
#endif VTIOCTL
}


vtdealloc( tu, tppdd )
int tppdd;
register struct vtdev *tu;
{
	register PKT_IOSTATUS *st;	/* pointer for returned status */

	st = &tu->vt_stat;

	/* get rid of virtual device connections */
	/* VIOS CALL */
	vios_iorequest( FCODE(VCTRL_FUNCTION, ASSIGNVD, (PRI_INP | PRI_OUT)),
			tu->vt_VDD, 0, 0, 0,
			NULL, 0, 0, 0, 0,
			st,NULL, 0, 0);
	while(st->code == 0) nulldev();

	/* get rid of physical device */
	if (tppdd) {
	    /* VIOS CALL */
	    vios_iorequest( FCODE(PCTRL_FUNCTION, REMOVEPD, 0),
			    tppdd, 0, 0, 0,
			    NULL, 0, 0, 0, 0,
			    st,NULL, 0, 0);
	    while(st->code == 0) nulldev();
	}
    }
