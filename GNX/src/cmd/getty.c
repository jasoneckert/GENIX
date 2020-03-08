# ifndef NOSCCS
static char *sccsid = "@(#)getty.c	1.7	7/29/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

/*
 * *sigh* ... yet another version of getty
 *
 *	6 June 1983 ACT, from earlier versions by ACT & FLB (at U of Maryland)
 *
 *	modified for NSC/GENIX environment
 */

#include <stdio.h>
#include <sgtty.h>
#include <signal.h>
#include <ctype.h>
#include <ident.h>

#define TTYDEFS	"/etc/gettytab"	/* tty name definition table */

#define	CERASE	'\b'
#define	CKILL	025		/* Control-U */
#define	CEOT	004		/* Control-D */
#define	CQUIT	034		/* Control-\ */
#define	CINTR	0177		/* DEL, RUBOUT */
#define	CSTOP	023		/* Control-S */
#define	CSTART	021		/* Control-Q */
#define	CBRK	0377		/* Undefined */

struct tab {
	struct tab *t_forw;	/* Forward link */
	char	*t_name,	/* This name */
		*t_next,	/* Next name */
		*t_msg;		/* Initialization string */
	int	t_iflags,	/* Initial flags */
		t_fflags,	/* Final flags */
		t_speed;	/* Baud rate */
} *ttable;

struct value {
	char	*v_name;	/* Value name */
	int	v_val;		/* Value */
};

int	crmod,
	upper,
	lower,
	genpar;

char	name[200],
	*mytty,
	*tname;

struct sgttyb	tmode;

extern char	*sbrk ();

/* These tables are sorted alphabetically for binary searching. */
struct value flags[] = {
	"ANYP",		EVENP|ODDP,
	"BS1",		BS1,
	"CBREAK",	CBREAK,
	"CR1",		CR1,
	"CR2",		CR2,
	"CR3",		CR3,
	"CRMOD",	CRMOD,
	"ECHO",		ECHO,
	"EVENP",	EVENP,
	"FF1",		FF1,
	"LCASE",	LCASE,
	"NL1",		NL1,
	"NL2",		NL2,
	"NL3",		NL3,
	"ODDP",		ODDP,
	"RAW",		RAW,
	"TAB1",		TAB1,
	"TAB2",		TAB2,
	"TANDEM",	TANDEM,
	"XTABS",	XTABS,
}, speeds[] = {
	"0",		B0,
	"110",		B110,
	"1200",		B1200,
	"134",		B134,
	"134.5",	B134,
	"150",		B150,
	"1800",		B1800,
	"19200",	EXTA,
	"200",		B200,
	"2400",		B2400,
	"300",		B300,
	"4800",		B4800,
	"50",		B50,
	"600",		B600,
	"75",		B75,
	"9600",		B9600,
	"?",		-1,	/* unknown, apply auto-baud routine */
	"EXTA",		EXTA,
	"EXTB",		EXTB,
};

/* partab[c&0177] gives c with even parity */
char partab[] = {
	0000, 0201, 0202, 0003, 0204, 0005, 0006, 0207,
	0210, 0011, 0012, 0213, 0014, 0215, 0216, 0017,
	0220, 0021, 0022, 0223, 0024, 0225, 0226, 0027,
	0030, 0231, 0232, 0033, 0234, 0035, 0036, 0237,
	0240, 0041, 0042, 0243, 0044, 0245, 0246, 0047,
	0050, 0251, 0252, 0053, 0254, 0055, 0056, 0257,
	0060, 0261, 0262, 0063, 0264, 0065, 0066, 0267,
	0270, 0071, 0072, 0273, 0074, 0275, 0276, 0077,
	0300, 0101, 0102, 0303, 0104, 0305, 0306, 0107,
	0110, 0311, 0312, 0113, 0314, 0115, 0116, 0317,
	0120, 0321, 0322, 0123, 0324, 0125, 0126, 0327,
	0330, 0131, 0132, 0333, 0134, 0335, 0336, 0137,
	0140, 0341, 0342, 0143, 0344, 0145, 0146, 0347,
	0350, 0151, 0152, 0353, 0154, 0355, 0356, 0157,
	0360, 0161, 0162, 0363, 0164, 0365, 0366, 0167,
	0170, 0371, 0372, 0173, 0374, 0175, 0176, 0377,
};

# define HNBUFLEN	32

