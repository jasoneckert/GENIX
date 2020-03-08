# ifndef NOSCCS
static char *sccsid = "@(#)sig.c	3.7	8/29/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/inode.h"
#include "../h/reg.h"
#include "../h/psr.h"
#include "../h/vm.h"
#include "../h/vlimit.h"
#include "../h/fpu.h"
#include "../h/mmu.h"

extern	long	floatf;		/* nonzero if floating point is allowed */
struct	cst *addrtocst();

/*
 * Priority for tracing
 */
#define	IPCPRI	PZERO

/*
 * Tracing variables.
 * Used to pass trace command from
 * parent to child being traced.
 * This data base cannot be
 * shared and is locked
 * per user.
 */
struct
{
	int	ip_lock;
	int	ip_req;
	int	*ip_addr;
	int	ip_data;
} ipc;

/*
 * Send the specified signal to
 * all processes with 'pgrp' as
 * process group.
 * Called by tty.c for quits and
 * interrupts.
 */
gsignal(pgrp, sig)
register pgrp;
{
	register struct proc *p;

	if(pgrp == 0)
		return;
	for(p = proc; p < procNPROC; p++)
		if(p->p_pgrp == pgrp)
			psignal(p, sig);
}
/*
 * Send the specified signal to
 * the specified process.
 */
psignal(p, sig)
register struct proc *p;
register int sig;
{
	register s;
	register int (*action)();
	long sigmask;

	if((unsigned)sig >= NSIG)
		return;
	sigmask = (1L << (sig-1));

	/*
	 * If proc is traced, always give parent a chance.
	 * Otherwise get the signal action from the bits in the proc table.
	 */
	if (p->p_flag & STRC)
		action = SIG_DFL;
	else {
		s = (p->p_siga1&sigmask) != 0;
		s <<= 1;
		s |= (p->p_siga0&sigmask) != 0;
		action = (int(*)())s;
		/*
		 * If the signal is ignored, we forget about it immediately.
		 */
		if (action == SIG_IGN)
			return;
	}
	if (sig)
		p->p_sig |= sigmask;
	/*
	 * Defer further processing for signals which are held.
	 */
	if (action == SIG_HOLD)
		return;
	s = spl6();
	switch (p->p_stat) {

	case SSLEEP:
		/*
		 * If process is sleeping at negative priority
		 * we can't interrupt the sleep... the signal will
		 * be noticed when the process returns through
		 * trap() or syscall().
		 */
		if (p->p_pri <= PZERO)
			goto out;
		/*
		 * Process is sleeping and traced... make it runnable
		 * so it can discover the signal in issig() and stop
		 * for the parent.
		 */
		if (p->p_flag&STRC)
			goto run;
		switch (sig) {

		case SIGSTOP:
		case SIGTSTP:
		case SIGTTIN:
		case SIGTTOU:
			/*
			 * These are the signals which by default
			 * stop a process.
			 */
			if (action != SIG_DFL)
				goto run;
			/*
			 * Don't clog system with children of init
			 * stopped from the keyboard.
			 */
			if (sig != SIGSTOP && p->p_pptr == &proc[1]) {
				psignal(p, SIGKILL);
				p->p_sig &= ~sigmask;
				return;
			}
			/*
			 * If a child in vfork(), stopping could
			 * cause deadlock.
			 */
			if (p->p_flag&SVFORK)
				goto out;
			p->p_sig &= ~sigmask;
			p->p_cursig = sig;
			stop(p);
			goto out;

		case SIGTINT:
		case SIGCHLD:
			/*
			 * These signals are special in that they
			 * don't get propogated... if the process
			 * isn't interested, forget it.
			 */
			if (action != SIG_DFL)
				goto run;
			p->p_sig &= ~sigmask;		/* take it away */
			goto out;

		default:
			/*
			 * All other signals cause the process to run
			 */
			goto run;
		}
		/*NOTREACHED*/

	case SSTOP:
		/*
		 * If traced process is already stopped,
		 * then no further action is necessary.
		 */
		if (p->p_flag&STRC)
			goto out;
		switch (sig) {

		case SIGKILL:
			/*
			 * Kill signal always sets processes running.
			 */
			goto run;

		case SIGCONT:
			/*
			 * If the process catches SIGCONT, let it handle
			 * the signal itself.  If it isn't waiting on
			 * an event, then it goes back to run state.
			 * Otherwise, process goes back to sleep state.
			 */
			if (action != SIG_DFL || p->p_wchan == 0)
				goto run;
			p->p_stat = SSLEEP;
			goto out;

		case SIGSTOP:
		case SIGTSTP:
		case SIGTTIN:
		case SIGTTOU:
			/*
			 * Already stopped, don't need to stop again.
			 * (If we did the shell could get confused.)
			 */
			p->p_sig &= ~sigmask;		/* take it away */
			goto out;

		default:
			/*
			 * If process is sleeping interruptibly, then
			 * unstick it so that when it is continued
			 * it can look at the signal.
			 * But don't setrun the process as its not to
			 * be unstopped by the signal alone.
			 */
			if (p->p_wchan && p->p_pri > PZERO)
				unsleep(p);
			goto out;
		}
		/*NOTREACHED*/

	default:
		/*
		 * SRUN, SIDL, SZOMB do nothing with the signal.
		 * It will either never be noticed, or noticed very soon.
		 */
		goto out;
	}
	/*NOTREACHED*/
run:
	/*
	 * Raise priority to at least PUSER.
	 */
#ifdef FASTVAX
	if (p->p_pri > PUSER)
		if ((p != u.u_procp || noproc) && p->p_stat == SRUN &&
		    (p->p_flag & SLOAD)) {
			remrq(p);
			p->p_pri = PUSER;
			setrq(p);
		} else
#else
			p->p_pri = PUSER;
#endif
	setrun(p);
out:
	splx(s);
}

