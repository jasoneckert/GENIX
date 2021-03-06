# ifndef NOSCCS
static char *sccsid = "@(#)cu.c	1.7	8/10/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

#include <stdio.h>
#include <signal.h>
#include <sgtty.h>
#include <setjmp.h>

/*
 *	cu telno [-t] [-s speed] [-l line] [-xlevel] [-b]
 *
 *	-t is for dial-out to terminal.
 *	speeds are: 110, 134, 150, 300, 1200. 1200 is default.
 *
 *	Escape with `~' at beginning of line.
 *	Ordinary diversions are ~<, ~> and ~>>.
 *	Silent output diversions are ~>: and ~>>:.
 *	Terminate output diversion with ~> alone.
 *	Quit is ~. and ~! gives local command or shell.
 *	Also ~$ for canned procedure pumping remote.
 *	~%put from [to]  and  ~%take from [to] invoke builtins
 *
 *	This version of cu has been modified to use the Hayes
 *	Smartmodem and Smartmodem 1200 dialers.  Its default speed
 *	is 1200 baud instead of 300 baud.  The default dialer device
 *	is /dev/ttyd0.
 *
 *	The -x flag specifies a debugging level as in the uucp
 *	programs.  It should be followed immediately (no space)
 *	by a digit.  The higher the value, the more verbose the
 *	debugging output.
 */

/*
 * defs that come from uucp.h
 */
#define NAMESIZE 15
#define FAIL -1
#define SAME 0
#define MAXMSGTIME 45
#define SLCKTIME 5400	/* system/device timeout (LCK.. files) in seconds */
#define ASSERT(e, f, v) if (!(e)) {\
	fprintf(stderr, "AERROR - (%s) ", "e");\
	fprintf(stderr, f, v);\
	cleanup(FAIL);\
}

#define DEBUG(l, f, s) if (Debug >= l) fprintf(stderr, f, s)

#define CRLF "\r\n"

#define wrc(ds) write(ds,&c,1)

char	*devcul	= "/dev/ttyd0";
char	*lspeed	= "1200";

int	ln;		/* fd for comm line */
char	tkill, terase;	/* current input kill & erase */
int	efk;		/* process of id of listener  */
char	c;
char	oc;
int	Debug = 0;

char	*connmsg[] = {
	"",
	"line busy",
	"call dropped",
	"no carrier",
	"can't fork",
	"line access",
	"line access",
	"tty hung",
	"usage: cu telno [-t] [-s speed] [-l line] [-xlevel] [-b]",
	"lock failed: line busy"
};

rdc(ds) {

	ds=read(ds,&c,1); 
	oc = c;
	c &= 0177;
	return (ds);
}

int intr;

sig2()
{
	signal(SIGINT, SIG_IGN); 
	intr = 1;
}

int	dout;
int	nhup;
int	nullbrk;	/* turn breaks (nulls) into dels */

/*
 *	main: get connection, set speed for line.
 *	spawn child to invoke rd to read from line, output to fd 1
 *	main line invokes wr to read tty, write to line
 */
