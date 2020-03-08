/*
 * tcu.c: version 3.22 of 10/13/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)tcu.c	3.22 (NSC) 10/13/83";
# endif

/*	tcu.c	Mesa 16000 TCD driver for tapes		82/10/6		*/
/*							David I. Bell	*/

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/file.h"
#include "../h/vm.h"
#include "../h/devvm.h"
#include "../h/mtio.h"
#include "../h/tcu.h"
#define	b_command	b_resid		/* tape command field in buffer */
#define	b_repcnt	b_bcount	/* repeat count for command */


/* There is only one tape drive allowed on the controller.  If that ever changes
 * then the following can be modified to be correct.  Meanwhile TCUUNIT is
 * defined to be zero to save code.
 */
#define	NTCU	1			/* number of tape drives (must be 1) */
#define	TCUUNIT(dev) (0)		/* "minor(dev) & 3", unit number */
#define	T_NOREWIND	04		/* don't rewind tape unit flag */


/* Software status codes for the tape.  Used to properly sequence commands. */
#define	TS_INIT	0			/* drive needs initing (must be 0) */
#define	TS_BOT	1			/* drive is at beginning of tape */
#define	TS_READ	2			/* drive reading data */
#define	TS_READMARK	3		/* drive looking for file mark */
#define	TS_WRITE 4			/* drive writing data */
#define	TS_WRITEMARK	5		/* drive writing file mark */
#define	TS_EOT	6			/* drive is at end of tape */


/* Return values from the tcuhandleerror routine. */
#define	ERRORRETRY	0		/* operation should be retried */
#define	ERRORFAILED	1		/* error is fatal to operation */
#define	ERRORFIXED	2		/* error should be ignored */
#define	ERROREOF	3		/* end of file */


/* Status for a tape drive */
struct	tape {
	char	tcu_openf;		/* lock against multiple opens */
	char	tcu_state;		/* current state of the tape */
	char	tcu_immed;		/* immediate status at interrupt */
	char	tcu_cmmd;		/* last command given to drive */
	struct	buf	tcu_buf;	/* buffer for header of IO list */
	struct	buf	tcu_ctlbuf;	/* buffer for controlling operations */
	struct	tcu_sense tcu_sense;	/* last sense bytes read from drive */
	struct	mtget	tcu_mtget;	/* accumulated error status */
};


/* Status of the tape controller port */
struct	port {
	struct	tcu_iocb tcu_iocb;	/* iocb for port */
	struct	buf	tcu_tab;	/* buffer list for IO */
	struct	tape	tcu_drives[NTCU];	/* drives (can only be 1) */
	long	*tcu_pagtab;		/* page table pointer */
	char	tcu_displ;		/* display indicator value */
};

struct	port	*tcuport;		/* pointer to port data */
char	tcudata[sizeof(struct port) + PGSIZE];	/* data for page alignment */

/* The following data is words 2 through 8 of the selftest command */
static	short	stdat[] = {
	0x2040, 0x0810, 0x0204, 0xfe01, 0xfbfd, 0xeff7, 0xbfdf
};


struct	buf	rtcubuf[NTCU];		/* buffers for raw IO */
extern	struct	buf	tcutab;		/* table of tape buffers */
static	int	dummy;			/* make optimizer preserve references */
int	minphys();			/* routine for physio */

/* Open a tape drive.  Tapes are unique open devices, so we refuse it if
 * it is already open.  Verify that the drive is available and can be
 * written if so desired.
 */
