/*
 * vm.c: version 4.27 of 11/4/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)vm.c	4.27 (NSC) 11/4/83";
# endif

/* Virtual memory module for National DS/16000 system	*/
/*	David I. Bell	Laura Neff     6/28/82		*/

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/inode.h"
#include "../h/reg.h"
#include "../h/psr.h"
#include "../h/devpm.h"
#include "../h/mmu.h"
#include "../h/vm.h"
#include "../h/vmmeter.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/vlimit.h"

struct	cst	*cstrover;		/* rover for free cst pages */
extern	long	sptagecounter;		/* age counter for shared texts */

/* Scan forward in an address space looking for the next existent page,
 * or, if inmem is nozero, for the next existent page actually in memory.
 * The scan begins at the specified address, and returns the next
 * existent address, or -1 if no more pages exist.
 */
scanmap(spti, addr, inmem)
	register spt_t	spti;		/* spt index */
	register unsigned long	addr;	/* current address */
	int	inmem;			/* in memory flag */
{
	register pte	*toppte;	/* current top level pte */
	register pte	*curpte;	/* current second level pte */
	register long	addroff;	/* address within second level pte */
	pte	*topbeg;		/* beginning of first level ptes */
	pte	*topend;		/* end of first level ptes */
	pte	*curbeg;		/* beginning of second level ptes */
	pte	*curend;		/* end of second level ptes */

	if ((spti >= nspt) || (addr > MAXADDRS)) panic("badscanmap");
	if (addr == MAXADDRS) return(-1);
	topbeg = spt[spti].spt_pte1;
	topend = topbeg + (NPTEPG * 2);
	toppte = topbeg + GETVI1(addr);
	addroff = GETV2OFF(addr);
	while (toppte < topend) {		/* scan first level pte */
		if (*toppte == PTENULL) {	/* skip null ptes */
			toppte = skippte(toppte, topend);
			addroff = 0;
			continue;
		}
		if (inmem && (PTETYPE(*toppte) != PT_MEM)) {
			toppte += MCPAGES;	/* skip non memory pte */
			addroff = 0;
			continue;
		}
		addr = ((int)toppte - (int)topbeg) * (MAXPTE2ADDRS/PTESIZE);
		curpte = pageinpt(spti, addr + addroff, 0, PTLEVEL2);
		if (curpte == NULL) {
			continue;	/* page table deleted while we waited */
		}
		curbeg = (pte *) ((int) curpte & ~MCOFSET);
		curend = curbeg + (NPTEPG * MCPAGES);
		while (curpte < curend) {	/* scan second level pte */
			if (*curpte == PTENULL) {	/* skip null ptes */
				curpte = skippte(curpte, curend);
				continue;
			}
			if (inmem && !PTEISMEM(*curpte)) {
				curpte += MCPAGES;	/* nonmem pte */
				continue;
			}
			addr += ((int)curpte - (int)curbeg) * (PGSIZE/PTESIZE);
			return(addr);			/* found address */
		}
		toppte += MCPAGES;
		addroff = 0;
	}
	return(-1);
}

