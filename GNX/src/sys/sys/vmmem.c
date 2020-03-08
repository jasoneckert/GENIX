# ifndef NOSCCS
static char *sccsid = "@(#)vmmem.c	4.17	9/3/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

/* Virtual memory module for National DS/16000 system	*/
/*	David I. Bell	Laura Neff     6/28/82		*/

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/psr.h"
#include "../h/devpm.h"
#include "../h/mmu.h"
#include "../h/vm.h"
#include "../h/vmmeter.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/vlimit.h"

struct	cst	*cstptr[MEMCHKS];
				/* ptrs into cst at MEMCHUNK boundaries */
int	mempgs[MEMCHKS];		/* existent memory (MEMCHUNK pieces) */
int	kalloccount;			/* pages allocated by kernel */
struct	cst	*freepagelist;		/* list of free pages */
struct	cst	*zeropagelist;		/* list of zeroed pages */
int	zerofree;			/* number of zeroed free pages */
int	pagesfree;			/* free pages (includes zerofree) */
int	zeroprogress;			/* address of page being zeroed */
int	topmem;				/* top of physical memory */
int	botalloc;			/* beginning of mem allocation area */
extern	int	nkalloc;		/* amount of allocation */
extern	char	*kalloctable;		/* bit table for allocation */

/* Allocate a page from one of the free lists.  If zeroflag is nonzero,
 * a zeroed page is allocated, otherwise an arbitrary page is returned.
 * One of three return values can occur, as follows:
 *   positive:	The page was successfully allocated without any paging
 *		or rescheduling.  The return value is the physical
 *		address of the allocated page.
 *   zero:	There were no immediately available free pages, and so
 *		some arbitrary page was swapped out to make another free
 *		page, and so rescheduling occurred.  Sleeping has already
 *		been done to wait for another free page, so the caller can
 *		immediately retry to get another free page.  However, the
 *		caller must be aware that other data could have changed
 *		while he slept (in particular pte tables and entries).
 *		So the caller should recalculate his actions.
 *   negative:	There is no more free memory or swap space available.  It
 *		is illegal for this error to occur for other than PTY_DATA
 *		type pages, since higher level routines should have reserved
 *		extra pages for PTY_PTE2 pages.
 *
 * This routine is usually called by one of the macros allocpage or
 * alloczeropage, which are defined as this routine with zeroflag set
 * appropriately.
 */
alloclistpage(spti, addr, type, zeroflag)
	register spt_t	spti;		/* spt which will own this page */
	register unsigned long addr;	/* address in address space */
{
	register struct	cst	*pg;	/* cst pointer of allocated page */
	register unsigned long pagadr;	/* physical address of page */
	register struct	cst	**list;	/* address of list header */
	int	s;

	if ((spti >= nspt) || (addr >= MAXADDRS) || (type == PTY_FREE)) {
		panic("alloclistpage");
	}
	s = spl6();			/* no interrupts while grabbing page */
	if ((availpages <= 0) && (type == PTY_DATA)
		&& (spt[spti].spt_reserved <= 0)) {
			splx(s);
			return(-1);	/* no more swap space */
	}
	if (pagesfree <= 0) {
		splx(s);
		swapoutpages();		/* force free page and return retry */
		return(0);
	}
	list = &freepagelist;		/* select appropriate list */
	if ((zeroflag) || (*list == NULL)) list = &zeropagelist;
	if (*list == NULL) list = &freepagelist;
	pg = *list;
	if ((pg == NULL) || (pg->cst_lock) || (pg->cst_type != PTY_FREE)) {
		panic("freepageisnt");
	}
	*list = pg->cst_next;		/* remove page from list */
	if (list == &zeropagelist) zerofree--;
	pagesfree--;
	availpages--;
	if ((type == PTY_DATA) && (spt[spti].spt_reserved > 0)) {
		spt[spti].spt_reserved--;
	}
	pg->cst_lock = 1;
	pg->cst_type = type;
	pg->cst_spti = spti;
	pg->cst_sptp = (addr >> MCSHIFT);
	pg->cst_backup = 0;
	pg->cst_file = 0;
	pg->cst_share = 0;
	pg->cst_next = NULL;
	splx(s);
	pagadr = (csttopg(pg) << MCSHIFT);
	if ((zeroflag) && (list != &zeropagelist)) {
		clearpage(pagadr);
	}
	return(pagadr);
}

/* Routine called when processor is otherwise idle to zero a free memory
 * page.  Each time through this routine will clear a few more words of
 * the selected memory page.  This routine is called while spl6().
 */
