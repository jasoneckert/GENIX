/*
 * bio.c: version 3.10 of 9/6/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)bio.c	3.10 (NSC) 9/6/83";
# endif


#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/mmu.h"
#include "../h/vm.h"
#include "../h/devvm.h"

extern int	maxphysio;

/*
 * The following several routines allocate and free
 * buffers with various side effects.  In general the
 * arguments to an allocate routine are a device and
 * a block number, and the value is a pointer to
 * to the buffer header; the buffer is marked "busy"
 * so that no one else can touch it.  If the block was
 * already in core, no I/O need be done; if it is
 * already busy, the process waits until it becomes free.
 * The following routines allocate a buffer:
 *	getblk
 *	bread
 *	breada
 *	baddr	(if it is incore)
 * Eventually the buffer must be released, possibly with the
 * side effect of writing it out, by using one of
 *	bwrite
 *	bdwrite
 *	bawrite
 *	brelse
 */

struct	buf bfreelist[BQUEUES];

#define	BUFHSZ	63
struct	bufhd bufhash[BUFHSZ];
#define	BUFHASH(dev, dblkno)	\
		((struct buf *)&bufhash[((int)(dev)+(int)(dblkno)) % BUFHSZ])

/*
 * Initialize hash links for buffers.
 */
bhinit()
{
	register int i;
	register struct bufhd *bp;

	for (bp = bufhash, i = 0; i < BUFHSZ; i++, bp++)
		bp->b_forw = bp->b_back = (struct buf *)bp;
}

/* #define	DISKMON	1 */

#ifdef	DISKMON
struct {
	int	nbuf;
	long	nread;
	long	nreada;
	long	ncache;
	long	nwrite;
	long	bufcount[200];	/* HAS TO BE LARGER THAN THE REAL NBUF */
} io_info;
#endif

/*
 * Read in (if necessary) the block and return a buffer pointer.
 */
struct buf *
bread(dev, blkno)
register dev_t dev;
daddr_t blkno;
{
	register struct buf *bp;

	bp = getblk(dev, blkno);
	if (bp->b_flags&B_DONE) {
#ifdef	DISKMON
		io_info.ncache++;
#endif
		return(bp);
	}
	bp->b_flags |= B_READ;
	bp->b_bcount = BSIZE;
	(*bdevsw[major(dev)].d_strategy)(bp);
#ifdef	DISKMON
	io_info.nread++;
#endif
	u.u_vm.vm_inblk++;		/* pay for read */
	iowait(bp);
	return(bp);
}

/*
 * Read in the block, like bread, but also start I/O on the
 * read-ahead block (which is not allocated to the caller)
 */
struct buf *
breada(dev, blkno, rablkno)
register dev_t dev;
daddr_t blkno, rablkno;
{
	register struct buf *bp, *rabp;

	bp = NULL;
	if (!incore(dev, blkno)) {
		bp = getblk(dev, blkno);
		if ((bp->b_flags&B_DONE) == 0) {
			bp->b_flags |= B_READ;
			bp->b_bcount = BSIZE;
			(*bdevsw[major(dev)].d_strategy)(bp);
#ifdef	DISKMON
			io_info.nread++;
#endif
			u.u_vm.vm_inblk++;		/* pay for read */
		}
	}
	/*
	 * The following crock check is for tapes.  Disable read ahead since
	 * read ahead will fail when end of tape is reached.  (The tape drive
	 * does not like to read when at the end of data).
	 */
	if (bdevsw[major(dev)].d_tab->b_flags & B_TAPE) rablkno = 0;

	if (rablkno && !incore(dev, rablkno)) {
		rabp = getblk(dev, rablkno);
		if (rabp->b_flags & B_DONE)
			brelse(rabp);
		else {
			rabp->b_flags |= B_READ|B_ASYNC;
			rabp->b_bcount = BSIZE;
			(*bdevsw[major(dev)].d_strategy)(rabp);
#ifdef	DISKMON
			io_info.nreada++;
#endif
			u.u_vm.vm_inblk++;		/* pay in advance */
		}
	}
	if(bp == NULL)
		return(bread(dev, blkno));
	iowait(bp);
	return(bp);
}

/*
 * Write the buffer, waiting for completion.
 * Then release the buffer.
 */
