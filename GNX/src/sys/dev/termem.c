/*
 * termem.c: version 3.10 of 6/30/83
 * Mesa Unix Device Driver
 * written by Glenn C. Skinner
 */
# ifdef SCCS
static char *sccsid = "@(#)termem.c	3.10 (NSC) 6/30/83";
# endif

/*
 * Emulate an ASCII terminal by supporting
 * some of the ANSI 3.64 escape sequences.
 *
 *	We are called from the console driver whenever characters
 *	are to be output to the display.  We filter this character
 *	stream, looking for escape sequences and interpreting them,
 *	making use of lower-level routines to actually paint characters
 *	on the display, to set the current x-y and cursor coordinates,
 *	and so on.
 *
 *	To interpret escape sequences, we must maintain the internal
 *	state of a fsm.  As (for now, at least) there is a single,
 *	global copy of this state, we are not reentrant.
 */

# if NBM > 0

# include "../h/vfont.h"
# include "../h/blt.h"
# include "../h/devvm.h"	/* for graphics board definitions	*/
# include "../h/param.h"	/* tty.h needs it...			*/
# include "../h/tty.h"		/* used in do_newline			*/

# define NUL	 000
# define BEL	 007
# define ESC	 033
# define DEL	0177

# define TABSIZE	8	/* Must be power of 2 */

# define GB_XINC	1024	/* Number of pixels from one col to the next */

# ifdef DEBUG
# define Static
# else
# define Static static
# endif

/*
 * Global state types and variables of the escape sequence interpreter:
 */

typedef enum WriteState {
	Normal,
	EscapeStart,
	CheckPrivate,
	NeedPlist,
	Scrolling
} WriteState;

Static WriteState	write_state = Normal;	/* current fsm state	*/

# define	NUM_PARMS 5	/* max # of esc seq parms we record	*/
Static short	pn [NUM_PARMS],	/* holds escape sequence parameters	*/
		*pnp = pn;	/* pointer to current param slot	*/
Static short	pnindex;	/* records # of parms seen so far	*/
# define 	pnLIMIT	(&pn[NUM_PARMS])

Static char	privateseq;	/* boolean: `private' escape seq	*/
extern	int	bmconsf;	/* true if console is the bitmap	*/

/*
 * Display state variables:
 */

/*
 * Screen coordinates:
 *	The home position is (1,1), in the upper left corner.
 *	Rmax and Cmax hold the coordinates of the lower right corner.
 *
 *	Ch and Cw hold character height and width.  Note that we
 *	assume fixed-width fonts in speaking of Cw.  Cw includes room
 *	for inter-character spacing; that is, it is really one pixel
 *	wider than the (fixed) character width.
 *
 *	Rl and Cl contain `leading': the amount in pixels that the lower
 *	left character position is displaced from the absolute lower left
 *	corner (in the pixel coordinate system this corner is more
 *	convenient than the upper left).  They are set to compensate for
 *	overall screen size not being an integral multiple of character
 *	size.
 *
 *	Rs and Cs contain row and column spacing amounts: the amount in
 *	pixels between the end of one character and the start of the next.
 *	Characters can be considered to occupy boxes with margins around
 *	the edges that are half of the spacing amounts wide.
 *
 *	Rcur and Ccur contain the current cursor location, in character
 *	coordinates, whereas Xcur and Ycur contain the cursor location
 *	in pixel coordinates.  The (Xcur,Ycur) pair is, in general, more
 *	accurate, since the cursor is not constrained to appear on
 *	character boundaries.
 */
Static
short	Rmax, Cmax,
	Rl, Cl,
	Rs, Cs,
	Cw, Ch,
	Rcur, Ccur,
	Xcur, Ycur;

/*
 * Macros to convert coordinates in the row-column coordinate system
 * to coordinates in the pixel coordinate system and vice versa:
 */
# define ctox(c)	(Cw*((c)-1)+Cl)		/* exact */
# define rtoy(r)	(Ch*(Rmax-(r))+Rl)	/* exact */
# define xtoc(x)	(((x)-Cl)/Cw+1)		/* inexact: x unconstrained */
# define ytor(y)	(Rmax-((y)-Rl)/Ch)	/* exact: y constrained */

/*
 * The contents of the current line:
 *	We use this array for backspace processing in the presence
 *	of variable width fonts.  By recording what's in the line so far,
 *	we can correctly back over characters.  This scheme fails when
 *	a cursor addressing sequence is received; we are not guaranteed
 *	to land on a character boundary, and even if we were, we would
 *	have to retain the complete screen image to be able to handle
 *	further backspacing.
 */
# define	CLINESZ	128
Static char	curline [CLINESZ],
		*clp = curline;
# define	clLIMIT	(&curline[CLINESZ])

/*
 * Character and display rendition state:
 *	Rendition contains the current character `face'.  We currently
 *	don't support bold or blinking characters.  Video records
 *	whether the screen is normal or reverse video (0 <==> normal).
 */
Static unsigned char	rendition;
# define R_RVIDEO	001
# define R_UNDER	002
# define R_BOLD		004
# define R_BLINK	010
Static unsigned char	video;

/*
 * Terminal mode information:
 * 	Insert_mode is a bool specifying whether or not the emulation
 *	is in character inster mode.  When true, each graphic character
 *	is preceded with insert_char (1) before being painted on the display;
 *	that is, conceptually, each graphic character is preceded by the
 *	insert-character escape sequence.
 */
Static char	insert_mode;