idleclearpage()
{
	register int	pagadr;		/* address of free page */
	register struct cst	*pg;	/* cst entry for page */

	if (zeroprogress <= 0) {	/* find new page to be zeroed */
		if (pagesfree < 10) return;
		pg = freepagelist;
		if (pg == NULL) return;
		if (pg->cst_type != PTY_FREE) panic("idleclearpage");
		freepagelist = pg->cst_next;
		pagesfree--;
		availpages--;
		zeroprogress = (csttopg(pg) << MCSHIFT); /* zero this page */
		return;
	}
	idleclearbytes();			/* clear a few bytes */
	pagadr = zeroprogress;
	if (pagadr & MCOFSET) return;		/* not done with page yet */

	/* Now have a newly zeroed page to add to zeroed page list */

	zeroprogress = 0;
	pagadr -= MCSIZE;
	pg = addrtocst(pagadr);
	pg->cst_next = zeropagelist;
	zeropagelist = pg;
	zerofree++;
	availpages++;
	if (pagesfree++ == 0) wakeup((caddr_t) &pagesfree);
	cnt.v_pgszrod++;			/* a new zeroed page */
}

/* Wait until a free memory page exists.  If a memory page is already
 * available, zero is returned immediately.  Otherwise, sleeping is done
 * until a free page exists, and then nonzero is returned.  However, there
 * is no guarantee that the free page actually exists, since several
 * users can wait for the same free page.
 */
freepagewait()
{
	int	s;

	if (pagesfree > 0) return(0);
	s = spl6();
	while (pagesfree <= 0)
		sleep((caddr_t) &pagesfree, PSWP);
	splx(s);
	return(1);
}



/* Free a physical memory page.  This is illegal if the page is locked, or
 * if the share count is not equal to zero.  The owning page table's lock
 * count is decremented.  The page should only be freed by the owner.
 */
freepage(physaddr)
	register long physaddr;		/* physical address of page */
{
	register struct	cst *pg;	/* cst entry for page */
	register struct cst *ownpg;	/* cst entry for owning page */
	register pte	*pteptr;	/* pointer to pte entry */
	register int	owntype;	/* owning page type */
	int	s;

	pg = addrtocst(physaddr);
	if ((pg->cst_lock) || (pg->cst_share)) panic("freeusedpage");
	switch (pg->cst_type) {
		case PTY_DATA:
			pteptr = pageinpt(pg->cst_spti,
				(pg->cst_sptp << MCSHIFT), 0, PTLEVEL2);
			owntype = PTY_PTE2;
			break;

		case PTY_PTE2:
			pteptr = spt[pg->cst_spti].spt_pte1 +
					GETVI1(pg->cst_sptp<<MCSHIFT);
			owntype = PTY_PTE1;
			break;

		default:
			panic("freebadpagetype");
	}
	if (pteptr == NULL) panic("nopagepte");
	ownpg = addrtocst(pteptr);
	if (ownpg->cst_type != owntype) panic("freebadowner");
	unlockpage(ownpg);			/* unlock owning page table */
	if (pg->cst_backup && (pg->cst_file == 0)) {
		swapfree(pg->cst_backup);	/* free swap page */
	}
	pg->cst_backup = 0;
	pg->cst_file = 0;
	pg->cst_type = PTY_FREE;
	pg->cst_spti = SYSSPTI;
	pg->cst_sptp = (physaddr << MCSHIFT);
	s = spl6();		/* no interrupts while adding to free list */
	pg->cst_next = freepagelist;
	freepagelist = pg;
	availpages++;
	if (pagesfree++ == 0) wakeup((caddr_t) &pagesfree);
	splx(s);
}

/* Clear all sharing associated with a physical memory address.  Needed
 * when a shared text page must be removed from memory.  All page maps which
 * reference the memory page are found, and their pointers converted back
 * into spt-type ptes (all such page maps will be resident in memory).  All
 * of the sharer's PG_REF and PG_MOD bits are OR'd into the owner's pte entry.
 *
 * NOTICE:  This routine currently assumes all sharing is done 1-1 (that 
 * is page 0 is shared with page 0, etc).  When this is no longer true,
 * this simple check must be converted to a scan of the pte tables.
 */