main(ac,av)
char *av[];
{
	int fk;
	int speed;
	char *telno;
	struct sgttyb stbuf;
	int cleanup();

	signal(SIGINT,  cleanup);
	signal(SIGHUP,  cleanup);
	signal(SIGQUIT, cleanup);
	if (ac < 2) {
		prf(connmsg[8]);
		exit(8);
	}
	for (; ac > 1; av++,ac--) {
		if (av[1][0] != '-')
			telno = av[1];
		else switch(av[1][1]) {
		case 't':
			dout = 1;
			--ac;
			continue;
		case 'b':
			nullbrk++;
			continue;
		case 'd':
			Debug = 1;
			continue;
		case 's':
			lspeed = av[2]; ++av; --ac;
			break;
		case 'l':
			devcul = av[2]; ++av; --ac;
			break;
		case 'x':
			Debug = av[1][2] - '0';
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			devcul[strlen(devcul)-1] = av[1][1];
			break;
		default:
			prf("Bad flag %s", av[1]);
			break;
		}
	}
	if (! exists (devcul))
		exit(9);

	/* Calculate desired speed for conn() to set: */
	switch (atoi (lspeed)) {
	case 110:
		speed = B110;  break;
	case 150:
		speed = B150;  break;
	case 300:
		speed = B300;  break;
	default:
	case 1200:
		speed = B1200; break;
	}

	/* Establish the connection: */
	ln = conn (devcul, telno, speed);
	if (ln < 0) {
		prf ("Connect failed: %s", connmsg[-ln]);
		cleanup(-ln);
	}

	if (dout) {
		/* Set up the line for actual use: */
		stbuf.sg_flags &= ~RAW;
		stbuf.sg_flags |=  ECHO;
		ioctl(ln, TIOCSETP, &stbuf);
	}

	prf ("Connected");

	if (dout)
		fk = -1;
	else
		fk = fork ();
	nhup = (int) signal (SIGINT, SIG_IGN);

	if (fk == 0) {
		/* Child process: */
		chwrsig ();
		rd      ();
		prf     ("\007Lost carrier");
		cleanup (3);
	}

	mode (1);
	efk = fk;
	wr   ();
	hang_it_up (ln);
	mode (0);
	if (fk != -1)
		kill (fk, SIGKILL);
	wait ((int *)NULL);
	stbuf.sg_ispeed = 0;
	stbuf.sg_ospeed = 0;
	ioctl   (ln, TIOCSETP, &stbuf);
	prf     ("Disconnected");
	cleanup (0);
}

/*
 * Issue a command to the modem.
 */
do_command (fd, cmd)
	int	fd;
	char	*cmd;
{
	register int	cmdlen;

	cmdlen = strlen (cmd);

	DEBUG (4, "issuing %s\n", cmd);

	if (cmdlen > 0)
		write (fd, cmd, cmdlen);
}

/******* (routine stolen from uucp source) *****
 *
 *	prefix(s1, s2)	check s2 for prefix s1
 *	char *s1, *s2;
 *
 *	return 0 - !=
 *	return 1 - == 
 */
prefix(s1, s2)
	char *s1, *s2;
{
	register char	c;

	while ((c = *s1++) == *s2++)
		if (c == '\0')
			return(1);
	return(c == '\0');
}

/*** (routine stolen fronm uucp source) ****
 *
 *	notin(sh, lg)	check for occurrence of substring "sh"
 *	char *sh, *lg;
 *
 *	return codes:
 *		0  -  found the string
 *		1  -  not in the string
 */
notin(sh, lg)
	register char	*sh, *lg;
{
	while (*lg != '\0') {
		if (prefix(sh, lg))
			return(0);
		else
			lg++;
	}
	return(1);
}

jmp_buf Sjbuf;

/*** (routine stolen from uucp source) *****
 *
 *	alarmtr()  -  catch alarm routine for "expect".
 */
alarmtr ()
{
	longjmp(Sjbuf, 1);
}

#define MR 300

/*** (routine stolen from uucp source) ******
 *
 *	expect(str, fn)	look for expected string
 *	char *str;
 *
 *	return codes:
 *		0     -  found
 *		FAIL  -  lost line or too many characters read
 *		some character  -  timed out
 */
