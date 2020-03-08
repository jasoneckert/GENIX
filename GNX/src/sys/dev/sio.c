/*
 * sio.c: version 3.24 of 10/3/83
 * Mesa Unix Device Driver
 */
# ifdef SCCS
static char *sccsid = "@(#)sio.c	3.24 (NSC) 10/3/83";
# endif

# if NSIO > 0 /* Don't bother compiling if config doesn't have sio board(s) */

# include "../h/sio.h"
# include "../h/param.h"
# include "../h/conf.h"
# include "../h/dir.h"
# include "../h/user.h"
# include "../h/tty.h"
# include "../h/file.h"
# include "../h/devvm.h"
# include "../h/icu.h"

/* Arrange for suitable symbol visibility: */
# ifndef Static
# ifdef  DEBUG
# define Static
# else
# define Static	static
# endif  DEBUG
# endif  Static

/*
 * The number of tty structs on SIO board(s)
 * This variable is for the pstat program to find.
 */
int	nttys	= NSIO * SIO_LINES;

/*
 * The SIO board's (or boards') tty structs:
 */
struct tty	sio [NSIO * SIO_LINES];

/*
 * A table of dma character counts:
 *	The SIO board counts the m_dma[line].dma_count field down
 *	as it puts characters out, so we need a record of the original
 *	amount somewhere else.
 */
Static u_short	dmacount [NSIO * SIO_LINES];

/*
 * Last known line state for each line.
 *	We need this to determine what changed when we get a modem interrupt.
 */
Static u_char	modem [NSIO * SIO_LINES];

/*
 * For each SIO board, a pointer to the start
 * of device memory for the board.
 */
Static sio_mem	*device [NSIO];

/*
 * Temporary, pending autoconfiguration:
 *	For each SIO board, a record of whether it's been attached yet.
 */
Static char	attached [NSIO];

/*
 * Instrumentation variables:
 */
int	sio_drain_count,	/* chars waiting at last fifo int	*/
	sio_max_drain_count,	/* max chars waiting at fifo int	*/
	sio_xints,		/* # of dma done interrupts		*/
	sio_xints_more;		/* # of dma done interrupts with more	*/
				/* completions occurring while in xint	*/

/*
 * Hook an SIO board into the system.
 *	This incarnation of the routine is preliminary and may well
 *	misinterpret what really needs to be done, especially when
 *	we add autoconfiguration into the system.
 */
sio_attach (board)
	register int	board;
{
	register sio_mem	*smp;
	register int		s;
	extern void		sio_goose ();

	/*
	 * Initialize the slot in device for this board.
	 * Note that this code assumes that sio_mem fully describes
	 * the entire address space consumed by the board, so that
	 * positive subscripts truly get us to the starting address
	 * of subsequent boards.  (See also dev[vp]m.h, for possible
	 * compaction of unused parts of board address space.)
	 */
	smp = &(((sio_mem *) SIO_BASE)[board]);
	device[board] = smp;

	/*
	 * Let the icu know we're here:
	 */
	icuenable (SIOMASK);

	/*
	 * Enable SIO board interrupts:
	 */
	smp->m_mask = I_DONE | I_MODEM | I_FIFO | I_ERROR;

	s = spl5 ();

	/* Reset the SIO board firmware: */
	sio_mkcmd (smp, C_SUBR, 0);
	smp->m_cmd[0].c_data0 = 0;;
	smp->m_cmd[0].c_data1 = 0;
	sio_go (smp);

	/*
	 * Select single-character fifo interrupt mode:
	 *	We eventually will want to be able to change
	 *	this dynamically, depending on load conditions.
	 */
	sio_mkcmd (smp, C_INT_SINGLE, 0);
	sio_go (smp);

	/* Flush out the fifo: */
	sio_mkcmd (smp, C_ZAP_FIFO, 0);
	sio_go (smp);

	/* Set up a callout to goose the board once a second: */
	timeout (sio_goose, 0, 60);

	(void) splx (s);
}

/*
 * Open a line on the SIO board.
 */