/*
 * Returns true if the current
 * process has a signal to process.
 * The signal to process is put in p_cursig.
 * This is asked at least once each time a process enters the
 * system (though this can usually be done without actually
 * calling issig by checking the pending signal masks.)
 * A signal does not do anything
 * directly to a process; it sets
 * a flag that asks the process to
 * do something to itself.
 */
issig()
{
	register struct proc *p;
	register int sig;
	long sigbits;

	p = u.u_procp;
	for (;;) {
		sigbits = p->p_sig;
		if ((p->p_flag&STRC) == 0)
			sigbits &= ~p->p_ignsig;
		if (p->p_flag&SVFORK)
#define bit(a) (1L<<(a-1))
			sigbits &= ~(bit(SIGSTOP)|bit(SIGTSTP)|bit(SIGTTIN)|bit(SIGTTOU));
		if (sigbits == 0)
			break;
		for (sig=1; sig<NSIG; sig++) {
			if (sigbits & 1L)
				break;
			sigbits >>= 1;
		}
		p->p_sig &= ~(1L << (sig-1));	/* take the signal! */
		p->p_cursig = sig;
		if (p->p_flag&STRC && (p->p_flag&SVFORK)==0) {
			/*
			 * If traced, always stop, and stay
			 * stopped until released by the parent.
			 */
			do {
				stop(p);
				swtch();
			} while (!procxmt() && p->p_flag&STRC);
			/*
			 * If parent wants us to take the signal,
			 * then it will leave it in p->p_cursig;
			 * otherwise we just look for signals again.
			 */
			sig = p->p_cursig;
			if (sig == 0)
				continue;
		}
		switch (u.u_signal[sig]) {

		case SIG_DFL:
			/*
			 * Don't take default actions on system processes.
			 */
			if (p <= &proc[1])
				break;
			switch (sig) {

			case SIGTSTP:
			case SIGTTIN:
			case SIGTTOU:
				/*
				 * Children of init aren't allowed to stop
				 * on signals from the keyboard.
				 */
				if (p->p_pptr == &proc[1]) {
					psignal(p, SIGKILL);
					continue;
				}
				/* fall into ... */

			case SIGSTOP:
				if (p->p_flag&STRC)
					continue;
				stop(p);
				swtch();
				continue;

			case SIGTINT:
			case SIGCONT:
			case SIGCHLD:
				/*
				 * These signals are normally not
				 * sent if the action is the default.
				 * This can happen only if you reset the
				 * signal action from an action which was
				 * not deferred to SIG_DFL before the
				 * system gets a chance to post the signal.
				 */
				continue;		/* == ignore */

			default:
				goto send;
			}
			/*NOTREACHED*/

		case SIG_HOLD:
		case SIG_IGN:
			/*
			 * Masking above should prevent us
			 * ever trying to take action on a held
			 * or ignored signal, unless process is traced.
			 */
			if ((p->p_flag&STRC) == 0)
				printf("issig\n");
			continue;

		default:
			/*
			 * This signal has an action, let
			 * psig process it.
			 */
			goto send;
		}
		/*NOTREACHED*/
	}
	/*
	 * Didn't find a signal to send.
	 */
	p->p_cursig = 0;
	return (0);

send:
	/*
	 * Let psig process the signal.
	 */
	return (sig);
}

