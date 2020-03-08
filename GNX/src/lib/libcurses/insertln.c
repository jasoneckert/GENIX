/*
 * insertln.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)insertln.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine performs an insert-line on the window, leaving
 * (_cury,_curx) unchanged.
 */
winsertln(win)
reg WINDOW	*win; {

	reg char	*temp;
	reg int		y;
	reg char	*end;

	temp = win->_y[win->_maxy-1];
	win->_firstch[win->_cury] = 0;
	win->_lastch[win->_cury] = win->_maxx - 1;
	for (y = win->_maxy - 1; y > win->_cury; --y) {
		win->_y[y] = win->_y[y-1];
		win->_firstch[y] = 0;
		win->_lastch[y] = win->_maxx - 1;
	}
	for (end = &temp[win->_maxx]; temp < end; )
		*temp++ = ' ';
	win->_y[win->_cury] = temp - win->_maxx;
	if (win->_cury == LINES - 1 && win->_y[LINES-1][COLS-1] != ' ')
		if (win->_scroll) {
			wrefresh(win);
			scroll(win);
			win->_cury--;
		}
		else
			return ERR;
	return OK;
}
