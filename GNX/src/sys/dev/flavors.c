# ifndef NOSCCS
static char *sccsid = "@(#)flavors.c	3.36	10/19/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

/* Module to handle debugging typeout routines.  All output in this module
 * should be done using cprintf instead of printf, so that the message buffer
 * is not cluttered up with the debugging information.
 */

#ifdef	FLAVORDUMP

#include "../h/param.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/systm.h"
#include "../h/tty.h"
#include "../h/vm.h"
#include "../h/devvm.h"
#include "../h/icu.h"
#include "../h/rs.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/buf.h"
#include "../h/dk.h"
#include "../h/vmswap.h"
#include "../h/vmtune.h"
#include "../h/vmmeter.h"
#include "../h/reboot.h"

extern	struct	vmmeter	sum, rate;
extern	int	mempgs[MEMCHKS];
extern	int	pagesfree,
		availpages,
		swapused,
		swapretries,
		mmuinfoflag,
		svcinfoflag,
		nameinfoflag,
		furetries,
		fudata1,
		fudata2,
		lastspti,
		topmem,
		topsys,
		swapouts,
		prepages,
		zerofree,
		ipc;

#if NSIO > 0
extern struct tty sio[];
#endif
#if NRS > 0
extern struct tty rs_tty[];
#endif
#if NBM > 0
extern struct tty bitmap[];
#endif

struct	cst	*cststatrover;

/* Called from console input interrupt routine to type debugging stuff */
flavordump(c)
register char c;
{
	register struct	proc *pp;

	switch (c) {
		case 'a':
			mmuinfoflag = !mmuinfoflag;
			if (mmuinfoflag)
				cprintf("Explaining abort traps\n");
			else
				cprintf("Quiet abort traps restored\n");
			break;

		case 'B':
			dobpt();
			break;

		case 'c':
			dumpcst();
			break;

		case 'd':
			dumpswap();
			break;

		case 'e':
			svcinfoflag = !svcinfoflag;
			if (svcinfoflag)
				cprintf("Explaining svc failures\n");
			else
				cprintf("Quiet svc failures restored\n");
			break;

		case 'E':
			dumpecc();
			break;

		case 'm':
			cprintf("\n");
			dumpptetable(lastspti);
			break;

		case 'M':
			cprintf("\n");
			dumpptetable(SYSSPTI);
			break;

		case 'n':
			nameinfoflag = !nameinfoflag;
			if (nameinfoflag)
				cprintf("Printing namei strings\n");
			else
				cprintf("Quiet namei restored\n");
			break;

		case 'p':
			cprintf("\nMemory pages free: %d  Available pages: %d\n",
				pagesfree, availpages);
			for (pp = proc; pp < procNPROC; pp++)
				procstat(pp);
			break;

		case 'R':
			cprintf("\nReboot\n");
			pp = u.u_procp;
			pp->p_stat = SRUN;
			(void) spl0();
			boot(RB_BOOT,0);
			break;

		case 's':
			cprintf("\nStatistics: ");
			cprintf(" user %d sec, system %d sec, idle %d sec \n",
				cp_time[CP_USER]/60, cp_time[CP_SYS]/60,
				cp_time[CP_IDLE]/60);
			schedstat();
			break;

		case 'S':
			cprintf("\nShutdown\n");
			pp = u.u_procp;
			pp->p_stat = SRUN;
			(void) spl0();
			boot(RB_BOOT,RB_HALT);
			break;

		case 't':
			procstat(&proc[lastspti]);
			break;

		case 'v':
			dumpspt();
			break;

		case 0177:
			psignal(&proc[lastspti], SIGKILL);
			break;

#		if NSIO > 0
		/* Grab a bunch of flavor printouts for the SIO board... */
		case '0':  case '1':  case '2':  case '3':
		case '4':  case '5':  case '6':  case '7':
			sio_dump (c - '0');
			break;
		case '!':
			sio_dumpmon ();
			break;
		case '@':
			sio_unwedge (0);
			break;
#		endif NSIO
	}
}