/*
 * Scrolling information:
 *	Scrollgrain records the number of pixels moved in each scrolling
 *	cycle.  Reasonable values for it are min(Ch,16), to scoll entire
 *	lines at a time (or as close as the hardware lets us come to it),
 *	and S_SCROLL_AMT to get a smooth scrolling effect.  Scrollleft is
 *	state information used during scrolling; it records how many pixels
 *	of scrolling remain to be done.  S_SCROLL_AMT is the number of pixels
 *	to scroll in each `smooth' scrolling cycle.  Reasonable values are
 *	2 or 3.
 */
Static unsigned char	scrollgrain;
Static char		scrollleft;
# define		S_SCROLL_AMT	3

/*
 * Font information:
 *	We preload one font; by default it is Typerite_10.  Fonts
 *	consist of a font_header followed by an array of character
 *	descriptions, followed finally by the character bitmaps.
 *	Finfo points to the beginning of the character description array,
 *	biased backward by first_ch entries, so that it can be indexed
 *	directly by characters.  Before indexing, the char to be used
 *	as index should first be range checked, as fonts are not
 *	guaranteed to contain all 256 8-bit characters.  Fontbase
 *	contains the bitmap starting address.
 *
 *	The values of finfo and fontbase are calculated
 *	respectively in init_termem() and paintinit().
 */
# ifndef FONT
# define FONT Typerite_10
# endif
extern font_header	FONT;
Static chardes		*finfo;
Static char		*fontbase;

/*
 * Character indices for special characters used in initialization:
 *	For fixed-width fonts, the BIGBLOB character consists of white
 *	space large enough to accomodate other characters within, along
 *	with inter-character spacing.
 */
# define BLOB		0200	/* max char height x max char width	*/
# define BIGBLOB	0201	/* BLOB plus inter-character spacing	*/
# define UNDERLINE	0202	/* char for building underlined chars	*/

/*
 * Form descriptors used by paintchar:
 */
Static form 	dform,		/* Display form		*/
		cform;		/* Character form	*/

/*
 * Assembly language routines and variables supplied by blt.s:
 */
extern	ginit       (/* unsigned, int */),
	scroll      (/* int */),
	clearscreen (/* unsigned */),
	setvideo    (/* int */),
	beep        (/* int, unsigned */),
	blt         (/* bltops, form *, form * */),
	cb_blt	    (/* bltops, form *, form * */);
	clearbox    (/* bltops, unsigned, form * */);
extern char	*vram;	/* Address of current page of video ram	*/


/*
 * Terminal emulation initialization.
 *	Set up initial values of the state variables.
 */
init_termem ()
{
	register chardes	*cdp;

	/*
	 * Compute font character info start address.
	 *	To speed indexing the finfo array later,
	 *	we pre-bias by FONT.first_ch here.
	 */
	finfo = (chardes *)((char *) &FONT + sizeof (font_header))
		- FONT.first_ch;

	/*
	 * Initialize the screen to normal video and dark background:
	 */ 	
	video = 0;
	ginit (0, video);

	paintinit ();

	/*
	 * By convention, the BLOB character is depicted as a blob
	 * that fills the full character extent.  The BIGBLOB character
	 * is a larger version of BLOB that adds in room for inter-character
	 * spacing.  Thus we can read off Ch and Cw directly from it.
	 * BIGBLOB's up and down values sum to the overall character height;
	 * the down value gives the distance above the bottom of the overall
	 * character box of the nominal character baseline.  This information
	 * is used in paintchar to center characters properly in the background
	 * obtained by painting a copy of BIGBLOB.
	 */
	cdp = &finfo[BIGBLOB];
	Ch  = cdp->up   + cdp->down;
	Cw  = cdp->left + cdp->right;

	cdp = &finfo[BLOB];
	Cs = Cw - (cdp->left + cdp->right);
	Rs = Ch - (cdp->up   + cdp->down);

	/* Using XMAX-1 and YMAX-1 forces >= 1 pixel of border. */
	Cmax = (XMAX-1) / Cw;
	Rmax = (YMAX-1) / Ch;

	Cl =  (XMAX+1 - Cmax*Cw) >> 1;
	Rl = ((YMAX+1 - Rmax*Ch) >> 1) + (YMAX+1 - Rmax*Ch) % 2;

	rendition   = 0;			/* Normal characters	*/
	scrollgrain = (Ch > 16) ? 16 : Ch;	/* Jump scroll		*/

	cursorinit ();
	goto_rc    (1, 1);
}

/*
 * The escape sequence interpreter.
 *	For the moment we assume we get one character at a time.
 *	Eventually, we probably want it to gobble up an entire
 *	clist block.
 *
 *	As written, the routine pretty much assumes a constant width font.
 *
 *	Bmoutputchar refuses to output nulls, instead using them to push
 *	its internal state through Scrolling and back to Normal.  Thus
 *	it expects to be called first with a null argument after
 *	the graphics board interrupts with scrolling complete and
 *	then with any other pending characters to be output.
 *
 *	There is one additional complication.  When a character line wraps
 *	and forces a scroll, we can't put it out immediately, but must wait
 *	until the scroll completes.  We use the static variable wrapped_char
 *	to record such characters and insert special case checks to handle
 *	wrap completion.  This solution is incredibly gross.  All of the code
 *	for handling scrolling should be rethought.
 */
