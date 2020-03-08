
/*
 * scan.c: version 1.2 of 1/24/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)scan.c	1.2 (NSC) 1/24/83";
# endif

/* Module to read self-terminating input while allowing editing of the input */


#include <setjmp.h>		/* read setjmp definitions */
#include <sgtty.h>		/* read stty definitions */
#include "scan.h"		/* read scan definitions */
#include "error.h"

#define	RUBCHAR	('H'-'@')	/* default character rubout character */
#define	RUBWORD ('W'-'@')	/* default word rubout character */
#define	RUBLINE ('U'-'@')	/* default line rubout character */
#define	RETYPECHAR ('R'-'@')	/* retype input line character */
#define	QUOTECHAR ('V'-'@')	/* default character quoting character */
#define	ERASETEXT "\b \b"	/* text for backspacing while clearing */
#define	ERASELEN 3		/* number of characters in text */
#define	STDIN	0		/* standard input channel */
#define	STDOUT	1		/* standard output channel */

jmp_buf	*scanjumpbuf;		/* jump buffer to use in scanchar */
char	scanbuffer[SCAN_SIZE+1];/* storage for characters */
char	*scanreadptr;		/* current read pointer */
char	*scanwriteptr;		/* current write pointer */
char	*scanprompt;		/* current prompt text */
char	scannoecho;		/* true if input is not echoing */
char	scanraw;		/* true if terminal is in raw mode */
char	(*scanroutine)();	/* routine to read characters */
static	char	rubchar;	/* rub out current character character */
static	char	rubword;	/* rub out current word character */
static	char	rubline;	/* rub out current line character */
static	char	retypechar;	/* retyping character */
static	char	quotechar;	/* quoting character */
static	struct	sgttyb sgbuf;	/* data for gtty use */



/* Initialize for scanning characters.  This types the prompt line, and
 * remembers the jump buffer for later calls by scanchar.  The user's
 * setjmp will be called if editing of the input is attempted.
 */
scaninit(routine, prompt, jumpbuf)
	char	(*routine)();		/* routine to get characters */
	char	*prompt;		/* string to prompt input with */
	jmp_buf	*jumpbuf;		/* jump buffer to use later */
{
	if (*prompt) write(STDOUT, prompt, strlen(prompt));
	scanroutine = routine;
	scanprompt = prompt;
	scanjumpbuf = jumpbuf;
	scannoecho = 0;
	scanraw = 0;
	rubchar = RUBCHAR;
	rubword = RUBWORD;
	rubline = RUBLINE;
	retypechar = RETYPECHAR;
	quotechar = QUOTECHAR;
	if (gtty(STDIN, &sgbuf) == 0) {
		rubchar = sgbuf.sg_erase;
		rubline = sgbuf.sg_kill;
		scanraw = (sgbuf.sg_flags & RAW);
		scannoecho = (scanraw || (sgbuf.sg_flags & ECHO) == 0);
	}
	scanwriteptr = scanbuffer;
	scanreadptr = scanbuffer;
}



/* Read the next input character.  If it is an editing character,
 * abort the current context and longjmp back to the scaninit call.
 * NOTE: For proper results, the caller should not alter the global
 * state until the full command has been read in.  This includes such
 * things as prompting for input or saving values.  Otherwise, improper
 * results will occur if the user edits the command.
 */
scanchar()
{
	char	*i;
	char	ch;		/* current character */
	char	*oldptr;	/* old scanwriteptr value */

loop:	if (scanreadptr < scanwriteptr) return(*scanreadptr++);
	ch = scanroutine();
	oldptr = scanwriteptr;
	if (ch == rubchar) {
		if (scanwriteptr <= scanbuffer) {
			scanerase(0);
			write(STDOUT, "\7", 1);
			goto loop;
		}
		scanwriteptr--;
		scanreadptr = scanbuffer;
		scanerase(1);
		longjmp(scanjumpbuf, SCAN_EDIT);
	}
	if (ch == rubword) {
		if (scanwriteptr <= scanbuffer) {
			scanerase(0);
			goto loop;
		}
		while ((--scanwriteptr >= scanbuffer) &&
			((*scanwriteptr == ' ') || (*scanwriteptr == '\t')));
		scanwriteptr++;
		while ((--scanwriteptr >= scanbuffer) &&
			((*scanwriteptr != ' ') && (*scanwriteptr != '\t')));
		scanwriteptr++;
		scanreadptr = scanbuffer;
		scanerase(oldptr - scanwriteptr);
		longjmp(scanjumpbuf, SCAN_EDIT);
	}
	if (ch == rubline) {
		if (scanwriteptr <= scanbuffer) {
			scanerase(0);
			goto loop;
		}
		scanwriteptr = scanbuffer;
		scanreadptr = scanbuffer;
		scanerase(oldptr - scanwriteptr);
		longjmp(scanjumpbuf, SCAN_EDIT);
	}
	if (ch == retypechar) {
		if (scanraw) write(STDOUT, "\015", 1);
		write(STDOUT, "\r\n", 2);
		if (*scanprompt) write(STDOUT, scanprompt, strlen(scanprompt));
		if (scanwriteptr > scanbuffer)
		    for(i = scanbuffer; i < scanwriteptr; i++) {
			if (*i == ESC) write(STDOUT, '$', 1);
			if (*i == '?') write(STDOUT, "\r\n", 2);
			if ((*i == '\n') || (*i == '\r'))
				write(STDOUT, "\r\n", 2);
			else write(STDOUT, i, 1);
		    }
		goto loop;
	}
	if (ch == quotechar) {
		scanerase(0);
		ch = scanroutine();
	}
	if (scanwriteptr >= scanbuffer + SCAN_SIZE) {
		write(STDOUT, "\7", 1);
		goto loop;
	}
	if (scannoecho) if (ch == ESC) write(STDOUT, "$", 1);
			else if (ch == '?') write(STDOUT, "\r\n", 2);
			else if ((ch == '\n') || (ch == '\r'))
				write(STDOUT, "\r\n", 2);
				else if (ch > 7) write(STDOUT, &ch, 1);

	*scanwriteptr++ = ch;
	return(*scanreadptr++);
}



/* Backup the read pointer if possible. */
scanbackup()
{
	if (scanreadptr > scanbuffer) scanreadptr--;
}



/* Abort reading of the current command input. */
scanabort()
{
	scanreadptr = scanbuffer;
	scanwriteptr = scanbuffer;
	longjmp(scanjumpbuf, SCAN_ABORT);
}



/* Erase text from the screen, including the last editing character. */
/* static */
scanerase(count)
{
	if (!scannoecho) count += 2;
	while (count-- > 0) write(STDOUT, ERASETEXT, ERASELEN);
}
