/*
 * getch.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getch.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine reads in a character from the window.
 */
wgetch(win)
reg WINDOW	*win; {

	reg bool	weset = FALSE;
	reg char	inp;

	if (!win->_scroll && (win->_flags&_FULLWIN)
	    && win->_curx == win->_maxx && win->_cury == win->_maxy)
		return ERR;
# ifdef DEBUG
	fprintf(outf, "WGETCH: _echoit = %c, _rawmode = %c\n", _echoit ? 'T' : 'F', _rawmode ? 'T' : 'F');
# endif
	if (_echoit && !_rawmode) {
		raw();
		weset++;
	}
	inp = getchar();
# ifdef DEBUG
	fprintf(outf,"WGETCH got '%s'\n",unctrl(inp));
# endif
	if (_echoit) {
		mvwaddch(curscr, win->_begy+win->_cury, win->_begx+win->_curx, inp);
		waddch(win, inp);
	}
	if (weset)
		noraw();
	return inp;
}
