/*
 * endwin.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)endwin.c	1.1 (NSC) 9/1/82";
# endif

/*
 * Clean things up before exiting
 */

# include	"curses.ext"

endwin()
{
	resetty();
	_puts(VE);
	_puts(TE);
	if (curscr) {
		if (curscr->_flags & _STANDOUT) {
			_puts(SE);
			curscr->_flags &= ~_STANDOUT;
		}
		_endwin = TRUE;
	}
}
