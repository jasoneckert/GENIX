# ifndef NOSCCS
static char *sccsid = "@(#)vmtrap.c	4.32	11/7/83";
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
#include "../h/inode.h"
#include "../h/conf.h"
#include "../h/vlimit.h"

#define	SUUSMASK 0x00c62cff	/* mask for allowed movsu/movus instructions */
#define	SUUSCODE 0x00000cae	/* integer for movsu/movus instructions */
#define	SUUSSIZE 3		/* number of bytes for allowed movsu/movus */
#define	RETTMASK 0x0000ffff	/* mask for allowed rett instruction */
#define	RETTCODE 0x00000042	/* integer for allowed rett instruction */
#define	RETTSIZE 2		/* number of bytes for allowed rett */
#define	ENTERMASK 0xff		/* mask for enter instruction */
#define	ENTERCODE 0x82		/* integer which is an enter instruction */
#define	STACKGRACE 32		/* leeway to allow for stack growing */

int	mmuinfoflag;			/* TEMPORARY - explain aborts */
int	prepages;			/* number of pages pre-paged */
extern	int	botalloc;		/* beginning of mem allocation area */
extern	int	nkalloc;		/* amount of allocation allowed */
extern	int	pagesfree;		/* number of free memory pages */
extern	char	etext;			/* end of kernel text area */
extern	dev_t	swapdev;		/* swap device */

/* Here on an abort trap to determine the cause of the abort and handle it.
 * Returns zero if the abort is restartable.  If the abort is illegal so that
 * the process has to be interrupted, returns the signal number to generate.
 */
aborttrap(pc, mpsr, ppl)
	struct	modpsr	mpsr;		/* processor status at trap */
{
	register long	status;		/* msr status bits (reason for abort) */
	register long	addr;		/* eia value (address causing abort) */
	register struct user *up;	/* pointer to u. area */
	register long	pcu;		/* abort pc was from user mode */
	register long	addru;		/* abort address was in user mode */

	status = mfpr(msr);		/* get reason for abort */
	addr = mfpr(eia);		/* and abort address */
	up = (struct user *)&u;		/* avoid external references */
	up->u_msrreg = status;		/* save status for lookers */
	up->u_eiareg = addr;		/* and save address */
	pcu = PSR(mpsr) & UMODE;	/* remember if pc was user mode */
	addru = addr & EIA_PTB1;	/* remember if addr was user mode */
	addr &= VA_USED;		/* only keep valid address bits */
	if (mmuinfoflag) {
		cprintf("MMU %s TRAP: %s %x  %s %x  MSR %x  PTE %x\n",
			(status & MSR_READERR) ? "READ" : "WRITE",
			pcu ? "UPC" : "EPC", pc,
			addru ? "UADR" : "EADR", addr, status,
			addru ? getmap(up->u_spti,addr) : getmap(SYSSPTI,addr));
	}
	if (ppl) panic("intlevelabort (ppl %d, pc %x, addr %x, msr %x)",
		ppl, pc, addr, status);		/* abort at interrupt level */
	if (!pcu && !addru) {
		up->u_vm.vm_minflt++;		/* Count kernel page faults */
	} else up->u_vm.vm_majflt++;		/*   and user page faults */
	cnt.v_faults++;
	clearberr();				/* clear msr status bits */
	if (status & MSR_BRKERR) {		/* breakpoint trap */
		if (!pcu) {			/* supervisor mode abort */
			psignal(up->u_procp, SIGBPT);
		}
		return(SIGBPT);
	}
	if ((status & MSR_TRANSERR) == 0) {	/* unknown abort reason */
		panic("unknownabort");
	}
	if (!pcu) {				/* supervisor mode abort */
		supabort(pc, status, addr, addru);
		return(0);			/* always "succeeds" */
	}
	if (!addru) {
		return(SIGSEGV);		/* MMU BUG: bogus reference */
	}
	if ((status & MSR_PROTERR) && (getmap(up->u_spti, addr) != PTENULL)) {
		return(SIGBUS);			/* protection error */
	}
	if (pagein(up->u_spti, addr, pc) < 0) {
		return(SIGSEGV);		/* cannot page in */
	}
	return(0);				/* success */
}

/* Here to handle a supervisor mode abort.  The aborting pc is in supervisor
 * mode, but the effective address causing the abort can be from either
 * user or supervisor mode.  The cases are as follows:
 * Supervisor mode:
 *	Page faults are allowed only for the user structures at BOTXU and
 *	the kernal data pages at botalloc.  Panic if these fail.
 * User mode:
 *	The supervisor instruction must be "rett 0", or any register/register
 *	form of the "movus" or "movsu" instructions.  Skip over the rett if
 *	it fails.  Skip over the "movsu" and "movus" instructions and set the
 *	"F" flag if they fail.  This action allows the code following these
 *	instructions to detect a failed reference and act on it.
 */
