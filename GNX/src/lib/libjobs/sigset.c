/*
 * sigset.c: version 1.4 of 9/11/83
 * signal system call interface package
 */
# ifdef SCCS
static char *sccsid = "@(#)sigset.c	1.4 (NSC) 9/11/83";
# endif

#include <signal.h>
#include <errno.h>
extern	errno;

static	int (*cactions[NSIG+1])();	/* saved callers signal actions */
static	char setflg[NSIG+1];		/* =1 means action perm ``sigset'' */
int	(*sigsys())();
int	_sigcatch();

/*
 * old signal protocol.  when signal occurs, further
 * instances of same signal are not blocked, so that
 * recursive signals are possible.  the action will
 * not be re-enabled by these routines when return
 * is made from the interrupt (compare sigset).
 */
int (*
signal(signum, action))()
register int signum;
register int (*action)();
{
	register int (*retval)();

	if (signum <= 0 || signum > NSIG) {
		errno = EINVAL;
		return (BADSIG);
	}
	retval = cactions[signum];
	cactions[signum] = action;
	if (action != SIG_IGN && action != SIG_DFL && action != SIG_HOLD)
		if (SIGISDEFER(action))
			action = DEFERSIG(_sigcatch);
		else
			action = _sigcatch;
	action = sigsys(signum, action);
	if (action == SIG_IGN || action == SIG_DFL || action == SIG_HOLD)
		retval = action;
	setflg[signum] = 0;	/* 'tis fleeting (madness may take its toll) */
	return (retval);
}

/*
 * set ``permanent'' action for this signal.  if a function,
 * it will be deferred when interupt occurs and enabled again
 * when return occurs.  after sigset, sighold and sigrelse can
 * be used to protect signum signal critical sections.
 */
int (*
sigset(signum, action))()
register int signum;
register int (*action)();
{
	register int (*retval)();

	if (signum <= 0 || signum > NSIG) {
		errno = EINVAL;
		return (BADSIG);
	}
	retval = cactions[signum];
	cactions[signum] = action;
	if (action != SIG_IGN && action != SIG_DFL && action != SIG_HOLD)
		action = DEFERSIG(_sigcatch);
	action = sigsys(signum, action);
	if (action == SIG_IGN || action == SIG_DFL || action == SIG_HOLD)
		retval = action;
	setflg[signum] = 1;		/* don't want to lose control! */
	return (retval);
}

/*
 * temporarily hold a signal until further notice
 * via sigpause or sigrelse
 */
sighold(signum)
register int signum;
{

	if (signum <= 0 || signum > NSIG) {
		errno = EINVAL;
		return;
	}
	sigsys(signum, SIG_HOLD);
}

/*
 * atomically release the signal and pause
 * if no signals pending.  signal will normally
 * be held on return (unless an routine called at
 * interrupt time resets it).
 */
sigpause(signum)
register signum;
{

	if (signum <= 0 || signum > NSIG || setflg[signum] == 0) {
		errno = EINVAL;
		return;
	}
	sigsys(signum|SIGDOPAUSE, DEFERSIG(_sigcatch));
}

/*
 * re-enable signals after sighold or possibly after sigpause
 */
sigrelse(signum)
register signum;
{
	if (signum <= 0 || signum > NSIG || setflg[signum] == 0) {
		errno = EINVAL;
		return (-1);
	}
	sigsys(signum, DEFERSIG(_sigcatch));
	return (0);
}

/*
 * ignore signal
 */
sigignore(signum)
int signum;
{

	sigsys(signum, SIG_IGN);
}

/*
 * called at interrupt time.  sigpeel will peel
 * off the interrupt frames and reenable the signal with
 * the argument action (cleanly).  if the action is
 * SIG_DFL sigpeel will do nothing, but we can as well
 * return ourselves without its help.
 * WARNING:  sigentry and sigpeel both assume that only one
 * register variable is used by this routine.
 */
_sigcatch(signum)
register signum;
{
	(*cactions[signum])(signum);	/* call the C routine */
	if (setflg[signum]) {
		sigpeel(signum, DEFERSIG(_sigcatch));
		/*NOTREACHED*/
	}
	/* old protocol, just return */
}