sio_open (dev, flag)
	dev_t	dev;
	int	flag;
{
	register struct tty	*tp;
	register sio_mem	*smp;
	register int		unit,
				line,
				s;
	int			board;
	extern int		sio_start ();

	unit  = minor (dev);
	board = unit / SIO_LINES;
	line  = unit & (SIO_LINES-1);

	/*
	 * Temporary, pending autoconfiguration:
	 *	If this is the first time through, attach the sio_board.
	 */
	if (! attached[board]) {
		sio_attach (board);
		attached[board] = 1;
	}

	smp = device [board];
	tp  = &sio [unit];

	/* Check for exclusive use: */
	if ((tp->t_state & XCLUDE) && u.u_uid != 0) {
		u.u_error = EBUSY;
		return;
	}

	/*
	 * If the line is already open, the
	 * following would seem to be redundant...
	 */
	tp->t_oproc  = sio_start;
	tp->t_addr   = (caddr_t) smp;
	tp->t_iproc  = NULL;
	tp->t_dev    = dev;	/* done later, but we need it now for sio_param */

	tp->t_state |= WOPEN;

	/* Enable the line: */
	s = spl5  ();
	sio_mkcmd (smp, C_LINE_ON, line);
	sio_go    (smp);
	(void) splx (s);

	/* Is the line already open? */
	if ((tp->t_state & ISOPEN) == 0) {
		/* No: set it up from scratch: */
		ttychars (tp);
		tp->t_ospeed = tp->t_ispeed = B9600;
		tp->t_flags  = ODDP | EVENP | ECHO | XTABS | CRMOD;
		/* Make the new state take effect: */
		sio_param (tp);
	}

	s = spl5 ();

	/* (Re)assert DTR: */
	sio_mkcmd (smp, C_MDM_SET, line);
	smp->m_cmd[0].c_data0 = M_DTR;
	sio_go (smp);

	/* Wait for carrier: */
	for ( ;; ) {
		/* Force the CARR_ON bit to be correct: */
		if (modem[unit] & M_DCD) {
			tp->t_state |=  CARR_ON;
			break;
		}
		else
			tp->t_state &= ~CARR_ON;

		tp->t_state |= WOPEN;

		sleep ((caddr_t) &tp->t_rawq, TTIPRI);
	}

	(void) splx (s);

	/*
	 * Complete the open by invoking a line-discipline specific
	 * open routine.  (This routine will turn WOPEN off.)
	 */
	(void) (*linesw[tp->t_line].l_open) (dev, tp);
}

/*
 * Close an SIO board line.
 *	Called on last close for the line.
 */
sio_close (dev, flag)
	dev_t	dev;
	int	flag;
{
	register struct tty	*tp;
	register sio_mem	*smp;
	register int		unit,
				line,
				s;

	unit = minor (dev);
	line = unit & (SIO_LINES-1);
	tp   = &sio[unit];
	smp  = (sio_mem *) tp->t_addr;

	/* First, do line-discipline specific closing actions: */
	(*linesw[tp->t_line].l_close) (tp);

	/*
	 * Next, do housekeeping:
	 *	I.e., make sure break isn't set for the line,
	 *	and make sure DTR is off if the line is hang up on close.
	 */
	s = spl5 ();
	sio_mkcmd (smp, C_MDM_CLR, line);
	smp->m_cmd[0].c_data0 = M_BRK;
	sio_go (smp);
	if (tp->t_state & HUPCLS) {
		sio_mkcmd (smp,  C_MDM_CLR, line);
		smp->m_cmd[0].c_data0 = M_DTR;
		sio_go (smp);
	}
	(void) splx (s);

	/* And finally, do general closing actions: */
	ttyclose (tp);
}

/*
 * Read from an SIO board line.
 *	We just figure out who we are and then call
 *	the appropriate line-discipline specific routine
 *	to do the actual work.
 */
sio_read (dev)
	dev_t dev;
{
	register struct tty	*tp;

	tp = &sio[minor(dev)];
	(*linesw[tp->t_line].l_read) (tp);
}

