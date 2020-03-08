# ifndef NOSCCS
static char *sccsid = "@(#)vmswap.c	3.61	9/4/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/buf.h"
#include "../h/inode.h"
#include "../h/conf.h"
#include "../h/pte.h"
#include "../h/reg.h"
#include "../h/devvm.h"
#include "../h/vm.h"
#include "../h/vmmeter.h"
#include "../h/vmswap.h"
#include "../h/mount.h"

long	swapouts;		/* number of swapout operations */
long	swapins;		/* number of swapin operations */
long	swapretries;		/* times we had to retry insertions */
int	swapused;		/* number of swapping pages used */
extern	int	totalpages;	/* total number of pages of memory */
extern	int	topsys;		/* top address of kernel */
extern	int	pagesfree;	/* total number of free pages of memory */
extern	int	limrssseen;	/* some process may limit its memory use */
extern	int	desfree;	/* desirable amount of free memory */
struct	cst	*cstrover;	/* rover for swapout routine */
extern	char	*swapbittable;	/* bit table for swap allocation */
extern	dev_t	swapdev;	/* swap device */
extern	int	nswapdev;	/* number of swap devices */
struct	swap	*getswapbuf();	/* routine to find swapping buffer */

/*
 * Swap a process in.
 */
swapin(p)
	register struct proc *p;
{
	int s;

	swapinu(p - proc);	/* swap in u. area for this process */
	s = spl5();
	p->p_flag |= SLOAD;
	if (p->p_stat == SRUN)
		setrq(p);
	splx(s);
	p->p_time = 0;
	cnt.v_swpin++;
	return (1);
}


/*
 * Look for a dormant (unused) text segment and swap it out.
 * We find all unused text segments, and, of these, swap the one with
 * the largest number of in-memory pages.
 */
swapdormtext()

{
	register struct spt *fs;
	register int savesize;
	register int saveidx;
	register int idx;

	savesize = 0;
	for (idx = filespti; idx < nspt; idx++) {
		fs = &spt[idx];
		if (fs->spt_inode == 0) continue;
		if (fs->spt_proccount || fs->spt_share) continue;
		if (fs->spt_mempages > savesize) {
			savesize = fs->spt_mempages;
			saveidx = idx;
		}
	}
	if (savesize > 0) {
		swapout(saveidx, 0);
		cnt.v_tswpout++;
	}
}

/*
 * Swap out process or text segment
 *
 * Parameter advice is an indication of how much memory the swapout should
 * free.  The swapout may actually create more or less memory than this.
 * In practice, we try to create desfree more pages to give us some breathing
 * room.
 *
 * This parameter is usually set when called from the schedular when
 * space is needed to swapin a process, and it's set to the number of pages
 * needed by that process.  The idea is to avoid completely swapping out
 * some huge program just because some small program like cron wants to run.
 *
 * If advice is zero, then the entire process or text is swapped out.
 */