/* Type the status of the specified process */
procstat(pp)
	register struct proc *pp;
{
	register char *str;
	register long slpadr;
	register int	ni;
	register char	ch;

	if (pp->p_stat == 0) return;
	ni = pp->p_pid;
	str = "";
	if (ni < 10000) str = " ";
	if (ni < 1000) str = "  ";
	if (ni < 100) str = "   ";
	if (ni < 10) str = "    ";
	ch = '-';
	if ((pp->p_flag&SLOAD) || (pp == u.u_procp)) ch = '*';
	cprintf("%c%s%d %s", ch, str, ni, pp->p_infoname);
	switch (pp->p_stat) {
		case SSLEEP:
		case SWAIT:
			str = "slp";
			break;
		case SRUN:
			str = "run";
			break;
		case SIDL:
			str = "ini";
			break;
		case SZOMB:
			str = "zom";
			break;
		case SSTOP:
			str = "stp";
			break;
		default:
			str = "???";
	}
	slpadr = pp->p_infopc;		/* get runtime and user mode pc */
	ni = pp->p_infotime;
	ch = 0;
	if (u.u_procp == pp) {
		slpadr = u.u_ar0[RPC];	/* update if current user */
		ni = u.u_vm.vm_utime + u.u_vm.vm_stime;
		ch = 1;
	}
	cprintf(" %s at %x", str, slpadr);
	if (ch) cprintf("*");
	cprintf(" time %d.%d size %d res %d",
		ni/hz, ((ni%hz) * 10) / hz,
		spt[pp-proc].spt_usedpages,
		spt[pp-proc].spt_mempages);

	if (pp->p_stat == SSLEEP) {
		slpadr = (int) pp->p_wchan;
		str = "??";
		if (slpadr == (int)&lbolt) str = "lbolt";
		if (slpadr == (int)&runout) str = "runout";
		if (slpadr == (int)&runin) str = "runin";
		if (slpadr == (int)&u) str = "pause";
		if (slpadr == (int)&ipc) str = "trace";
		if (slpadr == (int)&pagesfree) str = "freepage";
#if NRS > 0
		if ((slpadr >= (int)rs_tty) && (slpadr < (int)&rs_tty[1]))
			str = "tty";
#endif
#if NSIO > 0
		if ((slpadr >= (int)sio) && (slpadr < (int)&sio[8]))
			str = "tty";
#endif
#if NBM > 0
		if ((slpadr >= (int)bitmap) && (slpadr < (int)&bitmap[1]))
			str = "tty";
#endif
		if ((slpadr >= (int)proc) && (slpadr < (int)procNPROC))
			str = "proc";
		if ((slpadr >= (int)buf) && (slpadr < (int)&buf[nbuf]))
			str = "buf";
		if ((slpadr>=(int)cst) && (slpadr<=(int)addrtocst(topmem-1)))
			str = "page";
		cprintf(" wchn %x (%s)", slpadr, str);
	}
	cprintf("\n");
}

/* Dump a specified page table */
dumpptetable(spti)
	register spt_t	spti;		/* spt index */
{
	register int	begaddr;
	register int	pages;
	register int	addr;
	register int	prot;
	int	type, ref;
	int	x, i, totalpages, memorypages, swappedpages;
	int	refpages, map, newmap, shared, newshared;
	char	*str;

	x = spl7();
	cprintf("\nFree pages: %d, Paged pages: %d\n", pagesfree, swapused);
	cprintf("Memory map for proc %d (", proc[spti].p_pid);
	for (addr = 0; addr < 8; addr++)
		cprintf("%c", proc[spti].p_infoname[addr]);
	cprintf("):\n");
	totalpages = 0;
	swappedpages = 0;
	memorypages = 0;
	refpages = 0;
	addr = 0;
	while ((addr >= 0) && (addr < MAXADDRS)) {
		addr = scanmap(spti, addr, 0);
		if (addr < 0) break;
		begaddr = addr;
		map = getmap(spti, addr);
		type = PTETYPE(map);
		prot = PTEPROT(map);
		ref = map & (PG_REF|PG_MOD);
		shared = 0;
		if (type == PT_SPT) shared = 1;
		if ((type == PT_MEM) || (type == PT_LOCK)) {
			shared = (addrtocst(PTEMEM_ADDR(map))->cst_spti!=spti);
		}
		pages = 0;
		while (1) {
			pages++;
			addr += MCSIZE;
			if (addr >= MAXADDRS) break;
			addr = scanmap(spti, addr, 0);
			if (addr != (begaddr + pages * MCSIZE)) break;
			newmap = getmap(spti, addr);
			if ((newmap & (PG_REF|PG_MOD)) != ref) break;
			if (PTEPROT(newmap) != prot) break;
			if (PTETYPE(newmap) != type) break;
			newshared = 0;
			if (type == PT_SPT) newshared = 1;
			if ((type == PT_MEM) || (type == PT_LOCK)) {
				newshared = (addrtocst(
					PTEMEM_ADDR(newmap))->cst_spti!=spti);
			}
			if (newshared != shared) break;
		}
		if (prot == PG_RONA) str = "SYS RO";
		if (prot == PG_RWNA) str = "SYS RW";
		if (prot == PG_RWRO) str = "RO";
		if (prot == PG_RWRW) str = "RW";
		cprintf(str);
		switch (type) {
			case PT_MEM:	str = "\tMEM ";
					memorypages += pages;
					break;
			case PT_LOCK:	str = "\tLOCK ";
					memorypages += pages;
					break;
			case PT_DISK:	str = "\tDISK ";
					swappedpages += pages;
					break;
			case PT_SPY:	str = "\tSPY ";
					break;
			case PT_SPT:	str = "\tSPT ";
					break;
			case PT_IOP:	str = "\tIOP ";
					memorypages += pages;
					break;
			default:	str = "\t??? ";
		}
		cprintf(str);
		if (shared) cprintf("S");
		if (map & PG_REF) {
			cprintf("R");
			refpages += pages;
		}
		if (map & PG_MOD) cprintf("M");
		cprintf("\t%x-%x\t%d page%s\n", begaddr,
			begaddr + pages * MCSIZE - 1, pages,
			(pages == 1) ? "" : "s");
		totalpages += pages;
	}
	cprintf("Total size: %d (%d resident %d swapped %d referenced)\n",
		totalpages, memorypages, swappedpages, refpages);
	splx(x);
}