/* Clear the map entry in a page table for a given address */
clearmap(spti, addr, level)
	register spt_t	spti;		/* spt index */
	register unsigned long	addr;	/* virtual address to clear */
{
	register pte	*curpte;	/* page table to use */
	register pte	ptedata;	/* pte data */
	register struct	spt	*fs;	/* pointer to spt index */
	spt_t	ownspti;		/* spt index of owner */
	struct	cst	*curcst;	/* cst pointer to use */

	fs = &spt[spti];
retry:	curpte = pageinpt(spti, addr, 0, level); /* page in the page table */
	if (curpte == NULL) return;
	ptedata = getpte(curpte);
	switch (PTEHASH(ptedata)) {
	case PTEHASH_LOCK:
		unlockaddr(PTEMEM_ADDR(ptedata));
		/* proceed into PTEHASH_MEM case */

	case PTEHASH_MEM:
		curcst = addrtocst(PTEMEM_ADDR(ptedata));
		if (--curcst->cst_share < 0) panic("badcstshare");
		if (level == PTLEVEL2) {
			fs->spt_mempages--;
			if (PTEPROT(ptedata) == PG_RWRO) fs->spt_tmempages--;
		}
		else fs->spt_tablepages--;
		ownspti = curcst->cst_spti;
		if (ownspti == spti) {
			freepage(PTEMEM_ADDR(ptedata));
			break;
		}
		if ((level == PTLEVEL2) && (ptedata & PG_MOD)) {
			setmod(ownspti, (curcst->cst_sptp << MCSHIFT));
		}
		unlockaddr(curpte);		/* decrement pte lock */
		if (level == PTLEVEL2) downshare(ownspti, spti);
		break;

	case PTEHASH_SPT:
		if (level == PTLEVEL2) downshare(PTESPT_INDEX(ptedata), spti);
		else panic("clearbadptpte");
		break;

	case PTEHASH_DISK:
		if (PTEISSWAP(ptedata)) {	/* free swap space */
			swapfree(PTESWAP_PAGE(ptedata));
		}
		break;

	case PTEHASH_IOP:
		swapwait(PTEIOP_ADDR(ptedata));
		goto retry;

	case PTEHASH_SPY:
		if (level != PTLEVEL2) panic("clearbadptpte");
		if (spti == SYSSPTI) unlockaddr(curpte);
		break;

	case PTEHASH_NULL:
		return;

	default:
		panic("clearbadpte");
	}
	if (level == PTLEVEL2) fs->spt_usedpages--;
	setpte(curpte, PTENULL);
	if (fs->spt_flags & SPT_NOEIA) return;
	if (spti != SYSSPTI) addr |= EIA_PTB1;
	mteia(addr);
}

/* Clear all map entries for a given page table. Used when killing a process
 * or performing an exec.
 */
clearptetable(spti)
	register spt_t	spti;		/* spt index */
{
	register long	addr;		/* current address */
	register pte	*curpte;	/* current pte */
	register pte	*endpte;	/* last pte to clear */
	register pte	ptedata;	/* current pte data */
	struct	spt	*fs;		/* pointer to spt data */
	int	noeia;			/* don't invalidate addresses */

	fs = &spt[spti];
	if ((spti == SYSSPTI) || (spti >= nspt) || (fs->spt_share)) {
		panic("badclearpte");
	}
	if (fs->spt_tablepages == 0) return;	/* already clear */
	noeia = fs->spt_flags & SPT_NOEIA;	/* remember original flag */
	fs->spt_flags |= SPT_NOEIA;
	addr = 0;
	while ((addr = scanmap(spti, addr, 0)) >= 0) {
		clearmap(spti, addr, PTLEVEL2);
		addr += MCSIZE;
	}
	curpte = fs->spt_pte1;
	endpte = curpte + (NPTEPG * 2);
	while (curpte < endpte) {
		ptedata = *curpte;
		if (ptedata == PTENULL) {
			curpte = skippte(curpte, endpte);
			continue;
		}
		addr = (curpte - fs->spt_pte1) * MAXPTE2ADDRS;
		clearmap(spti, addr, PTLEVEL1);
		curpte += MCPAGES;
	}
	if ((fs->spt_tablepages) || (fs->spt_usedpages)
		|| (fs->spt_mempages) || (fs->spt_tmempages)) {
			panic("clearptecount (spti %d tblpgs %d usedpgs %d mempgs %d tmempgs %d)",
				spti, fs->spt_tablepages, fs->spt_usedpages,
				fs->spt_mempages, fs->spt_tmempages);
	}
	if (noeia) return;
	fs->spt_flags &= ~SPT_NOEIA;
	mtpr(ptb1, spt[u.u_spti].spt_pte1);	/* invalidate all addresses */
}