/*
 * Put the argument process into the stopped
 * state and notify the parent via wakeup and/or signal.
 */
stop(p)
	register struct proc *p;
{

	p->p_stat = SSTOP;
	p->p_flag &= ~SWTED;
	wakeup((caddr_t)p->p_pptr);
	/*
	 * Avoid sending signal to parent if process is traced
	 */
	if (p->p_flag&STRC)
		return;
	psignal(p->p_pptr, SIGCHLD);
}
/*
 * Perform the action specified by
 * the current signal.
 * The usual sequence is:
 *	if(issig())
 *		psig();
 * The signal bit has already been cleared by issig,
 * and the current signal number stored in p->p_cursig.
 */
psig()
{
	register struct proc *rp = u.u_procp;
	register int n = rp->p_cursig;
	long sigmask = 1L << (n-1);
	register int (*action)();

	if (rp->p_cursig == 0)
		panic("psig");
	action = u.u_signal[n];
	if (action != SIG_DFL) {
		if (action == SIG_IGN || action == SIG_HOLD)
			panic("psig action");
		u.u_error = 0;
		/*
		 * If this catch value indicates automatic holding of
		 * subsequent signals, set the hold value.
		 */
		if (SIGISDEFER(action)) {
			(void) spl6();
			if ((int)SIG_HOLD & 1)
				rp->p_siga0 |= sigmask;
			else
				rp->p_siga0 &= ~sigmask;
			if ((int)SIG_HOLD & 2)
				rp->p_siga1 |= sigmask;
			else
				rp->p_siga1 &= ~sigmask;
			u.u_signal[n] = SIG_HOLD;
			(void) spl0();
			action = SIGUNDEFER(action);
		} else if(n != SIGILL && n != SIGTRAP) {
			(void) spl6();
			if ((int)SIG_DFL & 1)
				rp->p_siga0 |= sigmask;
			else
				rp->p_siga0 &= ~sigmask;
			if ((int)SIG_DFL & 2)
				rp->p_siga1 |= sigmask;
			else
				rp->p_siga1 &= ~sigmask;
			u.u_signal[n] = SIG_DFL;
			(void) spl0();
		}
		sendsig(action, n);
		rp->p_cursig = 0;
		return;
	}
	switch (n) {

	case SIGPROF:
		return;
	case SIGILL:
	case SIGIOT:
	case SIGBUS:
	case SIGQUIT:
	case SIGTRAP:
	case SIGEMT:
	case SIGFPE:
	case SIGSEGV:
	case SIGSYS:
	case SIGDVZ:
		u.u_arg[0] = n;
		if(core())
			n += 0200;
	}
	exit(n);
}

/*
 * Create a core image on the file "core"
 * If you are looking for protection glitches,
 * there are probably a wealth of them here
 * when this occurs to a suid command.
 *
 * It writes USIZE block of the
 * user.h area followed by the entire
 * data+stack segments.
 */