swapout(spti, advice)
	register spt_t	spti;		/* spt index of process or text */
	int	advice;
{
	register struct proc *p;
	register struct user *xu;
	register pte *curpte;
	register pte ptedata;
	pte	*endpte;
	long	procid;			/* index into proc */
	int	s;
	long	addr;			/* virtual address */
	struct cst *pg;

	if ((spti == SYSSPTI) || (spti < 0) || (spti > nspt))
		panic("sptiswapout");

	if (advice) advice += desfree;	/* give a little breathing room */
	if (advice > spt[spti].spt_mempages) {
		advice = 0;		/* just swap the whole thing */
	}

	if (SPTISPROC(spti)) {
		procid = spti;
		p = &proc[procid];
		if (p->p_spti != spti) {	/* unusual case */
			for (p = proc;; p++) {
				if (p >= procNPROC) panic("noprocswapout");
				if (p->p_spti == spti) break;
			}
			procid = p - proc;
		}
		if (((p->p_flag & SLOAD) == 0) ||
		    ((p == u.u_procp) && (advice == 0))) {
			panic("badprocswapout (p %x up %x flg %x adv %d)",
				p, u.u_procp, p->p_flag, advice);
		}

		if (advice == 0) {
			/* remember current core sizes */
			xu = (struct user *)(BOTXU + (procid)*USIZE);
			xu->u_vm.vm_nswap++;
			cnt.v_swpout++;
			p->p_tdsize = xu->u_tsize + xu->u_dsize;
			p->p_ssize = TOPUSER - xu->u_ar0[RSP];
			if (p->p_ssize < 0) p->p_ssize = 0;
			if (p->p_ssize + p->p_tdsize > USERCORE)
				p->p_ssize = USERCORE - p->p_tdsize;

			s = spl5();
			p->p_flag &= ~SLOAD;
			p->p_time = 0;
			if (p->p_stat == SRUN) remrq(p);
			splx(s);

			/* eventually, unlock pte1 */
			uunlock(procid);		/* swap u. area */
		}
		else cnt.v_pswpout++;
	}

	if (advice == 0) advice = totalpages;	/* set it to a big number */
	for (addr = 0; (addr = scanmap(spti, addr, 1)) >= 0; addr += MCSIZE) {
		curpte = pageinpt(spti, addr, 0, PTLEVEL2);
		if (curpte == NULL) panic("badpageswapout");
		ptedata = *curpte;
		if (PTETYPE(ptedata) != PT_MEM) continue;
		pg = addrtocst(PTEMEM_ADDR(ptedata));
		if ((pg->cst_type == PTY_DATA) && (pg->cst_lock == 0) &&
			((pg->cst_backup>0) || (swapused<(nswap/MCPAGES)-5))) {
				swappage(PTEMEM_ADDR(ptedata));
		if (--advice <= 0) goto done;
		}
	}

	curpte = spt[spti].spt_pte1;
	endpte = curpte + (NPTEPG * 2);
	while (curpte < endpte) {
		ptedata = *curpte;
		if (ptedata == PTENULL) {
			curpte = skippte(curpte, endpte);
			continue;
		}
		if (PTETYPE(ptedata) == PT_MEM) {
			pg = addrtocst(PTEMEM_ADDR(ptedata));
			if ((pg->cst_type == PTY_PTE2) && (pg->cst_lock == 0)
				&& ((pg->cst_backup > 0) ||
					(swapused < (nswap/MCPAGES)-5))) {
						swappage(PTEMEM_ADDR(ptedata));
			}
			if (--advice <= 0) break;
		}
		curpte += MCPAGES;
	}

done:	if(runout) {
		runout = 0;
		wakeup((caddr_t)&runout);
	}
	return(1);
}

/* Insert a physical memory page into one of the swapping lists.  Dir is one
 * of B_READ or B_WRITE.  The memory page cannot already be in transit.
 * This routine assumes that the owning page table has the pte type of PT_IOP
 * so that all users will wait for the swapping to complete.  Therefore this
 * routine returns immediately after queueing the I/O.  If the swap buffer
 * is available, disk IO is started immediately.  Much of this routine runs
 * with interrupts enabled, and so must be prepared for doswapio to change
 * the swapping lists out from under us.  This is done by noticing that
 * sw_inval has changed.
 */
