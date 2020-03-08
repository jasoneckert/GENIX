/*
 * touchwin.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)touchwin.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 * make it look like the whole window has been changed.
 */
touchwin(win)
reg WINDOW	*win;
{
	reg int		y, maxy, maxx;

	maxy = win->_maxy;
	maxx = win->_maxx - 1;
	for (y = 0; y < maxy; y++) {
		win->_firstch[y] = 0;
		win->_lastch[y] = maxx;
	}
}
