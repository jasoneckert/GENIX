# ifndef NOSCCS
static char *sccsid = "@(#)login.c	1.7	8/31/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

/*
 * login [ name ]
 */

#include <sys/types.h>
#include <sgtty.h>
#include <utmp.h>
#include <signal.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <lastlog.h>

#define	SCPYN(a, b)	strncpy(a, b, sizeof(a))

#define NMAX sizeof(utmp.ut_name)
#define LMAX sizeof(utmp.ut_line)

#define	FALSE	0
#define	TRUE	-1

char	nolog[] =	"/etc/nologin";
char	qlog[]  =	".hushlogin";
char	securetty[] =	"/etc/securetty";
char	maildir[30] =	"/usr/spool/mail/";
char	lastlog[] =	"/usr/adm/lastlog";
struct	passwd nouser = {"", "nope"};
struct	sgttyb ttyb;
struct	utmp utmp;
char	minusnam[16] = "-";
char	homedir[64] = "HOME=";
char	shell[64] = "SHELL=";
char	term[64] = "TERM=";
char	user[20] = "USER=";
char	*envinit[] = {homedir, shell, "PATH=:/usr/nsc:/usr/ucb:/bin:/usr/bin", term, user, 0};
struct	passwd *pwd;

struct	passwd *getpwnam();
char	*strcat();
int	setpwent();
char	*ttyname();
char	*crypt();
char	*getpass();
char	*rindex();
char	*stypeof();
extern	char **environ;

#define	CTRL(c)	('c'&037)
#define	CERASE	CTRL(h)
#define	CEOT	CTRL(d)
#define	CKILL	CTRL(u)
#define	CQUIT	034		/* FS, cntl shift L */
#define	CINTR	0177		/* DEL */
#define	CSTOP	CTRL(s)
#define	CSTART	CTRL(q)
#define	CBRK	0377

struct	tchars tc = {
	CINTR, CQUIT, CSTART, CSTOP, CEOT, CBRK
};

struct	ltchars ltc = {
	CTRL(z), CTRL(y), CTRL(r), CTRL(o), CTRL(w), CTRL(v), CTRL(t),
};

