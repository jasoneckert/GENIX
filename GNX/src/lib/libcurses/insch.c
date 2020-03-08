/*
 * insch.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)insch.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine performs an insert-char on the line, leaving
 *	(_cury,_curx) unchanged.
 */
winsch(win, c)
reg WINDOW	*win;
char		c; {

	reg char	*temp1, *temp2;
	reg char	*end;

	end = &win->_y[win->_cury][win->_curx];
	temp1 = &win->_y[win->_cury][win->_maxx - 1];
	temp2 = temp1 - 1;
	while (temp1 > end)
		*temp1-- = *temp2--;
	*temp1 = c;
	win->_lastch[win->_cury] = win->_maxx - 1;
	if (win->_firstch[win->_cury] == _NOCHANGE ||
	    win->_firstch[win->_cury] > win->_curx)
		win->_firstch[win->_cury] = win->_curx;
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