/*
 * Write to an SIO board line.
 *	We just figure out who we are and then call
 *	the appropriate line-discipline specific routine
 *	to do the actual work.
 */
sio_write (dev)
	dev_t	dev;
{
	register struct tty	*tp;

	tp = &sio[minor(dev)];
	(*linesw[tp->t_line].l_write) (tp);
}

/*
 * Debugging variable:
 */
int	sio_badint_flag;

/*
 * Handle an SIO board interrupt by dispatching off to the appropriate
 * interrupt-handling routines.
 *	The board has only one interrupt, used for both transmitter
 *	and receiver interrupts.  The interrupt mask register records
 *	interrupt causes, with a bit for each possible `real' cause.
 *
 *	This routine isn't currently set up to work in a polling situation;
 *	if we are called from clock, we won't see the I_FIFO bit and thus
 *	won't dispatch to sio_rint to clear out chars.  Sio_rint should be
 *	called directly in this situation.
 */
sio_int (board)
	register int	board;
{
	register sio_mem	*smp;
	register u_char		int_status,
				linesdone,
				lines_pend;

	smp = device[board];

	/*
	 * Find out why we're here, by grabbing the
	 * clear-on-read interrupt status register.
	 */
	int_status = smp->m_status;

	/* Record some history for debugging: */
	linesdone  = smp->m_done;
	lines_pend = smp->m_done_pend;
	sio_monitor	( board
			, int_status
			, linesdone
			, lines_pend
			);

#	ifdef notdef
	/* Check for interrupt anomalies: */
	if ((int_status & I_DONE) == !linesdone) {
		printf  ( "sio%d: bad int status; stat: %x lines: %x lines_pend: %x\n"
			, board
			, int_status
			, linesdone
			, lines_pend
			);
		sio_badint_flag++;
	}
#	endif

	/* Take care of incoming characters: */
	if (int_status & I_FIFO)
		sio_rint (board);

	/* Take care of line transitions: */
	if (int_status & I_MODEM)
		sio_mint (board);

	/* Take care of dma completions: */
	if (int_status & I_DONE)
		sio_xint (board);

	/*
	 * Take care of dma errors.
	 *	Since all we can do is note that they've occurred,
	 *	we handle them here.
	 */
	if (int_status & I_ERROR)
		printf ("sio%d: dma transfer error\n", board);

	/*
	 * Check for and comment on unexpected interrupts:
	 *	These days, there's only one: the diagnostic interrupt.
	 */
	if (int_status & I_DIAG)
		printf ("sio%d: unexpected interrupt %x\n", board, int_status);
}

/*
 * Handle an SIO board receiver interrupt.
 */
sio_rint (board)
	int	board;
{
	register struct tty	*tp, *tp0;
	register short		*fifop;
	register int		c,
				overran;

	fifop = &(device[board]->m_fifo);
	tp0   = &sio [board * SIO_LINES];

	overran = 0;

	/* Drain the fifo: */
	sio_drain_count = 0;
	while ((c = *fifop) >= 0) {
		sio_drain_count++;

		/* Figure out who the char belongs to: */
		tp = tp0 + sio_getline (c);

		/* If nobody has the line open, just wake up sleepers. */
		if ((tp->t_state & ISOPEN) == 0) {
			wakeup ((caddr_t) &tp->t_rawq);
			continue;
		}

		/* Check for framing error: */
		if (c & FI_FRAME) {
			/*
			 * In raw mode, framing errors (breaks) turn
			 * into nulls; otherwise, they become interrupts.
			 */
			if (tp->t_flags & RAW)
				c = 0;
			else
				c = tun.t_intrc;
		}

		/* Check for overrun: */
		if ((c & FI_OVER) && overran == 0) {
			printf ("sio%d: fifo overrun\n", board);
			overran++;
		}

		/* Check parity: */
		if (c & FI_PAR) {	/* parity error... */
			if (  (tp->t_flags & (EVENP | ODDP)) == EVENP
			   || (tp->t_flags & (EVENP | ODDP)) == ODDP
			   )
				continue;
		}

		/*
		 * We have a legitimate character.
		 * Let the line discipline stick it onto an input queue.
		 */
#		if NBK > 0
		if (tp->t_line == NETLDISC) {
			c &= 0177;
			BKINPUT (c, tp);
		}
		else
#		endif
			(*linesw[tp->t_line].l_rint) (c, tp);
	}

	if (sio_drain_count > sio_max_drain_count)
		sio_max_drain_count = sio_drain_count;
}