bwrite(bp)
register struct buf *bp;
{
	register flag;

	flag = bp->b_flags;
	bp->b_flags &= ~(B_READ | B_DONE | B_ERROR | B_DELWRI | B_AGE);
	bp->b_bcount = BSIZE;
#ifdef	DISKMON
	io_info.nwrite++;
#endif
	if ((flag&B_DELWRI) == 0)
		u.u_vm.vm_oublk++;		/* noone paid yet */
	(*bdevsw[major(bp->b_dev)].d_strategy)(bp);
	if ((flag&B_ASYNC) == 0) {
		iowait(bp);
		brelse(bp);
	} else if (flag & B_DELWRI)
		bp->b_flags |= B_AGE;
	else
		geterror(bp);
}

/*
 * Release the buffer, marking it so that if it is grabbed
 * for another purpose it will be written out before being
 * given up (e.g. when writing a partial block where it is
 * assumed that another write for the same block will soon follow).
 * This can't be done for magtape, since writes must be done
 * in the same order as requested.
 */
bdwrite(bp)
register struct buf *bp;
{
	register struct buf *dp;

	if ((bp->b_flags&B_DELWRI) == 0)
		u.u_vm.vm_oublk++;		/* noone paid yet */
	dp = bdevsw[major(bp->b_dev)].d_tab;
	if(dp->b_flags & B_TAPE)
		bawrite(bp);
	else {
		bp->b_flags |= B_DELWRI | B_DONE;
		brelse(bp);
	}
}

/*
 * Release the buffer, start I/O on it, but don't wait for completion.
 */
bawrite(bp)
register struct buf *bp;
{

	bp->b_flags |= B_ASYNC;
	bwrite(bp);
}

/*
 * release the buffer, with no I/O implied.
 */
brelse(bp)
register struct buf *bp;
{
	register struct buf *flist;
	register s;

	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	if (bfreelist[0].b_flags&B_WANTED) {
		bfreelist[0].b_flags &= ~B_WANTED;
		wakeup((caddr_t)bfreelist);
	}
	if (bp->b_flags&B_ERROR)
		if (bp->b_flags & B_LOCKED)
			bp->b_flags &= ~B_ERROR;	/* try again later */
		else
			bp->b_dev = NODEV;  		/* no assoc */
	s = spl6();
	if (bp->b_flags & (B_ERROR|B_INVAL)) {
		/* block has no info ... put at front of most free list */
		flist = &bfreelist[BQUEUES-1];
		flist->av_forw->av_back = bp;
		bp->av_forw = flist->av_forw;
		flist->av_forw = bp;
		bp->av_back = flist;
	} else {
		if (bp->b_flags & B_LOCKED)
			flist = &bfreelist[BQ_LOCKED];
		else if (bp->b_flags & B_AGE)
			flist = &bfreelist[BQ_AGE];
		else
			flist = &bfreelist[BQ_LRU];
		flist->av_back->av_forw = bp;
		bp->av_back = flist->av_back;
		flist->av_back = bp;
		bp->av_forw = flist;
	}
	bp->b_flags &= ~(B_WANTED|B_BUSY|B_ASYNC|B_AGE);
	splx(s);
}


/*
 * See if the block is associated with some buffer
 * (mainly to avoid getting hung up on a wait in breada)
 */
incore(dev, blkno)
dev_t dev;
daddr_t blkno;
{
	register struct buf *bp;
	register struct buf *dp;
	register int dblkno = fsbtodb(blkno);

	dp = BUFHASH(dev, dblkno);
	for (bp = dp->b_forw; bp != dp; bp = bp->b_forw)
		if (bp->b_blkno == dblkno && bp->b_dev == dev &&
		    !(bp->b_flags & B_INVAL))
			return (1);
	return(0);
}

struct buf *
baddr(dev, blkno)
dev_t dev;
daddr_t blkno;
{

	if (incore(dev, blkno))
		return (bread(dev, blkno));
	return (0);
}

/*
 * Assign a buffer for the given block.  If the appropriate
 * block is already associated, return it; otherwise search
 * for the oldest non-busy buffer and reassign it.
 */