tcuopen(dev, flag)
	register dev_t	dev;		/* device number */
	int	flag;			/* nonzero if open for writes */
{
	register struct	tape	*tu;	/* pointer to unit */
	register short	bits;		/* sense byte bits */
	register char	*errstr;	/* error string */
	int	tcuunit;		/* unit number */

	tcuport = (struct port *) ((((int)tcudata) + PGOFSET) & ~PGOFSET);
	tcuunit = minor(dev) & 3;	/* "TCUUNIT(dev)", but correct here */
	tu = &tcuport->tcu_drives[tcuunit];
	if ((tcuunit >= NTCU) || (tu->tcu_openf)) {
		u.u_error = ENXIO;
		return;
	}
	tu->tcu_openf = 1;
	tcutab.b_flags |= B_TAPE;	/* remember this is tape device */

	/* The tape controller requires that the first three commands given to
	 * the tape be the selftest, sense IO, and rewind commands.  After that
	 * we can do what we want.
	 */
	if ((tu->tcu_state == TS_INIT) && (tcucommand(dev, TCUC_SELFTEST, 1))) {
		uprintf("tape error: selftest failed, status %x\n",
			tu->tcu_immed & 0xff);
		u.u_error = EIO;
		tu->tcu_openf = 0;
		return;
	}
	if (tcucommand(dev, TCUC_SENSE, 1)) {		/* ask about drive */
		errstr = "sense command failed";
		goto fail;
	}
	bits = tu->tcu_sense.tcusen_tapestat;
	if (bits & (TCUSB_OFFLINE | TCUSB_NOTAPE)) {
		errstr = "cartridge not in place";
		goto fail;
	}
	if ((flag) && (bits & TCUSB_WRITELOCK)) {
		errstr = "cartridge is write-locked";
		goto fail;
	}
	if (tu->tcu_state == TS_INIT) tcucommand(dev, TCUC_REWIND, 1);
	if (tcuport->tcu_pagtab == 0) {		/* allocate page table */
		tcuunit = kallocpage(1);
		if (tcuunit < 0) {		/* no swap space */
			u.u_error = ENOMEM;
			tu->tcu_openf = 0;
			return;
		}
		tcuport->tcu_pagtab = (long *) tcuunit;
	}
	return;


fail:	uprintf("tape error: %s\n", errstr);
	u.u_error = EIO;
	tu->tcu_openf = 0;
}

/* Close the tape drive.  If tape was open for writing or last operation
 * was a write, then write a tape mark.  Unless this is a non-rewinding
 * special file, rewind the tape.
 */
tcuclose(dev, flag)
	register dev_t	dev;		/* device number */
	register int	flag;		/* write flag */
{
	register struct	tape	*tu;	/* tape drive */

	tu = &tcuport->tcu_drives[TCUUNIT(dev)];
	if ((flag==FWRITE) || ((flag&FWRITE) && (tu->tcu_state == TS_WRITE))) {
		tcuarmwrite(dev);
		tcucommand(dev, TCUC_WRITEMARK, 1);	/* write file mark */
	}
	if ((minor(dev) & T_NOREWIND) == 0) {
		/*
		 * 0 count means don't hang waiting for rewind complete
		 * rather tcu_ctlbuf stays busy until the operation completes
		 * preventing further opens from completing by preventing
		 * a TCUC_SENSE from completing.
		 */
		tcucommand(dev, TCUC_REWIND, 0);	/* rewind tape */
	}
	tu->tcu_openf = 0;
	if (tcuport->tcu_pagtab) kfreepage(tcuport->tcu_pagtab);
	tcuport->tcu_pagtab = 0;
}

/* Execute a command on the tape drive a specified number of times.  This
 * should only be called for non-data transfer operations.  If count is zero,
 * then the operation will not be waited for, and the user will wait when
 * the drive is next opened.  Returns nonzero if the operation failed.
 */
tcucommand(dev, com, count)
	dev_t	dev;
{
	register struct	tape	*tu;		/* tape unit */
	register struct	buf	*bp;		/* buffer used for command */
	int	err;

	tu = &tcuport->tcu_drives[TCUUNIT(dev)];
	bp = &tu->tcu_ctlbuf;
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
	tcustrategy(bp);
	/*
	 * If count is zero, don't wait.  This occurs for rewinds from close,
	 * and for retensions or tape erases from ioctl.
	 */
	if (count == 0)
		return(0);
	iowait(bp);
	if (bp->b_flags & B_WANTED)
		wakeup((caddr_t)bp);
	err = bp->b_flags & B_ERROR;
	bp->b_flags = 0;		/* buffer available again */
	return(err);
}

