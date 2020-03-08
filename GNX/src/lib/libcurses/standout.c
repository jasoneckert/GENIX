/*
 * standout.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)standout.c	1.1 (NSC) 9/1/82";
# endif

/*
 * routines dealing with entering and exiting standout mode
 */

# include	"curses.ext"

/*
 * enter standout mode
 */
char *
wstandout(win)
reg WINDOW	*win;
{
	if (!SO && !UC)
		return FALSE;

	win->_flags |= _STANDOUT;
	return (SO ? SO : UC);
}

/*
 * exit standout mode
 */
char *
wstandend(win)
reg WINDOW	*win;
{
	if (!SO && !UC)
		return FALSE;

	win->_flags &= ~_STANDOUT;
	return (SE ? SE : UC);
}