/*
 * Conversions between baud rate codes as in <sgtty.h> and
 * as the SIO board expects them.
 *	A value of -1 means the board doesn't support the rate in question.
 *	We interpret EXTA as B19200 and don't support EXTB.
 *	B0 is special: it turns into a request to hang up the line.
 *	This request is interpreted before speeds are set.
 */
Static char	baud_cvt [] = {
	 0,	 0,	 1,	 2,
	 3,	 4,	-1,	 5,
	 6,	 7,	 8,	10,
	12,	14,	15,	-1
};

/*
 * Handle ioctl calls relevant to the SIO board.
 */
sio_ioctl (dev, cmd, addr, flag)
	dev_t	dev;
	int	cmd;
	caddr_t	addr;
	int	flag;
{
	register struct tty	*tp;
	register sio_mem	*smp;
	register int		line,
				unit,
				s;

	unit = minor (dev);
	tp   = &sio [unit];
	smp  = (sio_mem *) tp->t_addr;
	line = unit & (SIO_LINES-1);

	/* Each line discipline interprets commands differently... */
	cmd = (*linesw[tp->t_line].l_ioctl) (tp, cmd, addr);

	if (cmd == 0)
		return;

	/* See whether the command can be handled in the tty driver: */
	if (ttioctl (tp, cmd, addr, flag)) {
		if (cmd == TIOCSETP || cmd == TIOCSETN) {
			/*
			 * Have to set new line parameters: *tp now
			 * specifies the state we have to establish.
			 * First, we must verify that the SIO board
			 * supports the desired line speed...
			 */
			if (baud_cvt[tp->t_ospeed] == -1) {
				u.u_error = EINVAL;
				return;
			}
			sio_param (tp);
		}
	}
	else {
		/*
		 * The command couldn't be handled in the tty driver.
		 * Set up and issue a command to the SIO board to do it.
		 *	Interrupts must be locked out to prevent the command
		 *	from being destroyed before being issued.
		 */
		s = spl5 ();

		switch (cmd) {

		case TIOCSBRK:
			/*
			 * Much ugliness here.
			 *	The SIO board's usarts don't actually set
			 *	the break condition until getting a character
			 *	after being told to turn on break.  Therefore,
			 *	we have to send out a spurious null for the
			 *	sole purpose of making sure break actually
			 *	happens.  Ick!  (The acutal code for this
			 *	lives below, after we turn on break.)
			 */
			sio_mkcmd (smp, C_MDM_SET, line);
			smp->m_cmd[0].c_data0 = M_BRK;
			break;
		case TIOCCBRK:
			sio_mkcmd (smp, C_MDM_CLR, line);
			smp->m_cmd[0].c_data0 = M_BRK;
			break;
		case TIOCSDTR:
			sio_mkcmd (smp, C_MDM_SET, line);
			smp->m_cmd[0].c_data0 = M_DTR;
			break;
		case TIOCCDTR:
			sio_mkcmd (smp, C_MDM_CLR, line);
			smp->m_cmd[0].c_data0 = M_DTR;
			break;

		default:
			u.u_error = ENOTTY;
			(void) splx (s);
			return;
		}

		/* Fire off the command: */
		sio_go (smp);

		/* Take care of break ugliness, as discussed above: */
		if (cmd == TIOCSBRK) {
			if (tp->t_outq.c_cc <= 0)
				putc (0, &tp->t_outq);
			sio_start (tp);
		}

		(void) splx (s);
	}
}

/*
 * Set parameters from open or ioctl
 * into the relevant SIO board.
 */
