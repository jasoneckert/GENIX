# ifndef NOSCCS
static char *sccsid = "@(#)vmtext.c	1.18	10/4/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

/* Routines for handling shared text segments */

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/buf.h"
#include "../h/reg.h"
#include "../h/psr.h"
#include "../h/mmu.h"
#include "../h/vm.h"
#include "../h/devvm.h"
#include "../h/inode.h"
#include "../h/tty.h"
#include "../h/file.h"
#include "../h/vlimit.h"

#define	MAXTSIZ	(MCSIZE * 10000)	/* sane limit for text size */
#define	MAXDSIZ (MCSIZE * 1000000)	/* sane limit for data size */
#define	MAXSSIZ	(MCSIZE * 100)		/* sane limit for stack size */

long	sptagecounter;		/* age counter for spt slots */
struct	spt	*xalloc();	/* function to find available text entry */
struct	buf	*vbmap();	/* function to return file addresses */

/* Verify that the size of an executable file is reasonable, and that
 * there is enough free memory and swapping space to create the new
 * address space.  The necessary space is reserved for the process.
 * Returns nonzero if not enough space exists.
 */
xchecksize(ts, ds, bs, ss)
	unsigned int	ts;		/* text size */
	unsigned int	ds;		/* initialized data size */
	unsigned int	bs;		/* unitialized data size */
	unsigned int	ss;		/* stack size */
{
	int	datapages;		/* pages needed for text and data */
	int	stackpages;		/* pages needed for stack */
	int	ptepages;		/* pages needed for pte tables */
	register int	totalpages;	/* total pages needed */

	if ((ts > MAXTSIZ) || (ds > MAXDSIZ) || (bs > MAXDSIZ) ||
		(ds + bs > MAXDSIZ) || (ss > MAXSSIZ)) {
			return(1);	/* bogus size values */
	}
	if ((ss > u.u_limit[LIM_STACK]) ||
		((ds + bs + ss) > u.u_limit[LIM_DATA])) {
			return(1);	/* exceeds data or stack limits */
	}
	datapages = ((ts + ds + bs) + MCOFSET) / MCSIZE;
	stackpages = (ss + MCOFSET) / MCSIZE;
	ptepages = ((datapages + (NPTEPG-1)) / NPTEPG);
	ptepages += ((stackpages + (NPTEPG-1)) / NPTEPG);
	totalpages = datapages + stackpages + ptepages;
	if (totalpages > availpages) {
		return(1);		/* not enough swap space */
	}
	/* Doesn't work yet 
	availpages -= totalpages;
	spt[u.u_spti].spt_reserved = totalpages;
	*/
	return(0);
}

/* Routine to attach to a text segment.  This will allocate a slot in the
 * spt table for the new inode, and create spt-type pte entries in the given
 * address space to it.  If the spt table is full, the text is made a private
 * text for the process (slightly less efficient).  The file address space
 * contains both the text and the data.  The data is copied if and when it
 * is referenced.  Returns nonzero if failed.
 */
xattach(spti, ip, textsize, datasize)
	register spt_t	spti;		/* spt index of process */
	struct	inode	*ip;		/* inode of file to read */
{
	register spt_t	fspti;		/* file spt index to attach to */
	register struct	spt	*fs;	/* file spt pointer */
	register unsigned long addr;	/* address */
	register long	i;		/* loops and such */
	struct	cst	*pg;		/* page pointer */
	long	totalsize;		/* total size of file */

	if ((spti == SYSSPTI) || (spti > lastprocspti) || (ip == NULL)) {
		panic("badxattach");
	}
	totalsize = textsize + datasize;
	clearptetable(spti);		/* release and invalidate all pages */
	xblast(&spt[spti]);		/* release any private text */
	spt[spti].spt_flags |= SPT_NOEIA;	/* no more invalidation yet */
	if (u.u_xspti >= 0) {
		if (--spt[u.u_xspti].spt_proccount == 0) {
			xdorm(u.u_xspti);
		}
	}
	u.u_xspti = -1;			/* no more shared text */

retry:	for (fs = sptFILESPTI; fs < sptNSPT; fs++) {	/* search for text */
		if (fs->spt_inode != ip) continue;
		if (xlock(fs)) goto retry;	/* found it, try to lock it */
		fspti = (fs - spt);
		goto doattach;			/* ok, go attach to it */
	}