main (argc, argv)
	int	argc;
	char	**argv;
{
	register struct tab	*tp;
	int			ldisp = 0;
	struct tchars		tc;
	int			hnblen = HNBUFLEN;
	char			hnbuf [HNBUFLEN];
	char			*ttyname ();

#	ifdef notdef
	/* Commented out in original getty. */
	signal (SIGINT, 1);
	signal (SIGQUIT, 0);
#	endif notdef

	if (argc < 2)
		tname = "0";
	else
		tname = argv[1];
	mytty = &ttyname(0)[5];

	gethostname (hnbuf, &hnblen);

	readttys ();

	for (;;) {
		for (tp = ttable; tp; tp = tp -> t_forw)
			if (  *tp -> t_name == *tname
			   && strcmp (tp -> t_name, tname) == 0
			   )
				break;
		if (!tp)
			error (1, "Table entry for ``%s'' not found", tname);

		tc.t_intrc  = CINTR;
		tc.t_quitc  = CQUIT;
		tc.t_stopc  = CSTOP;
		tc.t_startc = CSTART;
		tc.t_brkc   = CBRK;
		tc.t_eofc   = CEOT;
		ioctl (0, TIOCSETC, &tc);
		ioctl (0, TIOCSETD, &ldisp);
		ioctl (0, TIOCCBRK, (long *) 0);
		tmode.sg_erase  = CERASE;
		tmode.sg_kill   = CKILL;
		tmode.sg_ispeed = tmode.sg_ospeed =
			tp -> t_speed < 0 ? auto_baud () : tp -> t_speed;
		tmode.sg_flags = tp -> t_iflags;
		genpar = ((tp->t_fflags & ANYP) != ANYP);
		stty (0, &tmode);

		/*
		 * Avoid sending CR or NL to another Unix system
		 * which may have a login process running on the
		 * line.  (12/4/81 FLB)
		 */
		if (tp -> t_msg)
			sprintf ( name
				, "%s%s\n\r\r\r\r%s%s%slogin: "
				, tp -> t_msg
				, myname
				, tmode.sg_ospeed > B1200 ? "\n\r\n\r" :
					"\n\r\r\r\r\r\n\r\r\r\r\r"
				, hnblen > 0 ? hnbuf : ""
				, hnblen > 0 ? " "   : ""
				);
		else
			sprintf ( name
				, "%s %s login: "
				, myname
				, hnblen > 0 ? hnbuf : ""
				);
		puts (name);

		/*
		 * Wait a while, then flush input to get rid of
		 * noise from open lines.  (10/13/81 FLB)
		 */
		sleep (1);
		stty  (0, &tmode);

		if (getname ()) {
			if (!upper && !lower)
				continue;
			tmode.sg_flags = tp -> t_fflags;
			if (crmod)
				tmode.sg_flags |= CRMOD;
			if (upper)
				tmode.sg_flags |= LCASE;
			if (lower)
				tmode.sg_flags &= ~LCASE;
			stty   (0, &tmode);
			putchr ('\n');
			execl  ("/bin/login", "login", name, 0);
			puts   ("Can't find login - contact system administrators\r\n");
			error  (1, "Can't find login");
		}
		tname = tp -> t_next;
	}
}

/* VARARGS1 */
error (fatal, fmt, a, b, c)
char *fmt, *a, *b, *c;
{
	register FILE *console = fopen ("/dev/console", "w");

	if (console) {
		fprintf (console, "error in getty: ");
		fprintf (console, fmt, a, b, c);
		fprintf (console, "\7\7\7\r\n");
		fclose (console);
	}
	if (!fatal)
		return;
	pause ();		/* hang till someone fixes */
	exit (-1);
}