sio_param (tp)
	register struct tty	*tp;
{
	register sio_mem	*smp;
	register int		line,
				s;
	register u_short	config;

	/* Retrieve board and line values from tp: */
	smp  = (sio_mem *) tp->t_addr;
	line = minor (tp->t_dev) & (SIO_LINES-1);

	/* If line speed is zero, turn off DTR: */
	if (tp->t_ispeed == 0) {
		s = spl5 ();
		sio_mkcmd (smp,  C_MDM_CLR, line);
		smp->m_cmd[0].c_data0 = M_DTR;
		sio_go (smp);
		(void) splx (s);
		return;
	}

	/*
	 * Calculate parameter values:
	 *	We give 110 and 134.5 baud no special treatment,
	 *	since they've been observed to work correctly on
	 *	our terminals.  It may be necessary to put in some
	 *	special casing for hard copy terminals running at
	 *	these speeds.
	 *
	 *	It is also assumed that the line speed is valid
	 *	(sio_ioctl checks this; other callers should, too).
	 */
	config  = U_MD_ASYNC1X | U_STOP1 | U_IN_CLK;
	config |= ((u_char) baud_cvt[tp->t_ospeed]) << 8;
	config |= ((tp->t_flags & RAW) || (tp->t_local & LLITOUT))
		?  U_LEN8
		: (U_LEN7 | UART_PAR_ON);
	if (tp->t_flags & EVENP)
		config |= UART_PAR_EVEN;

	/* Set up the configuration command and issue it: */
	s = spl5 ();
	sio_mkcmd (smp, C_CONFIG, line);
	smp->m_cmd[0].c_data0 = config & 0xff;
	smp->m_cmd[0].c_data1 = config >> 8;
	sio_go (smp);
	(void) splx (s);
}

/*
 * SIO board transmitter interrupt.
 *	Restart transmission on each line that was active,
 *	but that has terminated transmission since the last
 *	time through here.
 */
sio_xint (board)
	int	board;
{
	register struct tty	*tp;
	register sio_mem	*smp;
	u_char			linesdone;
	register u_char		linebit;
	register int		line,
				cnt;
	u_short			tmpcnt;	/* Can't be register */

	smp = device [board];
	linesdone = smp->m_done;

	/*
	 * Scan each line's entry in the completion register
	 * and process those with completed dma.
	 */
	for ( line = 0, linebit = 1, tp = &sio[board * SIO_LINES]
	    ; line < SIO_LINES
	    ; line++, linebit <<= 1, tp++
	    ) {
		if (linebit & linesdone) {
			/* The line is no longer busy nor being flushed. */
			tp->t_state &= ~(BUSY | FLUSH);

			/*
			 * Throw away the chars we've DMAed out,
			 * being careful to do byte accesses to the SIO board.
			 */
			bytecpy ( &tmpcnt
				, &smp->m_dma[line].dma_resid
				, sizeof (u_short)
				);
			cnt = tmpcnt;
			if (sio_badint_flag) {
				printf	( "sio%d, line %d, dma count: orig %d resid %d, addr: orig %x"
					, board
					, line
					, dmacount [minor (tp->t_dev)]
					, cnt
					, tp->t_outq
					);
			}
			cnt = dmacount [minor (tp->t_dev)] - cnt;
			ndflush (&tp->t_outq, cnt);
			if (sio_badint_flag) {
				printf (" after %x\007\n", tp->t_outq);
			}

			/* Restart transmission on the line: */
			if (tp->t_line)
				(*linesw[tp->t_line].l_start) (tp);
			else
				sio_start (tp);
		}
	}

	/*
	 * Acknowledge and dismiss the lines we've handled.
	 *	Grab some instrumentation information while
	 *	we're at it.
	 *	(See sio_int for discussion about exactly when we
	 *	should dismiss things; this may perform suboptimally.)
	 */
	sio_xints++;
	/* Will we be interrupted again immediately when we leave? */
	if (smp->m_done_pend)
		sio_xints_more++;
	/* Dismiss the lines: */
	if (linesdone)
		smp->m_done = 0;

	sio_badint_flag = 0;
}