bmoutputchar (c)
	register char	c;
{
	static char	wrapped_char;	/* probably should be external */

	/* Check for nulls, while still allowing the state to advance: */
	if (c == NUL && write_state != Scrolling)
		return;

	cursoroff ();

retry_wrapped:

	switch (write_state) {
	case Normal:	/* No escape sequence in progress. */
		/* Handle the character: */
		switch (c) {

		case BEL:
			beep (2000, 0x0000002fL); /* Arbitrary values */
			break;

		case '\b':
			/*
			 * If we've recorded contents in this line,
			 * back over them; otherwise punt.  (Note no
			 * range check required, since it was done
			 * previously when clp-1 was set.)
			 */
			if (clp > curline) {
				register chardes *cp = &finfo[*--clp];
				Xcur -= cp->left + cp->right + Cs;
				Ccur  = xtoc (Xcur);
			}
			else
				goto_rc (Rcur, Ccur-1);
			break;

		case '\t':
			/* These tabs are very fat for variable width fonts. */
			goto_rc (Rcur, ((Ccur+TABSIZE-1) & ~(TABSIZE-1)) + 1);
			break;

		case '\n':
			do_newline ();
			break;

		case '\r':
			goto_rc (Rcur, 1);
			break;

		case ESC:
			write_state = EscapeStart;
			break;

		default:	/* An unexceptional character. */
			/* Verify that we have a graphic for the character: */
			if (c < FONT.first_ch || c > FONT.last_ch)
				break;
			/* Check for line wrapping: */
			if (  Ccur > Cmax
			   || Xcur + finfo[c].left + finfo[c].right + Cs/2 > XMAX
			   ) {
				/*
				 * First move to the left margin,
				 * then pretend we got a newline.
				 */
				goto_rc    (Rcur, 1);
				do_newline ();
				/*
				 * If the newline forced a scroll, we must
				 * wait until scrolling is complete to put
				 * out the character...
				 */
				if (write_state == Scrolling) {
					wrapped_char = c;
					break;
				}
			}
			/* If in insert mode, the insertion happens now. */
			if (insert_mode)
				insert_char (1);
			paintchar (c);
			/* If this fails, backspacing will get confused... */
			if (clp < clLIMIT)
				*clp++ = c;
			Ccur = xtoc (Xcur);
			break;
		}
		break;

	case EscapeStart:	/* Have seen initial ESC of escape sequence. */
		/*
		 * RE for escape sequences:
		 *	ESC {'[' {'?'} (digit (digit ';')*)} letter
		 * See whether a parameter list follows.
		 */
		if (c == '[') {
			pnindex     = 0;
			pnp         = pn;
			*pn         = 0;
			write_state = CheckPrivate;
		}
		else {
			/* Simple escape sequence. */
			/* At the moment we don't handle any of this form. */
			write_state = Normal;
		}
		break;

	case CheckPrivate:	/* Check for `vendor private' esc seq. */
		write_state = NeedPlist;
		if (c == '?') {
			privateseq = 1;
			break;
		}
		privateseq = 0;
		/* Fall through to... */

	case NeedPlist:	/* In middle of escape sequence. */
		if (c <= '9' && c >= '0') {
			if (pnp < pnLIMIT)
				*pnp = 10 * *pnp + (c - '0');
		}
		else if (c == ';') {
			/* Parameter separator. */
			pnindex++;
			if (++pnp < pnLIMIT)
				*pnp = 0;
		}
		else {
			/* Have every param supplied; fill out the list. */
			pnindex++;
			while (++pnp < pnLIMIT)
				*pnp = 0;
			/* Interpret the sequence: */
			switch (c) {

			case '@':
				insert_char (pn[0]);
				break;
			case 'A':
				goto_rc (Rcur - (pn[0] ? pn[0] : 1), Ccur);
				break;
			case 'B':
				goto_rc (Rcur + (pn[0] ? pn[0] : 1), Ccur);
				break;
			case 'C':
				goto_rc (Rcur, Ccur + (pn[0] ? pn[0] : 1));
				break;
			case 'D':
				goto_rc (Rcur, Ccur - (pn[0] ? pn[0] : 1));
				break;
			case 'H':
				goto_rc (pn[0], pn[1]);
			 	break;
			case 'J':
				erase_display (pn[0]);
				break;
			case 'K':
				erase_line (pn[0]);
				break;
			case 'L':
				insert_line (pn[0]);
				break;
			case 'M':
				delete_line (pn[0]);
				break;
			case 'P':
				delete_char (pn[0]);
				break;
			case 'c':
				if (privateseq)
					newcursor (pn, pnindex);
				break;
			case 'h':
				set_reset (pn[0], privateseq, 1);
				break;
			case 'l':
				set_reset (pn[0], privateseq, 0);
				break;
			case 'm':
				set_rendition (pn, pnindex);
				break;
			default:
				break;
			}
 			write_state = Normal;
		}
		break;

	case Scrolling:
		/*
		 * The plot so far:
		 *	To handle a newline (or line wrapping), we initiated
		 *	a hardware scrolling operation, which has now
		 *	completed.  There remains some cleaning up to do.
		 * Note that whatever character we were called with here is
		 * ignored.  Thus it is best to call us with null as argument
		 * immediately after getting a scroll completion interrupt,
		 * as nulls are ignored anyway.
		 */
		do_newline ();
		if (write_state == Normal && wrapped_char) {
			/*
			 * We finally have a chance to complete the line wrap!
			 */
			c = wrapped_char;
			wrapped_char = 0;
			goto retry_wrapped;
		}
		break;
	}

	/*
	 * Reset the cursor.
	 *	If line wrapping is about to occur,
	 *	keep it at the last line position.
	 */
	cursorxy ((Xcur > ctox(Cmax)) ? ctox(Cmax) : Xcur, Ycur);
	cursoron  ();
}

