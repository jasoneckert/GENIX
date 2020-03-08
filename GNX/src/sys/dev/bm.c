/*
 * bm.c: version 3.17 of 8/17/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)bm.c	3.17 (NSC) 8/17/83";
# endif

/*
 * bit mapped graphics terminal driver
 */

#if NBM > 0

#include "../h/param.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/tty.h"
#include "../h/systm.h"
#include "../h/vm.h"
#include "../h/devvm.h"
#include "../h/icu.h"

struct tty	bitmap;
int		bmstart();
int		ttrstrt();
int		spl5(), splx();
extern char	partab[];
extern int	bmconsf;
unsigned short	flavors;

/*
 * Various registers and bit positions on the sib (graphics) board:
 */
#define CONSMASK	GB_IntMask
#define KEYBOARD	GB_Keyboard
#define CONSISTAT	GB_IntStatus
#define CONSCMDSTAT	GB_Control
#define SCL_BUSY	0x01
#define SCL_DONE	0x02
#define CONSCLEAR	GB_IntClear
#define KBD_ENABLE	0x08
#define SCL_ENABLE	0x02

extern char	cmr;	/* copy of the graphics board interrupt register */
extern u_short	dummy;

bmopen(dev, flag)
dev_t dev;
{
	register struct tty *tp;

	tp = &bitmap;
	tp->t_oproc = bmstart;
	tp->t_iproc = NULL;
	if ((tp->t_state&ISOPEN) == 0) {
		ttychars(tp);
		tp->t_state = ISOPEN|CARR_ON;
		tp->t_flags = EVENP|ECHO|CRMOD;
		tp->t_ispeed = B9600;
		tp->t_ospeed = B9600;

		/* Enable keyboard and scrolling interrupts: */
		cmr |= KBD_ENABLE | SCL_ENABLE;
		*CONSMASK = cmr;
		*CONSCLEAR = '\0';
		dummy = *KEYBOARD;
		icuenable(GRAFMASK);
		if (bmconsf == 0) cursoron();
	}
	if (tp->t_state&XCLUDE && u.u_uid != 0) {
		u.u_error = EBUSY;
		return;
	}
	ntyopen(dev, tp);
}

bmclose(dev,flag)
dev_t dev;
{
	register struct tty *tp;

	tp = &bitmap;
	ntyclose(tp);
	if (bmconsf == 0) {
		cmr &= ~(KBD_ENABLE | SCL_ENABLE);
		*CONSMASK = cmr;
		tp->t_state &= ~(ISOPEN|CARR_ON);
		cursoroff();
	}
}

bmread(dev)
dev_t dev;
{
	ntread (&bitmap);
}

bmwrite(dev)
dev_t dev;
{
	ntwrite (&bitmap);
}


/* Character received interrupt */
bmrint(dev)
dev_t dev;
{
	register unsigned short c;
	register unsigned short f;

	c = ~(*KEYBOARD);		/* get character and flavor info */
	f = c >> 8;
	c &= 0177;
	if (f || flavors) {		/* a flavor key */
#ifdef	FLAVORDUMP
		if (bmconsf && (f == 1) && (flavors == 1)) {
			flavors = f;
			flavordump(c);		/* dump debugging info */
			return;
		}
#endif
		flavors = f;
		return;
	}
	ntyinput(c, &bitmap);		/* normal character */
}


bmioctl(dev, cmd, addr, flag)
dev_t dev;
caddr_t addr;
{
	if (ttioctl (&bitmap, cmd, addr, flag) == 0)
		u.u_error = ENOTTY;
}


/*
 *	Transmit interrupt handler.
 *	Currently, the only relevant interrupt
 *	is the scrolling completion interrupt.
 */
bmxint(dev)
dev_t dev;
{
	register struct tty *tp;

	tp = &bitmap;
	tp->t_state &= ~BUSY;
	bmstart(tp);
}

/*
 * Initiate transmission of pending characters to the display.
 *	Spew them out until there are no more or we have to wait
 *	for a scrolling interrupt.
 *
 *	This version of the routine has been rewritten
 *	to allow interrupts in between characters.
 */
bmstart(tp)
register struct tty *tp;
{
	register c;
	register s;

	s = spl5();

	if (tp->t_state & (TIMEOUT|BUSY|TTSTOP)) {
		goto out;
	}

	if (tp->t_state&ASLEEP && tp->t_outq.c_cc <= TTLOWAT(tp)) {
		tp->t_state &= ~ASLEEP;
		wakeup((caddr_t)&tp->t_outq);
	}

	/* Let bmoutputchar's internal state clear (i.e., finish scrolling): */
	bmoutputchar (0);	/* bmoutputchar ignores nulls */

	/* Put out pending chars until we have to delay (due to scrolling): */
	for ( ;; ) {
		spl5 ();
		if (tp->t_outq.c_cc <= 0 || (tp->t_state & BUSY))
			break;
		c = getc(&tp->t_outq);
		if (tp->t_flags&RAW || tp->t_local&LLITOUT || (c<0177))
			bmoutputchar(c);
		else {
			timeout(ttrstrt, (caddr_t)tp, (c&0177));
			tp->t_state |= TIMEOUT;
			goto out;
		}
		spl0();
	}
out:
	splx (s);
}

/*
 * Put a character out on the bitmapped display from interrupt level.
 * This implies busy waiting for scrolls to complete.
 */
bmputchar (c)
	int	c;
{
	register crtcon;
	int	s;
	extern	int	nulldev();	/* to make optimizer happy */

	if (c == '\n') {
		bmputchar('\r');
	}

	/*
	 * If the display is busy (scroll operation in progress),
	 * wait for the operation to clear and then give bmoutputchar
	 * a chance to clear its state by sending it a null.
	 */
	s = spl7();
	while (bitmap.t_state & BUSY) {
		if ((crtcon = *CONSCMDSTAT) & SCL_BUSY) {
		    while ((crtcon = *CONSCMDSTAT) & SCL_BUSY)
			    nulldev();	/* busy wait (needed for optimizer) */
		    *CONSCLEAR = ~SCL_DONE;	/* dismiss scroll intr */
		}
		bitmap.t_state &= ~BUSY;
		bmoutputchar (0);
	}

	/* State is clear; put out the character: */
	bmoutputchar (c);
	splx(s);
}

#endif NBM
