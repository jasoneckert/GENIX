/*
 * rs.c: version 3.11 of 8/22/83
 * Mesa Unix Device Driver
 */
# ifdef SCCS
static char *sccsid = "@(#)rs.c	3.11 (NSC) 8/22/83";
# endif


#if NRS > 0
#define DEBUG

#include "../h/param.h"
#include "../h/dir.h"
#include "../h/conf.h"
#include "../h/devvm.h"
#include "../h/user.h"
#include "../h/icu.h"
#include "../h/tty.h"
#include "../h/rs.h"
#include "../h/timer.h"
#include "../h/pdma.h"

#define BYTE	*(char *)&	/*Causes compiler to generate byte movs*/
#define F1	'\01'


static RSDEV *device = (RSDEV *) CP_Usart;
static unsigned flavorkey;

struct tty rs_tty;

extern int ttrstrt();
extern int timeout();
extern char partab[];
extern int bmconsf;			/* true if we are not the console */
extern int interrupt();			/* DEBUG */
extern int nulldev();			/* to preserve optimizer references */

int rsstart();

static char *lpartab = partab;		/* local partab pointer */
static struct linesw *llinesw = linesw; /* local linesw pointer */

struct pdma rsdma = {0};		/* pseudo dma for rs device */
extern struct pdma *_rsdma;		/* used by driver */

#ifdef DEBUG
static int oe = 0;			/* DEBUG */
#ifdef BADICU
int rsstatus = 0;
jabsio(tp)
register struct tty *tp;
{
	register RSDEV *dp = device;
	register s;

	s = spl5();
	if(dp->status & RxRDY) {
		interrupt(RX);
	}
	if ((tp->t_state & BUSY) && ((rsstatus = dp->status) & TxRDY)) {
		interrupt(TX);
	}
	splx(s);
	/* While we are open, cause an interrupt 5 times/second */
	if (tp->t_state & ISOPEN)
		timeout(jabsio,tp,hz/5);
}
#endif
#endif

int rsxint();

rsopen(dev,flag)
dev_t	dev;
{
	register struct tty *tp;
	register struct pdma *pdp;

	pdp = &rsdma;
	tp = &rs_tty;				/* pointer to tty */
	tp->t_addr = (caddr_t)device;		/* pointer to device */
	tp->t_oproc = rsstart;
	tp->t_iproc = NULL;
	tp->t_state |= WOPEN;
	if ((tp->t_state & ISOPEN) == 0) {
						/* init the dma struct */
		_rsdma = pdp;			/* used by driver */
		pdp->p_addr = tp->t_addr;	/* rs device pointer */
		pdp->p_arg = (int)tp;		/* rs tty pointer */
		pdp->p_fcn = rsxint;		/* rs xint routine */
		ttychars(tp);
		tp->t_ospeed = tp->t_ispeed = B9600;
		tp->t_flags = ODDP|EVENP|ECHO|XTABS|CRMOD;
		ns2651init(tp);
		icuenable(TXMASK|RXMASK);

#ifdef BADICU

	/*
	**	Start monitoring the sio device....
	**	this will prod the device in 1/4 sec.
	*/
	timeout(jabsio,tp,15);
#endif

	} else if (tp->t_state&XCLUDE && u.u_uid != 0) {
		u.u_error = EBUSY;
		return;
	}
	(void) spl5();
	while ((tp->t_state & CARR_ON) == 0) {
		tp->t_state |= WOPEN;
		sleep((caddr_t)&tp->t_rawq, TTIPRI);
	}
	(void) spl0();
	(*llinesw[tp->t_line].l_open)(dev, tp);
#ifdef DEBUG
	oe = 0;		/* DEBUG */
#endif

}

rsclose(dev,flag)
{
	register struct tty *tp;

	tp = &rs_tty;
	(*llinesw[tp->t_line].l_close) (tp);
	ttyclose(tp);
#ifdef DEBUG
	if (oe) printf("%d overrun errors\n",oe);
#endif

}

rsread(dev)
dev_t dev;
{
	register struct tty *tp;

	tp = &rs_tty;
	(*llinesw[tp->t_line].l_read)(tp);
}