	/* text not in use, have to allocate it for first time */

	if (ip->i_flag & IWRITE) {	/* make sure file not writable */
		u.u_error = EBUSY;
		return(1);
	}
	fs = xalloc();			/* try to allocate free text entry */
	if (fs == NULL) goto retry;	/* freed one, go allocate it */
	if (fs == BADSPT) fs = &spt[spti];	/* no free ones, do privately */
	fs->spt_inode = ip;		/* say inode is a text file */
	ip->i_flag |= ITEXT;
	ip->i_count++;
	fspti = (fs - spt);
	if (xsetptetable(fspti, totalsize)) {	/* set up demand load ptes */
		xblast(fs);			/* failed */
		swapkill(u.u_procp, "file read error");
		spt[spti].spt_flags &= ~SPT_NOEIA;
		return(1);
	}
	if (fs < sptFILESPTI) {		/* if creating private text */
		for (addr = textsize; addr < totalsize; addr += MCSIZE) {
			i = pagein(spti, addr, -1);
			if (i < 0) return(1);
			pg = addrtocst(i);
			if (pg->cst_file) {
				pg->cst_backup = 0;	/* make data private */
				pg->cst_file = 0;
			}
			changemapprot(spti, addr, PG_RWRW);
		}
		spt[spti].spt_flags &= ~SPT_NOEIA;
		return(0);
	}

	/* Here to actually attach to the found text file */

doattach:
	fs->spt_age = ++sptagecounter;
	fs->spt_proccount++;
	xunlock(fs);			/* text now safe to unlock */
	u.u_xspti = fspti;		/* remember shared text index */
	for (addr = 0; addr < textsize; addr += MCSIZE) {
		i = getmap(fspti, addr);
		if (PTEISMEM(i)) {		/* can share same memory */
			i = PTEMEM_ADDR(i);
			setmap(spti, addr, PTEMEM(i, PG_RWRO));
			continue;
		}
		setmap(spti, addr, PTESPT(fspti, (addr >> MCSHIFT), PG_RWRO));
	}
	for (addr = textsize; addr < totalsize; addr += MCSIZE) {
		setmap(spti, addr, PTESPT(fspti, (addr >> MCSHIFT), PG_RWRW));
	}
	spt[spti].spt_flags &= ~SPT_NOEIA;
	return(0);
}

/* Routine to allocate a free text entry.  If an empty spt slot is found,
 * it is locked and a pointer to it is returned.  If all slots are in use,
 * the oldest available entry is freed and NULL is returned as an indication
 * to try again (necessary since we might block in clearptetable, and thus
 * some other user could init another spt entry for the same inode).  If
 * no texts are deletable, the special value BADSPT is returned.
 */
struct spt *xalloc()
{
	register struct	spt	*fs;	/* file spt pointer */
	register struct	spt	*bfs;	/* best spt entry so far */

	bfs = NULL;
	for (fs = sptFILESPTI; fs < sptNSPT; fs++) {
		if (fs->spt_proccount) continue;
		if (fs->spt_share) continue;
		if (fs->spt_flags & (SPT_INIT | SPT_WANT)) continue;
		if (fs->spt_inode == NULL) {
			fs->spt_flags = (SPT_INIT | SPT_NOEIA);
			return(fs);	/* return allocated entry */
		}
		if (fs->spt_inode->i_mode & ISVTX) continue;
		if ((bfs == NULL) || (fs->spt_age <= bfs->spt_age)) bfs = fs;
	}
	if (bfs == NULL) {
		return(BADSPT);		/* no freeable texts */
	}
	xlock(bfs);
	xblast(bfs);			/* free the text */
	return(NULL);			/* then say to retry */
}



/* Routine to check if a shared text entry is being used, and if not
 * to mark its spt.  This is used to indicate to the page replacement
 * algorithm that pages for this text are good choices to swap.
 */
