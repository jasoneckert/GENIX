/*
 * dpy.c: version 1.10 of 9/29/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)dpy.c	1.10 (NSC) 9/29/83";
# endif

/* Program to execute a command repeatedly and display output to screen */
/*				David I. Bell			 */

#include <curses.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <setjmp.h>
#include "scan.h"

#define	STDIN	0		/* standard input */
#define	MAXINT	0x7fffffff	/* maximum integer */
#define	SIZE	2048		/* size of pipe buffer */
#define	SLPTIME	10		/* default sleep time */
#define	SLPSLOW	60		/* increment sleep time after idled this much */
#define	SLPMAX	60		/* maximum sleep time to increase to */
#define	OVERLAP	1		/* default lines of overlap between pages */
#define	LINDPY	MAXINT		/* number of lines between displays */

int	origslptime;		/* original sleep time */
int	slptime;		/* current sleep time */
int	idletime;		/* time of last non-input */
int	noslow;			/* don't slow down sleep time */
int	nosleep;		/* don't sleep next time */
int	lindpy;			/* lines between displays */
int	dotitle;		/* show title line */
int	dodate;			/* show date in header line */
int	autopage;		/* advance pages this often */
int	autocount;		/* current counter value */
int	mypid;			/* my process id */
int	pageno;			/* page number */
int	lpp;			/* lines of data per page */
int	lastline;		/* index of last line on screen */
int	overlap;		/* overlap between pages */
int	more;			/* more output remaining */
jmp_buf	ttyjmp;			/* jump buffer for tty input */
char	**cmdargv;		/* pointer to command to execute */
char	buf[SIZE];		/* pipe buffer */
struct	sgttyb	ttyb;		/* block for tty mode changing */
extern	int	errno;		/* last error */
extern	int	sys_nerr;	/* number of error messages */
extern	char	*sys_errlist[];	/* error message array */
int	quit();
int	cmdint();
int	gotalrm();
int	ttyinput();
FILE	*xpopen();


main(argc, argv)
	register char	**argv;
{
	initparams();
	argv++;
	while ((argc >= 2) && (**argv == '-')) {
		switch (argv[0][1]) {
			case 'a':
				autopage = getnum(&argv[0][2], 1);
				autocount = autopage;
				break;

			case 't':
				dotitle = 0;
				break;

			case 'd':
				dodate = 0;
				break;

			case 'l':
				lindpy = getnum(&argv[0][2], 1);
				break;

			case 'o':
				overlap = getnum(&argv[0][2], 0);
				break;

			case 's':
				slptime = getnum(&argv[0][2], 0);
				origslptime = slptime;
				break;

			default:
				if ((argv[0][1] < '0') || (argv[0][1] > '9')) {
					printf("Bad option -%c\n", argv[0][1]);
					exit(1);
				}
				slptime = getnum(&argv[0][1], 0);
				origslptime = slptime;
				break;
		}
		argc--;
		argv++;
	}
	if (argc < 2) {
		printf("Usage: dpy [-a#] [-d] [-o#] [-s#] [-t] command args ...\n");
		exit(1);
	}
	cmdargv = argv;
	mypid = getpid();
	gtty(0, &ttyb);
	signal(SIGINT, quit);
	argc = LINTRUP;
	ioctl(0, TIOCLBIS, &argc);
	initscr();
	lastline = stdscr->_maxy - 1;
	lpp = lastline;
	if (dotitle) lpp--;
	noecho();
	crmode();
	leaveok(stdscr, 1);
	scaninit(ttyinput, 0, ttyjmp);
	sigset(SIGTINT, cmdint);
	while (1) {
		doscreen();
		if (--autocount == 0) {
			autocount = autopage;
			pageno++;
			if (more == 0) pageno = 0;
		}
		dosleep();
	}
}

/* Here on a terminal interrupt to read any commands which exist.
 * This is a tricky routine, in that it does not block waiting for input
 * but is reentered as often as necessary in order to complete a command.
 */
cmdint()
{
	register char	ch;
	register int	arg;
	register char	boolarg;
	register char	noarg;
	register char	exclam;
	long	fion;

	if (setjmp(ttyjmp) == SCAN_EOF) return;
	idletime = 0;
	noarg = 1;
	arg = 0;
	exclam = 0;
	ch = scanchar();
	while ((ch >= '0') && (ch <= '9')) {
		noarg = 0;
		arg = (arg * 10) + ch - '0';
		ch = scanchar();
	}
	boolarg = 1;
	if (ch == 'n') {
		boolarg = 0;
		ch = scanchar();
	}
	if (ch == '!') {
		exclam = 1;
		ch = scanchar();
	}
	switch(ch) {
		case '\014':			/* refresh screen */
			wrefresh(curscr);
			break;
		case '\n':			/* update immediately */
			break;
		case ' ':			/* move to next page */
			pageno++;
			if (more == 0) pageno = 0;
			autocount = autopage;
			break;
		case 'a':			/* automatically page */
			if (noarg) arg = 1;
			if (boolarg == 0) arg = 0;
			autopage = arg;
			autocount = autopage;
			break;
		case 'd':			/* show date in header */
			dodate = boolarg;
			break;
		case 'i':			/* initialize parameters */
			initparams();
			break;
		case 'o':			/* set page overlap */
			if (noarg) arg = OVERLAP;
			overlap = arg;
			break;
		case 'p':			/* set page number */
			pageno = arg - 1;
			if (pageno < 0) pageno = 0;
			autopage = 0;
			break;
		case 'q':			/* quit program */
			quit();
		case 's':			/* set sleep time */
			if (noarg) arg = SLPTIME;
			slptime = arg;
			origslptime = arg;
			noslow = exclam;
			break;
		case 't':			/* show title line */
			dotitle = boolarg;
			lpp = lastline;
			if (dotitle) lpp--;
			break;
		default:			/* bad commands */
		error:
			printf("\007");
			fflush(stdout);
			scanabort();
	}
	nosleep = 1;
	scanabort();
}