expect (str, fn)
	char	*str;
	int	fn;
{
	static char	rdvec [MR];
	register char	*rp = rdvec;
	int		nextch = 0,
			kr;
	int		alarm_tm;

	if (strcmp(str, "\"\"") == SAME)
		return(0);
	*rp = 0;

	/*
	 * If we are waiting for the Smartmodem to complete
	 * dialing and get a connection, allow more time
	 */
	if (strcmp(str, "CONNECT") == SAME)
		alarm_tm = 4 * MAXMSGTIME;
	else
		alarm_tm = MAXMSGTIME;
	if (setjmp(Sjbuf)) {
		DEBUG(4, "expect TIMEOUT\n%s", "");
		return(FAIL);
	}
	signal(SIGALRM, alarmtr);
	alarm(alarm_tm);

	while (notin(str, rdvec)) {
		kr = read(fn, &nextch, 1);
		if (kr <= 0) {
			DEBUG(4, "kr - %d\n", kr);
			alarm(0);
			DEBUG(4, "lost line kr - %d, ", kr);
			DEBUG(4, "fn - %d\n", fn);
			return(FAIL);
		}
		{
		int c;
		c = nextch & 0177;
		DEBUG(4, "%c", c > 040 ? c : '_');
		}
		if ((*rp = nextch & 0177) != '\0')
			rp++;
		*rp = '\0';
		if (rp >= rdvec + MR)
			return(FAIL);
	}
	alarm(0);
	return(0);
}


/*
 *	conn: establish dial-out connection.
 *	Example:  fd = conn("/dev/ttyh" "4500", B1200);
 *	Returns descriptor open to tty for reading and writing.
 *	Negative values (-1...-7) denote errors in connmsg.
 *	Uses alarm; requires SIGALRM handler.
 *	Be sure to disconnect tty when done, via HUPCL or stty 0.
 */
int
conn (dev, telno, speed)
	char	*dev, *telno;
	int	speed;
{
	struct sgttyb	stbuf;
	struct tchars	tcbuf;
	extern int	errno;
	char		*ltail;
	extern char	*rindex ();
	int		er, dh, t;
	char		dialbuf [40];	/* modem can't accept longer cmds */

	er = 0;

	DEBUG (3, "conn %s ", dev);
	DEBUG (3, "%s\n", telno);

	/* Get the dialout device's leafname  and lock it: */
	ltail = rindex (dev, '/') + 1;
	if (mlock (ltail) == FAIL) {
		er = 9;
		goto X;
	}

	/* Open the line and set it up */
	if ((dh = open (dev, 2)) < 0) {
		er = (errno == 6 ? 1 : 5); 
		goto X;
	}

	DEBUG (8, "  open of %s succeeded\n", dev);

	ioctl (dh, TIOCGETP, &stbuf);
	stbuf.sg_flags = EVENP | ODDP | RAW;
	stbuf.sg_ispeed  = speed;
	stbuf.sg_ospeed  = speed;
	ioctl (dh, TIOCSETP, &stbuf);
	ioctl (dh, TIOCEXCL,  (struct sgttyb *) NULL);
	ioctl (dh, TIOCHPCL,  (struct sgttyb *) NULL);
	ioctl (dh, TIOCFLUSH, (struct sgttyb *) NULL);

	/*
	 * Set the modem's state:
	 *	We use the modem's reset state with carrier wait
	 *	set according to the phone number's length.
	 */
	/* First, reset the unit: */
	do_command (dh, "AT Z\r");
	if (expect ("OK", dh)) {
		er = 2;
		goto X;
	}
	sleep (2);
	sprintf (dialbuf, "AT S7 = %d\r", 15 + 2 * strlen (telno));
	do_command (dh, dialbuf);
	if (expect ("OK", dh)) {
		er = 2;
		goto X;
	}
	sleep (1);

	/*
	 * Set up and issue the dialing command.
	 *	We ignore response from the command itself,
	 *	waiting for the connection to be established.
	 */
	sprintf (dialbuf, "AT DT%s\r", telno);
	do_command (dh, dialbuf);
	if (expect ("CONNECT", dh)) {
		er = 2; 
		goto X;
	}

X:
	return (er ? -er : dh);
}

/*
 * Hang up the phone line.
 *	We do this by dropping DTR; setting line speed to 0 does so.
 */
hang_it_up (fd)
	int	fd;
{
	struct  sgttyb	sb;

	ioctl (fd, TIOCCDTR, &sb);
}

