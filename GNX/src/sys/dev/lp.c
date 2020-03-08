/*
 * lp.c: version 3.7 of 8/4/83
 * Mesa Unix Device Driver
 */
# ifdef SCCS
static char *sccsid = "@(#)lp.c	3.7 (NSC) 8/4/83";
# endif

#if NLP>0

#include "../h/param.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/buf.h"
#include "../h/systm.h"
#include "../h/pte.h"
#include "../h/ioctl.h"
#include "../h/tty.h"
#include "../h/devvm.h"
#include "../h/icu.h"

#define	LPPRI	(PZERO+8)
#define	LPLWAT	300
#define	LPHWAT	650

#define MAXCOL	132
#define CAP	1

#define LPUNIT(dev) (minor(dev) >> 3)

int fakeint();
int lptout();
/* status bit offsets (port B) */
#define LPBUSY	(1<<4)	/* 0 = printer is busy */
#define PE	(1<<5)	/* 0 = printer fault or printer is open */
#define SLCT	(1<<6)	/* 0 = printer has been selected */
#define FAULT	(1<<7)	/* 1 = out of paper, light detected, etc. */
#define STATMASK 0x00f0 /* set all non-status bits to zero */
#define LPREADY  0x0030	/* selected and no fault, busy, or pe */

/* control bits (port C)
 * the prime signal is generated from bit 5
 * and is level triggered, so it is true when the bit is 0 and false when 1.
 */

#define PRIME		0x0a
#define POSTPRIME	0x0b
#define LPINIT		0xa2	/* port A is strobed output, port B is	   */
				/* mode 0 input				   */
#define IENABLEA	0x0d	/* enable ints for port A		   */
#define IENABLEB	0x05	/* enable ints for port B		   */

/* definition of printer device */
struct lpdevice {
	unsigned short data;		/* char to be printed (port A) */
	unsigned short status;		/* port B */
	unsigned short ctrl;		/* port C */
	unsigned short init;		/* "control" reg for 8255a */
};

struct lp_softc {
	struct	clist sc_outq;
	int	sc_state;
	int	sc_physcol;
	int	sc_logcol;
	int	sc_physline;
	char	sc_flags;
	int	sc_lpchar;
	struct	buf *sc_inbuf;
} sc;

/* bits for state */
#define	OPEN		1	/* device is open */
#define	ASLP		(1<<1)	/* awaiting draining of printer */
#define	ERRORINT	(1<<2)	/* error interrupt was received from printer */
#define	WANTCHAR	(1<<3)	/* printer is ready when a char is */
#define LPTOUT		(1<<4)	/* timeout while not selected	*/

extern	lbolt;

/*
 * lpopen -- reset the printer, then wait until it's selected and not busy.
 */

lpopen(dev, flag)
	dev_t dev;
	int flag;
{
	register int unit;
	register struct lpdevice *lpaddr = (struct lpdevice *) CP_PIO;

	if (sc.sc_state & OPEN) {
		u.u_error = EBUSY;
		return;
	}
	if ((lpaddr->status & STATMASK) ^ LPREADY) {
		u.u_error = ENXIO;
		return;
	}
	sc.sc_inbuf = geteblk();
	sc.sc_flags = minor(dev) & 07;

	/* initialize the 8255; mode 1, port A is
	 * output, port B is input. also enable ints for ports A and B
	 */
	lpaddr->init = LPINIT;
	lpaddr->init = IENABLEA;
	
	icuenable(APIOMASK|BPIOMASK);
	/* reset the printer, then wait until it's selected */
	if (minor(dev) == 0) {		/* PRINTRONIX */
		lpaddr->init = POSTPRIME;
		lpaddr->init = PRIME;
	} else {			/* CENTRONIX */
		lpaddr->init = PRIME;
		lpaddr->init = POSTPRIME;
	}
	while ((lpaddr->status & SLCT) != 0) {
		sleep (&sc, LPPRI);
		/*
		printf ("lpopen: awake (select)...\n");
		*/
	}

	/* set state when the printer is ready for a char */
	while ((lpaddr->status & LPBUSY) == 0) {
		sleep (&lbolt, LPPRI);
	}
	sc.sc_state = OPEN | WANTCHAR;

#ifdef notdef
	/* force interrupts in case the ICU misses one */
	timeout (fakeint, 0, hz);
#endif notdef

	lpcanon('\f');
}


#ifdef notdef
fakeint () {
	/* interrupt every second in case of missed interrupts */
	if (sc.sc_state & OPEN) {
		interrupt (APIO);
		timeout (fakeint, 0, hz);
	}
}
#endif notdef

/*
 * lpclose -- close the device, free the local line buffer.
 */

lpclose(flag)
	int flag;
{

	lpcanon('\f');
	brelse(sc.sc_inbuf);
	sc.sc_state = 0;
	/*
	icudisable(APIOMASK|BPIOMASK);
	*/
}

/* 
 * lpwrite --copy a line from user space to a local buffer, then call 
 * lpcanon to get the chars moved to the output queue.
 */

lpwrite() {
	register unsigned n;
	register char *cp;
	int pl;

	if (sc.sc_state & ERRORINT) {
		u.u_error = ENXIO;
		lpflush();
		return;
	}
	while (n = MIN(BSIZE, u.u_count)) {
		cp = sc.sc_inbuf->b_un.b_addr;
		pl = spl4();
		iomove(cp, n, B_WRITE);
		(void) splx (pl);
		do
			lpcanon(*cp++);
		while (--n);
	}
}

