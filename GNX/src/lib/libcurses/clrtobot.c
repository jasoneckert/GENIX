/*
 * clrtobot.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)clrtobot.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine erases everything on the window.
 */
wclrtobot(win)
reg WINDOW	*win; {

	reg int		y;
	reg char	*sp, *end, *maxx;
	reg int		startx, minx;

	startx = win->_curx;
	for (y = win->_cury; y < win->_maxy; y++) {
		minx = _NOCHANGE;
		end = &win->_y[y][win->_maxx];
		for (sp = &win->_y[y][startx]; sp < end; sp++)
			if (*sp != ' ') {
				maxx = sp;
				if (minx == _NOCHANGE)
					minx = sp - win->_y[y];
				*sp = ' ';
			}
		if (minx != _NOCHANGE) {
			if (win->_firstch[y] > minx
			     || win->_firstch[y] == _NOCHANGE)
				win->_firstch[y] = minx;
			if (win->_lastch[y] < maxx - win->_y[y])
				win->_lastch[y] = maxx - win->_y[y];
		}
		startx = 0;
	}
	win->_curx = win->_cury = 0;
}