/*
 *	wr: write to remote: 0 -> line.
 *	~.	terminate
 *	~<file	send file
 *	~!	local login-style shell
 *	~!cmd	execute cmd locally
 *	~$proc	execute proc locally, send output to line
 *	~%cmd	execute builtin cmd (put and take)
 *	~#	send 1-sec break
 *	~^Z	suspend cu process.
 */

wr()
{
	int ds,fk,lcl,x;
	char *p,b[600];
	for (;;) {
		p=b;
		while (rdc(0) == 1) {
			if (p == b) lcl=(c == '~');
			if (p == b+1 && b[0] == '~') lcl=(c!='~');
			if (nullbrk && c == 0) oc=c=0177; /* fake break kludge */
			if (!lcl) {
				c = oc;
				if (wrc(ln) == 0) {
					prf("line gone"); return;
				}
				c &= 0177;
			}
			if (lcl) {
				if (c == 0177) c=tkill;
				if (c == '\r' || c == '\n') goto A;
				if (!dout) wrc(0);
			}
			*p++=c;
			if (c == terase) {
				p=p-2; 
				if (p<b) p=b;
			}
			if (c == tkill || c == 0177 || c == '\4' || c == '\r' || c == '\n') p=b;
		}
		return;
A: 
		if (!dout) echo("");
		*p=0;
		switch (b[1]) {
		case '.':
		case '\004':
			return;
		case '#':
			ioctl(ln, TIOCSBRK, 0);
			sleep(1);
			ioctl(ln, TIOCCBRK, 0);
			continue;
		case '!':
		case '$':
			fk = fork();
			if (fk == 0) {
				char *getenv();
				char *shell = getenv("SHELL");
				if (shell == 0) shell = "/bin/sh";
				close(1);
				dup(b[1] == '$'? ln:2);
				close(ln);
				mode(0);
				if (!nhup) signal(SIGINT, SIG_DFL);
				if (b[2] == 0) execl(shell,shell,0);
				/* if (b[2] == 0) execl(shell,"-",0); */
				else execl(shell,"sh","-c",b+2,0);
				prf("Can't execute shell");
				exit(~0);
			}
			if (fk!=(-1)) {
				while (wait(&x)!=fk);
			}
			mode(1);
			if (b[1] == '!') echo("!");
			else {
				if (dout) echo("$");
			}
			break;
		case '<':
			if (b[2] == 0) break;
			if ((ds=open(b+2,0))<0) {
				prf("Can't divert %s",b+1); 
				break;
			}
			intr=x=0;
			mode(2);
			if (!nhup) signal(SIGINT, sig2);
			while (!intr && rdc(ds) == 1) {
				if (wrc(ln) == 0) {
					x=1; 
					break;
				}
			}
			signal(SIGINT, SIG_IGN);
			close(ds);
			mode(1);
			if (x) return;
			if (dout) echo("<");
			break;
		case '>':
		case ':':
			{
			FILE *fp; char tbuff[128]; register char *q;
			sprintf(tbuff,"/tmp/cu%d",efk);
			if(NULL==(fp = fopen(tbuff,"w"))) {
				prf("Can't tell other demon to divert");
				break;
			}
			fprintf(fp,"%s\n",(b[1]=='>'?&b[2]: &b[1] ));
			DEBUG (1, "name to be written in temporary: %s\n", &b[2]);
			fclose(fp);
			if (efk != -1) kill(efk,SIGEMT);
			}
			break;
#ifdef SIGTSTP
#define CTRLZ	26
		case CTRLZ:
			mode(0);
			kill(getpid(), SIGTSTP);
			mode(1);
			break;
#endif
		case '%':
			dopercen(&b[2]);
			break;
		default:
			prf("Use `~~' to start line with `~'");
		}
		continue;
	}
}

