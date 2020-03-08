/*
 * vmsys.c: version 3.53 of 9/9/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)vmsys.c	3.53 (NSC) 9/9/83";
# endif

/* System calls for handling virtual memory on 16000 UNIX */
/*	David I. Bell	7/1/82	*/


#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/psr.h"
#include "../h/mmu.h"
#include "../h/vm.h"
#include "../h/devvm.h"
#include "../h/inode.h"
#include "../h/tty.h"
#include "../h/file.h"
#include "../h/vlimit.h"


/* The following table lists the physical addresses which are allowed to
 * be mapped by the non super-user using the vspy call.  The super-user
 * has no restrictions.
 */
struct	spytable {
	long	spy_addr;	/* first allowed address */
	short	spy_count;	/* number of allowed pages */		
	char	spy_write;	/* writing is allowed */
} spytable[] = {
#if NBM > 0
	0xc00000, ptoc(153), 1,		/* first window of graphics memory */
	0xc20000, ptoc(153), 1,		/* second window of graphics memory */
	0xc80c00, ptoc(1), 1,		/* graphics control register */
	0xc81000, ptoc(1), 0,		/* keyboard and flavor keys */
	0xc81200, ptoc(1), 0,		/* joystick location */
#endif NMB
	0xffea00, ptoc(2), 1,		/* GPIB device */
	0, 0, 0				/* END OF TABLE */
};


long	lastspti;		/* last spt index (not SYSSPTI) for debugging */
int	limrssseen;		/* someone has limited resident set size */
pte	*uptetable[UPAGES/MCPAGES]; /* pointers to current u. pte entries */


/* SVC to fork in a memory efficient way.  The child uses the parent's address
 * space while the parent sleeps.  When the child exits or execs a new program,
 * it releases the address space and the parent proceeds.
 */
vfork()
{
	fork1(1);
}

/* SVC to spy on a physical page of memory.  Call from C:
 *	vspy(virtaddr, physaddr, writeflag);
 * where virtaddr is the user's virtual address, physaddr is the desired
 * physical address to be mapped, and writeflag is nonzero if the mapping
 * allows writes.  A negative physical address will remove the spy mapping
 * if it existed.  This SVC allows user programs to access device registers.
 * Unprivileged users can only spy on a restricted set of physical addresses.
 * The super-user has no restrictions.
 */
vspy()
{
	register struct a {
		unsigned long	useraddr;
		long 	physaddr;
		int	writeenable;
	} *uap;
	register pte	ptedata;		/* current pte data */
	register struct	spytable *st;		/* pointer to spying table */
	register long	physaddr;		/* physical address */

	uap = (struct a *)u.u_ap;
	physaddr = uap->physaddr;
	for (st = spytable; st->spy_count; st++) {
		if (physaddr < st->spy_addr) continue;
		if (physaddr >= st->spy_addr + (st->spy_count*MCSIZE)) continue;
		if ((uap->writeenable) && (st->spy_write == 0)) continue;
		break;
	}
	if ((u.u_uid) && (st->spy_count == 0) && (physaddr != -1)) {
		u.u_error = EPERM;		/* not privileged */
		return;
	}
	if ((uap->useraddr >= MAXADDRS) || (physaddr >= MAXADDRS)) {
		u.u_error = EINVAL;		/* bad addresses */
		return;
	}
	if (physaddr < 0) {		/* clear spy mapping */
		ptedata = getmap(u.u_spti, uap->useraddr);
		if (PTETYPE(ptedata) == PT_SPY) {
			clearmap(u.u_spti, uap->useraddr, PTLEVEL2);
		}
		return;
	}
	physaddr &= ~MCOFSET;
	ptedata = PG_RWRO;
	if (uap->writeenable) ptedata = PG_RWRW;
	setmap(u.u_spti, uap->useraddr, PTESPY(physaddr, ptedata));
}

