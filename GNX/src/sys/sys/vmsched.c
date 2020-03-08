/*
 * vmsched.c: version 3.32 of 8/8/83
 * Mesa Unix Source
 */
# ifdef SCCS
static char *sccsid = "@(#)vmsched.c	3.32 (NSC) 8/8/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/buf.h"
#include "../h/vm.h"
#include "../h/vmtune.h"
#include "../h/vmmeter.h"

#ifdef float
double	avenrun[3];		/* load average, of runnable procs */
#else
long	avenrun[3]=0;		/* 1000 * load average, of runnable procs */
long	avenscale = 1000;	/* for programs to find with nlist */
#endif

struct	vmmeter cnt, rate, sum;
struct	vmtotal	total;

/*
 * The main loop of the scheduling (swapping) process.
 *
 * The basic idea is:
 *	see if anyone wants to be swapped in;
 *	swap out processes until there is room;
 *	swap him in;
 *	repeat.
 * If the paging rate is too high, or the average free memory
 * is very low, then we do not consider swapping anyone in,
 * but rather look for someone to swap out.
 *
 * The runout flag is set whenever someone is swapped out.
 * Sched sleeps on it awaiting work.
 *
 * Sched sleeps on runin whenever it cannot find enough
 * core (by swapping out or otherwise) to fit the
 * selected swapped process.  It is awakened when the
 * core situation changes and in any case once per second.
 *
 * sched DOESN'T ACCOUNT FOR PAGE TABLE SIZE IN CALCULATIONS.
 */

char	runout = 0;			/* scheduling flag */
int	wantin = 0;			/* more scheduling */

int	maxslp = MAXSLP;
int	maxpgio = MAXPGIO;
int	minfree = MINFREE;
int	desfree = DESFREE;
int	lotsfree = LOTSFREE;

int	avefree = 0;
int	avefree30 = 0;
int	deficit = 0;	/* estimate of needs of new swapped in procs */

extern	int	pagesfree;

#define	swappable(p) \
	(((p)->p_flag&(SSYS|SWEXIT|SPAGE|SLOAD))==SLOAD)

/* insure non-zero */
#define	nz(x)	(x != 0 ? x : 1)

#define imax(x,y)	(x > y ? x : y)
#define imin(x,y)	(x < y ? x : y)

#define	NBIG	4
#define	MAXNBIG	7
int	nbig = NBIG;

struct bigp {
	struct	proc *bp_proc;
	int	bp_pri;
	struct	bigp *bp_link;
} bigp[MAXNBIG], bplist;

sched()
{
	register struct proc *rp, *p;
	register int outpri, rppri;
	int desperate, deservin, needs, divisor;
	int freenow, justfreed;
	int runnum;			/* number of nonkernel runnable procs */

loop:
	wantin = 0;
	deservin = 0;
	runnum = -1;
	p = 0;
	/*
	 * See if paging system is overloaded; if so swap someone out.
	 * Conditions for hard outswap are:
	 *	1. if there are at least 2 runnable processes (on the average)
	 * and	2. the paging rate is excessive or memory is now VERY low.
	 * and	3. the short (5-second) and longer (30-second) average
	 *	   memory is less than desirable.
	 */
	if ((avenrun[0] >= 2*avenscale) &&
	    (imax(avefree, avefree30) < desfree) &&
	    (rate.v_pgin + rate.v_pgout > maxpgio || avefree < minfree)) {
		desperate = 1;
		needs = desfree;
		goto dohard;
	}
	desperate = 0;
	/*
	 * Not desperate for core,
	 * look for someone who deserves to be brought in.
	 */
	outpri = -20000;
	for (rp = proc; rp < procNPROC; rp++) switch(rp->p_stat) {

	case SRUN:
		if ((rp->p_flag&SLOAD) == 0) {
			rppri = rp->p_time -
			    rp->p_tdsize / nz((maxpgio/2) * (MCSIZE)) +
			    rp->p_slptime - (rp->p_nice-NZERO)*8;
			if (rppri > outpri) {
				p = rp;
				outpri = rppri;
			}
		}
		else runnum++;		/* count runnable process */
		continue;

	case SSLEEP:
	case SSTOP:
		if ((rp->p_slptime <= maxslp) || !swappable(rp)) continue;
		if ((pagesfree >= desfree) &&
		    (spt[rp->p_spti].spt_mempages > 0)) continue;
		/* Kick out deadwood, but first try to get the memory
		 * by swapping any dormant text segments
		 */
		freenow = pagesfree;
		if (pagesfree < desfree) {
			do {
				swapdormtext();
				justfreed = pagesfree - freenow;
				freenow = pagesfree;
			}
			while ((justfreed > 0) && (freenow < desfree));
		}
		if ((freenow < desfree) || spt[rp->p_spti].spt_mempages == 0) {
			(void) swapout(rp->p_spti, 0);
					/* kick out deadwood */
		}
		goto loop;
	}

	/*
	 * No one wants in, so nothing to do.
	 */
	if (outpri == -20000) {
		(void) spl6();
		if (wantin) {
			wantin = 0;
			sleep((caddr_t)&lbolt, PSWP);
		} else {
			runout++;
			sleep((caddr_t)&runout, PSWP);
		}
		(void) spl0();
		goto loop;
	}
	/*
	 * Decide how deserving this guy is.  If he is deserving
	 * we will be willing to work harder to bring him in.
	 * Needs is an estimate of how much core he will need.
	 * If he has been out for a while, then we will
	 * bring him in with 1/2 the core he will need, otherwise
	 * we are conservative.
	 */
	deservin = 0;
	divisor = 1;
	if (outpri > maxslp/2) {
		deservin = 1;
		divisor = 2;
	}
	needs = (p->p_tdsize + p->p_ssize) / MCSIZE;
	needs = imin(needs, lotsfree);
	needs = imin(needs, p->p_maxrss);
	if ((pagesfree - deficit > needs / divisor) || (runnum <= 0)) {
		deficit += needs;
		if (swapin(p))
			goto loop;
		deficit -= imin(needs, deficit);
	}
dohard:	if (hardswap(p, desperate, deservin, needs)) {
		/*
		 * Want to swap someone in, but can't
		 * so wait on runin.
		 */
		(void) spl6();
		runin++;
		sleep((caddr_t)&runin, PSWP);
		(void) spl0();
	}
	goto loop;
}