supabort(pc, status, addr, addru)
	register unsigned long	pc;	/* supervisor pc causing abort */
	register long status;		/* msr status for abort */
	register unsigned long addr;	/* user address causing abort */
{
	register struct	user *up;	/* pointer to u. area */
	register long	instr;		/* instruction causing trap */

	if (pc >= (int)&etext) {	/* illegal pc for an abort */
		panic("badsupabortpc %x", pc);
	}
	if (!addru) {			/* abort on supervisor address */
		if ((((addr < BOTXU) || (addr >= BOTXU + (USIZE*nproc))) &&
			((addr<botalloc)||(addr>=botalloc+(nkalloc*MCSIZE))))
		|| (pagein(SYSSPTI, addr, -1) < 0))
			panic("supervisorabort (pc %x, addr %x, msr %x)",
				pc, addr, status);
		return;			/* successfully paged in kernel page */
	}

	 /* Here for the user mode cases: rett, movsu, and movus. */

	up = (struct user *)&u;		/* avoid external references */
	instr = *((long *) pc);		/* get instruction which aborted */
	if (((instr & SUUSMASK)!=SUUSCODE) && ((instr & RETTMASK)!=RETTCODE)) {
		panic("badabortinstr (pc %x, addr %x, instr %x)",
			pc, addr, instr);
	}
	if ((((status&MSR_PROTERR) == 0) || (getmap(up->u_spti,addr)==PTENULL))
		&& (pagein(up->u_spti, addr, -1) >= 0)) {
			return;		/* successfully paged in user page */
	}

	/* Here on a failed reference to a user page */

	if ((instr & RETTMASK) == RETTCODE) {		/* failed rett */
		up->u_abtar0[RPC] += RETTSIZE;	/* advance pc over rett */
		return;
	}
	up->u_abtar0[RPSRMOD] |= (PSR_F << 16);		/* set "F" bit */
	up->u_abtar0[RPC] += SUUSSIZE;	/* advance pc over movsu/movus */
}

/* Force in a page of a virtual address space to be resident (but not locked).
 * Returns the physical address corresponding to the given virtual address,
 * or -1 if the pagein failed.  The pc argument is only used for a real page
 * fault.  Other callers should supply a -1.  If the page has to be swapped in,
 * an attempt is made to prepage the next page.
 */
pagein(spti, addr, pc)
	register spt_t	spti;		/* spt index */
	register unsigned long	addr;	/* virtual address */
{
	register pte	*curpte;	/* pointer to pte entry */
	register pte	ptedata;	/* page table data */
	register long	pagadr;		/* page address */
	unsigned long	oldadr;		/* old page address */
	struct	cst	*pg;		/* cst pointer for ptes */

retry:	curpte = pageinpt(spti, addr, 0, PTLEVEL2); /* get current map info */
	ptedata = PTENULL;
	if (curpte != NULL) ptedata = *curpte;
	switch (PTEHASH(ptedata)) {

	case PTEHASH_MEM:
	case PTEHASH_LOCK:
		pagadr = PTEMEM_ADDR(ptedata);
		orpte(curpte, PG_REF);
		break;

	case PTEHASH_NULL:
		if (spti != u.u_spti) {
			if ((spti != SYSSPTI) || (addr < BOTXU) ||
				(addr >= BOTXU + (USIZE*nproc))) {
					return(-1);
			}
			pg = addrtocst(curpte);	/* curpte maybe NULL, but ok */
			lockpage(pg);
			pagadr = alloczeropage(SYSSPTI, addr, PTY_DATA);
			if (pagadr <= 0) {
				if (pagadr < 0) panic("pageinu");
				unlockpage(pg);
				goto retry;
			}
			setmap(SYSSPTI, addr, PTEMEM(pagadr, PG_RWNA|PG_REF));
			unlockaddr(pagadr);
			unlockpage(pg);
			break;
		}
		if (checknewpage(addr, pc)) {	/* see if new page is legal */
			return(-1);
		}
		pg = addrtocst(curpte);
		lockpage(pg);
		pagadr = alloczeropage(spti, addr, PTY_DATA);
		if (pagadr <= 0) {
			unlockpage(pg);
			if (pagadr == 0) goto retry;
			swapkill(u.u_procp, 0);
			return(-1);
		}
		setmap(spti, addr, PTEMEM(pagadr, PG_RWRW|PG_REF));
		unlockaddr(pagadr);
		unlockpage(pg);
		break;

	case PTEHASH_SPT:
		pg = addrtocst(curpte);
		lockpage(pg);
		pagadr = pagein(PTESPT_INDEX(ptedata),
			(PTESPT_PAGE(ptedata) << MCSHIFT), -1);
		if (pagadr < 0) {
			unlockpage(pg);
			return(-1);
		}
		lockaddr(pagadr);
		if (PTEPROT(ptedata) == PG_RWRW) {
			/* COPY ON REFERENCE.  KNOWN BECAUSE THE MAP ENTRY
		 	 * ALLOWED WRITES FOR THE USER, AND THIS PAGE BELONGS
			 * TO A FILE'S DATA SEGMENT.  (SEE XATTACH IN VMSYS).
		 	 */
			oldadr = pagadr;
			pagadr = allocpage(spti, addr, PTY_DATA);
			if (pagadr <= 0) {
				unlockaddr(oldadr);
				unlockpage(pg);
				if (pagadr == 0) goto retry;
				swapkill(u.u_procp, 0);
				return(-1);
			}
			copypage(oldadr, pagadr);
			unlockaddr(oldadr);
		}
		setmap(spti, addr, PTEMEM(pagadr, PTEPROT(ptedata)|PG_REF));
		unlockaddr(pagadr);
		unlockpage(pg);
		break;

	case PTEHASH_DISK:
		if (diskpagein(spti, addr, curpte, 0)) return(-1);
		goto retry;

	case PTEHASH_IOP:
		prepagein(spti, addr + MCSIZE);	/* bring in next page too */
		swapwait(PTEIOP_ADDR(ptedata));	/* wait for current page */
		goto retry;

	case PTEHASH_SPY:
		return(-1);
		break;

	default:
		panic("badpagein");
	}
	return(pagadr | GETVOFF(addr));
}