/* SVC to lock or unlock a user page in memory.  Call from C:
 *	physaddr = vlock(func, useraddr)
 * where useraddr is the user's virtual address, func is 1 to lock and 0 to
 * unlock, and physaddr is the corresponding physical address, or -1 on errors.
 */
vlock()
{
	register struct a {
		int	func;			/* function */
		unsigned long	addr;		/* virtual address */
	} *uap;
	register unsigned long	useraddr;	/* user address */
	register long	physaddr;		/* physical address */
	register pte	ptetype;		/* pte type */

	if (u.u_uid) {
		u.u_error = EPERM;		/* must be root */
		return;
	}
	uap = (struct a *)u.u_ap;
	useraddr = uap->addr;
	if ((useraddr >= MAXADDRS) || (uap->func < 0) || (uap->func > 1)) {
		u.u_error = EINVAL;			/* bad arguments */
		return;
	}
	physaddr = pagein(u.u_spti, useraddr, -1);
	ptetype = PTETYPE(getmap(u.u_spti, useraddr));
	switch (uap->func) {
	case 0:						/* unlock page */
		if (ptetype != PT_LOCK) return;
		changemap(u.u_spti, useraddr, PG_TYPE, PT_MEM, PTLEVEL2);
		unlockaddr(physaddr);
		return;
	case 1:						/* lock page */
		if (physaddr < 0) {
			u.u_error = ENOMEM;
			return;
		}
		u.u_r.r_val1 = physaddr;
		if (ptetype != PT_MEM) return;
		lockaddr(physaddr);
		changemap(u.u_spti, useraddr, PG_TYPE, PT_LOCK, PTLEVEL2);
		return;
	}
}

/* SVC to perform mapping operations */
vmap()
{
	register struct a {
		int	a_func;
		int	a_addr1;
		int	a_addr2;
		int	a_pid;
	} *uap;				/* argument pointer */
	register unsigned long	addr1;	/* first address */
	register unsigned long	addr2;	/* other address */
	register long	pagadr;		/* page address */
	register long	ospti;		/* spt index of destination */
	struct	proc	*p;		/* destination process */

	uap = (struct a *)u.u_ap;

	addr1 = uap->a_addr1;
	addr2 = uap->a_addr2;
	if (addr1 >= MAXADDRS) {
		u.u_error = EFAULT;
		return;
	}
	if (uap->a_func) {
		suser();
		ospti = SYSSPTI;
		p = NULL;
		if (uap->a_pid >= 0) {
			p = pfind(uap->a_pid);
			if (p == NULL) u.u_error = ESRCH;
			ospti = p->p_spti;
		}
		if (addr2 >= MAXADDRS) u.u_error = EFAULT;
		if (u.u_error) return;
	}
	switch (uap->a_func) {

	case 0:				/* remove page from address space */
		clearmap(u.u_spti, addr1, PTLEVEL2);
		break;

	case 1:				/* copy page of another process */
		pagadr = kallocpage(1);
		if (pagadr < 0) {
			u.u_error = ENOMEM;	/* no swap space */
			return;
		}
		addr2 = pagein(ospti, addr2, -1);
		if (addr2 >= 0) copypage(addr2, pagadr);
		else clearpage(pagadr);
		copyout((caddr_t)pagadr, addr1, MCSIZE);
		kfreepage(pagadr);
		break;

	case 2:				/* map page of another process */
	case 3:
		/* WRONG!!! DOES NOT WORK RIGHT IF PROCESS DISAPPEARS
		 * OR IF PAGE NEEDS TO BE SWAPPED OUT
		 */
		setmap(u.u_spti, addr1, PTESPT(ospti, addr2 >> MCSHIFT,
			(uap->a_func == 2) ? PG_RWRO : PG_RWRW));
		break;

	default:
		u.u_error = EINVAL;
		return;
	}
}

/*
 * Revoke access the current tty by all processes.
 * Used only by the super-user in init
 * to give ``clean'' terminals at login.
 */