/*
 * Move to the next line on the display.
 *	If scrolling is required, we separate the operation
 *	into multiple parts:
 *	1)	Initiate a scrolling cycle and record that we're in the
 *		middle of a scroll, so that our caller doesn't try
 *		to pump more characters at us until the scroll is done.
 *		Repeat this step until we've scrolled the entire amount
 *		required.  The amount scrolled at each step is
 *		determined by whether or not we're in `smooth' scroll
 *		mode and by Ch.
 *	2)	Clean up after the scroll is complete (by zapping any
 *		junk there may be sitting at the top of the display)
 *		and reset the state we've recorded.
 *	This scheme is ugly, but is more or less forced by the state
 *	machine we have for terminal emulation.  Sigh...
 */
Static
do_newline ()
{
	register int		topleading;
	form			topbox;
	register rectangle	*rp;
	extern struct tty	bitmap;

	if (write_state != Scrolling) {
		/* If on the bottom line, we must initiate scrolling. */
		if (Rcur >= Rmax) {
			write_state = Scrolling;
			scrollleft  = Ch;
			scrollchunk (&scrollleft);
		}
		else
			goto_rc (Rcur+1, Ccur);
	}
	else {
		if (scrollleft > 0) {
			scrollchunk (&scrollleft);
			return;
		}
		/*
		 * We're at the end of (an overall) scrolling operation.
		 * The hardware scroll part has completed; we must now finish
		 * up by zapping out any junk remaining from the former
		 * top line.  Note that (Rcur, Ccur) is already correct
		 * (somewhere on the bottom line of the display).
		 */
		topleading  = YMAX+1 - Rmax*Ch - Rl;
		topbox.xinc = GB_XINC;
		topbox.base = vram;
		rp          = &topbox.r;
		rp->x       = 0;
		rp->y       = rtoy (1) + Ch;
		rp->bw      = XMAX;
		rp->bh      = topleading;
		clearbox (mov, 0, &topbox);

		write_state = Normal;
	}
}

/*
 * Scroll the lesser of *ap, the maximum scrolling amount allowed
 * by the hardware, and scrollgrain, updating *ap by the amount scrolled.
 */
scrollchunk (ap)
	register unsigned	*ap;
{
	register unsigned	samt;

	if (*ap <= 0)
		return;

	samt  = (*ap >= 16)           ? 16   : *ap;
	samt  = (samt <= scrollgrain) ? samt : scrollgrain;
	*ap  -= samt;
	scroll (samt);

	/* Let the bitmap driver routines know we're busy: */
	bitmap.t_state |= BUSY;
}

/*
 *
 * Routines implementing various escape sequences.
 *
 */

/*
 * Position the cursor at (r,c).
 *	The upper left corner of the display is at (1,1),
 *	and coordinates are in character units.  Arguments
 *	out of range are replaced with the closest in-range values.
 *
 *	Note that this routine sets the global variables Rcur and Ccur
 *	and Xcur and Ycur.
 */
Static
goto_rc (r, c)
	register int	r, c;
{
	/* Check boundary conditions: */
	if (r <= 0)
		Rcur = 1;
	else if (r > Rmax)
		Rcur = Rmax;
	else
		Rcur = r;
	if (c <= 0)
		Ccur = 1;
	else if (c > Cmax)
		Ccur = Cmax;
	else
		Ccur = c;

	/* Throw out accumulated line information: */
	clp = curline;

	/* Translate to the pixel coordinate system & move: */
	Xcur = ctox (Ccur);
	Ycur = rtoy (Rcur);
}

/*
 * Emulate the ANSI erase in line escape sequence.
 *	Code determines how much to zap:
 *		0: cursor to end of line
 *		1: beginning of line to cursor
 *		2: entire line contents
 *	The routine thinks in terms of fixed width fonts and thus clears
 *	to column boundaries.  Partial characters will remain on the display
 *	if invoked with variable width fonts in effect.
 */
Static
erase_line (code)
	register int	code;
{
	register rectangle	*rp;
	form			box;

	box.xinc = GB_XINC;
	box.base = vram;
	rp       = &box.r;
	rp->y    = rtoy (Rcur);
	rp->bh   = Ch;

	if (code == 1 || code == 2) {
		rp->x  = ctox (1);
		rp->bw = Cw * Ccur;
		clearbox (mov, 0, &box);
	}
	if (code == 0 || code == 2) {
		rp->x  = ctox (Ccur);
		rp->bw = Cw * (Cmax - Ccur + 1);
		clearbox (mov, 0, &box);
	}

	/* Give up on keeping track of what the line has in it: */
	clp = curline;
}

/*
 * Emulate the ANSI erase in display escape sequence.
 *	Code determines how much to zap:
 *		0: cursor to end of screen
 *		1: beginning of screen to cursor
 *		2: entire screen contents
 */