/* Subroutine to bring in a page from the disk.  Called by pagein and prepagein.
 * Before actually starting I/O into memory, we first have to search the buffers
 * for the data, since it might be from a new file not yet written to the disk.
 * Prepage is nonzero if called from prepagein.  Returns nonzero on failure.
 * On normal return, the caller should recalculate the position of the page, as
 * nothing may be been done if no memory was available.
 * THIS ROUTINE ASSUMES THAT BSIZE IS THE SAME SIZE AS MCSIZE.
 */
diskpagein(spti, addr, curpte, prepage)
	spt_t	spti;			/* spt index */
	register pte	*curpte;	/* pointer to current pte */
{
	register pte	ptedata;	/* current pte data */
	register struct	cst	*pg;	/* current cst entry */
	register long	pagadr;		/* address of new page */
	struct	cst	*opg;		/* cst entry of pte table page */
	struct	buf	*bp;		/* buffer holding page data */
	struct	inode	*ip;		/* inode pointer for text file */
	dev_t	dev;			/* device text file is on */

	opg = addrtocst(curpte);
	lockpage(opg);			/* keep pte page in memory */
	pagadr = allocpage(spti, addr, PTY_DATA);	/* get new page */
	if (pagadr <= 0) {
		unlockpage(opg);
		if (pagadr == 0) return(0);
		if (prepage == 0) swapkill(u.u_procp, 0);
		return(-1);
	}
	ptedata = *curpte;		/* get current pte entry */
	setpte(curpte, PTENULL);	/* then clear it to keep swap space */
	spt[spti].spt_usedpages--;
	pg = addrtocst(pagadr);
	pg->cst_backup = PTESWAP_PAGE(ptedata);
	pg->cst_file = ((ptedata & PG_FILE) ? 1 : 0);
	dev = swapdev;			/* assume page from swap device */
	if (PTEISFILE(ptedata)) {	/* if not, find file's device */
		ip = spt[spti].spt_inode;
		if (ip == NULL) panic("notextptr");
		dev = ip->i_dev;
	}
	bp = baddr(dev, pg->cst_backup); /* look in buffer cache for page */
	if (bp) {			/* if found, can copy data from it */
		bcopy(bp->b_un.b_addr, pagadr, MCSIZE);
		brelse(bp);
		setmap(spti, addr, PTEMEM(pagadr, PTEPROT(ptedata)));
		unlockpage(pg);
		unlockpage(opg);
		return(0);
	}
	setmap(spti, addr, PTEIOP(pagadr, PTEPROT(ptedata)));
	swapinsert(pagadr, B_READ);		/* start the I/O */
	if (prepage == 0) {
		prepagein(spti, addr + MCSIZE);	/* try to get next page too */
		swapwait(pagadr);	/* wait for I/O done on current page */
	}
	unlockpage(opg);
	return(0);
}

/* Routine to force a second level page table to be resident (but not locked).  
 * This creates the page table if requested.  Returns a pointer to the map entry
 * which points at the given virtual address, or NULL if one doesn't exist.
 * The returned pte pointer is the one which points to beginning of a cluster.
 */