/* Strategy routine for a buffer.  Finds the proper unit, range checks
 * arguments, and schedules the operation.  This is called for both data
 * transfers and for tape controlling operations (rewinds, etc).
 */
tcustrategy(bp)
	register struct buf	*bp;	/* IO operation to perform */
{
	register struct	port	*ph;	/* controller header */
	register struct	tape	*tu;	/* tape drive */
	register struct	buf	*dp;	/* tape header buffer */

	ph = tcuport;
	tu = &ph->tcu_drives[TCUUNIT(bp->b_dev)];
	if ((bp != &tu->tcu_ctlbuf) &&
		((bp->b_bcount | (int) bp->b_un.b_addr) & PGOFSET)) {
			bp->b_flags |= B_ERROR;
			iodone(bp);
			return;		/* io must be page-aligned */
	}
	/*
	 * Put transfer at end of controller queue
	 */
	dp = &tu->tcu_buf;
	bp->av_forw = NULL;
	(void) spl5();
	if (dp->b_actf == NULL) {
		/*
		 * Transport not already active...
		 * put at end of controller queue.
		 */
		dp->b_actf = bp;
		dp->b_forw = NULL;
		if (ph->tcu_tab.b_actf == NULL)
			ph->tcu_tab.b_actf = dp;
		else
			ph->tcu_tab.b_actl->b_forw = dp;
		ph->tcu_tab.b_actl = dp;
	} else
		dp->b_actl->av_forw = bp;
	dp->b_actl = bp;
	if (ph->tcu_tab.b_active == 0)
		tcustart(ph);
	(void) spl0();
}

/* Start a data transfer for the tape controller, or start a controlling
 * tape operation.  The choice is made by noticing which buffer was been
 * passed to us.
 */