/* Dump the swapping lists */
dumpswap()
{
	register struct	swap	*sw;
	int	s;

	s = spl7();
	for (sw = swapbuf; sw < swapbufNSWAPBUF; sw++) {
		cprintf("\nSwap structure %d:\n", sw - swapbuf);
		cprintf("Device %d   Direction %s  Flags %x  Count %d\n",
			sw->sw_buf.b_dev,
			(sw->sw_dir == B_READ) ? "read" : "write",
			sw->sw_buf.b_flags, sw->sw_count);
		cprintf("Active list:");
		dumpcstlist(sw->sw_active);
		cprintf("Queued list:");
		dumpcstlist(sw->sw_queue);
	}
	splx(s);
}


/* Dump a list of cst entries */
dumpcstlist(pg)
	register struct	cst	*pg;
{
	while (pg) {
		cprintf(" %d", csttopg(pg));
		pg = pg->cst_next;
	}
	cprintf("\n");
}

/* Dump some of physical memory data */
dumpcst()
{
	register struct	cst *pg;	/* current page */
	register int	cnt;		/* counter */

	cprintf("\n\
page	type	spti	sptp	lock	share	backup	next	flags\n");
	if (cststatrover == NULL) cststatrover = addrtocst(topsys);
	cnt = 20;
	pg = cststatrover;
	while (cnt > 0) {
		if (pg->cst_type != PTY_FREE) {
			cprintf("%d\t%d\t%d\t%d\t%d\t%d\t%c%d\t%d\t%d\n",
			csttopg(pg), pg->cst_type, pg->cst_spti, pg->cst_sptp,
			pg->cst_lock, pg->cst_share, 
			(pg->cst_file ? 'F' : ' '), pg->cst_backup,
			pg->cst_next ? csttopg(pg->cst_next) : -1,
			pg->cst_intrans);
			cnt--;
		}
		pg++;
		if (pg > addrtocst(topmem - 1)) pg = addrtocst(topsys);
	}
	cststatrover = pg;
}

/* Dump memory configuration, and, eventually, ecc errors */
dumpecc()
{
	register int idx;
	register unsigned long memaddr;
	register unsigned long memamt;
	register unsigned long totmem;

	idx = 0;
	totmem = 0;
	cprintf("Memory configuration:\n");
	for (;;) {
		if (mempgs[idx] == -1) break;
		memaddr = mempgs[idx] << MCSHIFT;
		memamt = MEMCHUNK;
		while ((mempgs[idx] + (MEMCHUNK >> MCSHIFT)) == mempgs[++idx]) {
			memamt += MEMCHUNK;
		}
		cprintf("%x-%x\t%d K\n", memaddr, memaddr + memamt - 1,
				memamt / 1024);
		totmem += memamt;
	}
	cprintf("Total: %d K\n", totmem / 1024);
}

/* Dump the spt table */
dumpspt()
{
	register struct	spt	*fs;		/* spt pointer */

	cprintf("\n\
spti	ptb1	inode	prc shr	age	tables	used	mem	text	flags\n");
	for (fs = spt; fs < sptNSPT; fs++) {
		cprintf("%d\t%x\t%x\t%d %d\t%d\t%d\t%d\t%d\t%d\t%x\n",
		fs - spt, (int)fs->spt_pte1, (int)fs->spt_inode,
		fs->spt_proccount, fs->spt_share, fs->spt_age,
		fs->spt_tablepages, fs->spt_usedpages, fs->spt_mempages,
		fs->spt_tmempages, fs->spt_flags);
	}
}

/* Dump some scheduler statistics */
schedstat()
{
	register int	i;

	cprintf("\nContext switches: %d", sum.v_swtch);
	cprintf("  Avefree: %d  Avefree30: %d\n", avefree, avefree30);
	cprintf("Pagesfree: %d  Zeroedfree: %d\n", pagesfree, zerofree);
	cprintf("Pageins: %d  Pageouts: %d  Freepageouts: %d  Idlepageclears: %d\n",
			sum.v_pgin, sum.v_pgout, sum.v_fpgout, sum.v_pgszrod);
	cprintf("Pagein rate: %d  Pageout rate: %d  Deficit: %d  Prepages: %d\n",
			rate.v_pgin, rate.v_pgout, deficit, prepages);
	cprintf("Dormant text swaps: %d  Partial swaps: %d\n",
			sum.v_tswpout, sum.v_pswpout);
	cprintf("Swapouts: %d  Swapped pages: %d  Retries: %d  Disk status: %x\n",
		swapouts, sum.v_pgout, swapretries, *DCUA_STATUS & 0xff);
	cprintf("Total available pages: %d  Swapping space used: %d\n",
		availpages, swapused);
	cprintf("Fubyte retries: %d  Data: %x %x\n",
		furetries, fudata1, fudata2);
}

#endif FLAVORDUMP