vhangup()
{

	if (!suser())
		return;
	if (u.u_ttyp == NULL)
		return;
	forceclose(u.u_ttyd);
	if ((u.u_ttyp->t_state) & ISOPEN)
		gsignal(u.u_ttyp->t_pgrp, SIGHUP);
}

forceclose(dev)
	dev_t dev;
{
	register struct file *fp;
	register struct inode *ip;

	for (fp = file; fp < fileNFILE; fp++) {
		if (fp->f_count==0)
			continue;
		if ((ip = fp->f_inode) == 0)
			continue;
		if ((ip->i_mode & IFMT) != IFCHR)
			continue;
		if (ip->i_un.i_rdev != dev)
			continue;
		fp->f_flag &= ~(FREAD|FWRITE);
	}
}

/*
 * Affect per-process limits.
 * To just return old limit, specify negative new limit.
 */
vlimit()
{
	register struct a {
		unsigned which;
		int	limit;
	} *uap;
	register unsigned which;
	register int limit;

	uap = (struct a *)u.u_ap;
	which = uap->which;
	limit = uap->limit;
	if (which > NLIMITS) {
		u.u_error = EINVAL;
		return;
	}
	u.u_r.r_val1 = u.u_limit[which];
	if (limit < 0)
		return;
	switch (which) {
		case LIM_DATA:
		case LIM_STACK:
			if (limit > USERCORE) limit = USERCORE;
			break;
	}
	if (u.u_limit[LIM_NORAISE] &&
		((which==LIM_NORAISE) ? (limit==0):(limit>u.u_limit[which])) &&
		(!suser())) {
			u.u_error = EACCES;
			return;
	}
	u.u_limit[which] = limit;
	if (which == LIM_MAXRSS) {
		u.u_procp->p_maxrss = limit/NBPG;
		limrssseen = -1;
	}
}

/* Find processes which have outgrown their limit.  Called once a
 * second from vmmeter().
 */

vmtoobig()
{
	register struct proc	*p;
	register struct spt	*sp;

	if (limrssseen == 0) return;
	limrssseen = 0;
	for (p = proc; p < procNPROC; p++) {
		if ((p->p_stat == 0) || (p->p_stat == SZOMB)) continue;
		if (p->p_maxrss < INFINITY/NBPG) {
			limrssseen = -1;
			sp = &spt[p->p_spti];
			if (p->p_maxrss < sp->spt_mempages) {
				spt->spt_flags |= SPT_XSRSS;
			}
			else sp->spt_flags &= ~SPT_XSRSS;
		}
	}
}

/* SVC to return accounting information for the current process and/or
 * for terminated children of the current process.  Call from C:
 *	vtimes(par_vm, ch_vm)
 *	struct vtimes *par_vm, *ch_vm;
 * Either or both of par_vm and ch_vm may be zero, in which case only the
 * information for which the pointers are nonzero is returned.
 */
vtimes()
{
	register struct a {
		struct	vtimes *par_vm;
		struct	vtimes *ch_vm;
	} *uap = (struct a *)u.u_ap;

	if (uap->par_vm == 0) goto onlych;
	if (copyout((caddr_t)&u.u_vm, (caddr_t)uap->par_vm,
	    sizeof(struct vtimes)) < 0) {
		u.u_error = EFAULT;
	}
onlych:
	if (uap->ch_vm == 0) return;
	if (copyout((caddr_t)&u.u_cvm, (caddr_t)uap->ch_vm,
	    sizeof(struct vtimes)) < 0) {
		u.u_error = EFAULT;
	}
}




