
/*
 * cu16.c: version 1.2 of 3/2/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)cu16.c	1.2 (NSC) 3/2/83";
# endif


#define CTRLZ	26
#include <stdio.h>
#include <signal.h>
#include <sgtty.h>
/*
 *	Escape with `~' at beginning of line.
 *	Ordinary diversions are ~<, ~> and ~>>.
 *	Silent output diversions are ~>: and ~>>:.
 *	Terminate output diversion with ~> alone.
 *	Quit is ~. and ~! gives local command or shell.
 *	Also ~$ for canned procedure pumping remote.
 *	~%put from [to]  and  ~%take from [to] invoke builtins
 */

/*
 * new stuff
 *
 *	no echoing
 *	cr-lf and lf-cr are turned into a single newline 
 */

#define CRLF "\r\n"
#define wrc(ds) write(ds,&c,1)


char	*devcul	= "/dev/ddtline";
char	*lspeed	= "9600";

int	ln;	/* fd for comm line */
char	tkill, terase;	/* current input kill & erase */
int	efk;		/* process of id of listener  */
char	c;
char	oc;
int ucase = 0, recho = 0;

char	*connmsg[] = {
	"",
	"line busy",
	"call dropped",
	"no carrier",
	"can't fork",
	"acu access",
	"tty access",
	"tty hung",
	"usage: cu16 "
};

char lastc=0;

rdc(ds)
{
	ds = read(ds,&c,1);
	oc = c;
	c &= 0177;
	return ds;
}

xyrdc(ds)
{
	rdc(ds);
}

xrdc(ds)
{
int	q;
char	tmpc;

reread:
	q = read(ds,&tmpc,1); 
	if((tmpc=='\n' && lastc=='\r') || (tmpc=='\r' && lastc=='\n')) {
		lastc = 0;
		goto reread;
	}
	lastc = tmpc;
	if(tmpc=='\r')
		tmpc = '\n';
	oc = c = tmpc;
	c &= 0177;
	return (q);
}

int intr;

sig2()
{
	signal(SIGINT, SIG_IGN); 
	intr = 1;
}

int set14;

xsleep(n)
{
	xalarm(n);
	pause();
	xalarm(0);
}

xalarm(n)
{
	set14=n; 
	alarm(n);
}

sig14()
{
	signal(SIGALRM, sig14); 
	if (set14) alarm(1);
}

int	dout;
int	nhup;
int	dbflag;

/*
 *	main: get connection, set speed for line.
 *	fork child to invoke rd to read from line, output to fd 1
 *	main line invokes wr to read tty, write to line
 */
main(ac,av)
char **av;
{
	int fk;
	int speed;
	char *new_cul;
	struct sgttyb stbuf;
	char *getenv();

	if (ac > 1)
		for (; ac > 1; av++,ac--) {
			if (av[1][0] == '-')
			switch(av[1][1]) {
			case 's':
				lspeed = av[2]; ++av; --ac;
				break;
			default:
				prf("Bad flag %s", av[1]);
				break;
			}
		}
	signal(SIGALRM, sig14);
	if (new_cul = getenv("DDTPORT"))
	{
		printf("Port %s being used\n",new_cul);
		fflush(stdout);
		devcul = new_cul;
	}
	ln = conn(devcul);
	if (ln <= 0) {
		prf("Connect failed: %s",connmsg[-ln]);
		exit(-ln);
	}
	switch(atoi(lspeed)) {
	case 110:
		speed = B110;break;
	case 150:
		speed = B150;break;
	case 300:
		speed = B300;break;
	case 600:
		speed = B600;break;
	case 1200:
		speed = B1200;break;
	case 2400:
		speed = B2400;break;
	case 4800:
		speed = B4800;break;
	default:
	case 9600:
		speed = B9600;break;
	}
	stbuf.sg_ispeed = speed;
	stbuf.sg_ospeed = speed;
	stbuf.sg_flags = EVENP|ODDP|TANDEM;
	stbuf.sg_flags |= RAW;
	ioctl(ln, TIOCSETP, &stbuf);
	ioctl(ln, TIOCEXCL, (struct sgttyb *)NULL); 
	ioctl(ln, TIOCHPCL, (struct sgttyb *)NULL);
	prf("Connected");
	fk = fork();
	nhup = (int)signal(SIGINT, SIG_IGN);
	if (fk == 0) {
		chwrsig();
		rd();
		prf("\007Lost carrier");
		exit(3);
	}
	mode(1);
	efk = fk;
	wr();
	mode(0);
	if(fk > 0)kill(fk, SIGKILL);
	wait((int *)NULL);
	stbuf.sg_ispeed = 0;
	stbuf.sg_ospeed = 0;
	ioctl(ln, TIOCSETP, &stbuf);
	prf("Disconnected");
	exit(0);
}