dopercen(line)
register char *line;
{
	char *args[10];
	register narg, f;
	int rcount;
	for (narg = 0; narg < 10;) {
		while(*line == ' ' || *line == '\t')
			line++;
		if (*line == '\0')
			break;
		args[narg++] = line;
		while(*line != '\0' && *line != ' ' && *line != '\t')
			line++;
		if (*line == '\0')
			break;
		*line++ = '\0';
	}
	if (equal(args[0], "take")) {
		if (narg < 2) {
			prf("usage: ~%%take from [to]");
			return;
		}
		if (narg < 3)
			args[2] = args[1];
		wrln("echo '~>:'");
		wrln(args[2]);
		wrln(";tee /dev/null <");
		wrln(args[1]);
		wrln(";echo '~>'\n");
		return;
	} else if (equal(args[0], "put")) {
		if (narg < 2) {
			prf("usage: ~%%put from [to]");
			return;
		}
		if (narg < 3)
			args[2] = args[1];
		if ((f = open(args[1], 0)) < 0) {
			prf("cannot open: %s", args[1]);
			return;
		}
		wrln("stty -echo;cat >");
		wrln(args[2]);
		wrln(";stty echo\n");
		sleep(5);
		intr = 0;
		if (!nhup)
			signal(SIGINT, sig2);
		mode(2);
		rcount = 0;
		while(!intr && rdc(f) == 1) {
			rcount++;
			if (c == tkill || c == terase)
				wrln("\\");
			if (wrc(ln) != 1) {
				sleep(2);
				if (wrc(ln) != 1) {
					prf("character missed");
					intr = 1;
					break;
				}
			}
		}
		signal(SIGINT, SIG_IGN);
		close(f);
		if (intr) {
			wrln("\n");
			prf("stopped after %d bytes", rcount);
		}
		wrln("\004");
		sleep(5);
		mode(1);
		return;
	}
	prf("~%%%s unknown\n", args[0]);
}

equal(s1, s2)
register char *s1, *s2;
{
	while (*s1++ == *s2)
		if (*s2++ == '\0')
			return(1);
	return(0);
}

wrln(s)
register char *s;
{
	while (*s)
		write(ln, s++, 1);
}
/*	chwrsig:  Catch orders from wr process 
 *	to instigate diversion
 */
int whoami;
chwrsig(){
	int dodiver(); 
	whoami = getpid();
	signal(SIGEMT,dodiver);
}
int ds,slnt;
int justrung;
dodiver(){
	static char dobuff[128], morejunk[256]; register char *cp; 
	FILE *fp;
	justrung = 1;
	signal(SIGEMT,dodiver);
	sprintf(dobuff,"/tmp/cu%d",whoami);
	fp = fopen(dobuff,"r");
	if(fp==NULL) prf("Couldn't open temporary");
	unlink(dobuff);
	DEBUG (1, "Name of temporary: %s\n", dobuff);
	fgets(dobuff,128,fp); fclose(fp);
	DEBUG (1, "Name of target file: %s\n", dobuff);
	for(cp = dobuff-1; *++cp; ) /* squash newline */
		if(*cp=='\n') *cp=0;
	cp = dobuff;
	if (*cp=='>') cp++;
	if (*cp==':') {
		cp++;
		if(*cp==0) {
			slnt ^= 1;
			return;
		} else  {
			slnt = 1;
		}
	}
	if (ds >= 0) close(ds);
	if (*cp==0) {
		slnt = 0;
		ds = -1;
		return;
	}
	if (*dobuff!='>' || (ds=open(cp,1))<0) ds=creat(cp,0644);
	lseek(ds, (long)0, 2);
	if(ds < 0) prf("Creat failed:"), prf(cp);
	if (ds<0) prf("Can't divert %s",cp+1);
}


/*
 *	rd: read from remote: line -> 1
 *	catch:
 *	~>[>][:][file]
 *	stuff from file...
 *	~>	(ends diversion)
 */