tcustart(ph)
	register struct	port	*ph;	/* port structure */
{
	register struct	buf	*bp;	/* buffer for IO */
	register struct	buf	*dp;	/* buffer header for tape */
	register struct	tcu_iocb *io;	/* io control block */
	register struct	tape	*tu;	/* tape drive */
	long	addr;			/* address for IO */
	int	i;			/* block counter */
	long	blkcnt;			/* number of pages for IO */
	int	isread;			/* nonzero if reading */

	dp = ph->tcu_tab.b_actf;
	if (dp == NULL)
		return;
	bp = dp->b_actf;
	if (bp == NULL)
		return;		/* WRONG IF HAVE MULTIPLE UNITS (MUST LOOP) */
	tu = &ph->tcu_drives[TCUUNIT(bp->b_dev)];
	io = &ph->tcu_iocb;
	if (bp == &tu->tcu_ctlbuf) {
		switch (bp->b_command) {
			case TCUC_REWIND:
			case TCUC_RETENSION:
			case TCUC_TAPEERASE:
				tcudoio(bp->b_command, 0, 0, 0);
				tu->tcu_state = TS_BOT;
				binval(bp->b_dev);	/* toss out old data */
				break;

			case TCUC_WRITEMARK:
				tcudoio(TCUC_WRITEMARK, 0, 0, 0);
				tu->tcu_state = TS_WRITEMARK;
				break;

			case TCUC_SEARCHMARK:
				tcudoio(TCUC_SEARCHMARK, 0, 0, 0);
				tu->tcu_state = TS_READMARK;
				break;

			case TCUC_ARMERASE:
				tcudoio(TCUC_ARMERASE, 0, 0, 0);
				tu->tcu_state = TS_WRITE;
				break;

			case TCUC_SENSE:
				addr = (int) &tu->tcu_sense;
				tu->tcu_sense.tcusen_status = 0;	
				tu->tcu_sense.tcusen_tapestat = 0;
				tu->tcu_sense.tcusen_errcount = 0;
				tcudoio(TCUC_SENSE, 0, 2, &addr);
				/* does not change state */
				break;

			case TCUC_SELFTEST:
				/* The controller requires that the tape unit
				 * be inhibited before doing a selftest command.
				 * The inhibit will be cleared on the interrupt.
				 */
				dummy += *TCUA_SETINHIBIT;
				tcudoio(TCUC_SELFTEST, 0x80, 7, stdat);
				tu->tcu_state = TS_INIT;
				break;

			case TCUC_DISPLAY:
				tcudoio(TCUC_DISPLAY, tcuport->tcu_displ, 0, 0);
				/* does not change state */
				break;

			default: 
				panic("badtcucommand");
		}
		return;

		/* Now the tcu will interrupt when finished. */
	}

	/* Here if this is a real live data transfer */

	isread = 0;
	if (bp->b_flags & B_READ) isread = 1;
	if (!isread && (tu->tcu_state != TS_WRITE)
		&& (tu->tcu_state != TS_WRITEMARK)
		&& (tu->tcu_state != TS_EOT)) {
			tcudoio(TCUC_ARMERASE, 0, 0, 0); /* enable writing */
			tcuwait();
	}
	addr = (int)(bp->b_un.b_addr);
	blkcnt = (bp->b_bcount >> PGSHIFT);
	if (ph->tcu_pagtab == 0) panic("tcunotable");
	for (i = 0; i < blkcnt; i++) {		/* addresses of memory pages */
		ph->tcu_pagtab[i] = addr;
		if (bp->b_flags & B_PHYS) {	/* physio, get real address */
			ph->tcu_pagtab[i] = physaddr(bp->b_spti, addr);
			if (ph->tcu_pagtab[i] < 0) panic("tcubadmem");
		};
		addr += NBPG;
	}
	io->tcuio_opcode = (isread ? TCUC_READ : TCUC_WRITE);
	io->tcuio_buffer = physaddr(SYSSPTI, ph->tcu_pagtab);	/* io ptrs */
	io->tcuio_count = blkcnt;	/* set page count */
	io->tcuio_sense.tcusen_resid = blkcnt;	/* here too in case sick port */
	io->tcuio_sense.tcusen_status = 0;	/* clear controller status */
	io->tcuio_sense.tcusen_tapestat = 0;	/* clear tape status */
	io->tcuio_sense.tcusen_errcount = 0;	/* and error counter */
	io->tcuio_endstat = 0;		/* clear ending status */
	io->tcuio_chain = 0;		/* set no chaining */
	addr = (int) io;
	tcudoio(TCUC_START, 0, 2, &addr);		/* start IO */
	tu->tcu_state = (isread ? TS_READ : TS_WRITE);

	/* Now the tcu will interrupt when finished. */
}

/* Interrupt routine for the port.  Acknowledge the interrupt, check for
 * errors on the current operation, restart it if there was a repeat count,
 * and otherwise mark it done and start the next request.
 */