vmsadd(vp, wp)
	register struct vtimes *vp, *wp;
{

	vp->vm_utime += wp->vm_utime;
	vp->vm_stime += wp->vm_stime;
	vp->vm_nswap += wp->vm_nswap;
	vp->vm_idsrss += wp->vm_idsrss;
	vp->vm_ixrss += wp->vm_ixrss;
	if (vp->vm_maxrss < wp->vm_maxrss)
		vp->vm_maxrss = wp->vm_maxrss;
	vp->vm_majflt += wp->vm_majflt;
	vp->vm_minflt += wp->vm_minflt;
	vp->vm_inblk += wp->vm_inblk;
	vp->vm_oublk += wp->vm_oublk;
}

/* Set the spt index of the currently running process */
setspti(spti)
	register unsigned int	spti;		/* new spti index */
{
	if ((spti >= nspt) || (spt[spti].spt_pte1 == NULL)) {
		panic("setspti");
	}
	u.u_spti = spti;
	u.u_procp->p_spti = spti;
	mtpr(ptb1, spt[spti].spt_pte1);		/* set to use new page table */
}



/* Copy the current u. data into another process's u.  Used by fork.
 * We simply copy the pages, and let a page fault occur if necessary.
 */
copyu(procid)
	register unsigned long	procid;	/* process number to copy into */
{
	if (procid >= nproc) panic("badcopyu");
	bcopy(BOTU, BOTXU + (procid * USIZE), USIZE);
}



/* Lock down the u. area for a process.  This is done by possibly faulting
 * in the desired u. pages.
 */
swapinu(procid)
	register unsigned long	procid;	/* process number to swap in u. for */
{
	register long	ubase;		/* base address of u. */
	register long	uoff;		/* current page offset */
	register long	pagadr;		/* current page address */
	static	int	dummy;		/* so optimizer preserves references */

	ubase = BOTXU + (procid * USIZE);
	for (uoff = 0; uoff < USIZE; uoff += MCSIZE) {
		dummy += *BADR(ubase + uoff);	/* fault page in */
		pagadr = physaddr(SYSSPTI, ubase + uoff);
		if (pagadr < 0) panic("swapinu");
		lockaddr(pagadr);
	}
}


/* Unlock the u. area for a process. */
uunlock(procid)
	register unsigned long	procid;	/* process index to set u. of */
{
	register long	pagadr;		/* current page address */
	register long	ubase;		/* base address of u. */
	register long	uoff;		/* current page offset */

	ubase = BOTXU + (procid * USIZE);
	for (uoff = 0; uoff < USIZE; uoff += MCSIZE) {
		pagadr = physaddr(SYSSPTI, ubase + uoff);
		if (pagadr < 0) panic("unlock err for u.");
		unlockaddr(pagadr);
	}
}


/* Set up a process u. area as the current u. area.
 */
swtchu(procid)
	register unsigned long	procid;	/* process index to set u. of */
{
	register pte 	ptedata;	/* pte data for current page */
	register long	ubase;		/* base address of u. */
	register int	i;		/* index into u. ptes */
	pte	utab[UPAGES/MCPAGES];	/* ptes to set for new u. area */
	int	s;

	if (procid >= nproc) panic("badswtchu");
	if (u.u_procp == &proc[procid]) return;

	/* check that new u. pages are in memory and locked */

	ubase = BOTXU + (procid * USIZE);
	for (i = 0; i < (UPAGES/MCPAGES); i++) {
		ptedata = getmap(SYSSPTI, ubase + (i * MCSIZE));
		if ((PTETYPE(ptedata) != PT_MEM) ||
			(addrtocst(PTEMEM_ADDR(ptedata))->cst_lock == 0)) {
				panic("swtchu not locked");
		}
		utab[i] = PTESPY(PTEMEM_ADDR(ptedata), PG_RWNA|PG_REF|PG_MOD);
	}

	/* Set new u. as current one while interrupts are disabled.  For speed,
	 * setmap is not used, but instead the new u. pte entries are set by
	 * hand.  This depends on these pointers always being spy pointers, and
	 * the fact that the underlying page table never moves.
	 */
	s = spl6();
	for (i = 0; i < (UPAGES/MCPAGES); i++) {
		if (PTETYPE(*uptetable[i]) != PT_SPY) panic("swtchupte");
		setpte(uptetable[i], utab[i]);
		mteia(BOTU + (i * MCSIZE));
	}
	splx(s);
	if (procid != SYSSPTI) lastspti = procid;
}