struct buf *
getblk(dev, blkno)
dev_t dev;
daddr_t blkno;
{
	register struct buf *bp, *dp, *ep;
	register int dblkno = fsbtodb(blkno);
#ifdef	DISKMON
	register int i;
#endif

	dp = BUFHASH(dev, dblkno);
    loop:
	(void) spl0();
	for (bp = dp->b_forw; bp != dp; bp = bp->b_forw) {
		if (bp->b_blkno != dblkno || bp->b_dev != dev ||
		    bp->b_flags&B_INVAL)
			continue;
		(void) spl6();
		if (bp->b_flags&B_BUSY) {
			bp->b_flags |= B_WANTED;
			sleep((caddr_t)bp, PRIBIO+1);
			goto loop;
		}
		(void) spl0();
#ifdef	DISKMON
		i = 0;
		dp = bp->av_forw;
		while ((dp->b_flags & B_HEAD) == 0) {
			i++;
			dp = dp->av_forw;
		}
		if (i<nbuf)
			io_info.bufcount[i]++;
#endif
		notavail(bp);
		bp->b_flags |= B_CACHE;
		return(bp);
	}
	if (major(dev) >= nblkdev)
		panic("blkdev");
	(void) spl6();
	for (ep = &bfreelist[BQUEUES-1]; ep > bfreelist; ep--)
		if (ep->av_forw != ep)
			break;
	if (ep == bfreelist) {		/* no free blocks at all */
		ep->b_flags |= B_WANTED;
		sleep((caddr_t)ep, PRIBIO+1);
		goto loop;
	}
	(void) spl0();
	bp = ep->av_forw;
	notavail(bp);
	if (bp->b_flags & B_DELWRI) {
		bp->b_flags |= B_ASYNC;
		bwrite(bp);
		goto loop;
	}
	bp->b_flags = B_BUSY;
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	bp->b_forw = dp->b_forw;
	bp->b_back = dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;
	bp->b_dev = dev;
	bp->b_blkno = dblkno;
	return(bp);
}

/*
 * get an empty block,
 * not assigned to any particular device
 */

struct buf *
geteblk()
{
	register struct buf *bp, *dp;

loop:
	(void) spl6();
	for (dp = &bfreelist[BQUEUES-1]; dp > bfreelist; dp--)
		if (dp->av_forw != dp)
			break;
	if (dp == bfreelist) {		/* no free blocks */
		dp->b_flags |= B_WANTED;
		sleep((caddr_t)dp, PRIBIO+1);
		goto loop;
	}
	(void) spl0();
	bp = dp->av_forw;
	notavail(bp);
	if (bp->b_flags & B_DELWRI) {
		bp->b_flags |= B_ASYNC;
		bwrite(bp);
		goto loop;
	}
	bp->b_flags = B_BUSY|B_INVAL;
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	bp->b_forw = dp->b_forw;
	bp->b_back = dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;
	bp->b_dev = (dev_t)NODEV;
	return(bp);
}



/*
 * Wait for I/O completion on the buffer; return errors
 * to the user.
 */
iowait(bp)
register struct buf *bp;
{

	spl6();
	while ((bp->b_flags&B_DONE)==0)
		sleep((caddr_t)bp, PRIBIO);
	spl0();
	geterror(bp);
}

/*
 * Unlink a buffer from the available list and mark it busy.
 * (internal interface)
 */
notavail(bp)
register struct buf *bp;
{
	register s;

	s = spl6();
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	bp->b_flags |= B_BUSY;
	splx(s);
}

/*
 * Mark I/O complete on a buffer, release it if I/O is asynchronous,
 * and wake up anyone waiting for it.
 */
iodone(bp)
register struct buf *bp;
{
	if(bp->b_flags & B_DONE)
		panic("dup iodone");
	bp->b_flags |= B_DONE;
	if (bp->b_flags&B_ASYNC)
		brelse(bp);
	else {
		bp->b_flags &= ~B_WANTED;
		wakeup((caddr_t)bp);
	}
	if (bp->b_flags & B_PHYSMAP) {
		swapint(bp);		/* handle swapping completion */
	}
}

/*
 * Zero the core associated with a buffer.
 */
clrbuf(bp)
register struct buf *bp;
{
	bzero(bp->b_un.b_words, BSIZE);
	bp->b_resid = 0;
}