tcuintr()
{
	register struct	port	*ph;	/* tape port */
	register struct	tape	*tu;	/* tape unit */
	register struct	buf	*bp;	/* current buffer */
	register struct	buf	*dp;	/* tape header buffer */
	register char	code;		/* status code */
	char	cmmd;			/* command to drive */
	char	status;			/* tape status */
	char	state;			/* software state */
	int	count;			/* blocks transferred */
	int	resid;			/* residual */

	status = *TCUA_STATUS;
	code = STATUS(status);			/* extract status code */
	if (code == TCUSTS_BUSY) return;	/* spurious interrupt, ignore */
	ph = tcuport;
	dp = ph->tcu_tab.b_actf;
	if (code == TCUSTS_IDLE) {
		if (dp == NULL) return;		/* obscure, ignore it */
		panic("tcuidle");
	}
	*TCUA_ACK = '\0';			/* acknowledge the interrupt */
	dummy += *TCUA_CLRINHIBIT;		/* clear inhibit for selftest */
	bp = dp->b_actf;
	if (bp == NULL) {
		panic("tcunobuf");
	}
	tu = &ph->tcu_drives[TCUUNIT(bp->b_dev)];
	ph->tcu_tab.b_active = 0;
	/*
	 * Check for errors.
	 */
	tu->tcu_immed = status;
	state = tu->tcu_state;
	cmmd = tu->tcu_cmmd;
	if (cmmd == TCUC_START) {
		cmmd = tcuport->tcu_iocb.tcuio_opcode;
		if (ph->tcu_iocb.tcuio_endstat & TCUES_AVAIL) {
			tu->tcu_mtget.mt_retries +=
				ph->tcu_iocb.tcuio_sense.tcusen_errcount;
			ph->tcu_iocb.tcuio_sense.tcusen_errcount = 0;
		}
	}
	resid = 0;
	if (code == TCUSTS_ERROR) {
		resid = bp->b_resid;
		switch (tcuhandleerror(bp)) {
		case ERRORRETRY:
			tcustart(ph);		/* restart the operation */
			return;

		case ERRORFAILED:
			printf("tape error: status %x %x %x\n", status & 0xff,
				tu->tcu_sense.tcusen_status & 0xff,
				tu->tcu_sense.tcusen_tapestat & 0xffff);
			switch (cmmd) {		/* accumulate error counts */
				case TCUC_READ:
				case TCUC_SEARCHMARK:
					tu->tcu_mtget.mt_readerrors++;
					break;
				case TCUC_WRITE:
				case TCUC_WRITEMARK:
					tu->tcu_mtget.mt_writeerrors++;
					break;
			}
			tu->tcu_mtget.mt_dsreg = (status << 8) +
				(tu->tcu_sense.tcusen_status & 0xff);
			tu->tcu_mtget.mt_erreg = tu->tcu_sense.tcusen_tapestat;
			tu->tcu_mtget.mt_resid =
				tu->tcu_sense.tcusen_resid << PGSHIFT;
			bp->b_flags |= B_ERROR;
			/* proceed into next case */

		case ERROREOF:
			if ((cmmd == TCUC_READ) || (cmmd == TCUC_WRITE)) {
				resid = tu->tcu_sense.tcusen_resid << PGSHIFT;
				break;
			}
			/* proceed into next case */

		case ERRORFIXED:
			resid = 0;
		}
	}
	count = ph->tcu_iocb.tcuio_count-ph->tcu_iocb.tcuio_sense.tcusen_resid;
	if (count < 0) count = 0;
	switch (cmmd) {		/* accumulate blocks read or written */
		case TCUC_READ:
			tu->tcu_mtget.mt_reads += count;
			break;
		case TCUC_WRITE:
			tu->tcu_mtget.mt_writes += count;
			break;
		case TCUC_WRITEMARK:
			if ((bp->b_flags & B_ERROR) == 0)
				tu->tcu_mtget.mt_writes++;
			break;
		case TCUC_SEARCHMARK:
			if ((bp->b_flags & B_ERROR) == 0) {
				tu->tcu_mtget.mt_skips++;
				tu->tcu_mtget.mt_reads++;
			}
	}
	if ((bp == &tu->tcu_ctlbuf) && (bp->b_repcnt > 1)) {
		bp->b_repcnt--;
		tcustart(ph);		/* continue with another operation */
		return;
	}
	bp->b_resid = resid;
	ph->tcu_tab.b_errcnt = 0;
	dp->b_actf = bp->av_forw;
	iodone(bp);
	ph->tcu_tab.b_actf = dp->b_forw;
	if (dp->b_actf) {
		dp->b_forw = NULL;
		if (ph->tcu_tab.b_actf == NULL)
			ph->tcu_tab.b_actf = dp;
		else
			ph->tcu_tab.b_actl->b_forw = dp;
		ph->tcu_tab.b_actl = dp;
	}
	if (ph->tcu_tab.b_actf)
		tcustart(ph);		/* start new transfer */
}