swapinsert(physaddr, dir)
{
	register struct	cst	*pg;		/* cst entry of page to swap */
	register struct	cst	*cpg;		/* cst entry of current page */
	register struct	cst	*npg;		/* cst entry of next page */
	register struct	swap	*sw;		/* swapping list header */
	register long	backup;			/* desired swapping sector */
	long	oldinval;			/* old sw_inval value */
	int	s;

	pg = addrtocst(physaddr);
	if ((pg->cst_intrans) || (pg->cst_next != NULL)) {
		panic("swapinsert");
	}
	pg->cst_intrans = 1;
	sw = getswapbuf(pg, dir);	/* find proper swap buffer */
	backup = pg->cst_backup;
	if (backup == 0) {		/* assign swapping address */
		if ((dir == B_READ) || (pg->cst_file)) {
			panic("swapnobackup");
		}
		backup = swapalloc();
		pg->cst_backup = backup;
	}

retry:	oldinval = sw->sw_inval;
	cpg = NULL;
	while (1) {			/* search for page to insert after */
		npg = sw->sw_queue;
		if (cpg) npg = cpg->cst_next;
		if ((npg == NULL) || (npg->cst_backup > backup)) break;
		cpg = npg;
		if (oldinval != sw->sw_inval) {
			swapretries++;
			goto retry;
		}
	}
	s = spl5();		/* now verify our choice with interrupts off */
	if (oldinval != sw->sw_inval) {	/* missed out, must restart */
		splx(s);
		swapretries++;
		goto retry;
	}
	if (cpg) {			/* insert after found page */
		pg->cst_next = cpg->cst_next;
		cpg->cst_next = pg;
	} else {			/* or before first page */
		pg->cst_next = sw->sw_queue;
		sw->sw_queue = pg;
	}
	sw->sw_count++;
	doswapio(sw);			/* start swapping if necessary */
	(void) splx(s);
}

/* Here to start the next swapping operation.  If swapping is already in
 * progress, the call is ignored.  This routine must be called while
 * running at spl5(), since it is callable from interrupt level also.
 * This routine looks for a large disk-sector-contiguous transfer, and
 * transfers as many pages in one operation as possible.
 */
doswapio(sw)
	register struct	swap	*sw;	/* swapping header */
{
	register struct	cst	*pg;	/* current cst entry */
	register long	backup;		/* current backup address */
	register long	*swapptr;	/* pointer into swapping table */
	register struct	buf	*bp;	/* buffer pointer */
	long	*endswapptr;		/* end of swap table */
	struct	cst	*ppg;		/* previous cst entry */
	int	origbackup;		/* original backup address */

	pg = sw->sw_queue;
	if ((pg == NULL) || (sw->sw_active)) {
		return;			/* nothing to do, or already busy */
	}
	sw->sw_inval++;			/* invalidate swapinsert */
	bp = &sw->sw_buf;
	if (bp->b_flags & B_BUSY) panic("doswapio");
	bp->b_flags = (B_BUSY | B_PHYSMAP | sw->sw_dir);
	swapptr = sw->sw_pages;
	endswapptr = swapptr + SWAPSIZE;
	backup = pg->cst_backup;
	origbackup = backup;
	while ((pg) && (pg->cst_backup==backup) && (swapptr<endswapptr)) {
		setpte(swapptr, (csttopg(pg) << MCSHIFT) | PG_V);
					/* put addresses of page into table */
		swapptr += MCPAGES;
		backup++;
		ppg = pg;
		pg = pg->cst_next;
	}
	sw->sw_active = sw->sw_queue;	/* move pages onto active list */
	sw->sw_queue = pg;		/* remaining pages stay queued */
	ppg->cst_next = NULL;		/* terminate active list */
	bp->b_bcount = MCSIZE * (backup - origbackup);
	bp->b_blkno = MCPAGES * origbackup;
	bp->b_un.b_addr = (caddr_t) sw->sw_pages;
	(*bdevsw[major(bp->b_dev)].d_strategy)(bp);

	/* Now swapint will be called by bio when IO completes */
}

/* Called from interrupt level by bio when a swapping operation is complete
 * to remove a number of pages from a swapping list.  As part of this removal,
 * the owning page table pte entries are changed from type PT_IOP to PT_MEM
 * or PT_DISK and the lock count for the pages are decremented.  This allows
 * users who were sleeping on the completion of the swap to proceed.  If the
 * page was swapped out, the memory page is freed.  Finally, if pages remain
 * on the transfer queue, another swapping operation is started.
 * WARNING:	This routine assumes that the first data in the swap structure
 *		is the swap buffer.
 */