rd()
{
	extern int ds,slnt;
	char *p,*q,b[600];
	p=b;
	ds=(-1);
agin:
	while (rdc(ln) == 1) {
		if (!slnt) wrc(1);
		if (p < &b[600])
			*p++=c;
		if (c!='\n') continue;
		q=p; 
		p=b;
		if (b[0]!='~' || b[1]!='>') {
			if (*(q-2) == '\r') {
				q--; 
				*(q-1)=(*q);
			}
			if (ds>=0) write(ds,b,q-b);
			continue;
		}
		if (ds>=0) close(ds);
		if (slnt) {
			write(1, b, q - b);
			write(1, CRLF, sizeof(CRLF));
		}
		if (*(q-2) == '\r') q--;
		*(q-1)=0;
		slnt=0;
		q=b+2;
		if (*q == '>') q++;
		if (*q == ':') {
			slnt=1; 
			q++;
		}
		if (*q == 0) {
			ds=(-1); 
			continue;
		}
		if (b[2]!='>' || (ds=open(q,1))<0) ds=creat(q,0644);
		lseek(ds, (long)0, 2);
		if (ds<0) prf("Can't divert %s",b+1);
	}
	if(justrung) {
		justrung = 0;
		goto agin;
	}
}

struct {char lobyte; char hibyte;};
mode(f)
{
	struct sgttyb stbuf;
	if (dout) return;
	ioctl(0, TIOCGETP, &stbuf);
	tkill = stbuf.sg_kill;
	terase = stbuf.sg_erase;
	if (f == 0) {
		stbuf.sg_flags &= ~RAW;
		stbuf.sg_flags |= ECHO|CRMOD;
	}
	if (f == 1) {
		stbuf.sg_flags |= RAW;
		stbuf.sg_flags &= ~(ECHO|CRMOD);
	}
	if (f == 2) {
		stbuf.sg_flags &= ~RAW;
		stbuf.sg_flags &= ~(ECHO|CRMOD);
	}
	ioctl(0, TIOCSETP, &stbuf);
}

echo(s)
char *s;
{
	char *p;
	for (p=s;*p;p++);
	if (p>s) write(0,s,p-s);
	write(0,CRLF, sizeof(CRLF));
}

prf(f, s)
char *f;
char *s;
{
	fprintf(stderr, f, s);
	fprintf(stderr, CRLF);
}

exists(devname)
char *devname;
{
	if (access(devname, 0)==0)
		return(1);
	prf("%s does not exist", devname);
	return(0);
}

cleanup(code)
{
	rmlock(NULL);
	exit(code);
}

/*
 * This code is taken directly from uucp and follows the same
 * conventions.  This is important since uucp and cu should
 * respect each others locks.
 */

	/*  ulockf 3.2  10/26/79  11:40:29  */
/* #include "uucp.h" */
#include <sys/types.h>
#include <sys/stat.h>



/*******
 *	ulockf(file, atime)
 *	char *file;
 *	time_t atime;
 *
 *	ulockf  -  this routine will create a lock file (file).
 *	If one already exists, the create time is checked for
 *	older than the age time (atime).
 *	If it is older, an attempt will be made to unlink it
 *	and create a new one.
 *
 *	return codes:  0  |  FAIL
 */

# define PATHSIZE	256

ulockf(file, atime)
char *file;
time_t atime;
{
	struct stat stbuf;
	time_t ptime;
	int ret;
	static int pid = -1;
	static char tempfile[PATHSIZE];

	if (pid < 0) {
		pid = getpid();
		sprintf(tempfile, "/usr/spool/uucp/LTMP.%d", pid);
	}
	if (onelock(pid, tempfile, file) == -1) {
		DEBUG (7, "ulockf: %s exists\n", tempfile);
		/* lock file exists */
		/* get status to check age of the lock file */
		ret = stat(file, &stbuf);
		if (ret != -1) {
			time(&ptime);
			if ((ptime - stbuf.st_ctime) < atime) {
				/* file not old enough to delete */
				return(FAIL);
			}
		}
		ret = unlink(file);
		ret = onelock(pid, tempfile, file);
		if (ret != 0)
			return(FAIL);
	}
	stlock(file);
	return(0);
}


