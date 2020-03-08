/*
 * mvwin.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)mvwin.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 * relocate the starting position of a window
 */

mvwin(win, by, bx)
reg WINDOW	*win;
reg int		by, bx; {

	if (by + win->_maxy > LINES || bx + win->_maxx > COLS)
		return ERR;
	win->_begy = by;
	win->_begx = bx;
	touchwin(win);
	return OK;
}