swapint(bp)
	register struct	buf	*bp;		/* buffer which finished */
{
	register struct	swap	*sw;		/* finished swap header */
	register struct	cst	*pg;		/* cst entry of finished page */
	register spt_t	spti;			/* spt index of owner */
	register pte	ptedata;		/* current pte data */
	long	virtaddr;			/* virtual address */
	long	backup;				/* backup address */
	int	level;				/* pte level */
	long	count;				/* page count */

	sw = (struct swap *) bp;	/* get swap header (see warning) */
	if ((sw<swapbuf) || (sw>=swapbufNSWAPBUF) || (sw->sw_active==NULL)) {
		panic("swapint");		/* verify proper buffer */
	}
	if ((bp->b_flags & B_ERROR) || bp->b_resid) {
		panic("IO err in swap");
	}
	count = 0;
	backup = sw->sw_active->cst_backup;
	while (sw->sw_active) {
		pg = sw->sw_active;		/* unlink next page */
		sw->sw_active = pg->cst_next;
		pg->cst_next = NULL;
		if ((pg->cst_intrans == 0) || (pg->cst_lock == 0) ||
			(pg->cst_backup != backup)) {
				panic("swapremove");
		}
		level = PTLEVEL1;
		if (pg->cst_type != PTY_PTE2) level = PTLEVEL2;
		spti = pg->cst_spti;
		virtaddr = (pg->cst_sptp << MCSHIFT);
		ptedata = getpgmap(spti, virtaddr, level);
		if (PTETYPE(ptedata) != PT_IOP) {
			panic("swapbadpte %x",(long)ptedata);
		}
		changemap(spti, virtaddr, PG_TYPE, PT_MEM, level);
					/* change map entry back to MEM */
		pg->cst_intrans = 0;
		unlockpage(pg);
		if (sw->sw_dir == B_WRITE) {	/* change MEM pte to DISK */
			pg->cst_backup = 0;	/* prevent swap deallocation */
			setpgmap(spti, virtaddr, PTEDISK(pg->cst_file,
				backup, PTEPROT(ptedata)), level);
		}
		if (pg->cst_wanted) {		/* wake up whoever wants page */
			pg->cst_wanted = 0;
			wakeup((caddr_t) pg);
		}
		count++;
		backup++;
		sw->sw_count--;
	}
	if ((count * MCSIZE) != bp->b_bcount) panic("swapbadcount");
	if (bp->b_flags & B_READ) {		/* do accounting */
		swapins++;
		cnt.v_pgin += count;
	} else {
		swapouts++;
		cnt.v_pgout += count;
	}
	bp->b_flags &= ~(B_BUSY | B_DONE);	/* buffer not busy anymore */
	doswapio(sw);				/* start more swapping */
}

/* Find a convenient memory page and swap it out to create a free page.
 *
 * The following "goodness" definitions are used to rate a page of physical
 * memory as a choice for being paged out.
 *
 * The algorithm scans memory until it finds a page which has an acceptable
 * "goodness" value, and pages out that page.  If a complete scan of memory
 * reveals no such page, the algorithm picks the one with the best goodness
 * found.
 */

#define	MINGDNSS	1	/* page is legally swappable */
#define REFGDNSS	4	/* page hasn't been referenced recently */
#define	MODGDNSS	3	/* page hasn't been modified and is backed up */
#define SHAREGDNSS	1	/* page is not shared */
#define KSPTGDNSS	1	/* page owned by spt taking too much memory */
#define OKGDNSS		(MINGDNSS+REFGDNSS+1)	/* need to search no further */

