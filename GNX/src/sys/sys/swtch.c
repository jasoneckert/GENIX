/*
 * swtch.c: version 3.10 of 8/30/83
 * Run queue manipulation and context switch
 */
# ifdef SCCS
static char *sccsid = "@(#)swtch.c	3.10 (NSC) 8/30/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/file.h"
#include "../h/inode.h"
#include "../h/mmu.h"
#include "../h/vm.h"
#include "../h/devvm.h"
#include "../h/pte.h"
#include "../h/vmmeter.h"

#ifdef FASTVAX
#define	NQS	32		/* 32 run queues */
struct	prochd {
	struct	proc *ph_link;	/* linked list of running processes */
	struct	proc *ph_rlink;
} qs[NQS];
int	whichqs;		/* bit mask summarizing non-empty qs's */
#else
struct	proc *runq;
#endif

long	swtchage;			/* counter for ageing */
char	curpri;				/* more scheduling */
char	noproc;				/* if nonzero, no process is running */

/*
 * Initialize the (doubly-linked) run queues
 * to be empty.
 */
rqinit()
{
	register int i;

#ifdef FASTVAX
	for (i = 0; i < NQS; i++)
		qs[i].ph_link = qs[i].ph_rlink = (struct proc *)&qs[i];
#else
	runq = NULL;
#endif
}

/*
 * PORTABLE DEFINITIONS OF setrq(), remrq() and swtch().
 */

#ifndef FASTVAX
/*
 * when you are sure that it
 * is impossible to get the
 * 'proc on q' diagnostic, the
 * diagnostic loop can be removed.
 */
setrq(p)
struct proc *p;
{
	register struct proc *q;
	register s;

	if ((p->p_stat != SRUN) || ((p->p_flag & SLOAD) == 0)) panic("setrq");
	s = spl6();
	for(q=runq; q!=NULL; q=q->p_rlink)
		if(q == p) {
			printf("proc on q\n");
			asm("bpt");
			goto out;
		}
	p->p_rlink = runq;
	runq = p;
out:
	splx(s);
}

/*
 * Remove runnable job from run queue.
 * This is done when a runnable job is swapped
 * out so that it won't be selected in swtch().
 * It will be reinserted in the runq with setrq
 * when it is swapped back in.
 */
remrq(p)
	register struct proc *p;
{
	register struct proc *q;
	int s;

	s = spl6();
	if (p == runq)
		runq = p->p_rlink;
	else {
		for (q = runq; q; q = q->p_rlink)
			if (q->p_rlink == p) {
				q->p_rlink = p->p_rlink;
				goto done;
			}
		panic("remque");
	}
done:
	p->p_rlink = 0;
	splx(s);
}

/*
 * This routine is called to reschedule the CPU.
 * if the calling process is not in RUN state,
 * arrangements for it to restart must have
 * been made elsewhere, usually by calling via sleep.
 * There is a race here. A process may become
 * ready after it has been examined.
 * In this case, idle() will be called and
 * will return in at most 1HZ time.
 * i.e. its not worth putting an spl() in.
 */
swtch()
{
	register n;
	register struct proc *p, *pp;

	/*
	 * Remember current context on per-process stack.
	 * If "usual" resume thru ssav, return from swtch.
	 */
	if ( save(&u.u_ssav)) return;

	noproc = 1;
loop:
	(void) spl6();
	runrun = 0;
	pp = NULL;
	n = 128;

	/*
	 * Search for highest-priority runnable process
	 */
	for(p=runq; p!=NULL; p=p->p_rlink) {
		if(p->p_pri <= n) {
			pp = p;
			n = p->p_pri;
		}
	}
	/*
	 * If no process is runnable, idle.
	 */
	p = pp;
	if(p == NULL) {
		idleclearpage();	/* zero part of a free page */
		idle();
		goto loop;
	}
	if ((p->p_stat != SRUN) || ((p->p_flag & SLOAD) == 0)) panic("swtch");
	remrq(p);
	curpri = n;
	cnt.v_swtch++;
	noproc = 0;

/*
 * Now we want to change u. areas.  This is a little tricky because
 * we are running on the stack in the u. area we are about to move away
 * from.  If we just change the u. mappings our stack will go away.
 * Routine movestack will kludge this up for us -- it changes the
 * stack and frame pointers to point, not at the u. area itself, but
 * at the appropriate part of the BOTXU space whose real memory actually
 * underlies the spy-mapped u.
 * This keeps us running on the old stack until we do the resume.
 *
 * This must be done while interrupts are still off.
 *
 * someday must make sure level 1 pte is in memory.
 */
	if (p != u.u_procp) {
		if (u.u_procp->p_flag & SFPU) {
			savefpu();	/* save floating point registers */
			disablefpu();
		}
		if (u.u_procp->p_flag & SMMU) {
			clearben();
			mtpr(bpr0, 0);
			mtpr(bpr1, 0);
			u.u_bcnt = mfpr(bcnt);
			n = mfpr(sc);
			if (n) {
				u.u_pf0 = mfpr(pf0);
				u.u_pf1 = mfpr(pf1);
				u.u_sc = mfpr(sc);
			}
			mtpr(sc,0);
			mtpr(pf0, 0);
			mtpr(pf1, 0);

		}
		u.u_procp->p_infopc = u.u_ar0[RPC];
		u.u_procp->p_infotime = u.u_vm.vm_utime + u.u_vm.vm_stime;
		movestack((u.u_procp - proc)*USIZE + BOTXU - BOTU);
		swtchu(p - proc);	/* set mapping to new u. */
		(void) spl0();
		p->p_infoage = ++swtchage;
		if (p->p_flag & SFPU) {
			enablefpu();	/* restore floating point registers */
			restorefpu();
		}
		mtpr(ptb1, spt[u.u_spti].spt_pte1);	/* set page map */
		if (p->p_flag & SMMU) {
			mtpr(bpr0, u.u_bpr0);
			mtpr(bpr1, u.u_bpr1);
			mtpr(bcnt, u.u_bcnt);
			mtpr(sc, 0);
			mtpr(pf0, 0);
			mtpr(pf1, 0);
		}
		resume(&u.u_ssav);	/* resume */
	}
	(void) spl0();
}
#endif