/* Set up a particular entry in a page map to contain the given data.
 * The old contents of the map entry are cleared.  If the required level 2
 * page table does not exist, it will be allocated.
 */
setpgmap(spti, addr, ptedata, level)
	register spt_t	spti;		/* spt index */
	register unsigned long	addr;	/* virtual address to set */
	register pte	ptedata;	/* data to set */
{
	register pte	*curpte;	/* current pte entry */
	register struct	cst	*mpg;	/* cst entry for memory pte data */
	struct	cst	*pg;		/* cst entry for pte page */
	struct	spt	*fs;		/* pointer to spt entry */

	fs = &spt[spti];
	if (PTEISMEM(ptedata) || (PTETYPE(ptedata) == PT_IOP)) {
		mpg = addrtocst(PTEMEM_ADDR(ptedata));
		lockpage(mpg);			/* keep data page from moving */
	}
	curpte = pageinpt(spti, addr, 1, level); /* page in the page table */
	if (curpte == NULL) panic("setmapnopagein");
	pg = addrtocst(curpte);
	lockpage(pg);				/* keep pte page from moving */
	if (*curpte != PTENULL) {
		clearmap(spti, addr, level);	/* remove old entry */
	}
	switch(PTEHASH(ptedata)) {

	case PTEHASH_LOCK:
		lockpage(mpg);
		/* proceed into PTEHASH_MEM case */

	case PTEHASH_MEM:
	case PTEHASH_IOP:
		lockpage(pg);			/* increment level 2 pte lock */
		mpg->cst_share++;
		if (level == PTLEVEL2) {
			upshare(mpg->cst_spti, spti);
			fs->spt_mempages++;
			if (PTEPROT(ptedata) == PG_RWRO) fs->spt_tmempages++;
		}
		else {
			fs->spt_tablepages++;
		}
		unlockpage(mpg);
		break;

	case PTEHASH_SPT:
		if (level != PTLEVEL2) panic("setbadptpte");
		upshare(PTESPT_INDEX(ptedata), spti);
		break;

	case PTEHASH_SPY:
		if (level != PTLEVEL2) panic("setbadptpte");
		if (spti == SYSSPTI) lockpage(pg); /* keep references safe */
		break;

	case PTEHASH_DISK:
		break;

	default:
		panic("setbadpte");
	}
	if (level == PTLEVEL2) fs->spt_usedpages++;
	setpte(curpte, ptedata);
	unlockpage(pg);
	if (fs->spt_flags & SPT_NOEIA) return;
	if (spti != SYSSPTI) addr |= EIA_PTB1;
	mteia(addr);
}

/* Change an existing entry in a page map only slightly.  This routine
 * simply blasts an existing pte entry as desired.  Since it does no
 * bookkeeping or detailed validity checks, callers should be very careful.
 * Common useage is for modifying protections, reference bits, etc.
 */
changemap(spti, addr, mask, data, level)
	register spt_t	spti;		/* spt index */
	register unsigned long	addr;	/* virtual address to change */
	register long	mask;		/* bit mask for field to change */
	long	data;			/* data to be or'd into field */
{
	register pte	*curpte;	/* current pte entry */
	register pte	ptedata;	/* pte data */

	curpte = pageinpt(spti, addr, 1, level); /* page in the page table */
	if (curpte == NULL) panic("changemapnopagein");
	ptedata = getpte(curpte);
	ptedata &= ~mask;
	ptedata |= (data & mask);
	setpte(curpte, ptedata);
	if (spt[spti].spt_flags & SPT_NOEIA) return;
	if (spti != SYSSPTI) addr |= EIA_PTB1;
	mteia(addr);
}



/* Change the protection of a page in a page map.  Returns the previous
 * protection.  A null pte is asssumed to be writable, even though a later
 * attempt to create the page might be refused.
 */