rswrite(dev)
dev_t dev;
{
	register struct tty *tp;

	tp = &rs_tty;
	(*llinesw[tp->t_line].l_write)(tp);
}

rsrint(dev)
dev_t dev;
{
	register RSDEV *dp;
	register struct tty *tp;
	register char s;
	char c;

	dp = device;
	tp = &rs_tty;

	while((s = dp->status) & RxRDY) {	/* a char is present */
		c = dp->data;
		if((tp->t_state & ISOPEN) == 0) {
			wakeup((caddr_t)&tp->t_rawq);
			continue;
		}
		if (s&ERROR) {
			BYTE dp->cmd |= ERESET;
			if (s&FE) {			/* framing error */
				if (tp->t_flags & RAW)
					c = 0;
				else
					c = tun.t_intrc;
			}
			if (s&OE) {			/* overrun error */
#ifdef DEBUG
				oe++;
				/*printf("cons: overrun error\n");*/
#endif
			}
			if (s&PE) {			/* parity error */
				/*
				if ((tp->t_flags & (ANYP)) != (ANYP)) {
					continue;
				}
				*/
			}
		}
#ifdef	FLAVORDUMP
		if (flavorkey) {
		    flavorkey = ~flavorkey;
		    if (c == F1) {
			(*llinesw[tp->t_line].l_rint)(c, tp);
			continue;
		    }
		    flavordump(c);
		    continue;
		}
		if ((c == F1) && (bmconsf == 0)) {
		    flavorkey = ~flavorkey;
		    continue;
		}
#endif
		(*llinesw[tp->t_line].l_rint)(c, tp);
	}
}


rsxint(tp)
	register struct tty *tp;
{
	register struct pdma *dp;
	register s;
 
	dp = &rsdma;
	s = spl5();		/* block pdma interrupts */
	tp->t_state &= ~BUSY;
	if (tp->t_state & FLUSH)
		tp->t_state &= ~FLUSH;
	else
		ndflush(&tp->t_outq, dp->p_mem-tp->t_outq.c_cf);
	if (tp->t_line)
		(*linesw[tp->t_line].l_start)(tp);
	else
		rsstart(tp);
	splx(s);
}

rsstart(tp)
	register struct tty *tp;
{
	register struct pdma *dp;
	register int cc;
	int s;
 
	dp = &rsdma;
	s = spl5();
	if (tp->t_state & (TIMEOUT|BUSY|TTSTOP))
		goto out;
	if (tp->t_state&ASLEEP && tp->t_outq.c_cc <= TTLOWAT(tp)) {
		tp->t_state &= ~ASLEEP;
		wakeup((caddr_t)&tp->t_outq);
	}
	if (tp->t_outq.c_cc == 0)
		goto out;
	if (tp->t_flags&RAW || tp->t_local&LLITOUT)
		cc = ndqb(&tp->t_outq, 0);
	else {
		cc = ndqb(&tp->t_outq, 0200);
		if (cc == 0) {
			cc = getc(&tp->t_outq);
			timeout(ttrstrt, (caddr_t)tp, (cc&0x7f) + 6);
			tp->t_state |= TIMEOUT;
			goto out;
		}
	}
	tp->t_state |= BUSY;
	dp->p_end = dp->p_mem = tp->t_outq.c_cf;
	dp->p_end += cc;
	/*
	cc = *dp->p_mem++;
	BYTE device->data = cc;
	*/
	interrupt(TX);		/* Prod the device */
out:
	splx(s);
}
 

/*
 * Stop output on a line.
 */
/*ARGSUSED*/
rsstop(tp,flag)
register struct tty *tp;
{
	register struct pdma *dp;
	register int s;

	dp = &rsdma;
	s = spl5();
	if (tp->t_state & BUSY) {
		dp->p_end = dp->p_mem;
		if ((tp->t_state&TTSTOP)==0)
			tp->t_state |= FLUSH;
	}
	splx(s);
}