/* Routine to handle a tape error.  Returns one of the following:
 *	ERRORFIXED/	Operation recovered successfully.
 *	ERRORRETRY/	Operation should be restarted.
 *	ERRORFAILED/	Operation failed.
 *	ERROREOF/	Read saw file mark.
 */
tcuhandleerror(bp)
	struct	buf	*bp;			/* buffer with error */
{
	register struct	tape	*tu;		/* tape unit */
	register long	immstat;		/* immediate status bits */
	register long	status;			/* controller status bits */
	register long	tapestat;		/* status bytes from drive */
	register long	state;			/* current software state */
	long	addr;				/* address of sense block */

	tu = &tcuport->tcu_drives[TCUUNIT(bp->b_dev)];
	immstat = tu->tcu_immed;
	state = tu->tcu_state;
	if ((state == TS_READ) || (state == TS_WRITE)) {
		tu->tcu_sense = tcuport->tcu_iocb.tcuio_sense;
			/* copy the sense bytes stored in iocb */
	}
	if (immstat & TCUIS_STSAVAIL) {		/* must ask for sense bytes */
		tu->tcu_sense.tcusen_errcount = 0;
		addr = (int) &tu->tcu_sense;
		tcudoio(TCUC_SENSE, 0, 2, &addr);
		if (tcuwait()) {
			printf("tape error: sense command failed\n");
			return(ERRORFAILED);
		}
		tu->tcu_mtget.mt_retries += tu->tcu_sense.tcusen_errcount;
	}
	status = tu->tcu_sense.tcusen_status;
	tapestat = tu->tcu_sense.tcusen_tapestat;
	if (immstat & TCUIS_SEQERR) {		/* hardware sequence error */
		return(ERRORFAILED);
	}
	if (immstat & TCUIS_MIFCHK) {		/* MESA interface error */
		tu->tcu_state = TS_INIT;	/* force a reset */
		return(ERRORFAILED);
	}
	if (immstat & TCUIS_CTRLCHK) switch (CTLFAULTCODE(status)) {
		case TCUCF_EXCCMMD:
			/* The TCU can give the "exception on command" error
			 * on any command after a read command (even for a
			 * rewind!!).  This is because the drive reads ahead
			 * and will see a following file mark, thus causing
			 * the next command to fail because the file mark is
			 * an exception condition.  Such a feature!!
			 */
			if ((tapestat & ~TCUSB_WRITELOCK) ==
				(TCUSB_FILEMARK | TCUSB_ANY0)) {
					return(ERRORRETRY);
			}
			return (ERRORFAILED);
		case TCUCF_SEQERR:
			tu->tcu_state = TS_INIT;	/* do reset */
		default:
			return(ERRORFAILED);
	}
	if (tapestat & (TCUSB_RESET|TCUSB_OFFLINE|TCUSB_NOTAPE|TCUSB_ILLCMMD)) {
		if (tapestat & TCUSB_RESET) tu->tcu_state = TS_INIT;
		binval(bp->b_dev);		/* toss out old data */
		return(ERRORFAILED);		/* unrecoverable error */
	}
	if ((tapestat & (TCUSB_WRITELOCK | TCUSB_ENDOFTAPE))
		&& (((state == TS_WRITE)) || (state == TS_WRITEMARK))) {
			return(ERRORFAILED);	/* bad writes */
	}
	if (tapestat & TCUSB_FILEMARK) {	/* read file mark */
		if (state == TS_READMARK) return(ERRORFIXED);
		if (state == TS_READ) return(ERROREOF);
		return(ERRORFAILED);
	}
	/* Note:  TCUSB_BADBLOCK and TCUSB_DATAERROR are also set when
	 * TCUSB_NODATA is set, so they can't be checked for here.
	 */
	if (tapestat & TCUSB_NODATA) {		/* read past end of tape */
		tu->tcu_state = TS_EOT;
		if ((state != TS_READ) && (state != TS_READMARK)) {
			return(ERRORFAILED);
		}
		bp->b_flags |= B_ERROR;
		bp->b_error = ENXIO;
		return(ERRORFIXED);		/* "successful" error */
	}
	if (tapestat == TCUSB_RETRYS) {		/* recovered after 8 retries */
		lprintf("tape %s: recovered after 8 retries\n",
		((state==TS_READ) || (state==TS_READMARK)) ? "READ" : "WRITE");
		return(ERRORFIXED);
	}
	return(ERRORFAILED);			/* all other errors fail */
}