extern int	ttrstrt ();

/*
 * Start (or restart) the SIO board's transmitter on the given line.
 */
sio_start (tp)
	register struct tty	*tp;
{
	register sio_mem	*smp;
	register int		unit,
				line,
				nch;
	sio_dma			dma;
	int			s;

	unit = minor (tp->t_dev);
	line = unit & (SIO_LINES-1);
	smp  = (sio_mem *) tp->t_addr;

	/*
	 * Must hold interrupts in the following code
	 * to prevent state of the tp from changing.
	 */
	s = spl5 ();

	/*
	 * If it's currently active or delaying, there's nothing to do.
	 */
	if (tp->t_state & (TIMEOUT | BUSY | TTSTOP))
		goto out;

	/*
	 * If there are sleepers and output has drained
	 * below low water, wake up the sleepers.
	 */
	if ((tp->t_state & ASLEEP) && tp->t_outq.c_cc <= TTLOWAT(tp)) {
		tp->t_state &= ~ASLEEP;
		wakeup ((caddr_t) &tp->t_outq);
	}

	/* Check for work... */
	if (tp->t_outq.c_cc == 0)
		goto out;

	/*
	 * Get number of characters that we can dma out.
	 *	We have to worry about embedded delay characters
	 *	unless we're in raw mode or putting things out literally.
	 */
	if (tp->t_flags & RAW || tp->t_local & LLITOUT)
		nch = ndqb (&tp->t_outq, 0);
	else {
		nch = ndqb (&tp->t_outq, 0200);
		/*
		 * If the first thing on the queue is a delay, process it.
		 */
		if (nch == 0) {
			nch = getc (&tp->t_outq);
			timeout (ttrstrt, (caddr_t) tp, (nch & 0x7f) + 6);
			tp->t_state |= TIMEOUT;
			goto out;
		}
	}

	/*
	 * Set up the dma and fire it off.
	 */
	dmacount[unit] = nch;
	dma.dma_addr   = (caddr_t) tp->t_outq.c_cf;
	dma.dma_count  = nch;
	bytecpy   (&smp->m_dma[line], &dma, sizeof dma);
	sio_mkcmd (smp, C_DMA_GO, line);
	sio_go    (smp);

	tp->t_state |= BUSY;

out:
	(void) splx (s);
}

/*
 * Stop output on a line, e.g., for ^S/^Q or output flush.
 */
sio_stop (tp, flag)
	register struct tty	*tp;
	int			flag;
{
	register sio_mem	*smp;
	register int		line,
				s;

	smp = (sio_mem *) tp->t_addr;

	/*
	 * Block input/output interrupts while messing with state.
	 */
	s = spl5 ();

	if (tp->t_state & BUSY) {
		/*
		 * Device is transmitting; stop output by
		 * issuing a stop dma command for the line.
		 */
		line = minor (tp->t_dev) & (SIO_LINES-1);
		sio_mkcmd (smp, C_DMA_NO, line);
		sio_go (smp);
	}

	(void) splx (s);
}

/*
 * Handle a modem interrupt.
 */