rsioctl(dev,cmd,addr,flag)
dev_t dev;
caddr_t addr;
{
	register struct tty *tp;
	register RSDEV *dp;
	int s;

	tp = &rs_tty;
	dp = device;
	cmd = (*llinesw[tp->t_line].l_ioctl)(tp, cmd, addr);
	if (cmd == 0)
		return;
	if (ttioctl(tp, cmd, addr, flag)) {
		if (cmd == TIOCSETP || cmd == TIOCSETN)
			ns2651mode(tp);
	} else switch(cmd) {

	case TIOCSBRK:
		/*
		 * The 2651 must be transmitting a character for the break
		 * setting to take effect. It seems as if a character will
		 * be transmitted, and the TxD line will go low causing a
		 * framing error to be detected by the receiver. The docu-
		 * mentation is somewhat ambiguous about this tho...
		*/
		BYTE dp->cmd |= BRK;
		s = spl5();
		if (tp->t_outq.c_cc <= 0) {
			putc(0,&tp->t_outq);
			rsstart(tp);
		}
		splx(s);
		break;
	case TIOCCBRK:
		BYTE dp->cmd &= ~BRK;
		break;
	case TIOCSDTR:
		BYTE dp->cmd |= DTR;
		break;
	case TIOCCDTR:
		BYTE dp->cmd &= ~DTR;
		break;
	default:
		u.u_error = ENOTTY;
	}
}

rsputchar(c)
register c;
{
	register timo,s;
	register RSDEV *dp = device;

	s = spl7();
	timo = 30000;
	/*
	 * Try waiting for the console tty to come ready,
	 * otherwise give up after a reasonable time.
	 */
	while ((dp->status & TxRDY) == 0) {
		if (--timo <= 0) break;
		nulldev();		/* preserve references for optimizer */
	}
	if(c == 0)
		return;
	BYTE dp->data = c;
	if(c == '\n')
		rsputchar('\r');
	rsputchar(0);
	splx(s);
}

ns2651init(tp)
register struct tty *tp;
{
	register RSDEV *dp;
	extern char dummy;	/* to make optimizer preserve references */

	ns2651mode(tp);
	dp = (RSDEV *)tp->t_addr;
	BYTE dp->cmd = CMD;
	dummy = dp->data;		/* toss initial char */
}

ns2651mode(tp)
register struct tty *tp;
{
	extern char dummy;
	register RSDEV *dp;
	register u_char mode = MODE1;

	ns8253init(tp);		/* init the baud clock first... */
	dp = (RSDEV *)tp->t_addr;
	if((tp->t_flags & (EVENP|ODDP)) != (EVENP|ODDP)) {
		mode |= PENABLE;
		if (tp->t_flags & EVENP)
			mode |= EPAR;
	}
	dummy = dp->cmd;		/* Resets the register pointers */
	BYTE dp->mode = mode;		/* Mode reg 1 */
	BYTE dp->mode = 0;		/* Mode reg 2 */

	if (BYTE dp->status & DCD) {
	    tp->t_state |= CARR_ON;
	} else {
	    tp->t_state &= ~CARR_ON;
	}
}

#define rxmode	0x36			/* square wave, 16 bit low */
#define txmode	0x76			/*  ditto above... */

/* Basic clock initialization values for the 8253 baud clock generation...
   This array is indexed by the baud value stored in the tty structure,
   to determine the correct baud clock initialization values. An entry of
   0 means the baud rate is not supported...
 */

static u_short baudrates[] = {
	0,2304,1536,1047,857,768,0,384,
	192,96,64,48,24,12,6,0};

/* Baud rate clock initialization routine */

ns8253init(tp)				/* National 8253 timer init */
register struct tty *tp;
{
	register TIMER *clock = (TIMER *) CP_Time;
	u_short clkcnt;
	register char *rp = (char *)&clkcnt;

	clkcnt = baudrates[tp->t_ispeed];
	if (clkcnt) {
		BYTE clock->mode  = rxmode;
		BYTE clock->rxclk = *rp;		/* Lo byte... */
		BYTE clock->rxclk = rp[1];		/* Hi byte... */
		clkcnt = baudrates[tp->t_ospeed];
		if (clkcnt) {
			BYTE clock->mode  = txmode;
			BYTE clock->txclk = *rp;
			BYTE clock->txclk = rp[1];
			return;
		}
	}
	u.u_error = ENOTTY;
	return;
}
#endif NRS > 0
