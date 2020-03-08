/*
 * clock.c: version 3.25 of 8/21/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)clock.c	3.25 (NSC) 8/21/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/callout.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/vm.h"
#include "../h/reg.h"
#include "../h/psr.h"
#include "../h/icu.h"
#include "../h/dk.h"
#include "../h/vlimit.h"

struct	callout	calltodo, *callfree=NULL;
time_t	cp_time[CPUSTATES]=0;
time_t	time=0;			/* time in sec from 1970 */
time_t	bootime=0;		/* time of last boot */
int	lbolt=0;		/* time of day in 60th not in time */
char	runrun=0;		/* processor scheduling flag */
char	runin=0;		/* swapping scheduling flag */
extern	long	avenscale;	/* scaling factor for fake floating point */

extern 	char *panicstr;

#define	SCHMAG	8/10

/*
 * Hardclock is called straight from
 * the real time clock interrupt.
 * We limit the work we do at real clock interrupt time to:
 *	decrementing time to callouts
 *	recording cpu time usage
 *	modifying priority of current process
 *	arrange for soft clock interrupt
 *	kernel pc profiling
 *
 * At software (softclock) interrupt time we:
 *	implement callouts
 *	maintain date
 *	lightning bolt wakeup (every second)
 *	alarm clock signals
 *	jab the scheduler
 *
 * On the mesa, softclock interrupts are implemented by
 * software interrupts.  Note that we may have multiple softclock
 * interrupts compressed into one (due to excessive interrupt load),
 * but that hardclock interrupts should never be lost.
 *
 * The low word of the ps parameter contains the interrupted "processor"
 * level instead of the mod register.
 */
#undef BASEPRI
#define BASEPRI(p) (p.mod != 0)

hardclock(pc, ps)
char *pc;
struct modpsr ps;
{
	register struct callout *p1;
	register struct	proc *pp;
	register struct spt *sptp;
	register struct user *up;
	int cpstate;
	int *cpulim;

	/*
	 * update callout times
	 */
	for (p1 = calltodo.c_next; p1 && p1->c_time <= 0; p1 = p1->c_next)
		;
	if (p1)
		p1->c_time--;

	/*
	 * Maintain iostat and per_process cpu statistics
	 */
	up = (struct user *)&u;
	if (!noproc) {
		sptp = &spt[up->u_spti];
		up->u_vm.vm_idsrss += sptp->spt_mempages - sptp->spt_tmempages;
		up->u_vm.vm_ixrss += sptp->spt_tmempages;
		if (up->u_vm.vm_maxrss < sptp->spt_mempages) {
			up->u_vm.vm_maxrss = sptp->spt_mempages;
		}
		cpulim = &up->u_limit[LIM_CPU]; /* avoid array references */
		if ((up->u_vm.vm_utime + up->u_vm.vm_stime + 1)/hz > *cpulim) {
			if ((up->u_procp->p_flag & SGRCGVN) == 0) {
				up->u_procp->p_flag |= SGRCGVN;
				if (*cpulim < INFINITY - 5) *cpulim += 5;
				else *cpulim = INFINITY;
			}
			else {
				/* force the signal to default action, so
				 * the process will be terminated */
				up->u_procp->p_siga0 &= ~(1L << (SIGXCPU - 1));
				up->u_procp->p_siga1 &= ~(1L << (SIGXCPU - 1));
			}
			psignal(up->u_procp,SIGXCPU);
		}
	}
	/*
	 * Update iostat information.
	 */
	if (USERMODE(ps)) {
		up->u_vm.vm_utime++;
		if(up->u_procp->p_nice > NZERO)
			cpstate = CP_NICE;
		else
			cpstate = CP_USER;
	} else {
		cpstate = CP_SYS;
		if (noproc)
			cpstate = CP_IDLE;
		else
			up->u_vm.vm_stime++;
	}
	cp_time[cpstate]++;

	/*
	 * Adjust priority of current process.
	 */
	if (!noproc) {
		pp = up->u_procp;
		pp->p_cpticks++;
		if(++pp->p_cpu == 0)
			pp->p_cpu--;
		if(pp->p_cpu % 4 == 0) {
			(void) setpri(pp);
			if (pp->p_pri >= PUSER)
				pp->p_pri = pp->p_usrpri;
		}
	}

