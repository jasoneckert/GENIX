/*
 * mem.c: version 3.5 of 10/10/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)mem.c	3.5 (NSC) 10/10/83";
# endif

/*
 * Memory special file
 *	minor device 0 is physical memory
 *	minor device 1 is kernel memory 
 *	minor device 2 is EOF/RATHOLE
 */

#include "../h/param.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/conf.h"
#include "../h/buf.h"
#include "../h/systm.h"
#include "../h/pte.h"
#include "../h/vm.h"
#include "../h/devvm.h"

int	mmplock;		/* interlock for physical memory references */

mmread(dev)
{
	switch (minor(dev)) {
		case 0:			/* read physical memory */
			mmpcopy(0);
			return;
		case 1:			/* read kernel virtual memory */
			mmkcopy(0);
			return;
		case 2:			/* read the null file */
			return;
	}
	u.u_error = ENXIO;
}

mmwrite(dev)
{
	switch (minor(dev)) {
		case 0:			/* write to physical memory */
			mmpcopy(1);
			return;
		case 1:			/* write to kernel virtual memory */
			mmkcopy(1);
			return;
		case 2:			/* write to the null file */
			u.u_offset += u.u_count;
			u.u_count = 0;
			return;
	}
	u.u_error = ENXIO;
}

/* Subroutine to read or write kernel virtual memory for user.  Very tricky
 * since we cannot allow a page fault.
 */
mmkcopy(writeflag)
{
	register unsigned long v;	/* kernel virtual address */
	register unsigned long c;	/* byte count */
	register pte	ptedata;	/* current kernel map entry */
	register long	physaddr;	/* address of kernel page */

	while (u.u_count != 0 && u.u_error == 0) {
		v = u.u_offset;
		if (v >= MAXADDRS)
			goto fault;
		c = MCSIZE - (v & MCOFSET);
		if (u.u_count < c)
			c = u.u_count;
		ptedata = getmap(SYSSPTI, v);
		if ((ptedata == PTENULL) ||
			((writeflag) && (PTEPROT(ptedata) == PG_RONA)))
				goto fault;
		physaddr = pagein(SYSSPTI, v, -1);
		if ((physaddr < 0) && (PTETYPE(ptedata) != PT_SPY))
			goto fault;		/* failed to page in */
		if (physaddr >= 0) lockaddr(physaddr);
		if (writeflag)
			v = copyin(u.u_base, (caddr_t)v, c);
		else
			v = copyout((caddr_t)v, u.u_base, c);
		if (physaddr >= 0) unlockaddr(physaddr);
		if (v)
			goto fault;
		u.u_count -= c;
		u.u_base += c;
		u.u_offset += c;
	}
	return;

fault:	u.u_error = EFAULT;
}


/* Subroutine to read or write physical memory for user.  Do this by
 * setting up a map entry to point to the physical address.  Only one
 * user at a time can be allowed through here.
 */
mmpcopy(writeflag)
{
	register unsigned long v;	/* physical address */
	register unsigned long c;	/* byte count */
	register unsigned long o;	/* offset within page */

	while (u.u_count != 0 && u.u_error == 0) {
		v = u.u_offset;
		if (v >= MAXADDRS)
			goto fault;
		o = v & MCOFSET;
		v &= ~MCOFSET;
		c = MCSIZE - o;
		if (u.u_count < c)
			c = u.u_count;
		while (mmplock & B_BUSY) {	/* if already in use, wait */
			mmplock |= B_WANTED;
			sleep((caddr_t)&mmplock, PRIBIO);
		}
		mmplock = B_BUSY;		/* busy now */
		setmap(SYSSPTI, TEMPPAGE, PTESPY(v, PG_RWNA));
		if (writeflag)
			v = copyin(u.u_base, (caddr_t)TEMPPAGE + o, c);
		else
			v = copyout((caddr_t)TEMPPAGE + o, u.u_base, c);
		if (mmplock & B_WANTED) wakeup((caddr_t)&mmplock);
		mmplock = 0;			/* available again */
		if (v)
			goto fault;
		u.u_count -= c;
		u.u_base += c;
		u.u_offset += c;
	}
	return;

fault:	u.u_error = EFAULT;
}