xdorm(fspti)
{
	register struct spt	*fs;	/* file spt pointer */
	register struct	inode	*ip;	/* inode pointer */

	if (!SPTISFILE(fspti)) panic("xdorm");
	fs = &spt[fspti];
	ip = fs->spt_inode;
	fs->spt_age = ++sptagecounter;
	fs->spt_flags &= ~SPT_XSRSS;
	if ((ip != NULL) && (fs->spt_proccount == 0) && (fs->spt_share == 0)
		&& (fs->spt_mempages > 0) && ((ip->i_mode & ISVTX) == 0)) {
			fs->spt_flags |= SPT_XSRSS;
	}
}

/* Routine to attempt to free up a shared text entry.  Clear the page table
 * and the spt entry if the share count is zero, and noone wants this text.
 * Returns nonzero if no text was freed.
 */
xrele(ip)
	register struct	inode	*ip;	/* inode of file */
{
	register struct	spt	*fs;	/* file spt pointer */

	if ((ip == NULL) || (ip->i_mode & ISVTX)) return(1);
	for (fs = sptFILESPTI; ; fs++) {	/* search for text entry */
		if (fs >= sptNSPT) return(1);
		if (fs->spt_inode == ip) break;
	}
	if (fs->spt_share || fs->spt_proccount || (fs->spt_flags & SPT_INIT)) {
		return(1);		/* text is busy */
	}
	xlock(fs);
	xblast(fs);
	return(0);
}



/* Remove a file inode from an spt entry.  This will remove the text
 * status of the file if this was the last use of the file.  This works
 * for either process or text type spt entries.  If a text entry, the spt
 * entry must be locked and unused.
 */
xblast(fs)
	register struct	spt	*fs;	/* spt entry to remove file from */
{
	register struct	inode	*ip;	/* inode of entry */
	register int	flags;		/* flags to clear */
	register int	istext;		/* true if this is a text entry */

	if ((fs == NULL) || (fs->spt_inode == NULL)) return;
	istext = (fs >= sptFILESPTI);
	if (istext && (((fs->spt_flags & SPT_INIT) == 0) ||
		fs->spt_share || fs->spt_proccount)) {
			panic("xblast");
	}
	ip = fs->spt_inode;
	flags = ITEXT;
	if (istext) {			/* if a text entry, clear it out */
		clearptetable(fs - spt);
		fs->spt_inode = NULL;
		xunlock(fs);
	} else {			/* if a process, look for other users */
		fs->spt_inode = NULL;
		for (fs = spt; (flags && (fs < sptNSPT)); fs++) {
			if (fs->spt_inode == ip) flags = 0;	/* still used */
		}
	}
	ip->i_flag &= ~flags;		/* possibly clear text status */
	if (ip->i_flag & ILOCK)		/* decrement inode use count */
		ip->i_count--;
	else
		iput(ip);
}

/* Attempt to lock a text segment so that it is not allocated or deallocated
 * while it is being modified.   Returns nonzero if the text was already
 * locked (after waiting for it to be unlocked), or zero if we locked it.
 */
xlock(fs)
	register struct	spt	*fs;
{
	if (fs->spt_flags & SPT_INIT) {
		fs->spt_flags |= SPT_WANT;
		sleep((caddr_t)fs, PZERO-1);
		return(1);
	}
	fs->spt_flags |= SPT_INIT;
	return(0);
}


/* Unlock a shared text entry and wake up anyone who was waiting on it. */
xunlock(fs)
	register struct	spt	*fs;
{
	if ((fs->spt_flags & SPT_INIT) == 0) panic("xunlock");
	if (fs->spt_flags & SPT_WANT)
		wakeup((caddr_t)fs);
	fs->spt_flags &= ~(SPT_INIT | SPT_WANT);
}



/* Unmounting a file system requires all inodes for that device
 * be freed.  Since a process may be referencing an inode we
 * must try to free all the spts that came from that file system.
 * we get called from sumount()
 */
xunmount(dev)
	dev_t dev;
{
	register struct	spt	*fs;	/* file spt pointer */

	for (fs = sptFILESPTI; fs < sptNSPT; fs++) {
		if ((fs->spt_inode != NULL) &&
		    (fs->spt_inode->i_un.i_rdev == dev))
			xrele(fs->spt_inode);
	}
}