/* Routine to write a command to the port.  Commands are written 16 bits at
 * a time, using one address for all but the last word, and another address
 * for the final word.  Arguments:
 *   opcode/	A byte of opcode determining the immediate command.
 *   opdata/	A byte of data written in the same word as the opcode.
 *   argcount/	The number of additional 16 bit words to be written.
 *   argptr/	The list of words to write.
 */

tcudoio(opcode, opdata, argcount, argptr)
	char	opcode;			/* opcode in first byte */
	char	opdata;			/* char of data in second byte */
	register int	argcount;	/* number of remaining words of data */
	register short	*argptr;	/* pointer to word list */
{
	register short	firstarg;	/* first word to write */

	tcuport->tcu_drives[0].tcu_cmmd = opcode;	/* CROCK */
	firstarg = ((opcode & 0xff) | ((opdata & 0xff) << 8));
	if (argcount <= 0) {
		*TCUA_CMMDLAST = firstarg;
		return;
	}
	*TCUA_CMMDFIRST = firstarg;
	while (--argcount > 0) *TCUA_CMMDFIRST = *argptr++;
	*TCUA_CMMDLAST = *argptr;
}



/* Arm the drive for writing.  Needed if at beginning of tape.  Returns
 * nonzero on failure.
 */
tcuarmwrite(dev)
	dev_t	dev;
{
	register struct	tape	*tu;		/* drive being armed */

	tu = &tcuport->tcu_drives[TCUUNIT(dev)];
	switch (tu->tcu_state) {
		case TS_WRITE:		/* already writing */
		case TS_WRITEMARK:
			return(0);

		case TS_BOT:		/* beginning of tape, needs arming */
			return(tcucommand(dev, TCUC_ARMERASE, 1));

		case TS_EOT:		/* end of tape, can write right away */
			tu->tcu_state = TS_WRITE;
			return(0);
	}
	return(1);			/* other states can't start writing */
}

/* Routines to do raw IO for a unit.
 */
tcuread(dev)			/* character read routine */
	dev_t dev;
{
	int unit = minor(dev) >> 3;

	if (unit >= NTCU)
		u.u_error = ENXIO;
	else
		physio(tcustrategy, &rtcubuf[unit], dev, B_READ, minphys);
}


tcuwrite(dev)			/* character write routine */
	dev_t dev;
{
	register int unit = minor(dev) >> 3;

	if (unit >= NTCU)
		u.u_error = ENXIO;
	else
		physio(tcustrategy, &rtcubuf[unit], dev, B_WRITE, minphys);
}


tcudump(dev)			/* dump core after a system crash */
	dev_t dev;
{
	return(0);
}

/* Perform IOCTL functions (rewinds, skips, write file marks, etc).
 */