Static
erase_display (code)
	register int	code;
{
	form		zaplines;
	register form	*zlp;

	/* Check for special cases that we can optimize: */
	if (code == 2 || (code == 0 && Xcur == ctox(1) && Ycur == rtoy(1))) {
		clearscreen (0);
		return;
	}

	/*
	 * Set up common parts of the form describing the block
	 * of complete lines above or below Rcur to be cleared:
	 */
	zlp       = &zaplines;
	zlp->base = vram;
	zlp->xinc = GB_XINC;
	zlp->r.x  = ctox (1);
	zlp->r.bw = Cw * Cmax;

	/*
	 * Zap out the left or right end of the line containing
	 * the cursor and finish setting up zaplines:
	 */
	if (code == 1) {
		erase_line (1);
		zlp->r.y  = rtoy (Rcur - 1);
		zlp->r.bh = Ch * (Rcur - 1);
	}
	if (code == 0) {
		erase_line (0);
		zlp->r.y  = rtoy (Rmax);
		zlp->r.bh = Ch * (Rmax - Rcur);
	}

	/* Blast out the remaining rectangle: */
	clearbox (mov, 0, zlp);
}

/*
 * Insert num blank lines at the current cursor position.
 *	The (former) current line moves down to lie immediately under
 *	the newly created lines.  The cursor moves to the left end of
 *	the first newly created line.  Lines forced off the bottom of
 *	the display are lost.
 */
Static
insert_line (num)
	register int	num;
{
	form		before,
			after;
	register form	*fp;
	register int	width,
			height,
			openbase;

	/* An omitted parameter to the escape sequence defaults to 1: */
	if (num <= 0)
		num = 1;

	/* Restrict the amount to no more lines than remain from Rcur: */
	if (num > Rmax - Rcur + 1)
		num = Rmax - Rcur + 1;

	width     = Cw * Cmax;
	openbase  = Rcur + num - 1;	/* base row of newly opened area */

	/* Set up invariant portions of the before form: */
	fp       = &before;
	fp->base = vram;
	fp->xinc = GB_XINC;
	fp->r.x  = ctox (1);
	fp->r.bw = width;

	/*
	 * If some of the current display contents will remain below
	 * the newly opened area, slide the surviving contents down
	 * by setting up before and after forms and then invoking copybox.
	 */
	if (Rmax - num >= Rcur) {
		fp->r.y  = rtoy (Rmax - num);
		fp->r.bh = height
			 = Ch * (Rmax - openbase + 1);

		fp       = &after;
		fp->base = vram;
		fp->xinc = GB_XINC;
		fp->r.x  = ctox (1);
		fp->r.y  = rtoy (Rmax);
		fp->r.bh = height;
		fp->r.bw = width;

		copybox (&before, fp);
	}

	/* Clear out the newly-opened area: */
	fp       = &before;
	fp->r.y  = rtoy ((openbase > Rmax) ? Rmax : openbase);
	fp->r.bh = Ch * ((openbase > Rmax) ? Rmax - Rcur + 1 : num);
	clearbox (mov, 0, fp);

	goto_rc (Rcur, 1);
	/* We no longer know anything about line contents: */
	clp = curline;
}

/*
 * Delete num lines starting from the line containing the cursor.
 *	The cursor remains in its original position.  Surviving lines
 *	move up to fill the gap created.  Blank lines are introduced at
 *	the bottom of the display.
 */
Static
delete_line (num)
	register int	num;
{
	form		before,
			after;
	register form	*fp;
	register int	width,
			height,
			lines_rem;

	/* An omitted parameter to the escape sequence defaults to 1: */
	if (num <= 0)
		num = 1;

	/* Restrict the amount to no more lines than remain from Rcur: */
	lines_rem = Rmax - Rcur + 1;
	if (num > lines_rem)
		num = lines_rem;

	width = Cw * Cmax;

	/* Set up invariant portions of the before form: */
	fp       = &before;
	fp->base = vram;
	fp->xinc = GB_XINC;
	fp->r.x  = ctox (1);
	fp->r.bw = width;
	fp->r.y  = rtoy (Rmax);

	/*
	 * If any lines will remain below those deleted,
	 * slide them up to start at the (original) cursor line,
	 * by setting up before and after forms and then invoking copybox.
	 */
	if (num < lines_rem) {
		fp->r.bh = height
			 = Ch * (lines_rem - num);

		fp       = &after;
		fp->base = vram;
		fp->xinc = GB_XINC;
		fp->r.x  = before.r.x;
		fp->r.bw = width;
		fp->r.y  = rtoy (Rmax - num);
		fp->r.bh = height;

		copybox (&before, fp);
	}

	/* Clear out lines opened at the bottom: */
	height   = Ch * num;
	fp       = &before;
	fp->r.bh = height;
	clearbox (mov, 0, fp);

	/* Don't pretend to know about this line's contents: */
	clp = curline;
}

/*
 * Insert num blank characters at the current cursor position.
 *	The cursor remains in its original position.  Characters from
 *	the cursor position rightward shift right to create room for the
 *	blanks.  Characters forced off the right end of the line are lost.
 *
 *	Note: this routine makes no attempt at diligence in maintaining
 *	curline.
 */
