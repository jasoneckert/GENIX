/*
 * erase.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)erase.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine erases everything on the window.
 */
werase(win)
reg WINDOW	*win; {

	reg int		y;
	reg char	*sp, *end, *start, *maxx;
	reg int		minx;

# ifdef DEBUG
	fprintf(outf, "WERASE(%0.2o)\n", win);
# endif
	for (y = 0; y < win->_maxy; y++) {
		minx = _NOCHANGE;
		start = win->_y[y];
		end = &start[win->_maxx];
		for (sp = start; sp < end; sp++)
			if (*sp != ' ') {
				maxx = sp;
				if (minx == _NOCHANGE)
					minx = sp - start;
				*sp = ' ';
			}
		if (minx != _NOCHANGE) {
			if (win->_firstch[y] > minx
			     || win->_firstch[y] == _NOCHANGE)
				win->_firstch[y] = minx;
			if (win->_lastch[y] < maxx - win->_y[y])
				win->_lastch[y] = maxx - win->_y[y];
		}
	}
	win->_curx = win->_cury = 0;
}