main(argc, argv)
char **argv;
{
	register char *namep;
	int t, f, c;
	int invalid;
	int quietlog;
	int i;
	FILE *nlfd;
	char *ttyn;
	int	tiol;
	int	ldisc = 0;

#	define	HNSIZE 32
	int	hnbufsz = HNSIZE;
	char	hnbuf[HNSIZE];

	alarm(60);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	nice(-100);
	nice(20);
	nice(0);

	/*
	 * Set up terminal mode.
	 *	The value given the local mode word
	 *	assumes that CERASE == '\b'.
	 */
	ioctl (0, TIOCNXCL, 0);
	ioctl (0, TIOCGETP, &ttyb);
	ttyb.sg_erase = CERASE;
	ttyb.sg_kill  = CKILL;
	ioctl (0, TIOCSETP, &ttyb);
	tiol = (ttyb.sg_ospeed > B300)
		? LCRTBS | LCRTERA | LCRTKIL | LCTLECH | LDECCTQ
		: LPRTERA | LCTLECH;
	ioctl (0, TIOCLSET, &tiol);
	ioctl (0, TIOCSETC, &tc);
	ioctl (0, TIOCSLTC, &ltc);

	for (t=3; t<20; t++)
		close(t);
	ttyn = ttyname(0);
	if (ttyn==(char *)0)
		ttyn = "/dev/tty??";

	gethostname (hnbuf, &hnbufsz);

	do {
		ldisc = 0;
		ioctl(0, TIOCSETD, &ldisc);
		invalid = FALSE;
		SCPYN(utmp.ut_name, "");
		if (argc>1) {
			SCPYN(utmp.ut_name, argv[1]);
			argc = 0;
		}
		while (utmp.ut_name[0] == '\0') {
			namep = utmp.ut_name;
			/* Put out login banner, using hostname if present: */
			if (hnbufsz > 0)
				printf ("%s login: ", hnbuf);
			else
				printf("login: ");
			while ((c = getchar()) != '\n') {
				if (c == ' ')
					c = '_';
				if (c == EOF)
					exit(0);
				if (namep < utmp.ut_name+NMAX)
					*namep++ = c;
			}
		}
		setpwent();
		if ((pwd = getpwnam(utmp.ut_name)) == NULL)
			pwd = &nouser;
		endpwent();
		if (!strcmp(pwd->pw_shell, "/bin/csh")) {
			ldisc = NTTYDISC;
			ioctl(0, TIOCSETD, &ldisc);
		}
		if (*pwd->pw_passwd != '\0') {
			nice(-4);
			namep = crypt(getpass("Password:"),pwd->pw_passwd);
			nice(4);
			if (strcmp(namep, pwd->pw_passwd))
				invalid = TRUE;
		}
		if (pwd->pw_uid != 0 && (nlfd = fopen(nolog, "r")) > 0) {
			/* logins are disabled except for root */
			while ((c = getc(nlfd)) != EOF)
				putchar(c);
			fflush(stdout);
			sleep(5);
			exit(0);
		}
		if (!invalid && pwd->pw_uid == 0 &&
		    !rootterm(ttyn+sizeof("/dev/")-1)) {
			FILE *console = fopen("/dev/console", "w");
			if (console != NULL) {
				fprintf(console, "\r\nROOT LOGIN REFUSED %s\r\n"
				    , ttyn+sizeof("/dev/")-1
				);
				fclose(console);
			}
			invalid = TRUE;
		}
		if (invalid) {
			printf("Login incorrect\n");
			if (ttyn[sizeof("/dev/tty")-1] == 'd') {
				FILE *console = fopen("/dev/console", "w");
				if (console != NULL) {
					fprintf(console, "\r\nBADDIALUP %s %s\r\n"
					    , ttyn+sizeof("/dev/")-1
					    , utmp.ut_name);
					fclose(console);
				}
			}
		}
		if (*pwd->pw_shell == '\0')
			pwd->pw_shell = "/bin/sh";
		i = strlen(pwd->pw_shell);
		if (chdir(pwd->pw_dir) < 0 && !invalid ) {
			if (chdir("/") < 0) {
				printf("No directory!\n");
				invalid = TRUE;
			} else {
				printf("No directory!  Logging in with home=/\n");
				pwd->pw_dir = "/";
			}
		}
	} while (invalid);

	time(&utmp.ut_time);
	t = ttyslot();
	if (t>0 && (f = open("/etc/utmp", 1)) >= 0) {
		lseek(f, (long)(t*sizeof(utmp)), 0);
		SCPYN(utmp.ut_line, rindex(ttyn, '/')+1);
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	if (t>0 && (f = open("/usr/adm/wtmp", 1)) >= 0) {
		lseek(f, 0L, 2);
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	quietlog = FALSE;
	if (access(qlog, 0) == 0)
		quietlog = TRUE;
	if ( !quietlog && (f = open(lastlog, 2)) >= 0 ) {
		struct lastlog ll;

		lseek(f, (long)pwd->pw_uid * sizeof (struct lastlog), 0);
		if (read(f, (char *) &ll, sizeof ll) == sizeof ll &&
		    ll.ll_time != 0) {
			printf("Last login: %.*s on %.*s\n"
			    , 24-5
			    , (char *) ctime(&ll.ll_time)
			    , sizeof(ll.ll_line)
			    , ll.ll_line
			);
		}
		lseek(f, (long)pwd->pw_uid * sizeof (struct lastlog), 0);
		time(&ll.ll_time);
		SCPYN(ll.ll_line, rindex(ttyn, '/')+1);
		write(f, (char *) &ll, sizeof ll);
		close(f);
	}
	chown(ttyn, pwd->pw_uid, pwd->pw_gid);
	setgid(pwd->pw_gid);
	setuid(pwd->pw_uid);
	environ = envinit;
	strncat(homedir, pwd->pw_dir, sizeof(homedir)-6);
	strncat(shell, pwd->pw_shell, sizeof(shell)-7);
	strncat(term, stypeof(ttyn), sizeof(term)-6);
	strncat(user, pwd->pw_name, sizeof(user)-6);
	if ((namep = rindex(pwd->pw_shell, '/')) == NULL)
		namep = pwd->pw_shell;
	else
		namep++;
	strcat(minusnam, namep);
	alarm(0);
#ifdef ARPAVAX
	if (pwd->pw_gid == 27)			/* UGLY ! */
		umask(2);
	else
#endif
		umask(022);
	if (ttyn[sizeof("/dev/tty")-1] == 'd') {
		FILE *console = fopen("/dev/console", "w");
		if (console != NULL) {
			fprintf(console, "\r\nDIALUP %s %s\r\n"
			    , ttyn+sizeof("/dev/")-1
			    , pwd->pw_name
			);
			fclose(console);
		}
	}
	if ( !quietlog ) {
		showmotd();
		strcat(maildir, pwd->pw_name);
		if (access(maildir,4)==0) {
			struct stat statb;
			stat(maildir, &statb);
			if (statb.st_size)
				printf("You have mail.\n");
		}
	}
	
	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGTSTP, SIG_IGN);
	execlp(pwd->pw_shell, minusnam, 0);
	perror(pwd->pw_shell);
	printf("No shell\n");
	exit(0);
}

int	stopmotd;
catch()
{
	signal(SIGINT, SIG_IGN);
	stopmotd++;
}

/*
 * return true if OK for root to login on this terminal
 */
rootterm(tty)
	char	*tty;
{
	register FILE *fd;
	char	buf[100];

	if ((fd = fopen(securetty, "r")) == NULL)
		return(1);
	while (fgets(buf, sizeof buf, fd) != NULL) {
		buf[strlen(buf)-1] = '\0';
		if (strcmp(tty, buf) == 0) {
			fclose(fd);
			return(1);
		}
	}
	fclose(fd);
	return(0);
}

showmotd()
{
	FILE *mf;
	register c;

	signal(SIGINT, catch);
	if ((mf = fopen("/etc/motd","r")) != NULL) {
		while ((c = getc(mf)) != EOF && stopmotd == 0)
			putchar(c);
		fclose(mf);
	}
	signal(SIGINT, SIG_IGN);
}

#undef	UNKNOWN
#define UNKNOWN "su"

char *
stypeof(ttyid)
char	*ttyid;
{
	static char	typebuf[16];
	char		buf[50];
	register FILE	*f;
	register char	*p, *t, *q;

	if (ttyid == NULL)
		return (UNKNOWN);
	f = fopen("/etc/ttytype", "r");
	if (f == NULL)
		return (UNKNOWN);
	/* split off end of name */
	for (p = q = ttyid; *p != 0; p++)
		if (*p == '/')
			q = p + 1;

	/* scan the file */
	while (fgets(buf, sizeof buf, f) != NULL)
	{
		for (t=buf; *t!=' ' && *t != '\t'; t++)
			;
		*t++ = 0;
		while (*t == ' ' || *t == '\t')
			t++;
		for (p=t; *p>' '; p++)
			;
		*p = 0;
		if (strcmp(q,t)==0) {
			strcpy(typebuf, buf);
			fclose(f);
			return (typebuf);
		}
	}
	fclose (f);
	return (UNKNOWN);
}
