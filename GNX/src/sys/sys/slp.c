/*
 * slp.c: version 3.21 of 8/27/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)slp.c	3.21 (NSC) 8/27/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/file.h"
#include "../h/inode.h"
#include "../h/buf.h"
#include "../h/vm.h"
#include "../h/vmsystm.h"
#include "../h/devvm.h"

int mpid=0;
extern	int	pagesfree;
extern	int	desfree;
struct	forkstat forkstat = 0;

#define SQSIZE 0100	/* Must be power of 2 */
#define HASH(x)	(( (int) x >> 5) & (SQSIZE-1))
struct proc *slpque[SQSIZE];

/*
 * Give up the processor till a wakeup occurs
 * on chan, at which time the process
 * enters the scheduling queue at priority pri.
 * The most important effect of pri is that when
 * pri<=PZERO a signal cannot disturb the sleep;
 * if pri>PZERO signals will be processed.
 * Callers of this routine must be prepared for
 * premature return, and check that the reason for
 * sleeping has gone away.
 */
sleep(chan, pri)
caddr_t chan;
{
	register struct proc *rp, **hp;
	register s;

	rp = u.u_procp;
	s = spl6();
	if (chan==0 || rp->p_stat != SRUN || rp->p_rlink)
		panic("sleep");
	rp->p_wchan = chan;
	rp->p_slptime = 0;
	rp->p_pri = pri;
	hp = &slpque[HASH(chan)];
	rp->p_link = *hp;
	*hp = rp;
	if (pri > PZERO) {
		if (issig()) {
			if (rp->p_wchan)
				unsleep(rp);
			rp->p_stat = SRUN;	/* redundant set? */
			(void) spl0();
			goto psig;
		}
		if (rp->p_wchan == 0)
			goto out;
		rp->p_stat = SSLEEP;
		(void) spl0();
		swtch();
		if (issig())
			goto psig;
	} else {
		rp->p_stat = SSLEEP;
		(void) spl0();
		swtch();
	}
out:
	splx(s);
	return;

	/*
	 * If priority was low (>PZERO) and
	 * there has been a signal, execute non-local goto through
	 * u.u_qsav, aborting the system call in progress (see trap.c)
	 * (or finishing a tsleep, see below)
	 */
psig:
	resume(&u.u_qsav);
	/*NOTREACHED*/
}

/*
 * Sleep on chan at pri.
 * Return in no more than the indicated number of seconds.
 * (If seconds==0, no timeout implied)
 * Return	TS_OK if chan was awakened normally
 *		TS_TIME if timeout occurred
 *		TS_SIG if asynchronous signal occurred
 *
 * SHOULD HAVE OPTION TO SLEEP TO ABSOLUTE TIME OR AN
 * INCREMENT IN MILLISECONDS!
 */
/*
tsleep(chan, pri, seconds)
	caddr_t chan;
	int pri, seconds;
{
	label_t lqsav;
	register struct proc *pp;
	register sec, n, rval;

	pp = u.u_procp;
	n = spl7();
	sec = 0;
	rval = 0;
	if (pp->p_clktim && pp->p_clktim<seconds)
		seconds = 0;
	if (seconds) {
		pp->p_flag |= STIMO;
		sec = pp->p_clktim-seconds;
		pp->p_clktim = seconds;
	}
	bcopy((caddr_t)u.u_qsav, (caddr_t)lqsav, sizeof (label_t));
	if (setjmp(u.u_qsav))
		rval = TS_SIG;
	else {
		sleep(chan, pri);
		if ((pp->p_flag&STIMO)==0 && seconds)
			rval = TS_TIME;
		else
			rval = TS_OK;
	}
	pp->p_flag &= ~STIMO;
	bcopy((caddr_t)lqsav, (caddr_t)u.u_qsav, sizeof (label_t));
	if (sec > 0)
		pp->p_clktim += sec;
	else
		pp->p_clktim = 0;
	splx(n);
	return (rval);
}
*/

/*
 * Remove a process from its wait queue
 */
unsleep(p)
	register struct proc *p;
{
	register struct proc **hp;
	register s;

	s = spl6();
	if (p->p_wchan) {
		hp = &slpque[HASH(p->p_wchan)];
		while (*hp != p)
			hp = &(*hp)->p_link;
		*hp = p->p_link;
		p->p_wchan = 0;
	}
	splx(s);
}

/*
 * Wake up all processes sleeping on chan.
 */
wakeup(chan)
	register caddr_t chan;
{
	register struct proc *p, **q, **h;
	int s;

	s = spl6();
	h = &slpque[HASH(chan)];
restart:
	for (q = h; p = *q; ) {
		if (p->p_rlink || p->p_stat != SSLEEP && p->p_stat != SSTOP)
			panic("wakeup");
		if (p->p_wchan==chan) {
			p->p_wchan = 0;
			*q = p->p_link;
/*			p->p_slptime = 0; */
			if (p->p_stat == SSLEEP) {
				setrun(p);
				goto restart;
			}
		} else
			q = &p->p_link;
	}
	splx(s);
}

/*
 * Set the process running;
 * arrange for it to be swapped in if necessary.
 */
