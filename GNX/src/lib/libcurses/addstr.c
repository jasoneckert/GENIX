/*
 * addstr.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)addstr.c	1.1 (NSC) 9/1/82";
# endif

# include	"curses.ext"

/*
 *	This routine adds a string starting at (_cury,_curx)
 */
waddstr(win,str)
reg WINDOW	*win; 
reg char	*str;
{
# ifdef DEBUG
	fprintf(outf, "WADDSTR(\"%s\")\n", str);
# endif
	while (*str)
		if (waddch(win, *str++) == ERR)
			return ERR;
	return OK;
}