sio_mint (board)
	int	board;
{
	register struct tty	*tp;
	register sio_mem	*smp;
	u_char			linesdone,
				oldmodem;
	register u_char		linebit,
				havecarrier;
	register int		line,
				unit;

	smp = device[board];
	linesdone = smp->m_mdm_lines;

	/*
	 * Scan each line's entry in the completion register
	 * and process those with modem transitions:
	 */
	for ( line = 0, linebit = 1, tp = &sio[board * SIO_LINES]
	    ; line < SIO_LINES
	    ; line++, linebit <<= 1, tp++
	    ) {
		if (linebit & linesdone) {
			/* Record changed state: */
			unit        = board * SIO_LINES + line;
			oldmodem    = modem[unit];
			modem[unit] = smp->m_modem[line];

			/*
			 * Currently we only care about carrier transitions,
			 * not DSR transitions.
			 */
			havecarrier = modem[unit] & M_DCD;
			if (! ((havecarrier ^ oldmodem) & M_DCD))
				continue;

			wakeup ((caddr_t) &tp->t_rawq);

			if (  (tp->t_state & WOPEN) == 0
			   && (tp->t_local & LMDMBUF)
			   ) {
				/*
				 * We have to start/stop output
				 * on carrier transition:
				 */
				if (havecarrier) {
					tp->t_state &= ~TTSTOP;
					ttstart (tp);
				}
				else {
					tp->t_state |= TTSTOP;
					sio_stop (tp, 0);
				}
			}
			else if (havecarrier) {
				tp->t_state |= CARR_ON;
			}
			else {
				if (  (tp->t_state & WOPEN) == 0
				   && (tp->t_local & LNOHANG) == 0
				   ) {
					/* Tell procs: hangup has occured: */
					gsignal (tp->t_pgrp, SIGHUP);
					gsignal (tp->t_pgrp, SIGCONT);

					/* Turn DTR & RTS off on the line: */
					sio_mkcmd (smp, C_MDM_CLR, line);
					smp->m_cmd[0].c_data0 = M_DTR | M_RTS;
					sio_go (smp);

					flushtty (tp, FREAD | FWRITE);
				}
				tp->t_state &= ~CARR_ON;
			}
		}
	}

	/* Clear the interrupt by hitting the lines changed register: */
	if (linesdone)
		smp->m_mdm_lines = 0;
}

/*
 * Arrange for forced SIO board interrupts once a second.
 *	Doing so compensates for the ICU's nasty habit of changing
 *	level-triggered interrupts to edge-triggered interrupts
 *	and then losing edges.
 */
void
sio_goose (board)
	int	board;
{
	interrupt (SIO);
	timeout   (sio_goose, 0, 60);
}

# ifdef notdef
/*
 * At software clock interrupt time or after a reset,
 * empty all the SIO board fifos.
 */
sio_timer ()
{
	register int	board;

	for (board = 0; board < NSIO; board++)
		sio_rint (board);
}
# endif notdef

/*
 * Construct a command for the SIO board.
 *	It is assumed that SIO board interrupts are already locked out.
 *
 *	Only the command skeleton is constructed; if the command requires
 *	the c_data[01] fields they must be initialized elsewhere.
 *
 *	Before writing the command skeleton to the SIO board, this routine
 *	waits for the board to finish accepting the previous command.
 *	(The board indicates that it has accepted a command by zeroing
 *	out the byte containing the c_cmd field.)
 *
 *	The trickery below with unions and structure assignments
 *	is to force the C compiler to generate only byte accesses
 *	to the SIO board.
 */
Static
sio_mkcmd (smp, cmd, line)
	register sio_mem	*smp;
	u_char			cmd;
	int			line;
{
	register sio_cmd	*scp;
	struct sio_cmd_byte0	byte0;
	extern int		nulldev();	

	scp = &(smp->m_cmd[0]);

	byte0.c_cmd  = cmd;
	byte0.c_chan = line;
	byte0.c_int  = 0;

	/* Wait for previous command to clear before writing in new one: */
	while (smp->m_cmd[0].C_un.C_byte)
		nulldev();		/* needed to preserve references */

	scp->C_un.C_bits = byte0;

	scp++;
	byte0.c_cmd      = C_DONE;	/* end command chain */
	scp->C_un.C_bits = byte0;
}


/*
 * Circular buffer containing debugging
 * information relevant to interrupt handling:
 */
typedef struct sio_int_mon {
	u_int	im_index;	/* interrupt # (wraps @ 100K)	*/
	u_char	im_board,
		im_stat,	/* interrupt status		*/
		im_lines,	/* dma lines completed		*/
		im_lines_pend;	/* pending dma lines completed	*/
} sio_int_mon;

# define SIO_MON_DEPTH	20
sio_int_mon	sio_im [SIO_MON_DEPTH];
sio_int_mon	*simp = sio_im;
u_int		sio_mon_index = 0;