	/*
	 * Time moves on.
	 */
	++lbolt;

#	if NBM > 0
	update_cursor ();
#	endif

#ifdef notdef
	/*
	 * For now... prod the disk 5 times a second (not after panics)
	 */

	if (panicstr == 0) {
		if ((lbolt % (hz/5)) == 0) {
			interrupt(DISK);
			interrupt(TAPE);
		}
	}
#endif notdef
	/*
	 * Schedule a software interrupt for the rest
	 * of clock activities.
	 */
	setsoftclock();
}

/*
 * The digital decay cpu usage priority assignment is scaled to run in
 * time as expanded by the 1 minute load average.  Each second we
 * multiply the the previous cpu usage estimate by
 *		nrscale*avenrun[0]
 * The following relates the load average to the period over which
 * cpu usage is 90% forgotten:
 *	loadav 1	 5 seconds
 *	loadav 5	24 seconds
 *	loadav 10	47 seconds
 *	loadav 20	93 seconds
 * This is a great improvement on the previous algorithm which
 * decayed the priorities by a constant, and decayed away all knowledge
 * of previous activity in about 20 seconds.  Under heavy load,
 * the previous algorithm degenerated to round-robin with poor response
 * time when there was a high load average.
 */
#undef ave
#ifdef float
#define	ave(a,b) ((int)(((int)(a*b))/(b+1)))
#else
#define	ave(a,b) (((a*b)/avenscale)/((b/avenscale)+1))
#endif
int	nrscale = 2;
#ifdef float
extern	double	avenrun[];
double	*avnrun = avenrun;
#else
extern	long	avenrun[];
long	*avnrun = avenrun;
#endif

/*
 * Constant for decay filter for cpu usage field
 * in process table (used by ps au).
 */
#ifdef	float
double	ccpu = 0.95122942450071400909;		/* exp(-1/20) */
#else
long	ccpu = 951;				/* exp(-1/20) * avenscale */
#endif

/*
 * Software clock interrupt.
 * This routine runs at lower priority than device interrupts.
 */
softclock(pc,ps)
caddr_t pc;
struct modpsr ps;
{
	register struct callout *p1;
	register struct proc *pp;
	register int a, s;
	caddr_t arg;
	int (*func)();

	/*
	 * Perform callouts (but not after panic's!)
	 */
	if (panicstr == 0) {
		for (;;) {
			s = spl7();
			if ((p1 = calltodo.c_next) == 0 || p1->c_time > 0) {
				splx(s);
				break;
			}
			calltodo.c_next = p1->c_next;
			arg = p1->c_arg;
			func = p1->c_func;
			p1->c_next = callfree;
			callfree = p1;
			(void) splx(s);
			(*func)(arg);
		}
	}

	/*
	 * Reschedule every 1/10th of a second
	 */
	if ((lbolt % (hz/10)) == 0) {
		runrun++;
		/*aston();*/
	}