Static
insert_char (num)
	register int	num;
{
	form		before,
			after;
	register form	*fp;
	register int	width,
			height,
			chars_rem;

	/* Restrict num to reasonable values: */
	if (num <= 0)
		num = 1;

	chars_rem = Cmax - Ccur + 1;
	if (num > chars_rem)
		num = chars_rem;

	height = Ch;

	/* Set up invariant portions of the before form: */
	fp       = &before;
	fp->base = vram;
	fp->xinc = GB_XINC;
	fp->r.y  = rtoy (Rcur);
	fp->r.bh = height;
	fp->r.x  = ctox (Ccur);

	/*
	 * If there will remain characters shifted right that don't
	 * fall off the end, slide them over, by setting up a suitable
	 * call of copybox.
	 */
	if (Ccur + num <= Cmax) {
		fp->r.bw = width
			 = Cw * (chars_rem - num);

		fp       = &after;
		fp->base = vram;
		fp->xinc = GB_XINC;
		fp->r.y  = before.r.y;
		fp->r.bh = height;
		fp->r.x  = ctox (Ccur + num);
		fp->r.bw = width;

		copybox (&before, fp);
	}

	/* Clear out the insertion area: */
	fp       = &before;
	fp->r.bw = Cw * num;
	clearbox (mov, 0, fp);

	/*
	 * Adjust clp, being lazy, so that we discard any previous
 	 * knowledge of line contents to the right of Ccur:
	 */
	if (clp > &curline[Ccur - 1 ])
		clp = &curline[Ccur - 1];
}

/*
 * Delete num characters at the current cursor position.
 *	The cursor remains in its original position.  Characters to the
 *	right of the gap move left to close the gap.  The line is cleared
 *	at its new right end (i.e., characters moving left don't leave
 *	ghosts behind).
 *
 *	Note: this routine makes no attempt to keep curline as accurate
 *	as it might; instead it is lazy about it and elides memory of line
 *	contents past the point of deletion.
 */
Static
delete_char (num)
	register int	num;
{
	form		before,
			after;
	register form	*fp;
	register int	width,
			height,
			chars_rem;

	/* Restrict num to reasonable values: */
	if (num <= 0)
		num = 1;

	chars_rem = Cmax - Ccur + 1;
	if (num > chars_rem)
		num = chars_rem;

	height = Ch;

	/* Set up invariant portions of the before form: */
	fp       = &before;
	fp->base = vram;
	fp->xinc = GB_XINC;
	fp->r.y  = rtoy (Rcur);
	fp->r.bh = height;

	/*
	 * If any characters will (potentially: disregarding curline)
	 * remain to the right of those deleted, slide them to the left
	 * to fill the gap, by setting up a suitable call of copybox.
	 */
	if (Ccur + num <= Cmax) {
		fp->r.x  = ctox (Ccur + num);
		fp->r.bw = width
			 = Cw * (chars_rem - num);

		fp       = &after;
		fp->base = vram;
		fp->xinc = GB_XINC;
		fp->r.y  = before.r.y;
		fp->r.bh = height;
		fp->r.x  = ctox (Ccur);
		fp->r.bw = width;

		copybox (&before, fp);
	}

	/*
	 * Clear out the columns at the right that characters
	 * have (potentially) shifted out of:
	 */
	fp       = &before;
	fp->r.x  = ctox (Cmax - num + 1);
	fp->r.bw = Cw * num;
	clearbox (mov, 0, fp);

	/*
	 * Adjust clp, being lazy, so that we discard any previous knowledge
	 * of line contents to the right of Ccur:
	 */
	if (clp > &curline[Ccur - 1])
		clp = &curline[Ccur - 1];
}

/*
 * Set or reset a terminal mode.
 *	Code and private combine to determine exactly which mode is to
 *	be changed; onoff specifies how it is to be changed.
 *
 *	We currently support normal/reverse video, smooth/jump scroll,
 *	and character insert/normal mode.
 */
Static
set_reset (code, private, onoff)
	int	code,
		private,
		onoff;
{
	if (private) {
		switch (code) {
		case 4:
			scrollgrain = (onoff == 1)
					? S_SCROLL_AMT
					: ((Ch > 16) ? 16 : Ch);
			break;
		case 5:
			setvideo (video = onoff);
			break;
		default:
			break;
		}
	}
	else {
		switch (code) {
		case 4:
			insert_mode = onoff;
			break;
		default:
			break;
		}
	}
}

/*
 * Set character rendition.
 *	Attributes contains a list of attributes to apply; atnum contains
 *	its length.  An attribute value of zero causes all previously set
 *	attributes to be cancelled.
 */
Static
set_rendition (attributes, atnum)
	short	attributes [],
		atnum;
{
	register int	a, i;

	for (i = 0; i < atnum; i++) {
		if ((a = attributes[i]) == 0)
			rendition  = 0;
		else {
			rendition |=	(a == 1) ? R_BOLD   :
					(a == 4) ? R_UNDER  :
					(a == 5) ? R_BLINK  :
					(a == 7) ? R_RVIDEO :
					0;
		}
	}
}

/*
 * Switch to a new cursor.
 *	Parms contains information gathered from an escape sequence.
 *	This routine mostly feeds parameters to setcursor, supplying
 *	defaults as appropriate.
 */
Static
newcursor (parms, parmnum)
	short	parms[],
		parmnum;
{
	register short	ci,
			blinkon,
			blinkoff;
	extern int	hz;

	/* Set up cursor index: */
	ci = (parms[0] == 0) ? BLOB : parms[0];
	if (ci < FONT.first_ch || ci > FONT.last_ch)
		ci = BLOB;

	/*
	 * Set up blink rates.
	 *	If one is missing, use hz - (the other) for its value.
	 *	If both are missing, use hz/2 for both.
	 */
	if (parmnum < 2)
		blinkon = blinkoff = hz/2;
	else {
		blinkon  = parms[1];
		blinkoff = (parmnum < 3) ? hz - blinkon : parms[2];
	}

	setcursor (&FONT, ci, blinkon, blinkoff);
}



/*
 * Initialize variables used by paintchar.
 */
