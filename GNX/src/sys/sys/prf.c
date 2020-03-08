# ifndef NOSCCS
static char *sccsid = "@(#)prf.c	3.22	9/28/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/mount.h"
#include "../h/filsys.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/pte.h"
#include "../h/vm.h"
#include "../h/tty.h"
#include "../h/panic.h"
#include "../h/msgbuf.h"
#include "../h/reboot.h"
#include "../h/proc.h"
#include "../h/reg.h"

#define	PRT_ALL		0		/* printf logs and types */
#define	PRT_LOG		1		/* printf only logs output */
#define	PRT_TYPE	2		/* printf only types to console */


/*
 * In case console is off,
 * panicstr contains argument to last
 * call to panic.
 */

char	*panicstr;		/* panic string */
int	printhow;		/* current printf action */
struct	panic	pblk;		/* panic block */
extern	int	bmconsf;	/* true if bitmap terminal is console */


/*
 * Scaled down version of C Library printf.
 * Only %s %u %d (==%u) %o %x %D %c are recognized.
 * Used to print diagnostic information
 * directly on console tty and log the output.
 * Since it is not interrupt driven,
 * all system activities are pretty much
 * suspended.
 * Printf should not be used for chit-chat.
 */
/*VARARGS1*/
printf(fmt, x1)
	char *fmt;
	unsigned x1;
{
	prf(fmt, &x1, 0, PRT_ALL);
}


/* Write a message in the log file, but not on the console.
 * Used to log recovered disk errors without messing up the console.
 */
/*VARARGS1*/
lprintf(fmt, x1)
	char *fmt;
	unsigned x1;
{
	prf(fmt, &x1, 0, PRT_LOG);
}


/* Write a message to the console, but not to the log file.
 * Used by information typeout routines.
 */
/*VARARGS1*/
cprintf(fmt, x1)
	char *fmt;
	unsigned x1;
{
	prf(fmt, &x1, 0, PRT_TYPE);
}


/*
 * Uprintf prints to the current user's terminal,
 * guarantees not to sleep (so can be called by interrupt routines)
 * and does no watermark checking - (so no verbose messages).
 */
/*VARARGS1*/
uprintf(fmt, x1)
	char *fmt;
	unsigned x1;
{
	if (u.u_ttyp) prf(fmt, &x1, u.u_ttyp, 0);
}


/*
 * Tprintf is like uprintf, except that output can be sent to the designated
 * terminal line.
 */
/*VARARGS1*/
tprintf(tp, fmt, x1)
	struct tty *tp;		/* terminal to send to */
	char *fmt;		/* format string */
	unsigned x1;		/* argument list */
{
	if (tp) prf(fmt, &x1, tp, 0);
}



/* Worker routine called by all of the various printf routines */
prf(fmt, adx, tp, how)
	register char *fmt;
	register u_int *adx;
	register struct tty *tp;
{
	register char c;
	register char *s;
	int oldhow;

	oldhow = printhow;	/* save old value in case of nested calls */
	printhow = how;
loop:	while ((c = *fmt++) != '%') {
		if (c == '\0') {
			printhow = oldhow;
			return;
		}
		putchar(c, tp);
	}
	switch (c = *fmt++) {

	case 'X':
		printx((long)*adx, tp);
		break;
	case 'c':
		putchar(*adx, tp);
		break;
	case 'd':
	case 'o':
	case 'x':
		printn((long)*adx, c=='o'? 8: (c=='x'? 16:10), tp);
		break;
	case 'r':		/* recursive printf for panic to use */
		prf((char *)*adx, adx[1], tp);
		adx++;
		break;
	case 's':
		s = (char *)*adx;
		while (c = *s++) {
			putchar(c, tp);
		}
		break;
	}
	adx++;
	goto loop;
}

/*
 * Print the full 8 hex digits of an integer
 */
printx(x, tp)
	long x;
	struct tty *tp;
{
	register int i;

	for (i = 0; i < 8; i++)
		putchar("0123456789ABCDEF"[(x>>((7-i)*4))&0xf], tp);
}

/*
 * Print an unsigned integer in base b.
 */
printn(n, b, tp)
	long n;
	struct tty *tp;
{
	register long a;

	if (n<0) {	/* shouldn't happen */
		putchar('-', tp);
		n = -n;
	}
	if(a = n/b)
		printn(a, b, tp);
	putchar("0123456789ABCDEF"[(int)(n%b)], tp);
}


/*
 * Panic is called on unresolvable fatal errors.  It syncs, prints
 * "panic: mesg" and then loops.  The "real" panic routine first saves
 * useful data into the panic structure pblk, and then comes here.
 * The panic string can be formatted like printf to include useful data.
 * If we are called twice, then we avoid trying to sync the disks as
 * this often leads to recursive panics.
 */
cpanic(s, adx)
	register char *s;
	u_int	*adx;
{
	int bootopt = panicstr ? RB_AUTOBOOT|RB_NOSYNC : RB_AUTOBOOT;
	panicstr = s;
	printf("\npanic: %r\n", s, adx);
	(void) spl0();
	boot(RB_PANIC, bootopt);
}

/*
 * prdev prints a warning message of the
 * form "mesg on dev x/y".
 * x and y are the major and minor parts of
 * the device argument.
 */