/*
 * Need resources (kernel map or memory), swap someone out.
 * Select the nbig largest jobs, then the oldest of these
 * is ``most likely to get booted.''
 */
hardswap(inp, desperate, deservin, advice)
	register struct proc *inp;
	int desperate, deservin;
{
	int sleeper, inpri, rppri;
	register struct bigp *bp, *nbp;
	register struct proc *p, *rp;
	int biggot, gives;
	int	runnum;			/* number of runnable processes */
	int	prevfree;

	prevfree = pagesfree;
	swapdormtext();			/* try to swap a dormant text first */
	if (pagesfree > prevfree) return(0);	/* found dormant text */

	sleeper = 0;
	if (nbig > MAXNBIG)
		nbig = MAXNBIG;
	if (nbig < 1)
		nbig = 1;
	biggot = 0;
	bplist.bp_link = 0;
	runnum = 0;
	for (rp = proc; rp < procNPROC; rp++) {
		if (rp->p_stat==SRUN) runnum++;
		if (!swappable(rp))
			continue;
		if (rp->p_stat==SZOMB)
			continue;
		if (rp == inp || rp == u.u_procp)
			continue;
		if (rp->p_slptime > maxslp &&
		    (rp->p_stat==SSLEEP&&rp->p_pri>PZERO||rp->p_stat==SSTOP)) {
			if (sleeper < rp->p_slptime) {
				p = rp;
				sleeper = rp->p_slptime;
			}
		} else if (!sleeper && (rp->p_stat==SRUN)
					|| rp->p_stat==SSLEEP) {
			rppri = spt[rp->p_spti].spt_mempages;
			if (biggot < nbig)
				nbp = &bigp[biggot++];
			else {
				nbp = bplist.bp_link;
				if (nbp->bp_pri > rppri)
					continue;
				bplist.bp_link = nbp->bp_link;
			}
			for (bp = &bplist; bp->bp_link; bp = bp->bp_link)
				if (rppri < bp->bp_link->bp_pri)
					break;
			nbp->bp_link = bp->bp_link;
			bp->bp_link = nbp;
			nbp->bp_pri = rppri;
			nbp->bp_proc = rp;
		}
	}
	if (!sleeper) {
		p = NULL;
		inpri = -1000;
		for (bp = bplist.bp_link; bp; bp = bp->bp_link) {
			rp = bp->bp_proc;

	/* Don't swap the only runnable process.
	 * The schedular is always runnable at this point, so check for
	 * just one more process runnable (ie two processes runnable)
	 */
			if ((rp->p_stat == SRUN) && (runnum <= 2)) continue;
			rppri = rp->p_time+rp->p_nice-NZERO;
			if (rppri >= inpri) {
				p = rp;
				inpri = rppri;
			}
		}
	}

	/*
	 * If we found a long-time sleeper, or we are desperate and
	 * found anyone to swap out, or if someone deserves to come
	 * in and we didn't find a sleeper, but found someone who
	 * has been in core for a reasonable length of time, then
	 * we kick the poor luser out.
	 */
	if (sleeper || desperate && p || deservin && inpri > maxslp) {
		if (desperate) {
			/*
			 * Want to give this space to the rest of
			 * the processes in core so give them a chance
			 * by increasing the deficit.
			 */
			gives = spt[p->p_spti].spt_mempages;
			gives = imin(gives, lotsfree);
			deficit += gives;
		} else
			gives = 0;	/* someone else taketh away */
		if (swapout(p->p_spti, advice) == 0)
			deficit -= imin(gives, deficit);
		return(0);
	}
	else return(-1);
}

