/*
 * delch.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)delch.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine performs an insert-char on the line, leaving
 *	(_cury,_curx) unchanged.
 */
wdelch(win)
reg WINDOW	*win; {

	reg char	*temp1, *temp2;
	reg char	*end;

	end = &win->_y[win->_cury][win->_maxx - 1];
	temp2 = &win->_y[win->_cury][win->_curx + 1];
	temp1 = temp2 - 1;
	while (temp1 < end)
		*temp1++ = *temp2++;
	*temp1 = ' ';
	win->_lastch[win->_cury] = win->_maxx - 1;
	if (win->_firstch[win->_cury] == _NOCHANGE ||
	    win->_firstch[win->_cury] > win->_curx)
		win->_firstch[win->_cury] = win->_curx;
	return OK;
}