pte *
pageinpt(spti, addr, create, level)
	register spt_t	spti;		/* spt index */
	register unsigned long addr;	/* virtual address */
{
	register pte	*curpte;	/* page table pointer */
	register pte	ptedata;	/* page table data */
	register long	pagadr;		/* page address */
	struct	cst	*pg;		/* cst pointer to page */

	if ((spti >= nspt) || (spt[spti].spt_pte1 == NULL)) {
		panic("badpageinpt");
	}
	if ((level != PTLEVEL2) && (level != PTLEVEL1)) panic("pageinptlevel");
	if (addr >= MAXADDRS) return(NULL);
retry:	curpte = spt[spti].spt_pte1 + GETVI1(addr);
	if (level == PTLEVEL1) return(curpte);
	ptedata = *curpte;
	switch (PTEHASH(ptedata)) {

	case PTEHASH_MEM:
		pagadr = PTEMEM_ADDR(ptedata);
		break;

	case PTEHASH_NULL:
		if (!create) return(NULL);
		lockaddr(curpte);
		pagadr = alloczeropage(spti, addr & (~(VA_OFFSET|VA_INDEX2)),
					PTY_PTE2);
		if (pagadr <= 0) {
			unlockaddr(curpte);
			goto retry;
		}
		pg = addrtocst(pagadr);
		pg->cst_share++;
		ptedata = PTEMEM(pagadr, PG_RWRW);
		setpte(curpte, ptedata);
		unlockpage(pg);
		spt[spti].spt_tablepages++;
		break;

	case PTEHASH_DISK:
		if (PTEISFILE(ptedata)) panic("filepte2");
		lockaddr(curpte);
		pagadr = allocpage(spti, addr & (~(VA_OFFSET|VA_INDEX2)),
					PTY_PTE2);
		if (pagadr <= 0) {
			unlockaddr(curpte);
			goto retry;
		}
		pg = addrtocst(pagadr);
		pg->cst_backup = PTESWAP_PAGE(ptedata);
		pg->cst_file = 0;
		setpte(curpte, PTENULL);	/* stop freeing of swap space */
		setpgmap(spti, addr, PTEIOP(pagadr,PTEPROT(ptedata)),PTLEVEL1);
		swapinsert(pagadr, B_READ);
		swapwait(pagadr);
		goto retry;

	case PTEHASH_IOP:
		swapwait(PTEIOP_ADDR(ptedata));
		goto retry;

	default:
		panic("badpageinptpte");
	}
	curpte = (pte *) pagadr;
	curpte += GETVI2(addr);
	return(curpte);
}

/* Subroutine to pre-page in an address for an address space.  This starts
 * a disk read if possible for the page, but does not wait for it to complete.
 * For this routine to accomplish anything, memory must be available, the
 * first and second level page tables must be in memory, and the data page 
 * must be on the disk.
 */
prepagein(spti, addr)
	register spt_t	spti;		/* spt index */
	register unsigned long	addr;	/* virtual address */
{
	register pte	ptedata;	/* current pte data */
	register pte	*curpte;	/* current pte pointer */

	if ((pagesfree < 10) || (spti >= nspt) || (addr >= MAXADDRS)) {
		return;		/* don't prepage if not enough mem */
	}
	curpte = spt[spti].spt_pte1;
	if (curpte == NULL) return;		/* no 1st level table */
	curpte += GETVI1(addr);
	ptedata = *curpte;
	if (!PTEISMEM(ptedata)) return;		/* 2nd level not in memory */
	curpte = (pte *) PTEMEM_ADDR(ptedata);
	curpte += GETVI2(addr);
	ptedata = *curpte;
	switch (PTETYPE(ptedata)) {	/* check type of data page */
		case PT_DISK:
			if (diskpagein(spti, addr, curpte, 1)) break;
			prepages++;
			break;

		case PT_SPT:
			prepagein(PTESPT_INDEX(ptedata),
				(PTESPT_PAGE(ptedata) << PGSHIFT));
			break;
	}
}

/* Subroutine to determine if a new memory page is legal for the current user.
 * An address within the data segment is legal.  Otherwise, beyond the stack
 * limit implied by LIM_STACK is illegal.  Otherwise, within the stack segment
 * or nearby is legal.  Otherwise, if from an enter instruction is legal.
 * Otherwise, is between stack and data segments and is illegal.
 * Returns nonzero if address is illegal.
 */
checknewpage(addr, pc)
	register long	addr;		/* address to verify */
	register long	pc;		/* user pc, or -1 if none */
{
	if ((unsigned)addr >= TOPUSER) goto bad;
	if (addr < (int)(u.u_tsize + u.u_dsize)) goto good;
	if (addr < (TOPUSER - u.u_limit[LIM_STACK])) goto bad;
	if (addr >= (u.u_ar0[RSP] - STACKGRACE)) goto good;
	if ((pc >= 0) && ((fubyte(pc) & ENTERMASK) == ENTERCODE)) goto good;
bad:	return(-1);
good:	return(0);
}