prdev(str, dev)
char *str;
register dev_t dev;
{
	register struct	mount	*mp;
	register struct	filsys	*fp;
	
	for(mp = mount; mp < mountNMOUNT; mp++)
		if(mp->m_bufp != NULL)
			if(dev == mp->m_dev)
			{
				fp = mp->m_bufp->b_un.b_filsys;
				printf("%s on %s (%u/%u)\n",
					 str,fp->s_fname,  major(dev),
					 minor(dev));
				return;
			}
	printf("%s on dev %d/%d\n", str, major(dev), minor(dev));
}

/*
 * deverr prints a diagnostic from
 * a device driver.
 * It prints the device, block number,
 * and some hex words (usually some error
 * status registers) passed as arguments.
 */
deverror(bp, o1, o2)
register struct buf *bp;
{

	prdev("err", bp->b_dev);
	printf("bn=%D er=%x,%x\n", bp->b_blkno, o1, o2);
}


/*
 * Print a character on console or designated terminal.  If tp is null, then
 * the output might go to console, and the last MSGBUFS characters might be
 * saved in msgbuf for inspection later, as determined by printhow.
 */
/*ARGSUSED*/
putchar(c, tp)
	register char	c;
	register struct tty *tp;	/* terminal line, or NULL if console */
{
	if (c == '\0') return;
	if (tp) {
		if (tp->t_state&CARR_ON) {
			register s = spl6();
			if (c == '\n')
				(void) ntyoutput('\r', tp);
			(void) ntyoutput(c, tp);
			ttstart(tp);
			splx(s);
		}
		return;
	}
	if ((printhow != PRT_TYPE) && (c != '\r') && (c != 0177)) {
		if (msgbuf.msg_magic != MSG_MAGIC) {
			msgbuf.msg_bufx = 0;
			msgbuf.msg_magic = MSG_MAGIC;
		}
		if (msgbuf.msg_bufx < 0 || msgbuf.msg_bufx >= MSG_BSIZE)
			msgbuf.msg_bufx = 0;
		msgbuf.msg_bufc[msgbuf.msg_bufx++] = c;
	}
	if (printhow == PRT_LOG)
		return;
#if NBM > 0 && NRS > 0		/* talk to appropriate console device */
	if (bmconsf)
		bmputchar(c);
	else
		rsputchar(c);
#else
#if NBM > 0
	bmputchar(c);
#else
	rsputchar(c);
#endif
#endif
}

/* Type the status of the most recently running process on the terminal.
 * Format:  load pid (name) pc memory cpu state 
 */
pstat(tp)
	register struct	tty *tp;		/* terminal to output to */
{
	register struct	proc *p;		/* active process */
	struct	proc *ap;			/* auxillary proc pointer */
	register int	i;			/* random uses */
	int	group, state, load, min, sec;	/* random variables */
	int	pc, resmem, totmem;		/* process data */
	char	*str;				/* state string */

	if ((tp == 0) || (tp->t_outq.c_cc >= (TTHIWAT(tp) + 10))) {
		return;		/* don't overfill terminal buffer */
	}

	/* search for the most recently running process in our terminal group */

	group = tp->t_pgrp;
	ap = NULL;
	i = 0;
	load = 0;
	for (p = proc; p < procNPROC; p++) {
		state = p->p_stat;
		if (state == 0) continue;
		if (state == SRUN) load++;
		if (p->p_pgrp != group) continue;
		if (p->p_infoage <= i) continue;
		i = p->p_infoage;
		ap = p;
	}
	p = ap;
	if (p == NULL) {			/* if no process found, quit */
		tprintf(tp, "load %d no current process\n", load);
		return;
	}

	/* collect data about the found process and type it */

	state = p->p_stat;		/* get state, last pc, and runtime */
	pc = p->p_infopc;
	min = p->p_infotime;
	if (u.u_procp == p) {		/* get new data if current proc */
		pc = u.u_ar0[RPC];
		min = u.u_vm.vm_utime + u.u_vm.vm_stime;
	}
	min = (min * 10) / hz;
	sec = min % 600;
	min /= 600;
	resmem = 0;			/* get current memory sizes */
	totmem = 0;
	if (state != SZOMB) {
		i = p->p_spti;
		resmem = spt[i].spt_mempages;
		totmem = spt[i].spt_usedpages;
		if (resmem > totmem) totmem = resmem;
	}
	switch (state) {		/* get state string */
		case SRUN:
			str = "run";
			if (u.u_procp == p) str = "running";
			break;
		case SSLEEP:
			str = "sleep";
			i = (int) p->p_wchan;
			if (i == (int)&u) str = "pause";
			if ((i >= (int)tp) && (i < (int)(tp+1)))
				str = "tty wait";
			if (p->p_pri < PZERO) str = "io wait";
			break;
		case SSTOP:
			str = "stopped"; break;
		case SIDL:
			str = "creating"; break;
		case SZOMB:
			str = "zombie"; break;
		default:
			str = "unknown";
	}
	tprintf(tp, "load %d pid %d (%s) pc %x mem %d/%d cpu ",
		load, p->p_pid, p->p_infoname, pc, resmem, totmem);
	if (min) tprintf(tp, "%d%s", min, (sec>99)?":":":0");
	tprintf(tp, "%d.%d %s%s\n", sec/10, sec%10,
		(p->p_flag & SLOAD) ? "":"swapped ", str);
}