tcuioctl(dev, cmd, addr, flag)
	dev_t	dev;		/* unit number */
	caddr_t	addr;		/* address */
{
	register int	func;		/* function code to execute */
	register int	arm;		/* function needs to arm writes */
	register int	count;		/* repeat counter */
	register struct	buf	*bp;	/* buffer pointer */
	register struct	tape	*tu;	/* tape unit data */
	struct	mtop	mtop;		/* block for reading user data */

	tu = &tcuport->tcu_drives[TCUUNIT(dev)];
	switch (cmd) {
	case MTIOCTOP:	/* tape operation */
		if (copyin((caddr_t)addr, (caddr_t)&mtop, sizeof(mtop))) {
			u.u_error = EFAULT;
			return;
		}
		arm = 0;
		count = 1;
		switch(mtop.mt_op) {
		case MTWEOF:
			func = TCUC_WRITEMARK;
			count = mtop.mt_count;
			arm = 1;
			break;
		case MTFSF:
			func = TCUC_SEARCHMARK;
			count = mtop.mt_count;
			break;
		case MTFSR:
			count = mtop.mt_count * PGSIZE;
			bp = geteblk();
			bp->b_dev = dev;
			while ((count > 0) && (u.u_error == 0)) {
				bp->b_flags = B_BUSY | B_READ;
				bp->b_bcount = count;
				if (count > BSIZE) bp->b_bcount = BSIZE;
				bp->b_error = 0;
				tcustrategy(bp);
				iowait(bp);
				geterror(bp);
				count -= (bp->b_bcount - bp->b_resid);
				if (bp->b_resid) count -= PGSIZE;
					/* account for reading file mark */
			}
			bp->b_dev = (dev_t)NODEV;
			brelse(bp);
			return;
		case MTBSF:
		case MTBSR:
			u.u_error = ENXIO;
			return;
		case MTREW:
		case MTOFFL:
			func = TCUC_REWIND;
			break;
		case MTNOP:
			return;

		/* The following functions are non-standard */

		case MTRETEN:			/* retension tape */
			func = TCUC_RETENSION;
			break;
		case MTERASE:			/* erase whole tape */
			func = TCUC_TAPEERASE;
			arm = 1;
			break;
		case MTDISPL:			/* set display indicator */
			func = TCUC_DISPLAY;
			tcuport->tcu_displ = mtop.mt_count;
			break;
		default:
			u.u_error = ENXIO;
			return;
		}
		if (arm && ((flag & FWRITE) == 0)) {
			u.u_error = EBADF;	/* writing when read-only */
			return;
		}
		if ((arm && tcuarmwrite(dev)) ||
			(tcucommand(dev, func, count))) {
				u.u_error = EIO;
				return;
		}
		if (func == TCUC_TAPEERASE) {	/* needed to "fix up" state */
			tcucommand(dev, TCUC_REWIND, 1);
		}
		return;

	case MTIOCGET:	/* return tape status */
		tcucommand(dev, TCUC_SENSE, 1);	/* try to accumulate retries */
		tu->tcu_mtget.mt_type = MT_ITCD;
		if (copyout(&tu->tcu_mtget, addr, sizeof(struct mtget))) {
			u.u_error = EFAULT;
		}
		return;

	case MTIOCZERO:	/* clear tape status (non-standard function) */
		tu->tcu_mtget.mt_dsreg = 0;
		tu->tcu_mtget.mt_erreg = 0;
		tu->tcu_mtget.mt_resid = 0;
		tu->tcu_mtget.mt_skips = 0;
		tu->tcu_mtget.mt_reads = 0;
		tu->tcu_mtget.mt_writes = 0;
		tu->tcu_mtget.mt_readerrors = 0;
		tu->tcu_mtget.mt_writeerrors = 0;
		tu->tcu_mtget.mt_retries = 0;
		return;

	default:
		u.u_error = ENXIO;
	}
}

/* Wait until the tcu has completed a command, and return whether or not
 * the operation was successful.  Interrupts should be disabled for this
 * call.
 */
tcuwait()
{
	register char	stat;		/* current status */
	register char	code;		/* current code */
	register int	count;		/* timeout counter */

	count = 1000000;
	while (1) {
		code = STATUS(*TCUA_STATUS);
		if (code != TCUSTS_BUSY) break;
		if (count-- <= 0) panic("tcutimeout");
	}
	if (code != TCUSTS_IDLE) *TCUA_ACK = '\0';	/* ack the command */
	return (code != TCUSTS_DONE);	/* return error value */
}