swapoutpages()
{
	register struct cst *pg;	/* for the cst scan */
	register pte	*curpte;	/* pte pointer for page */
	register int	cnt;		/* counter */
	register int	goodness;	/* how good a page to swap */
	register int	level;		/* data page or pte2 page */
	struct	cst	*bestpg;	/* best swappable page */
	pte	ptedata;		/* pte data for page */
	int	bestgood;		/* goodness of best swappable page */
	int	firstpass;
	struct	spt	*sp;
	char	noxsspt;

	if (pagesfree) return;
	pg = cstrover;
	bestpg = NULL;
	bestgood = 0;
	cnt = totalpages;
	firstpass = 0;
	noxsspt = 1;

	/* find all spt's which are too big */
	if (limrssseen) sp = spt;
	else sp = sptFILESPTI;
	while (sp < sptNSPT) {
		if ((sp->spt_flags & SPT_XSRSS) && sp->spt_mempages) {
			noxsspt = 0;
			break;
		}
		sp++;
	}

	while (1) {
		if (--cnt < 0) {	/* see if anyone can be swapped out */
			if (bestpg) break;
			if ((firstpass++ == 0) &&
			    (hardswap(u.u_procp, 1, 1, 0) == 0)) {
				if (pagesfree) return;
			 	cnt = totalpages;	/* try another pass */
				pg = cstrover;
			}
			else panic("noswappage");
		}
		pg++;
		if (pg >= cstNCST) pg = addrtocst(topsys);
		if (pg->cst_lock) continue;
		if (pg->cst_type == PTY_PTE2) {
			level = PTLEVEL1;
		}
		else	if ((pg->cst_type == PTY_DATA) ||
			    (pg->cst_type == PTY_SYSTEM)) {
				level = PTLEVEL2;
			}
			else continue;
		curpte = pageinpt(pg->cst_spti, (pg->cst_sptp << MCSHIFT),
					0, level);
		if (curpte == NULL) panic("badpageforswap");
		ptedata = getpte(curpte);

		/*
		 * Check to see if this page has been referenced.
		 * If so, mark it as unreferenced for next pass.
		 */
		goodness = REFGDNSS + MINGDNSS;
		if (ptedata & PG_REF) {
			andpte(curpte, ~PG_REF);
			goodness = MINGDNSS;
		}
		if ((level == PTLEVEL2) && ((ptedata & PG_MOD) == 0) &&
				(pg->cst_backup > 0)) {
			goodness += MODGDNSS;
		}
		if (pg->cst_share <= 1) goodness += SHAREGDNSS;
		if (noxsspt || (spt[pg->cst_spti].spt_flags & SPT_XSRSS)) {
			goodness += KSPTGDNSS;
		}
		if (goodness > bestgood) {
			bestgood = goodness;
			bestpg = pg;
		}
		if (goodness >= OKGDNSS) break;
	}
	cstrover = bestpg;
	swappage(csttopg(bestpg) << MCSHIFT);	/* swap this page out */
	freepagewait();			/* and wait for any free page */
}

/* Write a page of memory to the swapping space.  This will find all sharers
 * of the memory page, convert their pte entries back into PT_SPT pointers,
 * then move the owner's page to the swapping space.
 */
swappage(physaddr)
	register unsigned long 	physaddr;	/* physical address of page */
{
	register struct cst *pg;	/* cst entry for page */
	register spt_t	spti;		/* owning spt index */
	register unsigned long virtaddr;/* owning virtual address */
	register pte	ptedata;	/* pte entry */
	int	block;			/* block number for IO */
	int	level;

	pg = addrtocst(physaddr);
	level = PTLEVEL1;
	if (pg->cst_type != PTY_PTE2) level = PTLEVEL2;
	if (pg->cst_lock) panic("swaplockpage");
	if (pg->cst_share > 1) unsharemem(physaddr);	/* remove sharers */
	spti = pg->cst_spti;
	virtaddr = (pg->cst_sptp << MCSHIFT);
	block = pg->cst_backup;
	ptedata = getpgmap(spti, virtaddr, level);
	if ((PTETYPE(ptedata) != PT_MEM) || (PTEMEM_ADDR(ptedata)!=physaddr)) {
		panic("swappage (pte %x)", ptedata);
	}
	if (block && (level == PTLEVEL2) && ((ptedata & PG_MOD) == 0)) {
		pg->cst_backup = 0;	/* keep swap space allocated */
		setmap(spti, virtaddr,
			PTEDISK(pg->cst_file, block, PTEPROT(ptedata)));
		cnt.v_fpgout++;
		return;
	}
	lockpage(pg);
	changemap(spti, virtaddr, PG_TYPE, PT_IOP, level);
	swapinsert(physaddr, B_WRITE);
}