/* Initialize the page tables for paging from a text file, by scouring
 * up the indirect blocks in order.  Address zero and up of the text file
 * is mapped to the text file.  The last page of data is special in that
 * it contains symbol table information, and so the remainder of the page
 * must be zeroed by hand.  Returns nonzero on failure.
 */
xsetptetable(spti, count)
	spt_t	spti;			/* spt index of file */
	size_t count;			/* amount to be mapped */
{
	register struct inode *ip;	/* inode for text file */
	register daddr_t *pp;		/* pointer to disk addresses */
	register daddr_t blkno;		/* block address on disk */
	register daddr_t blkindex;	/* block index into text file */
	register int addr;		/* virtual address in text */
	struct	buf	*bp;		/* buffer holding file addresses */
	int	fcount;			/* count remaining in last page */
	struct	cst	*pg;		/* cst pointer for page */

	if ((spti >= nspt) || ((ip = spt[spti].spt_inode) == NULL)) {
		return(1);
	}
	fcount = count & MCOFSET;	/* remember fractional amount */
	blkindex = 1;			/* start with block after header */
	pp = &ip->i_un.i_addr[1];	/* point at in-memory inodes */
	bp = NULL;
	addr = 0;
	while (count > 0) {
		if ((addr & BMASK) == 0) {	/* need new file block */
			if ((blkindex % NINDIR) == (NADDR - 3)) {
				if (bp) brelse(bp);
				bp = vbmap(ip, blkindex);
				if (bp == NULL) {
					return(1);	/* hole in file */
				}
				pp = &bp->b_un.b_daddr[0]; /* new addresses */
			}
			blkno = *pp++;		/* get next disk address */
			if (blkno == 0) {
				if (bp) brelse(bp);
				return(1);	/* hole in file */
			}
			blkindex++;
		}
		setmap(spti, addr, PTEFILE(blkno, PG_RWRO));
		addr += MCSIZE;
		count -= MCSIZE;
		blkno++;
	}
	if (bp) brelse(bp);
	if (fcount) {		/* treat last page special (has symbol table) */
		addr = pagein(spti, addr - MCSIZE, -1);	/* read in last page */
		if (addr < 0) return(1);
		pg = addrtocst(addr);
		pg->cst_backup = 0;			/* make data private */
		pg->cst_file = 0;
		bzero(addr + fcount, MCSIZE - fcount);	/* zero rest of page */
	}
	return(0);
}

/*
 * Vbmap returns a block full of indirect pointers for a given block offset
 * in a file.  It returns 0 if a missing address block was encountered.
 */
struct buf *
vbmap(ip, bn)
register struct inode *ip;
daddr_t bn;
{
	register i;
	struct buf *bp;
	int j, sh;
	daddr_t nb;
	dev_t dev = ip->i_dev;

	if (bn < NADDR-3)
		panic("vbmap");
	if (ip == 0)
		return (0);

	/*
	 * addresses NADDR-3, NADDR-2, and NADDR-1
	 * have single, double, triple indirect blocks.
	 * the first step is to determine
	 * how many levels of indirection.
	 */
	sh = 0;
	nb = 1;
	bn -= NADDR-3;
	for (j = 3; j > 0; j--) {
		sh += NSHIFT;
		nb <<= NSHIFT;
		if(bn < nb)
			break;
		bn -= nb;
	}
	if (j == 0)
		goto noblk;

	/*
	 * fetch the address from the inode
	 */
	nb = ip->i_un.i_addr[NADDR-j];

	/*
	 * fetch through the indirect blocks
	 */
	for (;;) {
		if (nb == 0)
			return (0);
		bp = bread(dev, nb);
		if (bp->b_flags & B_ERROR) {
			brelse(bp);
			goto noblk;
		}
		if (j == 3)
			break;
		j++;
		sh -= NSHIFT;
		i = (bn>>sh) & NMASK;
		nb = bp->b_un.b_daddr[i];
		brelse(bp);
		if (nb == 0)
			goto noblk;
	}
	return (bp);

noblk:
	return ((struct buf *)0);
}