Static
sio_monitor (board, stat, lines, lines_pend)
	register int		board;
	u_char			stat,
				lines,
				lines_pend;
{
	register sio_mem	*smp = device[board];

	if (++sio_mon_index >= 100000)
		sio_mon_index = 0;

	/* Record a new entry in the monitoring structure: */
	simp->im_index      = sio_mon_index;
	simp->im_board      = board;
	simp->im_stat       = stat;
	simp->im_lines      = lines;
	simp->im_lines_pend = lines_pend;
	if (++simp >= &sio_im[SIO_MON_DEPTH])
		simp = sio_im;
}

# ifdef FLAVORDUMP	/* begin debugging routines */

/*
 * Print out the monitoring information.
 */
void
sio_dumpmon ()
{
	register sio_int_mon	*sp;

	cprintf ("\nboard\tstatus\tlines\tlines_pend\tint #\n");
	for (sp = simp; sp < &sio_im[SIO_MON_DEPTH]; sp++)
		cprintf  ( "%d\t%x\t%x\t%x\t\t%d\n"
			, sp->im_board
			, sp->im_stat
			, sp->im_lines
			, sp->im_lines_pend
			, sp->im_index
			);
	for (sp = sio_im; sp < simp; sp++)
		cprintf  ( "%d\t%x\t%x\t%x\t\t%d\n"
			, sp->im_board
			, sp->im_stat
			, sp->im_lines
			, sp->im_lines_pend
			, sp->im_index
			);
}

/*
 * Unwedge lines associated with the board given as argument.
 *	This routine is a hack to compensate for interrupts
 *	missed (or never delivered) elsewhere.  It assumes that
 *	sio_monitor() has already been called for the most recent
 *	interrupt.
 */
void
sio_unwedge (board)
	int	board;
{
	register struct tty	*tp;
	register sio_mem	*smp;
	register int		line;
	u_short			dma_cnt;
	int			s;

	smp = device[board];

	s = spl5 ();

	/*
	 * To a first approximation, a line is wedged if its state
	 * is BUSY and its associated dma count out on the SIO
	 * board is 0.  Check for this situation.
	 */
	for (line = 0; line < SIO_LINES; line++) {
		tp = &sio [board * SIO_LINES + line];
		bytecpy ( &dma_cnt
			, &smp->m_dma[line].dma_count
			, sizeof (u_short)
			);
		if (tp->t_state & BUSY)
			cprintf ("sio%d: checking line %d\n", board, line);
		if ((tp->t_state & BUSY) && dma_cnt == 0) {
			/* Got one... */
			cprintf ("sio%d: freeing line %d\n", board, line);
			tp->t_state &= ~(BUSY | FLUSH);
			ndflush (&tp->t_outq, dmacount[minor (tp->t_dev)]);
			sio_start (tp);
		}
		else if (tp->t_state & BUSY) {
			/* Check for match in dma counts: */
			if (dma_cnt == dmacount [minor (tp->t_dev)])
				cprintf  ( "sio%d: apparent lost dma\n"
					, board
					);
			else
				cprintf  ( "sio%d: dma mismatch soft %d hard %d\n"
					, board
					, dmacount [minor (tp->t_dev)]
					, dma_cnt
					);
		}
	}

	(void) splx (s);
}

/*
 * Dump the state of the unit given as argument.
 */
void
sio_dump (unit)
	register int	unit;
{
	register struct tty	*tp;
	register int		s,
				line;
	register sio_mem	*smp;

	s    = spl7 ();
	tp   = &sio [unit];
	line = unit & (SIO_LINES-1);
	smp  = device [unit / SIO_LINES];
	cprintf  ( "unit %d flags %o state %o modem %o outqcc %d rawqcc %d canqcc %d\n"
		, unit
		, tp->t_flags
		, tp->t_state
		, smp->m_modem[line]
		, tp->t_outq.c_cc
		, tp->t_rawq.c_cc
		, tp->t_canq.c_cc
		);

	(void) splx (s);
}

# endif FLAVORDUMP
# endif NSIO