Static
paintinit ()
{
	register form	*fp;

	/*
	 * Set up the display form.
	 * 	Base should be reset each time before use,
	 *	to allow for switching between the two screen areas
	 *	in video memory.
	 */
	fp       = &dform;
	fp->xinc = GB_XINC;	/* Bit distance between scan lines	*/
	fp->base = vram;	/* Video ram base addr (current page)	*/
	fp->r.x  = 0;
	fp->r.y  = 0;
	fp->r.bw = XMAX+1;
	fp->r.bh = YMAX+1;

	/* Set up the character form: */
	fp       = &cform;
	fp->xinc = 16;
	fp->base = fontbase
		 = ((char *) &FONT) + sizeof (font_header) +
			(FONT.last_ch - FONT.first_ch + 1) * sizeof (chardes);
	fp->r.x  = 0;		/* The rectangle part is filled in	*/
	fp->r.y  = 0;		/* later with character-specific info.	*/
	fp->r.bw = 0;
	fp->r.bh = 0;
}

/*
 * Do the actual work of painting a character on the display.
 *
 *	Preconditions:
 *		The character will fit on the current line.
 *		The font contains an entry for the character.
 *
 *	We assume that fonts contain an actual graphic
 *	for the space character, so that they need not be
 *	treated specially.
 */
Static
paintchar (c)
	char	c;
{
	register chardes	*cdp, *blp, *ulp;
	register form		*fp;
	register rectangle	*d;
	form			dest, template;
	char			scratch[64];

	blp = &finfo[BIGBLOB];
	cdp = &finfo[c];

	/*
	 * Make sure there is actually something to do:
	 *	Note that we already know that the character will fit.
	 */
	if (cdp->nbytes == 0)
		return;

	/*
	 * Take care of underlined characters.
	 *	The strategy is to build a composite character out of
	 *	UNDERLINE and the argument character in scratch and then
	 *	to proceed normally using this new character.  We build
	 *	the composite character analogously to the way we put
	 *	normal characters on the display.
	 */
	if (rendition & R_UNDER) {
		ulp = &finfo[UNDERLINE];

		/* Set up destination (in scratch): */
		fp       = &dest;
		fp->base = scratch;
		fp->xinc = 16;		/* same as for character area */
		fp->r.x  = 0;
		fp->r.y  = 0;
		fp->r.bh = Ch;
		fp->r.bw = cdp->left + cdp->right + Cs;

		/* Set up underline character: */
		fp       = &template;
		fp->base = fontbase + ulp->offset;
		fp->xinc = 16;
		fp->r.x  = 0;
		fp->r.y  = 0;

		/* Put the underline in place: */
		blt (mov, fp, &dest);

		/* Set up the desired character: */
		fp->base = fontbase + cdp->offset;
		fp       = &dest;
		fp->r.x  = ulp->left - cdp->left;
		fp->r.y  = ulp->down - cdp->down;
		fp->r.bh = cdp->up   + cdp->down;
		fp->r.bw = cdp->left + cdp->right;

		/* Put the character in place: */
		blt (or, &template, fp);

		/*
		 * Now shift pointers to make the composite character
		 * appear in place of the original character.
		 *	(The shift is incomplete: we still have to
		 *	worry about the bitmap base address.)
		 */
		cdp = ulp;
	}

	/*
	 * Set up destination form:
	 *	Use only enough of BIGBLOB's width to cover the char
	 *	along with the inter-character spacing.
	 */
	dest      = dform;
	dest.base = vram;	/* Get current display page addr. */
	d         = &dest.r;
	d->x      = Xcur;
	d->y      = Ycur;
	d->bh     = Ch;
	d->bw     = cdp->left + cdp->right + Cs;

	/*
	 * Set up the character form:
	 */
	template      = cform;
	template.base = (rendition & R_UNDER)
				? scratch
				: fontbase + cdp->offset;
	d             = &template.r;
	d->x          = Xcur + blp->left - cdp->left;
	d->y          = Ycur + blp->down - cdp->down;
	d->bh         = cdp->up   + cdp->down;
	d->bw         = cdp->left + cdp->right;

	/*
	 * Now render the character on the display:
	 */
	cb_blt ((rendition & R_RVIDEO) ? mov : bic, &template, &dest);

	/*
	 * Update Xcur.
	 *	If we are underlining, intercharacter spacing has
	 *	already been taken into account and shouldn't be
	 *	redone here.
	 */
	Xcur += cdp->left + cdp->right;
	if (!(rendition & R_UNDER))
		Xcur += Cs;

	/* Note that it is possible that Xcur > XMAX here. */
}

/*
 * Cursor manipluation:
 *	Since paintcursor simply xor's the cursor onto the current
 *	cursor position, blinking the cursor proceeds by successive
 *	invocations of paintcursor.  When characters are added or
 *	removed at the cursor position, the cursor must first be
 *	turned off; otherwise the xor operation produces junk.
 */

/*
 * Cursor state information:
 *	Cursor_state records the current cursor state: < 0 means
 *	turned off, but active; == 0 means inactive (explicitly disabled);
 *	> 0 means currently turned on.  Cursor_time is meaningful only if
 *	cursor_state is true.  If so, it is used as a counter, being updated
 *	every clock tick.  When -cursor_off_time <= cursor_time < 0, the cursor
 *	is in the off phase of its blink cycle.  When cursor_time == 0,
 *	the cursor is turned on, and while 0 <= cursor_time <
 *	cursor_on_time, it remains on.  When restarting cursor display
 *	after cursor_state has been false, cursor_time is initialized
 *	to -CURSOR_DELAY, allowing us a short interval to change our
 *	minds about whether we really want it on.  (When putting a
 *	rapid succession of characters to the screen, it is best that
 *	the cursor remain off until the end.  This scheme lets us do so.)
 *
 *	CursorShape and CursorDest are forms describing respectively, the
 *	glyph used as cursor and the chunk of the display onto which
 *	it is painted.
 *
 *	Xcursor and Ycursor contain the current cursor coordinates.
 */