changemapprot(spti, addr, newprot)
	register spt_t	spti;		/* spt index */
	register unsigned long	addr;	/* virtual address to change */
	register int	newprot;	/* new protection */
{
	register pte	ptedata;	/* current pte data */
	register int	oldprot;	/* previous protection */
	pte	*curpte;		/* current pte pointer */

retry:	curpte = pageinpt(spti, addr, 0, PTLEVEL2);
	ptedata = PTENULL;
	if (curpte) ptedata = *curpte;
	if (ptedata == PTENULL) return(PG_RWRW);
	if (PTETYPE(ptedata) == PT_IOP) {	/* wait till I/O done */
		swapwait(PTEIOP_ADDR(ptedata));
		goto retry;
	}
	oldprot = ptedata & PG_PROT;
	if (newprot == oldprot) return(oldprot);
	if (PTEISMEM(ptedata)) {	/* keep in-memory text count correct */
		spt[spti].spt_tmempages += 
			((newprot == PG_RWRO) - (oldprot == PG_RWRO));
	}
	changemap(spti, addr, PG_PROT, newprot, PTLEVEL2);
	return(oldprot);
}

/* Make us own a particular address instead of sharing it with someone or
 * having it mapped from a file.  Used by ptrace when writing into shared text.
 * Returns nonzero on failure.
 */
unsharemap(spti, addr)
	register spt_t	spti;		/* spt index */
	register unsigned long	addr;	/* virtual address */
{
	register pte	ptedata;	/* current pte data */
	register long	pagadr;		/* address of new page */
	register long	oldadr;		/* address of old page */
	struct	cst	*opg;		/* cst entry for old page */
	int	ptetype;		/* pte type */

retry:	ptedata = getmap(spti, addr);
	if (ptedata == PTENULL) return(0);
	ptetype = PTETYPE(ptedata);
	switch(PTEHASH(ptetype)) {

	case PTEHASH_LOCK:
	case PTEHASH_MEM:
		oldadr = PTEMEM_ADDR(ptedata);
		break;

	case PTEHASH_DISK:
		if (PTEISSWAP(ptedata)) return(0);
		oldadr = pagein(spti, addr, -1);
		if (oldadr < 0) return(1);
		ptetype = PT_MEM;
		break;

	case PTEHASH_SPT:
		oldadr = pagein(PTESPT_INDEX(ptedata),
			(PTESPT_PAGE(ptedata) << MCSHIFT), -1);
		if (oldadr < 0) return(1);
		ptetype = PT_MEM;
		break;

	case PTEHASH_IOP:
		swapwait(PTEIOP_ADDR(ptedata));
		goto retry;

	case PTEHASH_SPY:
		return(0);

	default:
		panic("unsharemap");
	}
	opg = addrtocst(oldadr);
	if (opg->cst_spti == spti) {		/* page owned by us */
		if (opg->cst_file) {		/* make private if from file */
			opg->cst_file = 0;
			opg->cst_backup = 0;
		}
		return(0);
	}
	lockpage(opg);
	while ((pagadr = allocpage(spti, addr, PTY_DATA)) <= 0) {
		if (pagadr == 0) continue;
		unlockpage(opg);
		return(1);
	}
	copypage(oldadr, pagadr);
	setmap(spti, addr, PTEBUILD(ptetype, pagadr, PTEPROT(ptedata)));
	unlockpage(opg);
	unlockaddr(pagadr);
	return(0);
}

/* Make a copy of an address space.  Used by fork.  Returns nonzero on failure.
 */