vmmeter()
{
	register unsigned *cp, *rp, *sp, *end;

	deficit -= imin(deficit,
	    imax(deficit / 10, maxpgio / 2));
	ave(avefree, pagesfree, 5);
	ave(avefree30, pagesfree, 30);
	cp = &cnt.v_first; rp = &rate.v_first; sp = &sum.v_first;
	end = &cnt.v_last;
	while (cp <= end) {
		ave(*rp, *cp, 5);
		*sp += *cp;
		*cp = 0;
		rp++; cp++; sp++;
	}
	vmtoobig();
	if (time % 5 == 0) {
		vmtotal();
		cp = &cnt.v_rfirst; rp = &rate.v_rfirst; sp = &sum.v_rfirst;
		end = &cnt.v_rlast;
		while (cp <= end) {
			*rp = *cp;
			*sp += *cp;
			*cp = 0;
			rp++; cp++; sp++;
		}
	}
	if (avefree < minfree && runout || proc[0].p_slptime > maxslp/2) {
		runout = 0;
		runin = 0;
		wakeup((caddr_t)&runin);
		wakeup((caddr_t)&runout);
	}
}

vmtotal()
{
	register struct proc *p;
	register struct spt *sp;
	register struct vmtotal *tp = &total;
	int nrun = 0;

	tp->t_vmtxt = 0;
	tp->t_avmtxt = 0;
	tp->t_rmtxt = 0;
	tp->t_armtxt = 0;
	for (sp = sptFILESPTI; sp < sptNSPT; sp++) {
		if (sp->spt_inode) {
			tp->t_avmtxt += sp->spt_usedpages;
			tp->t_armtxt += sp->spt_mempages;
		}
		tp->t_vmtxt += sp->spt_usedpages;
		tp->t_rmtxt += sp->spt_mempages;
	}
	tp->t_vm = 0;
	tp->t_avm = 0;
	tp->t_rm = 0;
	tp->t_arm = 0;
	tp->t_rq = 0;
	tp->t_dw = 0;
	tp->t_pw = 0;
	tp->t_sl = 0;
	tp->t_sw = 0;
	sp = spt;
	for (p = proc; p < procNPROC; p++) {
		if (p->p_flag & SSYS)
			continue;
		if (p->p_stat && (p->p_stat != SZOMB)) {
			tp->t_vm += sp[p->p_spti].spt_usedpages;
			tp->t_rm += sp[p->p_spti].spt_mempages;
			switch (p->p_stat) {

			case SSLEEP:
			case SSTOP:
				if (p->p_pri <= PZERO)
					nrun++;
				if (p->p_flag & SPAGE)
					tp->t_pw++;
				else if (p->p_flag & SLOAD) {
					if (p->p_pri <= PZERO)
						tp->t_dw++;
					else if (p->p_slptime < maxslp)
						tp->t_sl++;
				} else if (p->p_slptime < maxslp)
					tp->t_sw++;
				if (p->p_slptime < maxslp)
					goto active;
				break;

			case SRUN:
			case SIDL:
				nrun++;
				if (p->p_flag & SLOAD)
					tp->t_rq++;
				else
					tp->t_sw++;
active:
				tp->t_avm += sp[p->p_spti].spt_usedpages;
				tp->t_arm += sp[p->p_spti].spt_mempages;
				break;
			}
		}
	}
	tp->t_free = avefree;
	loadav(avenrun, nrun);
}

/*
 * Constants for averages over 1, 5, and 15 minutes
 * when sampling at 5 second intervals.
 */
#ifdef float
double	cexp[3] = {
	0.9200444146293232,	/* exp(-1/12) */
	0.9834714538216174,	/* exp(-1/60) */
	0.9944598480048967,	/* exp(-1/180) */
};
#else
long	cexp[3] = {
	920,			/* exp(-1/12) * avenscale */
	983,			/* exp(-1/60) * avenscale */
	994,			/* exp(-1/180) * avenscale */
};
#endif

/*
 * Compute a tenex style load average of a quantity on
 * 1, 5 and 15 minute intervals.
 */
#ifdef float
loadav(avg, n)
	register double *avg;
	int n;
{
	register int i;

	for (i = 0; i < 3; i++)
		avg[i] = cexp[i] * avg[i] + n * (1.0 - cexp[i]);
}
#else
loadav(avg, n)
	register long *avg;
	int n;
{
	register int i;
	for (i=0; i < 3; i++) {
		avg[i] = (cexp[i] * avg[i]) / avenscale +
			 n * (avenscale - cexp[i]);
	}
}
#endif
