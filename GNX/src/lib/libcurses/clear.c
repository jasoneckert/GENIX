/*
 * clear.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)clear.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine clears the window.
 */
wclear(win)
reg WINDOW	*win; {

	if (win == curscr) {
# ifdef DEBUG
		fprintf(outf,"WCLEAR: win == curscr\n");
		fprintf(outf,"WCLEAR: curscr = %d\n",curscr);
		fprintf(outf,"WCLEAR: stdscr = %d\n",stdscr);
# endif
		clear();
		return refresh();
	}
	werase(win);
	win->_clear = TRUE;
	return OK;
}