/* Find and initialize the swapping buffer associated with a memory page.
 * The first nswapdev entries in the swapbuf table are reserved for the
 * swap device.  The remainder are associated 1-1 with the mount table
 * devices.  This is safe because a file system cannot be unmounted while
 * a text file is in use.  Writes are only allowed for the swap device.
 * Dir is one of B_READ or B_WRITE.
 */
struct swap *
getswapbuf(pg, dir)
	register struct	cst	*pg;	/* memory page of interest */
{
	register struct	swap	*sw;	/* swap buffer pointer */
	register struct	mount	*mp;	/* mount table pointer */
	register struct	inode	*ip;	/* inode pointer */
	register dev_t	dev;		/* device */

	sw = swapbuf;			/* assume swap device first */
	dev = swapdev;
	if (pg->cst_file) {		/* if not, do more work */
		if (dir == B_WRITE) panic("swaptext");
		ip = spt[pg->cst_spti].spt_inode;	/* get text file */
		if (ip == NULL) panic("swapnotext");
		dev = ip->i_dev;
		for (mp = mount;; mp++) {	/* find device in mount table */
			if (mp >= mountNMOUNT) panic("swapnomount");
			if ((mp->m_bufp != NULL) && (mp->m_dev == dev)) break;
		}
		sw += ((2 * nswapdev) + (mp - mount));	/* increment by index */
	}
	if (dir == B_WRITE) sw++;	/* use odd entry if writing */
	sw->sw_dir = dir;		/* initialize data */
	sw->sw_buf.b_dev = dev;
	return(sw);
}

/* Routine to wait until swapping I/O is completed for a physical memory
 * address.  After the wait, the caller must validate that the page still
 * belongs to the desired page table, since someone else might have removed it.
 */
swapwait(physaddr)
{
	register struct	cst	*pg;	/* cst entry for physical page */
	int	s;

	pg = addrtocst(physaddr);
	s = spl6();
	while (pg->cst_intrans) {
		pg->cst_wanted = 1;
		sleep((caddr_t) pg, PSWP);
	}
	(void) splx(s);
}


/* Routine called to kill a process due to some swapping problem.
 * If mesg is nonzero, then it is typed as the reason for the kill,
 * otherwise lack of swapping space is assumed.
 */
swapkill(p, mesg)
	register struct	proc *p;	/* process to kill */
	register char *mesg;		/* message to type */
{
	if (mesg == NULL) mesg = "no swap space";
	printf("pid %d (%s): killed due to %s\n",
		p->p_pid, p->p_infoname, mesg);
	uprintf("sorry, pid %d (%s) was killed due to %s\n",
		p->p_pid, p->p_infoname, mesg);
	psignal(p, SIGKILL);
}

/* Allocate a page of swapping space */
swapalloc()
{
	long	page;

	page = bitalloc(swapbittable, (nswap/MCPAGES), 0);
	if (page < 0) panic("noswapspace (avail %d)", availpages);
	swapused++;
	availpages--;
	return(page);
}



/* Deallocate a page of swapping space */
swapfree(page)
	unsigned long	page;
{
	if (page >= (nswap/MCPAGES)) panic("badswappage");
	if (bitfree(swapbittable, page)) panic("freeswaptwice");
	swapused--;
	availpages++;
}