core()
{
	register struct inode *ip;
	register unsigned s;
	extern schar();
	register int	coresize;

	coresize = u.u_limit[LIM_CORE];
	u.u_error = 0;
	u.u_dirp = "core";
	ip = namei(schar, 1);
	if(ip == NULL) {
		if(u.u_error)
			return(0);
		ip = maknode(0666);
		if (ip==NULL)
			return(0);
	}
	/*
	 * allow dump only if permissions allow, "core" is
	 * regular file, has exactly one link, and
	 * program is not set-uid/gid
	 */
	if(!access(ip, IWRITE) &&
	   (ip->i_mode&IFMT) == IFREG &&
	   ip->i_nlink == 1 &&
	   u.u_gid == u.u_rgid &&
	   u.u_uid == u.u_ruid) {
		itrunc(ip);
		if (u.u_procp->p_flag & SFPU) {
			savefpu();	/* copy fpu registers back into u. */
		}
		u.u_offset = 0;
		u.u_base = (caddr_t)&u;
		u.u_count = MIN(coresize,USIZE);
		coresize -= u.u_count;
		u.u_segflg = 1;
		if (u.u_count > 0) writei(ip);		/* write u. area */
		u.u_base = (caddr_t) 0;
		u.u_count = MIN(coresize,u.u_tsize + u.u_dsize);
		coresize -= u.u_count;
		u.u_segflg = 0;
		if (u.u_count > 0) writei(ip);		/* text plus data */
		u.u_ssize = TOPUSER - u.u_ar0[RSP];
		u.u_base = (caddr_t) u.u_ar0[RSP];
		u.u_count = MIN(coresize,u.u_ssize);
		if (u.u_count > 0) writei(ip);		/* stack */
	} else
		u.u_error = EFAULT;
	iput(ip);
	return(u.u_error==0);
}

/*
 * sys-trace system call.
 */
ptrace()
{
	register struct proc *p;
	register struct a {
		int	req;
		int	pid;
		int	*addr;
		int	data;
	} *uap;

	uap = (struct a *)u.u_ap;
	if (uap->req <= 0) {
		u.u_procp->p_flag |= STRC;
		return;
	}
	p = pfind(uap->pid);
	if (p == 0 || p->p_stat != SSTOP || p->p_ppid != u.u_procp->p_pid) {
		u.u_error = ESRCH;
		return;
	}
	while (ipc.ip_lock)
		sleep((caddr_t)&ipc, IPCPRI);
	ipc.ip_lock = p->p_pid;
	ipc.ip_data = uap->data;
	ipc.ip_addr = uap->addr;
	ipc.ip_req = uap->req;
	p->p_flag &= ~SWTED;
	setrun(p);
	while (ipc.ip_req > 0) {
		if (p->p_stat == SSTOP) setrun(p);
				/* start again if stopped 4.1 */
		sleep((caddr_t)&ipc, IPCPRI);
	}
	u.u_r.r_val1 = ipc.ip_data;
	if (ipc.ip_req < 0)
		u.u_error = EIO;
	ipc.ip_lock = 0;
	wakeup((caddr_t)&ipc);
}

/*
 * Code that the child process
 * executes to implement the command
 * of the parent process in tracing.
 */
