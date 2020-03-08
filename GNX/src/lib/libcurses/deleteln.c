/*
 * deleteln.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)deleteln.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine deletes a line from the screen.  It leaves
 *	(_cury,_curx) unchanged.
 */
wdeleteln(win)
reg WINDOW	*win; {

	reg char	*temp;
	reg int		y;
	reg char	*end;

	temp = win->_y[win->_cury];
	for (y = win->_cury; y < win->_maxy; y++) {
		win->_y[y] = win->_y[y+1];
		win->_firstch[y] = 0;
		win->_lastch[y] = win->_maxx - 1;
	}
	for (end = &temp[win->_maxx]; temp < end; )
		*temp++ = ' ';
	win->_y[win->_maxy-1] = temp - win->_maxx;
}