/* Read next character from the terminal if it is ready */
ttyinput()
{
	long	ch;

	if (ioctl(0, FIONREAD, &ch) || (ch <= 0)) scaneof();
	read(STDIN, &ch, 1);
	return(ch & 0177);
}

/* This routine pauses until the required sleep time has elapsed, or
 * until a tty input interrupt causes the sleep to be aborted.
 */
dosleep()
{
	register int	endtime;
	register int	lefttime;

	endtime = time(0);
	slptime = origslptime;
	if (noslow == 0) {		/* increase sleep time if necessary */
		if (idletime == 0) idletime = endtime;
		lefttime = (endtime - idletime) / SLPSLOW;
		if (lefttime > SLPMAX) lefttime = SLPMAX;
		if (slptime < lefttime) slptime = lefttime;
	}
	endtime += slptime;
	while ((nosleep == 0) && (slptime > 0)) {
		lefttime = endtime - time(0);
		if (lefttime <= 0) break;
		signal(SIGALRM, gotalrm);
		alarm(lefttime);
		pause();	/* can wakeup on alarm or tty input */
		alarm(0);
	}
	nosleep = 0;
}

gotalrm() {}

/* Initialize all default parameters */
initparams()
{
	dotitle = 1;
	lpp = lastline - 1;
	dodate = 1;
	pageno = 0;
	more = 0;
	autopage = 0;
	autocount = 0;
	noslow = 0;
	overlap = OVERLAP;
	slptime = SLPTIME;
	origslptime = SLPTIME;
	lindpy = LINDPY;
}


/* Here to exit from the program */
quit()
{
	char	*s;

	clear();
	refresh();
	endwin();
	stty(0, &ttyb);
	exit(0);
}


/* Scan a number from a string with a default value */
getnum(s, d)
	register char	*s;
{
	register int	n;

	if (*s == 0) return(d);
	n = 0;
	while (*s) {
		if ((*s < '0') || (*s > '9')) {
			printf("Bad number\n");
			exit(1);
		}
		n = n * 10 + *s - '0';
		s++;
	}
	return(n);
}

/* Show the newest output of the selected command */
doscreen()
{
	register char	*cp;
	register FILE	*f;
	register int	count;
	register char	**argv;
	register int	skip;
	int	clock;
	int	err;

	move(0,0);
	if (dotitle) {
		printw("Cmd:");
		argv = cmdargv;
		while (*argv) printw(" %s", *argv++);
		printw("   Slp: %d", slptime);
		if (origslptime != slptime)
			printw("(%d)", origslptime);
		printw("   Pag: %d", pageno + 1);
		if (dodate) {
			clock = time(0);
			printw("   Time: %.19s", ctime(&clock));
		}
		addch('\n');
	}
	f = xpopen();
	if (f == NULL) {
		errorw("Cannot create process");
		goto done;
	}
	skip = pageno * (lpp - overlap);
	more = 0;
	err = 0;
	while ((count = fread(buf, 1, SIZE, f)) > 0) {
		for (cp = buf; count--; cp++) {
			if (err) {			/* screen is full */
				if (*cp != '\n') more = 1;
				continue;
			}
			if (skip > 0) {			/* skipping pages */
				if (*cp == '\n') skip--;
				continue;
			}
			err = (addch(*cp) == ERR);	/* add character */
		}
	}
	if (count < 0) {
		errorw("\nPipe read failed");
	}
	xpclose(f);
	if (more) {
		move(lastline, 0);
		printw("--More--\n");
	}
done:	clrtobot();
	move(0,0);
	refresh();
}


/* Type an error message to the window, similar to perror */
errorw(str)
	char	*str;
{
	printw("%s: ");
	if ((errno < 0) || (errno >= sys_nerr)) {
		printw("error %d\n", errno);
		return;
	}
	printw("%s\n", sys_errlist[errno]);
}

/* The following are the standard popen and pclose routines, modified to
 * execute a command directly without involvement of the shell.
 */
#define	RDR	0
#define	WTR	1
static	int	popen_pid[20];

FILE *
xpopen()
{
	int p[2];
	register myside, hisside, pid;

	if (pipe(p) < 0)
		return NULL;
	myside = p[RDR];
	hisside = p[WTR];
	if ((pid = vfork()) == 0) {
		/* myside and hisside reverse roles in child */
		close(myside);
		dup2(hisside, 1);
		dup2(hisside, 2);
		close(hisside);
		execvp(*cmdargv, cmdargv);
		perror(*cmdargv);
		_exit(1);
	}
	if (pid == -1)
		return NULL;
	popen_pid[myside] = pid;
	close(hisside);
	return(fdopen(myside, "r"));
}

xpclose(ptr)
FILE *ptr;
{
	register f, r, (*hstat)(), (*istat)(), (*qstat)();
	int status;

	f = fileno(ptr);
	fclose(ptr);
	istat = signal(SIGINT, SIG_HOLD);
	qstat = signal(SIGQUIT, SIG_HOLD);
	hstat = signal(SIGHUP, SIG_HOLD);
	while (1) {
		r = wait(&status);
		if (r == popen_pid[f]) break;
		if (r >= 0) continue;
		status = -1;
		if (errno != EINTR) break;
	}
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	signal(SIGHUP, hstat);
	return(status);
}