getname ()
{
	register char	*np;
	register int	c;
	char		cs;

	crmod = 0;
	upper = 0;
	lower = 0;
	np = name;
	for (;;) {
		if (read (0, &cs, 1) < 1) {
			/* probably lost carrier */
			exit (0);
		}
		if (! (c = cs & 0177)) {
			/* null or break, to next entry */
			return (0);
		}
		if (c == CEOT)
			disconnect ();
		if (c == '\r' || c == '\n' || np >= name + sizeof name - 1)
			break;
		putchr (c);
		if (c >= 'a' && c <= 'z')
			++lower;
		else if (c >= 'A' && c <= 'Z')
			++upper;
		else if (c == CERASE) {
			if (np > name) {
				--np;
				if (c == '\b' && tmode.sg_ospeed > B300) {
					putchr (' ');
					putchr ('\b');
				}
			}
			continue;
		}
		else if (c == CKILL) {
			if (tmode.sg_erase == '\b' && tmode.sg_ospeed > B300) {
				while (np-- > name) {
					putchr ('\b');
					putchr (' ');
					putchr ('\b');
				}
			}
			else {
				putchr ('\n');
				putchr ('\r');
			}
			np = name;
			continue;
		}
		else if (c == ' ')
			c = '_';
		*np++ = c;
	}
	*np = 0;
	if (c == '\r')
		++crmod;
	if (upper && !lower)
		for (np = name; *np; np++)
			if (isupper (*np))
				*np = tolower (*np);
	return (1);
}

puts (s)
	register char	*s;
{
	while (*s)
		putchr (*s++);
}

/*
 * Put out a character, possibly with (even only) parity.
 */
putchr (cc)
{
	char	c;

	c = cc;
	if (genpar)
		c |= partab[c&0177] & 0200;
	write (1, &c, 1);
}


/*
 * ttys file read support routines
 */

char	*tt_sptr,
	tt_linebuf[BUFSIZ],
	*extract ();

/*
 * Extract a string ending with term and save it
 */
char *
saveit (term)
{
	register char	*p = extract (term, term),
			*s,
			*t;
	static int	nleft;
	static char	*space;

	if (nleft < strlen (p) + 1) {
		space = sbrk (BUFSIZ);
		nleft += BUFSIZ;
	}
	s = t = space;
	while (*t++ = *p++)
		;
	nleft -= t - s;
	space += t - s;
	return s;
}

/*
 * Extract a string from tt_linebuf, starting at tt_sptr,
 * terminated by either term1 or term2 (or 0).
 */
char *
extract (term1, term2)
	register int	term1, term2;
{
	register char	*s = tt_sptr,
			*p;
	register int	c,
			i;
	static char	buf[250];

	p = buf;
	while (*s == ' ' || *s == '\t')
		s++;
	while ((c = *s++) && c != term1 && c != term2) {
		if (c == '\\') {
			c = *s++;
			if (c == 'n')
				*p++ = '\n';
			else if (c == 'r')
				*p++ = '\r';
			else if (c == 't')
				*p++ = '\t';
			else if (c == 'b')
				*p++ = '\b';
			else if (c == 'f')
				*p++ = '\f';
			else if (c >= '0' && c < '8') {
				*p = 0;
				for (i = 0; i < 3; i++) {
					*p = (*p << 3) + c - '0';
					c = *s++;
					if (c < '0' || c > '7') {
						s--;
						break;
					}
				}
				p++;
			}
			else
				*p++ = c;
			continue;
		}
		*p++ = c;
	}
	*p = 0;
	while (*s == ' ' || *s == '\t')
		s++;
	tt_sptr = s;
	return buf;
}

ev (s, table, hi)
	register char		*s;
	register struct value	*table;
	register int		hi;
{
	register int	lo = 0,
			mid,
			c;

	while (lo <= hi) {
		mid = (lo + hi) >> 1;
		c = strcmp (s, table[mid].v_name);
		if (c == 0)
			return table[mid].v_val;
		else if (c < 0)
			hi = mid - 1;
		else
			lo = mid + 1;
	}
	error (1, "\"%s\" not found", s);
}
value (table, tabsiz)
	struct value	*table;
{
	register int	rv = 0;

	do
		rv |= ev (extract (' ', '|'), table, tabsiz);
	while (tt_sptr[-1] == '|' || tt_sptr[0] == '|');
	return rv;
}

/*
 * this is it: read the ttys file
 */