Static char	cursor_state;
Static short	cursor_time;
Static short	cursor_on_time,
		cursor_off_time;
# define CURSOR_DELAY		 4

Static form	CursorShape,
		CursorDest;

Static short	Xcursor,
		Ycursor;

Static char	CXcurs,		/* centering offsets to put cursor into	*/
		CYcurs;		/* BIGBLOB sized box properly		*/

/*
 * Turn off the cursor.
 */

cursoroff ()
{
	/* If currently on and actually displayed, zap it out: */
	if (cursor_state > 0)
		paintcursor ();

	cursor_state = 0;
}

/*
 * Turn on the cursor.
 *	There will be a delay of CURSOR_DELAY ticks before
 *	it actually comes on.
 */
cursoron ()
{
	extern struct tty bitmap;

	/* If already on, don't touch it: */
	if (cursor_state > 0)
		return;
	/* If terminal isn't open, don't display the cursor */
	if ((bmconsf == 0) && ((bitmap.t_state&ISOPEN) == 0))
		return;
	cursor_time  = -CURSOR_DELAY;
	cursor_state = -1;
}

/*
 * Do clock interrupt time cursor processing.
 */
update_cursor ()
{
	/* Get out fast unless there's real work to do: */
	if (  cursor_state == 0
	   || ++cursor_time < 0
	   || (cursor_time > 0 && cursor_time < cursor_on_time)
	   )
		return;

	if (cursor_time >= cursor_on_time) {
		/* On --> off transition: */
		if ((cursor_time = -cursor_off_time) < 0 && cursor_state > 0)
			paintcursor ();
	}
	else {
		/* Off --> on transition: */
		if (cursor_state < 0)
			paintcursor ();
	}
}

/*
 * Xor the cursor onto the display at the current cursor coordinates.
 *	It is assumed that CursShape and CursDest have been set up previously.
 */
Static
paintcursor ()
{
	register form	*cdp = &CursorDest;

	cdp->base = vram;
	cdp->r.x  = Xcursor + CXcurs;
	cdp->r.y  = Ycursor + CYcurs;
	blt (xor, &CursorShape, cdp);

	/* Toggle recorded state: */
	cursor_state = -cursor_state;
}

/*
 * Move the cursor's next appearance to pixel coordinates (x,y).
 *	Precondition: the cursor is off.
 *	No range checking is performed.
 */
Static
cursorxy (x, y)
	short x, y;
{
	Xcursor = x;
	Ycursor = y;
}

/*
 * Initialize the cursor data structures.
 *	By default we use BLOB for the cursor's shape.
 *	Most of the setup work actually takes place in setcursor.
 *	Some is deferred until paintcursor, to make sure we are
 *	using up to date values for position, etc.
 */
Static
cursorinit ()
{
	register form		*cdp = &CursorDest;
	extern int		hz;

	/* Set up CursorDest: */
	cdp->xinc = GB_XINC;
/*	cdp->base = vram;	reset before every use...	*/
/*	cdp->r.x  = 0;			"			*/
/*	cdp->r.y  = 0;			"			*/
/*	cdp->r.bw = 0;		set in setcursor		*/
/*	cdp->r.bh = 0;			"			*/

	/* Set up CursorShape: */
	setcursor (&FONT, BLOB, hz/2, hz/2);

	/* The cursor starts out life turned on in the home position: */
	cursorxy (ctox(1), rtoy(1));
	cursor_state = 0;
	cursoron ();
}

/*
 * Set cursor shape and blink duration.
 *	Change the cursor's shape to be that of the character
 *	indexed by ci within font. Set blinking durations to
 *	blinkon and blinkoff.
 */
Static
setcursor (font, ci, blinkon, blinkoff)
	register font_header	*font;
	unsigned char		ci;
	short			blinkon,
				blinkoff;
{
	chardes			*chardesbase;
	register chardes	*cdp;
	register char		*bitmapbase;
	register form		*cp;

	/* Calculate font base addresses: */
	chardesbase = (chardes *) (((char *) font) + sizeof (font_header));
	cdp         = &chardesbase [ci - font->first_ch];

	bitmapbase  = (char *) chardesbase +
		(font->last_ch - font->first_ch + 1) * sizeof (chardes);

	/* Set up CursorShape: */
	cp       = &CursorShape;
	cp->base = bitmapbase + cdp->offset;
	cp->xinc = 16;

	/* Fill in the rest of CursorDest: */
	cp       = &CursorDest;
	cp->r.bw = cdp->left + cdp->right;
	cp->r.bh = cdp->up   + cdp->down;

	/* Calculate centering amounts: */
	CXcurs = finfo[BIGBLOB].left - cdp->left;
	CYcurs = finfo[BIGBLOB].down - cdp->down;

	/* Reset blink times: */
	cursor_on_time  = (blinkon  <= 0) ? 0 : blinkon;
	cursor_off_time = (blinkoff <= 0) ? 0 : blinkoff;
}

# endif NBM