	if(lbolt >= hz) {
		/*
		 * This doesn't mean much on VAX since we run at
		 * software interrupt time... if hardclock()
		 * calls softclock() directly, it prevents
		 * this code from running when the priority
		 * was raised when the clock interrupt occurred.
		 */
		if (BASEPRI(ps))
			return;
		/*
		 * If we didn't run a few times because of
		 * long blockage at high ipl, we don't
		 * really want to run this code several times,
		 * so squish out all multiples of hz here.
		 */
		time += lbolt / hz;
		lbolt %= hz;

		/*
		 * Wakeup lightning bolt sleepers.
		 * Processes sleep on lbolt to wait
		 * for short amounts of time (e.g. 1 second).
		 */
		wakeup((caddr_t)&lbolt);
		/*
		 * Recompute process priority and process
		 * sleep() system calls as well as internal
		 * sleeps with timeouts (tsleep() kernel routine).
		 */
		for(pp = proc; pp < procNPROC; pp++)
		if (pp->p_stat && pp->p_stat!=SZOMB) {
			/*
			 * Increase resident time, to max of 127 seconds
			 * (it is kept in a character.)  For
			 * loaded processes this is time in core; for
			 * swapped processes, this is time on drum.
			 */
			if(pp->p_time != 127)
				pp->p_time++;
			/*
			 * If process has clock counting down, and it
			 * expires, set it running (if this is a tsleep()),
			 * or give it an SIGALRM (if the user process
			 * is using alarm signals.
			 */
			if(pp->p_clktim) {
				if(--pp->p_clktim == 0) {
					psignal(pp, SIGALRM);
				}
			}
			/*
			 * If process is blocked, increment computed
			 * time blocked.  This is used in swap scheduling.
			 */
			if (pp->p_stat== SSLEEP || pp->p_stat == SSTOP) {
				if (pp->p_slptime != 127)
					pp->p_slptime++;
			}
			/*
			 * Update digital filter estimation of process
			 * cpu utilization for loaded processes.
			 */
			if (pp->p_flag&SLOAD) {
				pp->p_pctcpu = ccpu * pp->p_pctcpu +
#ifdef	float
				    (1.0 - ccpu) * (pp->p_cpticks/(float)hz);
#else
				(100*(avenscale-ccpu)*pp->p_cpticks*avenscale)/hz;
				pp->p_pctcpu += (avenscale / 2);
				pp->p_pctcpu /= avenscale;
#endif
			}
			/*
			 * Recompute process priority.  The number p_cpu
			 * is a weighted estimate of cpu time consumed.
			 * A process which consumes cpu time has this
			 * increase regularly.  We here decrease it by
			 * a fraction based on load average giving a digital
			 * decay filter which damps out in about 5 seconds
			 * when seconds are measured in time expanded by the
			 * load average.
			 *
			 * If a process is niced, then the nice directly
			 * affects the new priority.  The final priority
			 * is in the range 0 to 255, to fit in a character.
			 */
			pp->p_cpticks = 0;
			a = ave((pp->p_cpu & 0377), *avnrun*nrscale) +
			     pp->p_nice - NZERO;
			if (a < 0)
				a = 0;
			if (a > 255)
				a = 255;
			pp->p_cpu = a;
			(void) setpri(pp);
			/*
			 * Now have computed new process priority
			 * in p->p_usrpri.  Carefully change p->p_pri.
			 * A process is on a run queue associated with
			 * this priority, so we must block out process
			 * state changes during the transition.
			 */
			s = spl6();
			if (pp->p_pri >= PUSER) {
#ifdef FASTVAX
				if ((pp != u.u_procp || noproc) &&
				    pp->p_stat == SRUN &&
				    (pp->p_flag & SLOAD) &&
				    pp->p_pri != pp->p_usrpri) {
					remrq(pp);
					pp->p_pri = pp->p_usrpri;
					setrq(pp);
				} else
#else
					pp->p_pri = pp->p_usrpri;
#endif
			}
			splx(s);
		}

		/* Perform virtual memory metering */
		vmmeter();

		/*
		 * If the swap process is trying to bring
		 * a process in, have it look again to see
		 * if it is possible now.
		 */
		if (runin != 0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
	}
	/* If the processor status indicates that profiling is enabled,
	 * send the process a signal.
	 */
	if (USERMODE(ps) && (u.u_signal[SIGPROF] != 0)) {
		psignal(u.u_procp, SIGPROF);
	}

}

/*
 * Timeout is called to arrange that
 * fun(arg) is called in tim/hz seconds.
 * An entry is linked into the callout
 * structure.  The time in each structure
 * entry is the number of hz's more
 * than the previous entry.
 * In this way, decrementing the
 * first entry has the effect of
 * updating all entries.
 *
 * The panic is there because there is nothing
 * intelligent to be done if an entry won't fit.
 */
timeout(fun, arg, tim)
	int (*fun)();
	caddr_t arg;
{
	register struct callout *p1, *p2, *pnew;
	register int t;
	int s;

/* DEBUGGING CODE */
	int ttrstrt();

	if (fun == ttrstrt && arg == 0)
		panic("timeout ttrstr arg");
/* END DEBUGGING CODE */
	t = tim;
	s = spl7();
	pnew = callfree;
	if (pnew == NULL)
		panic("Timeout");
	callfree = pnew->c_next;
	pnew->c_arg = arg;
	pnew->c_func = fun;
	for (p1 = &calltodo; (p2 = p1->c_next) && p2->c_time < t; p1 = p2)
		t -= p2->c_time;
	p1->c_next = pnew;
	pnew->c_next = p2;
	pnew->c_time = t;
	if (p2)
		p2->c_time -= t;
	splx(s);
}

callinit()
{
	/*
	 * Initialize callouts
	 */

	register int i;

	callfree = callout;
	for (i = 1; i < ncallout; i++)
		callout[i-1].c_next = &callout[i];
}