readttys ()
{
	register struct tab	*tp;
	register FILE		*ttyf;
	static struct tab	*stp;
	static int		ntp;

	ttyf = fopen (TTYDEFS, "r");
	if (!ttyf) {
		error (0, "Can't find ttydefs file %s", TTYDEFS);
fakeit:
		ttable = tp = (struct tab *) sbrk (sizeof (struct tab));
		tp -> t_forw = 0;
		tname = tp -> t_name = tp -> t_next = "oops";
		tp -> t_speed = B300;
		tp -> t_iflags = ANYP|RAW;
		tp -> t_fflags = ANYP|XTABS|CRMOD|ECHO;
		tp -> t_msg = "\n\r\n\r";
		return;
	}

	/*
	 * While there are lines, read them in, allocate table space,
	 * and set up table entry.  Comments are lines starting with "#".
	 */

	while (fgets (tt_linebuf, BUFSIZ, ttyf)) {
		if (tt_linebuf[0] == '#' || tt_linebuf[1] == 0)
			continue;
		tt_linebuf[strlen(tt_linebuf) - 1] = 0;
		tt_sptr = tt_linebuf;
		if (ntp < 1) {
			stp = (struct tab *) sbrk (20 * sizeof *tp);
			ntp += 20;
		}
		tp = stp++;
		ntp--;
		tp -> t_forw = ttable;
		tp -> t_name = saveit (' ');
		tp -> t_next = saveit (' ');
		tp -> t_speed = value (speeds, sizeof speeds / sizeof *speeds);
		tp -> t_iflags = value (flags, sizeof flags / sizeof *flags);
		tp -> t_fflags = value (flags, sizeof flags / sizeof *flags);
		if (*tt_sptr)
			tp -> t_msg = saveit (0);
		else
			tp -> t_msg = 0;
		ttable = tp;
	}
	fclose (ttyf);
	if (!ttable)
		goto fakeit;
}

/*
 * Drop data terminal ready for one second.  This is important for Gandalf
 * lines which must be told to disconnect.  This affects all autobauding
 * lines after a 60 second timeout.  2-Feb-1983 FLB
 */
time_out ()
{
	puts ("\r\r\n\nTimeout\r\r\n\n");
	sleep (2);
	tmode.sg_ispeed = tmode.sg_ospeed = B0;
	stty (0, &tmode);
	sleep (3);
	exit (0);
}

/*
 * Try to figure out what speed the terminal is set to based on what
 * a carriage-return looks like at 2400 baud.  This doesn't distinguish
 * all speeds, but gets the good ones at least.  5/18/82 FLB
 */
struct autobaud {
	int s_speed;
	char *s_string;
} a_tab[] = {
	B110,	"\3\0\0\0",
	B300,	"\3\200\200\0",
	B600,	"\3\0\376\0",
	B1200,	"\2\346\200",
	B1200,	"\2\346\340",
	B1800,	"\2\0\376",
	B1800,	"\2\0\377",
	B2400,	"\1\15",
	B2400,	"\1\215",
	B4800,	"\1\361",	/* needed for Gandalf */
	B4800,	"\1\362",
	B4800,	"\1\363",	/* needed for Gandalf */
	B4800,	"\1\371",	/* needed for Gandalf */
	B4800,	"\1\375",	/* needed for Gandalf */
	B4800,	"\1\376",	/* needed for Gandalf */
	B9600,	"\1\377",	/* needed for Gandalf */
	B110,	"\1\0",		/* so a ``break'' will let you escape */
	0, 0
};

auto_baud ()
{
    register struct autobaud	*tp;
    struct sgttyb		ttyb;
    char			buf[10];
    int				i;

    ttyb.sg_ispeed = ttyb.sg_ospeed = B2400;
    ttyb.sg_flags = ANYP|RAW;
    stty (0, &ttyb);

    for (;;) {
	signal (SIGALRM, time_out);		/* 2-Feb-1983 FLB */
	alarm (60);				/* 2-Feb-1983 FLB */
	input (buf);

	for (tp = a_tab; tp -> s_speed; tp++)
	    for (i = 0; buf[i] == tp -> s_string[i] && i < sizeof buf; i++)
		if (i == *buf) {
		    signal (SIGALRM, time_out);	/* 2-Feb-1983 FLB */
		    alarm (60);			/* 2-Feb-1983 FLB */
		    return (tp -> s_speed);
		}
	stty (0, &ttyb);
    }
}

a_zap () {}

input (s)
	char	*s;
{
	register char	*cp = s + 1;

	read (0, cp, 1);
	cp++;
	signal (SIGALRM, a_zap);
	alarm (1);
	while (read (0, cp, 1) > 0)
		cp++;

	*s = (cp - s) - 1;
}

disconnect ()
{
	puts ("\r\n\rDisconnected.\r\n\r\r\4");
	sleep (3);
	tmode.sg_ispeed = tmode.sg_ospeed = B0;
	stty (0, &tmode);
	sleep (2);
	exit (0);
}
