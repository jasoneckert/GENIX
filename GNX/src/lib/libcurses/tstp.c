/*
 * tstp.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)tstp.c	1.1 (NSC) 9/1/82";
# endif

# include	<signal.h>

# ifdef SIGTSTP

# include	"curses.ext"

/*
 * handle stop and start signals
 */
tstp() {

	SGTTY	tty;
# ifdef DEBUG
	if (outf)
		fflush(outf);
# endif
	tty = _tty;
	mvcur(0, COLS - 1, LINES - 1, 0);
	endwin();
	fflush(stdout);
	kill(0, SIGTSTP);
	signal(SIGTSTP, tstp);
	_tty = tty;
	stty(_tty_ch, &_tty);
	wrefresh(curscr);
}
# endif