setrun(p)
	register struct proc *p;
{
	register int s;

	s = spl6();
	switch (p->p_stat) {

	case 0:
	case SWAIT:
	case SRUN:
	case SZOMB:
	default:
		panic("setrun");

	case SSTOP:
	case SSLEEP:
		unsleep(p);		/* e.g. when sending signals */
		break;

	case SIDL:
		break;
	}
	p->p_stat = SRUN;
	if (p->p_flag & SLOAD)
		setrq(p);
	splx(s);
	if (p->p_pri < curpri) {
		runrun++;
	}
	if ((p->p_flag&SLOAD) == 0) {
		if (runout != 0) {
			runout = 0;
			wakeup((caddr_t)&runout);
		}
		wantin++;
	}
}

/*
 * Set user priority.
 * The rescheduling flag (runrun)
 * is set if the priority is better
 * than the currently running process.
 */
setpri(pp)
	register struct proc *pp;
{
	register int p;

	p = (pp->p_cpu & 0377)/4;
	p += PUSER + 2*(pp->p_nice - NZERO);
	if (spt[pp->p_spti].spt_mempages > pp->p_maxrss && pagesfree < desfree)
		p += 2*4;	/* effectively, nice(4) */
	if (p > 127)
		p = 127;
	if (p < curpri) {
		runrun++;
		/* aston(); */
	}
	pp->p_usrpri = p;
	return(p);
}

/*
 * Create a new process-- the internal version of
 * sys fork.
 * It returns 1 in the new process, 0 in the old.
 */
newproc(isvfork)
{
	int a1, a2;
	spt_t	oldspti;
	register struct proc *rpp, *rip;
	register int n;
	register int procid;

	rip = NULL;
	/*
	 * First, just locate a slot for a process
	 * and copy the useful info from this process into it.
	 * The panic "cannot happen" because fork has already
	 * checked for the existence of a slot.
	 */
retry:
	mpid++;
	if (mpid >= 30000) {
		mpid = 0;
		goto retry;
	}
	for(rpp = proc; rpp < procNPROC; rpp++) {
		if(rpp->p_stat == NULL && rip==NULL)
			rip = rpp;
		if (rpp->p_pid==mpid || rpp->p_pgrp==mpid)
			goto retry;
	}
	if ((rpp = rip)==NULL)
		panic("no procs");

	/*
	 * make proc entry for new proc
	 */

	rip = u.u_procp;
	rpp->p_stat = SIDL;
	rpp->p_clktim = 0;
	rpp->p_flag = SLOAD | (rip->p_flag & (SDETACH|SNUSIG|SFPU));
	rpp->p_pptr = rip;
	rpp->p_siga0 = rip->p_siga0;
	rpp->p_siga1 = rip->p_siga1;
	rpp->p_cursig = 0;
	rpp->p_wchan = 0;
	if (isvfork) rpp->p_flag |= SVFORK;
	rpp->p_maxrss = rip->p_maxrss;
	rpp->p_uid = rip->p_uid;
	rpp->p_pgrp = rip->p_pgrp;
	rpp->p_nice = rip->p_nice;
	rpp->p_pid = mpid;
	rpp->p_ppid = rip->p_pid;
	rpp->p_time = 0;
	rpp->p_cpu = 0;
	rpp->p_pctcpu = 0;
	rpp->p_cpticks = 0;
	rpp->p_infotime = 0;
	rpp->p_infopc = u.u_ar0[RPC];
	n = PIDHASH(rpp->p_pid);
	rpp->p_idhash = pidhash[n];
	procid = rpp - proc;
	pidhash[n] = procid;

	/*
	 * make duplicate entries
	 * where needed
	 */

	for(n=0; n<NOFILE; n++)
		if(u.u_ofile[n] != NULL)
			u.u_ofile[n]->f_count++;
	u.u_cdir->i_count++;
	if (u.u_rdir)
		u.u_rdir->i_count++;
	if (u.u_xspti >= 0)
		spt[u.u_xspti].spt_proccount++;
	/*
	 * Partially simulate the environment
	 * of the new process so that when it is actually
	 * created (by copying) it will look right.
	 */
	oldspti = u.u_spti;
	if (!isvfork) {
		setspti(procid);		/* set our spt index */
		if (oldspti != SYSSPTI) {/* Don't copy kernel (making init)*/
			copyptetable(oldspti, u.u_spti);/* copy address space */
		forkstat.cntfork++;
		forkstat.sizfork += spt[oldspti].spt_usedpages;
		}
	}
	else {
		forkstat.cntvfork++;
		forkstat.sizvfork += spt[oldspti].spt_usedpages;
	}
	rpp->p_spti = rip->p_spti;
	swapinu(procid);		/* Bring new u. into memory */
	/*
	 * When the resume is executed for the new process,
	 * here's where it will resume.
	 */
	if (save(&u.u_ssav)) {
		return(1);
	}
	copyu(procid);		/* copy our u. to new process */
	/* Update the proc pointer in the newly init'ed u */
	((struct user *)(BOTXU + (procid * USIZE)))->u_procp = rpp;
	setspti(oldspti);
	(void) spl6();
	rpp->p_stat = SRUN;
	setrq(rpp);
	(void) spl0();
	if (isvfork) {		/* wait until child does an exit or exec */
		while (rpp->p_flag & SVFORK) sleep((caddr_t)rpp, PZERO - 1);
		u.u_dsize = rip->p_tdsize - u.u_tsize;	/* update data size */
	}
	return(0);
}