copyptetable(oldspti, newspti)
	register spt_t	oldspti;	/* old spt index */
	register spt_t	newspti;	/* new spt index */
{
	register long	addr;		/* current address */
	register pte	ptedata;	/* current pte entry */	
	register long	newaddr;	/* new page address */
	struct	inode	*ip;		/* inode of text file */
	long	oldaddr;		/* old page address */
	struct	cst	*opg;		/* cst entry for old address */

	if (oldspti == newspti) panic("badcopypte");
	clearptetable(newspti);
	xblast(&spt[newspti]);
	ip = spt[oldspti].spt_inode;
	spt[newspti].spt_inode = ip;
	if (ip) ip->i_count++;		/* if private text, increment count */
	addr = 0;
	while ((addr = scanmap(oldspti, addr, 0)) >= 0) {
retry:		ptedata = getmap(oldspti, addr);
		switch (PTEHASH(ptedata)) {
		case PTEHASH_MEM:
		case PTEHASH_LOCK:
			oldaddr = PTEMEM_ADDR(ptedata);
			opg = addrtocst(oldaddr);
			lockpage(opg);
			if (opg->cst_spti != oldspti) {
				setmap(newspti, addr,
					PTEMEM(oldaddr, PTEPROT(ptedata)));
				unlockpage(opg);
				break;		/* joined sharing by oldspti */
			}
			while ((newaddr=allocpage(newspti,addr,PTY_DATA))<=0) {
				if (newaddr == 0) continue;
				unlockpage(opg);
				clearptetable(newspti);
				return(1);
			}
			copypage(oldaddr, newaddr);
			unlockpage(opg);
			setmap(newspti, addr, PTEMEM(newaddr,PTEPROT(ptedata)));
			unlockaddr(newaddr);
			break;

		case PTEHASH_IOP:
			swapwait(PTEIOP_ADDR(ptedata));
			goto retry;

		case PTEHASH_DISK:
			if (PTEISSWAP(ptedata)) {	/* read private page */
				pagein(oldspti, addr, -1);
				goto retry;
			}
			/* proceed into PTEHASH_SPT case */

		case PTEHASH_SPT:
		case PTEHASH_SPY:
			setmap(newspti, addr, ptedata);
			break;

		default:
			panic("copybadpte");
		}
		addr += MCSIZE;
	}
	return(0);
}

/* Increment the share count for an spt entry if appropriate */
upshare(ownspti, spti)
	register spt_t	ownspti;	/* spt index to be changed */
	register spt_t	spti;		/* current spt index */
{
	if ((ownspti >= nspt) || (spti >= nspt)) panic("badupshare");
	if (ownspti != spti) spt[ownspti].spt_share++;
}



/* Decrement the share count for an spt entry if appropriate */
downshare(ownspti, spti)
	register spt_t	ownspti;	/* spt index to be changed */
	register spt_t	spti;		/* current spt index */
{
	if (spti == ownspti) return;
	if ((ownspti >= nspt) || (spti >= nspt)) panic("baddownshare");
	if (--spt[ownspti].spt_share < 0) panic("badsptshare");
	spt[ownspti].spt_age = sptagecounter++;
}

/* Return the pte data associated with an address */
getpgmap(spti, addr, level)
	register spt_t	spti;		/* spt index */
	register unsigned long	addr;	/* virtual address */
{
	register pte	*curpte;	/* pointer to pte */
	register pte	ptedata;	/* pte data */

	curpte = pageinpt(spti, addr, 0, level);
	if (curpte == NULL) return(PTENULL);
	ptedata = getpte(curpte);
	return(ptedata);
}


/* Set the modify bit (PG_MOD) for an address in page map.  This is used
 * when writing into the user's address space without using the movsu
 * instruction (copyout is an example).  Based on the way that the MMU
 * caches pte entries, it is not necessary to invalidate the user's address
 * (since if the pte is already in the cache, it will only be written back
 * to memory if the page is later modified, which will be a no-op).
 */
setmod(spti, addr)
	spt_t	spti;			/* spt index */
	unsigned long	addr;		/* virtual address */
{
	register pte	*curpte;	/* pte entry */

	curpte = pageinpt(spti, addr, 0, PTLEVEL2);
	if (curpte != NULL) orpte(curpte, (PG_REF|PG_MOD));
}