/*
 * make sure all write-behind blocks
 * on dev (or NODEV for all)
 * are flushed out.
 * (from umount and update)
 */
bflush(dev)
register dev_t dev;
{
	register struct buf *bp;
	register struct buf *flist;
	register struct buf *end = &bfreelist[BQUEUES];

loop:
	(void) spl6();
	for (flist = bfreelist; flist < end; flist++)
	for (bp = flist->av_forw; bp != flist; bp = bp->av_forw) {
		if ((bp->b_flags & B_DELWRI) == 0)
			continue;
		if (dev == NODEV || dev==bp->b_dev) {
			bp->b_flags |= B_ASYNC;
			notavail(bp);
			bwrite(bp);
			goto loop;
		}
	}
	(void) spl0();
}

/*
 * Raw I/O. The arguments are
 *	The strategy routine for the device
 *	A buffer, which will always be a special buffer
 *	  header owned exclusively by the device for this purpose
 *	The device number
 *	Read/write flag
 * Essentially all the work is computing physical addresses and
 * validating them.
 * If the user has the proper access priviliges, the pages involved in the
 * I/O are faulted and locked.  After the completion of the I/O, the above
 * pages are unlocked.
 */
physio(strat, bp, dev, rw, mincnt)
int (*strat)(); 
register struct buf *bp;
unsigned (*mincnt)();
{
	register int c;
	register char *a;

	if (checkmem(u.u_spti, u.u_base, u.u_count, rw == B_READ)) {
		u.u_error = EFAULT;
		return;
	}
	(void) spl6();
	while (bp->b_flags&B_BUSY) {
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PRIBIO+1);
	}
	bp->b_error = 0;
	bp->b_spti = u.u_spti;
	bp->b_un.b_addr = u.u_base;
	while (u.u_count != 0) {
		bp->b_flags = B_BUSY | B_PHYS | rw;
		bp->b_dev = dev;
		bp->b_blkno = u.u_offset >> PGSHIFT;
		bp->b_bcount = u.u_count;
		(*mincnt)(bp);
		c = bp->b_bcount;
		a = bp->b_un.b_addr;
		if (lockmem(u.u_spti, a, c)) panic("physiolock");
		(*strat)(bp);
		(void) spl6();
		while ((bp->b_flags&B_DONE) == 0)
			sleep((caddr_t)bp, PRIBIO);
		unlockmem(u.u_spti, a, c, rw == B_READ);
		if (bp->b_flags&B_WANTED)
			wakeup((caddr_t)bp);
		(void) spl0();
		bp->b_un.b_addr += c;
		u.u_count -= c;
		u.u_offset += c;
		if (bp->b_flags&B_ERROR)
			break;
	}
	bp->b_flags &= ~(B_BUSY|B_WANTED|B_PHYS);
	u.u_count = bp->b_resid;
	geterror(bp);
}


/*ARGSUSED*/
unsigned
minphys(bp)
struct buf *bp;
{
	if (bp->b_bcount > maxphysio)
		bp->b_bcount = maxphysio;
}


/*
 * Pick up the device's error number and pass it to the user;
 * if there is an error but the number is 0 set a generalized
 * code.  Actually the latter is always true because devices
 * don't yet return specific errors.
 */
geterror(bp)
register struct buf *bp;
{

	if (bp->b_flags&B_ERROR)
		if ((u.u_error = bp->b_error)==0)
			u.u_error = EIO;
}

/*
 * Invalidate in core blocks belonging to closed or umounted filesystem
 *
 * This is not nicely done at all - the buffer ought to be removed from the
 * hash chains & have its dev/blkno fields clobbered, but unfortunately we
 * can't do that here, as it is quite possible that the block is still
 * being used for i/o. Eventually, all disc drivers should be forced to
 * have a close routine, which ought ensure that the queue is empty, then
 * properly flush the queues. Until that happy day, this suffices for
 * correctness.						... kre
 */
binval(dev)
dev_t dev;
{
	register struct buf *bp;
	register struct bufhd *hp;
#define dp ((struct buf *)hp)

	for (hp = bufhash; hp < &bufhash[BUFHSZ]; hp++)
		for (bp = dp->b_forw; bp != dp; bp = bp->b_forw)
			if (bp->b_dev == dev)
				bp->b_flags |= B_INVAL;
}