/*
 *	conn: establish connection.
 *	Example:  fd = conn("/dev/ttyh");
 *	Returns descriptor open to tty for reading and writing.
 *	Negative values (-1...-7) denote errors in connmsg.
 *	Uses alarm and fork/wait; requires sig14 handler.
 *	Be sure to disconnect tty when done, via HUPCL or stty 0.
 */

conn(dev)
char *dev;
{
	struct sgttyb stbuf;
	extern errno;
	char *p, *q, b[30];
	int er, fk, dn, dh, t;
	er=0; 
	fk=(-1);
	if ((fk=fork()) == (-1)) {
		er=4; 
		goto X;
	}
	if (fk == 0) {
		if (open(dev,2) < 0) {
			printf("can't open %s\n",dev);
			fflush(stdout);
		}
		for (;;) pause();
	}
	xsleep(2);
	dh = open(dev,2);
	xalarm(0);
	if (dh<0) {
		er=(errno == 4? 3:6); 
		goto X;
	}
	ioctl(dh, TIOCGETP, &stbuf);
	stbuf.sg_flags &= ~ECHO;
	xalarm(10);
	ioctl(dh, TIOCSETP, &stbuf);
	ioctl(dh, TIOCHPCL, (struct sgttyb *)NULL);
	xalarm(0);
X: 
	if (fk!=(-1)) {
		kill(fk, SIGKILL);
		xalarm(10);
		while ((t=wait((int *)NULL))!=(-1) && t!=fk);
		xalarm(0);
	}
	return (er? -er:dh);
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
 *	~b	load downloader code
 *	~d file	down load code to downloader
 *	~e	toggle echo/no echo
 *	~c	toggle case switch
 */

wr()
{
	int ds,fk,lcl,x;
	char *p,b[600];
	struct sgttyb stbuf,rst_stbuf;
	for (;;) {
		p=b;
		while (rdc(0) == 1) {
			if (p == b) lcl=(c == '~');
			if (p == b+1 && b[0] == '~') lcl=(c!='~');
			if (!lcl) {
				if (c == CTRLZ) goto controlz;
				if (ucase && (oc>= 'a' && oc <= 'z'))
					oc -= 'a' - 'A';
				c = oc;
				if (wrc(ln) == 0) {
					prf("line gone"); return;
				} else if (recho)
					write(0,&oc,1);
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
			else 
				echo("$");
			
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
			if(dbflag) prf("name to be written in temporary:"),prf(&b[2]);
			fclose(fp);
			kill(efk,SIGEMT);
			}
			break;
		case CTRLZ:
controlz:
			mode(0);
			kill(getpid(), SIGTSTP);
			mode(1);
			break;
		case '%':
			dopercen(&b[2]);
			break;
		case 'b':
			prf("boot load");
			/*	developement board binary boot from ddo	*/
			wrln("L8300 EFA40000F0005FA8C00083F4E7ADC00015\r");
			xsleep(1);
			wrln("L8310 83F40231E7ADC00083F502295F285CA82C\r");
			xsleep(1);
			wrln("L8320 C00083F0E7E5ADC0008400021840E5ADDC\r");
			xsleep(1);
			wrln("L8330 C0008400C00083F08F28472DC00083F4D9\r");
			xsleep(1);
			wrln("L8340 1A64F274A501C0C000029A7914A8C0C05B\r");
			xsleep(1);
			wrln("L8350 0000540404001204000000000000000072\r");
			xsleep(1);
			wrln("CPC=8300\r");
			break;

		case 'c':
			ucase = ucase?0:1;
			break;
		case 'e':
			recho = recho?0:1;
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
		xsleep(5);
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
				xsleep(2);
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
		xsleep(5);
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
	if(dbflag) {
		prf("Name of temporary:");
		prf(dobuff);
	}
	fgets(dobuff,128,fp); fclose(fp);
	if(dbflag) {
		prf("Name of target file:");
		prf(dobuff);
	}
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
	char b[600];
	register char *p,*q;
	p=b;
	ds=(-1);
agin:
	while (xrdc(ln) == 1) {
		if (!slnt)
			wrc(1);
		*p++=c;
		if (c!='\n')
			continue;
		write(1, "\r", 1);
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
		stbuf.sg_flags &= ~LCASE;
	}
	if (f == 1) {
		stbuf.sg_flags |= RAW;
		stbuf.sg_flags &= ~(CRMOD|ECHO);
	}
	if (f == 2) {
		stbuf.sg_flags &= ~RAW;
		stbuf.sg_flags &= ~(CRMOD);
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