/* Routine to lock user memory for use in physical IO.  Returns nonzero if
 * the memory cannot be locked.  WARNING:  There is no guarantee that the
 * required memory pages are available.  This has to be checked beforehand.
 */
lockmem(spti, addr, count)
	register spt_t	spti;		/* spt index of address space */
	register unsigned long	addr;	/* beginning address */
	register long	count;		/* number of bytes to lock */
{
	register long	physaddr;	/* physical address of page */
	long	lockcount;		/* amount locked so far */
	long	origaddr;		/* original address */

	if ((count<=0) || (addr>=MAXADDRS) || ((addr+count) >= MAXADDRS)) {
		return(1);
	}
	count += (addr & MCOFSET);	/* adjust down to page boundary */
	addr &= ~MCOFSET;
	origaddr = addr;		/* save in case need to unlock */
	lockcount = 0;
	while (count > 0) {
		physaddr = pagein(spti, addr, -1);
		if (physaddr < 0) {
			unlockmem(spti, origaddr, lockcount, 0);
			return(-1);
		}
		lockaddr(physaddr);	/* lock the page */
		addr += MCSIZE;
		lockcount += MCSIZE;
		count -= MCSIZE;
	}
	return(0);
}

/* Unlock a number of user pages.  No validity checking is performed.
 * If writeflag is set, the pages will be marked as having been modified.
 * This is useful after reading from a device, since the device will not
 * update the modify flags.
 */
unlockmem(spti, addr, count, writeflag)
	register spt_t	spti;		/* spt index of address space */
	register unsigned long	addr;	/* beginning address */
	register long	count;		/* number of bytes to unlock */
	long	writeflag;		/* nonzero if was written into */
{
	register pte	*curpte;	/* current pte entry */

	if (count <= 0) return;
	count += (addr & MCOFSET);	/* adjust down to page boundary */
	addr &= ~MCOFSET;
	while (count > 0) {
		curpte = pageinpt(spti, addr, 0, PTLEVEL2);
		if ((curpte == NULL) || !PTEISMEM(*curpte)) {
			panic("badunlockmempage");
		}
		if (writeflag) *curpte |= (PG_REF | PG_MOD);
		unlockaddr(PTEMEM_ADDR(*curpte));
		addr += MCSIZE;
		count -= MCSIZE;
	}
}

/* Verify whether or not a range of user addresses is accessable for
 * reading/writing.  Returns nonzero if access is not allowed.  This
 * routine makes no attempt to make the pages resident, but will create
 * pages for null pte entries.
 */
checkmem(spti, addr, count, writeflag)
	register spt_t	spti;		/* address space to check */
	register unsigned long addr;	/* starting address */
	register long count;		/* number of bytes to check */
	register long writeflag;	/* nonzero if checking writes */
{
	register pte	ptedata;	/* current pte entry */

	if ((count<=0) || (addr>=MAXADDRS) || ((addr+count) >= MAXADDRS)) {
		return(-1);
	}
	count += (addr & MCOFSET);	/* adjust down to page boundary */
	addr &= ~MCOFSET;
	while (count > 0) {
		ptedata = getmap(spti, addr);
		if (ptedata == PTENULL) {		/* try to create it */
			if (pagein(spti, addr, -1) < 0) return(-1);
			ptedata = getmap(spti, addr);
		}
		if ((PTETYPE(ptedata) == PT_SPY) ||
			((writeflag) && (PTEPROT(ptedata) != PG_RWRW))) {
				return(-1);
		}
		addr += MCSIZE;
		count -= MCSIZE;
	}
	return(0);
}
