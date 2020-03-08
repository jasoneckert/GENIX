/*
 * move.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)move.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine moves the cursor to the given point
 */
wmove(win, y, x)
reg WINDOW	*win;
reg int		y, x; {

# ifdef DEBUG
	fprintf(outf, "MOVE to (%d, %d)\n", y, x);
# endif
	if (x >= win->_maxx || y >= win->_maxy)
		return ERR;
	win->_curx = x;
	win->_cury = y;
	return OK;
}