/*
 * lpcanon -- convert a line of chars to printable chars and call lpoutput
 * to put them in the output queue. the routine also issues form feeds at the
 * end of a page.
 */

lpcanon(c)
	register int c;
{
	register int logcol, physcol;

	if (sc.sc_flags&CAP) {
		register c2;

		if (c>='a' && c<='z')
			c += 'A'-'a'; 
		else switch (c) {

			case '{':
				c2 = '(';
				goto esc;

			case '}':
				c2 = ')';
				goto esc;

			case '`':
				c2 = '\'';
				goto esc;

			case '|':
				c2 = '!';
				goto esc;

			case '~':
				c2 = '^';

			esc:
				lpcanon(c2);
				sc.sc_logcol--;
				c = '-';
		}
	}
	logcol = sc.sc_logcol;
	physcol = sc.sc_physcol;
	if (c == ' ')
		logcol++;
	else switch(c) {

	case '\t':
		logcol = (logcol+8) & ~7;
		break;

	case '\f':
		if (sc.sc_physline == 0 && physcol == 0)
			break;
		/* fall into ... */

	case '\n':
		lpoutput('\r');	/* for printers that need CRLF at end of line */
		lpoutput(c);
		if (c == '\f')
			sc.sc_physline = 0;
		else
			sc.sc_physline++;
		physcol = 0;
		/* fall into ... */

	case '\r':
		logcol = 0;
		break;

	case '\b':
		if (logcol > 0)
			logcol--;
		break;

	default:
		if (logcol < physcol) {
			lpoutput('\r');
			physcol = 0;
		}
		if (logcol < MAXCOL) {
			while (logcol > physcol) {
				lpoutput(' ');
				physcol++;
			}
			lpoutput(c);
			physcol++;
		}
		logcol++;
	}
	if (logcol > 1000)	/* ignore long lines  */
		logcol = 1000;
	sc.sc_logcol = logcol;
	sc.sc_physcol = physcol;
}

/*
 * lpoutput -- put a char on the output queue. if the printer is ready, 
 * call lpintr to send it a char.
 */

lpoutput(c)
	int c;
{
	int pl;

	if (sc.sc_outq.c_cc >= LPHWAT) {
		sc.sc_state |= ASLP;
		sleep((caddr_t)&sc.sc_outq, LPPRI);
	}
	pl = spl4();
	putc(c, &sc.sc_outq);
	(void) splx (pl);
	/* if the printer is ready for a char, give it one */
	if (sc.sc_state & WANTCHAR) {
		sc.sc_state &= ~WANTCHAR;
		pl = spl4();
		lpintr();
		(void) splx (pl);
	}
}

/*
 * lperrint -- tell the driver that the printer has caused an error interrupt.
 */

lperrint() {
	register struct lpdevice *lpaddr = (struct lpdevice *) CP_PIO;
	register char status;

	status = lpaddr->status;

	if ((status & PE) == 0) {
		sc.sc_state |= ERRORINT;
		printf ("/dev/lp: received paper error from printer.\n");
	}
	else if (status & FAULT) {
		printf ("/dev/lp: received fault from printer.\n");
	}

	/* if we're just selected, wakeup lpopen */
	else if ((status & SLCT) == 0) {
		wakeup (&sc);
	}
}

/*
 * lpintr -- handle printer interrupts which occur when the printer is
 * ready to accept another char. if there is no char to pass it, the
 * routine changes the softchar state so that it can be called
 * when there is a char to be printed.
 */

lpintr() {
	register struct lpdevice *lpaddr = (struct lpdevice *) CP_PIO;
	register int status;
	char ch;

	status = lpaddr->status;
	/* if interrupt not really from printer, ignore it */
	if ((status & STATMASK) ^ LPREADY) {
		return;
	}

	if ((sc.sc_outq.c_cc <= 0) || sc.sc_state & LPTOUT) {
		/* tell the world we're waiting for a char */
		sc.sc_state |= WANTCHAR;
		return;
	}

	if (status & SLCT) {	/* no longer selected */
		sc.sc_state |= LPTOUT;
		timeout (lptout, 0, 5*hz);
		return;
	}

	ch = getc(&sc.sc_outq);
	lpaddr->data = ch;
	status = lpaddr->ctrl;
	sc.sc_state &= ~WANTCHAR;
	if (sc.sc_outq.c_cc<=LPLWAT && sc.sc_state&ASLP) {
		sc.sc_state &= ~ASLP;
		wakeup((caddr_t)&sc.sc_outq);	/* top half should go on */
	}
}

/*
 * lptout -- wait until we're selected and not busy before sending a char.
 * this routine is originally put on the timeout list because the printer
 * has been deselected.
 */

lptout () {
	register struct lpdevice *lpaddr = (struct lpdevice *) CP_PIO;
	register char status;
	int pl;

	status = lpaddr->status;
	if ((status & SLCT) || ((status & LPBUSY) == 0)) {
		timeout (lptout, 0, 5*hz);
	}
	else {	/* restart the device */
		sc.sc_state &= ~LPTOUT;
		pl = spl4();
		lpintr();
		(void) splx (pl);
	}
}

/*
 * lpflush -- flush the character queue after a printer error.
 */

lpflush () {
	while (getc(&sc.sc_outq) >= 0) ;
}

#endif NLP
