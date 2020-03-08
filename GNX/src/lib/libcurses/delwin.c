/*
 * delwin.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)delwin.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine deletes a window and releases it back to the system.
 */
delwin(win)
reg WINDOW	*win; {

	reg int	i;

	if (!(win->_flags & _SUBWIN))
		for (i = 0; i < win->_maxy && win->_y[i]; i++)
			cfree(win->_y[i]);
	cfree(win->_y);
	cfree(win);
}