procxmt()
{
	register int addr;
	register int *p;
	register int i;
	register int ii;
	register int err;

	if (ipc.ip_lock != u.u_procp->p_pid)
		return(0);
	addr = (int)ipc.ip_addr;
	i = ipc.ip_req;
	ipc.ip_req = 0;
	/* 2bsd wakes up &ipc here, 4.1 at end and exits; do 4.1 */
	switch (i) {

	/* read user I */
	case 1:
	/* read user D */
	case 2:
		i = fuword((caddr_t)addr);
		if ((i == -1) && ((fubyte((caddr_t)addr) == -1) ||
			(fubyte((caddr_t)(addr + 3)) == -1)))
				goto error;
		ipc.ip_data = i;
		break;

	/* read u */
	case 3:
		if ((addr >= R7) && (addr <= RPSRMOD)) {
		    ipc.ip_data = u.u_ar0[addr];
		} else {
		    if ((addr >= FSR) && (addr <= F0)) {
			if ((u.u_procp->p_flag & SFPU) == 0) {
			    if (floatf == 0) goto error;
			    u.u_procp->p_flag |= SFPU;
			    u.u_fsr = 0;
			    enablefpu();
			    restorefpu();
			}
			savefpu();
			if (addr == FSR) ipc.ip_data = u.u_fsr;
			else ipc.ip_data = u.u_fregs[F0 - addr];
		    } else
		    if (addr == UMSR) ipc.ip_data = u.u_msrreg;
		    else
		    if (addr == UEIA) ipc.ip_data = u.u_eiareg;
		    else
		    if (addr<0 || addr >= USIZE - 3) goto error;
		    else ipc.ip_data = *((int *)((int)&u + addr));
		}
		break;

	/* write user I */
	case 4:
		if (unsharemap(u.u_spti, addr)) goto error;
		if (unsharemap(u.u_spti, addr + 3)) goto error;
		i = changemapprot(u.u_spti, addr, PG_RWRW);
		ii = changemapprot(u.u_spti, addr + 3, PG_RWRW);
		err = suword((caddr_t)addr, ipc.ip_data);
		changemapprot(u.u_spti, addr + 3, ii);
		changemapprot(u.u_spti, addr, i);
		if (err < 0) goto error;
		break;

	/* write user D */
	case 5:
		err = suword((caddr_t)addr, ipc.ip_data);
		if (err < 0) goto error;
		break;

	/* write u */
	case 6:
		if ((addr >= R7) && (addr <= RPSRMOD)) {
		    if ((addr>R2) && (addr < RSP) && (addr != RFP)) goto error;
		    if (addr == RPSRMOD) {
			ipc.ip_data &= 0xffffff;/* make user call with */
			/* both psr/mod values, change all but highbyte */
			ipc.ip_data |= (0xff000000 & u.u_ar0[RPSRMOD]);
		    }
		    u.u_ar0[addr] = ipc.ip_data;
		} else {
		    if ((addr >= FSR) && (addr <= F0)) {
			if ((u.u_procp->p_flag & SFPU) == 0) {
			    if (floatf == 0) goto error;
			    u.u_procp->p_flag |= SFPU;
			    u.u_fsr = 0;
			    enablefpu();
			    restorefpu();
			}
			savefpu();
			if (addr == FSR) u.u_fsr = ipc.ip_data; 
			else u.u_fregs[F0 - addr] = ipc.ip_data;
			restorefpu();
		    } else goto error;
		}
		break;

	/* set signal and continue */
	/*  one version causes a trace-trap */
	case 9:
		u.u_ar0[RPSRMOD] |= (TBIT << 16);
	case 7:
		if (addr != 1)
			u.u_ar0[RPC] = addr;
		if (ipc.ip_data > NSIG)
			goto error;
		u.u_msrreg = 0;
		u.u_procp->p_cursig = ipc.ip_data;
		wakeup((caddr_t)&ipc);		/* 4.1 wakeups */
		return(1);

	/* force exit */
	case 8:
		wakeup((caddr_t)&ipc);		/* 4.1 wakeups */
		exit(u.u_procp->p_cursig);

	/* set mmu breakpoint */
	case 10:
		addr &= 0x3cffffff;
		addr |= 0x80000000;	/* set AS to ptb1, be virtual */
		if (ipc.ip_data) u.u_bpr1 = addr;
		else u.u_bpr0 = addr;
		u.u_procp->p_flag |= SMMU;
		break;
	case 11:
		addr &= 0xffffff;
		u.u_bcnt = addr;
		mtpr(bcnt,addr);
		break;

	default:
	error:
		ipc.ip_req = -1;
	}
	wakeup((caddr_t)&ipc);		/* 4.1 wakeups */
	return(0);
}