unsharemem(addr)
	register long	addr;		/* physical address to unshare */
{
	register spt_t	spti;		/* current spt index */
	register struct cst *ownpg;	/* cst pointer of physical page */
	register spt_t	ownspti;	/* spt index which owns page */
	register long	ownaddr;	/* address in owned addressing space */
	pte	*curpte;		/* address of pte to set */
	pte	ptedata;		/* new pte data to set */
	pte	oldptedata;		/* previous pte value */
	pte	ptebits;		/* bits to OR into owning pte */

	addr &= ~MCOFSET;
	ownpg = addrtocst(addr);
	ownspti = ownpg->cst_spti;
	ownaddr = (ownpg->cst_sptp << MCSHIFT);
	ptebits = 0;
	ptedata = PTESPT(ownspti, ownpg->cst_sptp, PG_RWRO);
		/* NOTICE: We are assuming shared pages are write-locked */

	/* Find all memory sharers, and convert them back to SPT sharers */

	for (spti = 1; ownpg->cst_share > 1; spti++) {
		if (spti > lastprocspti) panic("nosharepte");
		if (spti == ownspti) continue;
		if (physaddr(spti, ownaddr) != addr) continue;
		oldptedata = getmap(spti, ownaddr);
		if (!PTEISMEM(oldptedata)) continue;
		ptebits |= oldptedata;
		setmap(spti, ownaddr, ptedata);
	}
	ptebits &= (PG_REF|PG_MOD);
	curpte = pageinpt(ownspti, ownaddr, 0, PTLEVEL2);
	orpte(curpte, ptebits);
}

/* Allocate a page of memory for the kernel.  It is locked if desired.
 * Returns the address of the allocated page, or -1 if no pages are available.
 * Warning:  Unlike most kernel addresses, the address returned is a virtual
 * address, which does not lie in the same physical address.  Physaddr is
 * needed to compute the physical address.  This routine can block, and so
 * is not callable at interrupt level.
 */
kallocpage(lock)
{
	register long	bit;			/* bit which was allocated */
	register long	addr;			/* virtual address of page */
	register long	physaddr;		/* physical address of page */

	if (availpages <= 0) return(-1);	/* no swap space */
	bit = bitalloc(kalloctable, nkalloc, 0);
	if (bit < 0) return(-1);	/* no free addresses */
	addr = botalloc + (bit << MCSHIFT);	/* compute page address */
	while ((physaddr = allocpage(SYSSPTI, addr, PTY_DATA)) <= 0) {
		if (physaddr == 0) continue;
		bitfree(kalloctable, bit);
		return(-1);		/* no swap space */
	}
	setmap(SYSSPTI, addr,
		PTEBUILD((lock ? PT_LOCK : PT_MEM), physaddr, PG_RWNA));
	unlockaddr(physaddr);
	kalloccount++;
	return(addr);
}


/* Free a page of memory for the kernel */
kfreepage(addr)
	register long addr;		/* address of page to free */
{
	register long	bit;		/* bit position */

	bit = (addr - botalloc) >> MCSHIFT;
	if ((addr < botalloc) || (bit >= nkalloc)) panic("kfreepage");
	clearmap(SYSSPTI, addr, PTLEVEL2);
	kalloccount--;
	if (bitfree(kalloctable, bit)) panic("kfreepagetwice");
}


/* Lock a physical page of memory given its address */
lockaddr(addr)
{
	lockpage(addrtocst(addr));
}


/* Unlock a physical page of memory given its address */
unlockaddr(addr)
{
	unlockpage(addrtocst(addr));
}


/* Panic reached if unlocking a page too much.  Called by unlockpage macro */
unlockpanic()
{
	panic("unlockpage");
}

/* Return the physical memory address corresponding to a given virtual
 * address.  Returns -1 if physical address does not exist.
 */
physaddr(spti, addr)
	register spt_t	spti;		/* spt index */
	register unsigned long addr;	/* virtual address */
{
	register pte	*curpte;	/* current pte entry */

	if ((spti >= nspt) || (spt[spti].spt_pte1 == NULL)) panic("badphysadr");
	if (addr >= MAXADDRS) return(-1);
	curpte = spt[spti].spt_pte1 + GETVI1(addr);
	if (PTEINVALID(*curpte)) return(-1);
	curpte = (pte *) PTEMEM_ADDR(*curpte);
	curpte += GETVI2(addr);
	if (PTEINVALID(*curpte)) return (-1);
	return (PTEMEM_ADDR(*curpte) | GETVOFF(addr));
}

/* Return a pointer to a cst entry given a physical address.
 * A panic is issued if the physical address does not exist.
 */
struct cst *
addrtocst(addr)
	register unsigned long	addr;	/* physical address */
{
	register struct cst	*cstp;	/* cst pointer */

	if ((addr >= topmem) || ((cstp = cstptr[addr >> MEMSHIFT]) == NULL)) {
		panic("addrtocst %x", addr);
	}
	cstp = (struct cst *)((int)cstp +
		(sizeof(struct cst)) * ((addr & MEMOFFSET) >> MCSHIFT));
	return(cstp);
}


/* Return a physical page number given a pointer to a cst entry.
 */
csttopg(cstp)
	register struct cst	*cstp;	/* cst pointer */
{
	register int	mbank;		/* bank of memory pointed at */

	mbank = (cstp - cst) / (MEMCHUNK / MCSIZE);
	return(mempgs[mbank] +
		(cstp - cstptr[mempgs[mbank] >> (MEMSHIFT - MCSHIFT)]));
}