#define MAXLOCKS 10	/* maximum number of lock files */
char *Lockfile[MAXLOCKS];
int Nlocks = 0;

/***
 *	stlock(name)	put name in list of lock files
 *	char *name;
 *
 *	return codes:  none
 */

stlock(name)
char *name;
{
	char *p;
	extern char *calloc();
	int i;

	for (i = 0; i < Nlocks; i++) {
		if (Lockfile[i] == NULL)
			break;
	}
	ASSERT(i < MAXLOCKS, "TOO MANY LOCKS %d", i);
	if (i >= Nlocks)
		i = Nlocks++;
	p = calloc(strlen(name) + 1, sizeof (char));
	ASSERT(p != NULL, "CAN NOT ALLOCATE FOR %s", name);
	strcpy(p, name);
	Lockfile[i] = p;
	return;
}


/***
 *	rmlock(name)	remove all lock files in list
 *	char *name;	or name
 *
 *	return codes: none
 */

rmlock(name)
char *name;
{
	register int	i;

	for (i = 0; i < Nlocks; i++) {
		if (Lockfile[i] == NULL)
			continue;
		if (name == NULL
		|| strcmp(name, Lockfile[i]) == SAME) {
			unlink(Lockfile[i]);
			free(Lockfile[i]);
			Lockfile[i] = NULL;
		}
	}
	return;
}


/*  this stuff from pjw  */
/*  /usr/pjw/bin/recover - check pids to remove unnecessary locks */
/*	isalock(name) returns 0 if the name is a lock */
/*	unlock(name)  unlocks name if it is a lock*/
/*	onelock(pid,tempfile,name) makes lock a name
	on behalf of pid.  Tempfile must be in the same
	file system as name. */
/*	lock(pid,tempfile,names) either locks all the
	names or none of them */
isalock(name) char *name;
{
	struct stat xstat;
	if(stat(name,&xstat)<0) return(0);
	if(xstat.st_size!=sizeof(int)) return(0);
	return(1);
}
unlock(name) char *name;
{
	if(isalock(name)) return(unlink(name));
	else return(-1);
}
onelock(pid,tempfile,name) char *tempfile,*name;
{	int fd;
	fd=creat(tempfile,0444);
	if(fd<0) return(-1);
	write(fd,(char *) &pid,sizeof(int));
	close(fd);
	if(link(tempfile,name)<0)
	{	unlink(tempfile);
		return(-1);
	}
	unlink(tempfile);
	return(0);
}
lock(pid,tempfile,names) char *tempfile,**names;
{	int i,j;
	for(i=0;names[i]!=0;i++)
	{	if(onelock(pid,tempfile,names[i])==0) continue;
		for(j=0;j<i;j++) unlink(names[j]);
		return(-1);
	}
	return(0);
}

#define LOCKPRE "/usr/spool/uucp/LCK."

/***
 *	delock(s)	remove a lock file
 *	char *s;
 *
 *	return codes:  0  |  FAIL
 */

delock(s)
char *s;
{
	char ln[30];

	sprintf(ln, "%s.%s", LOCKPRE, s);
	rmlock(ln);
}


/***
 *	mlock(sys)	create system lock
 *	char *sys;
 *
 *	return codes:  0  |  FAIL
 */

mlock(sys)
char *sys;
{
	char lname[30];

	DEBUG (8, "mlock %s\n", sys);

	sprintf(lname, "%s.%s", LOCKPRE, sys);
	return(ulockf(lname, (time_t) SLCKTIME ) < 0 ? FAIL : 0);
}



/***
 *	ultouch()	update access and modify times for lock files
 *
 *	return code - none
 */

ultouch()
{
	time_t time();
	int i;
	struct ut {
		time_t actime;
		time_t modtime;
	} ut;

	ut.actime = time(&ut.modtime);
	for (i = 0; i < Nlocks; i++) {
		if (Lockfile[i] == NULL)
			continue;
		utime(Lockfile[i], &ut);
	}
	return;
}
