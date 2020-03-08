/*
 * w.c: version 1.10 of 5/31/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)w.c	1.10 (NSC) 5/31/83";
# endif

/*
 * w - print system status (who and what)
 *
 * This program is similar to the systat command on Tenex/Tops 10/20
 * It needs read permission on /dev/kmem, and the vmap system call.
 */
#include <sys/param.h>
#include <nlist.h>
#include <stdio.h>
#include <ctype.h>
#include <utmp.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>

#define NMAX sizeof(utmp.ut_name)
#define LMAX sizeof(utmp.ut_line)

#define ARGWIDTH	28	/* # chars left on 75 col crt for args */

struct pr {
	short	w_pid;			/* proc.p_pid */
	char	w_flag;			/* proc.p_flag */
	short	w_size;			/* proc.p_size */
	int	w_igintr;		/* INTR+3*QUIT, 0=die, 1=ign, 2=catch */
	time_t	w_time;			/* CPU time used by this process */
	time_t	w_ctime;		/* CPU time used by children */
	dev_t	w_tty;			/* tty device of process */
	char	w_comm[15];		/* user.u_comm, null terminated */
	char	w_args[ARGWIDTH+1];	/* args if interesting process */
} *pr;
int	nproc;

struct	nlist nl[] = {
	{ "_proc" },
#define	X_PROC		0
	{ "_nproc" },
#define	X_NPROC		1
	{ "_u" },
#define	X_U		2
	{ "_bootime" },
#define	X_BOOTIME	3
	{ "_avenrun" },
#define	X_AVENRUN	4
	{ 0 },
};

FILE	*ps;
FILE	*ut;
FILE	*bootfd;
int	kmem;
dev_t	tty;
char	doing[520];		/* process attached to terminal */
time_t	proctime;		/* cpu time of process in doing */
/*double*/ long	avenrun[3];	/* no floating point */
struct	proc *aproc;

#define	DIV60(t)	((t+30)/60)    /* x/60 rounded */ 
#define	TTYEQ		(tty == pr[i].w_tty)
#define IGINT		(1+3*1)		/* ignoring both SIGINT & SIGQUIT */

char	*getargs();
char	*fread();
char	*ctime();
char	*rindex();
FILE	*popen();
struct	tm *localtime();

int	debug;			/* true if -d flag: debugging output */
int	header = 1;		/* true if -h flag: don't print heading */
int	lflag = 1;		/* true if -l flag: long style output */
int	login;			/* true if invoked as login shell */
int	idle;			/* number of minutes user is idle */
int	nusers;			/* number of users logged in now */
char *	sel_user;		/* login of particular user selected */
char firstchar;			/* first char of name of prog invoked as */
time_t	jobtime;		/* total cpu time visible */
time_t	now;			/* the current time of day */
struct	tm *nowt;		/* current time as time struct */
time_t	bootime, uptime;	/* time of last reboot & elapsed time since */
int	np;			/* number of processes currently active */
struct	utmp utmp;
struct	proc mproc;
struct	user up;
char	fill[512];

main(argc, argv)
	char **argv;
{
	int days, hrs, mins;
	register int i, j;
	char *cp;
	register int curpid, empty;
	char obuf[BUFSIZ];

	setbuf(stdout, obuf);
	login = (argv[0][0] == '-');
	cp = rindex(argv[0], '/');
	firstchar = login ? argv[0][1] : (cp==0) ? argv[0][0] : cp[1];
	cp = argv[0];	/* for Usage */

	while (argc > 1) {
		if (argv[1][0] == '-') {
			for (i=1; argv[1][i]; i++) {
				switch(argv[1][i]) {

				case 'd':
					debug++;
					break;

				case 'h':
					header = 0;
					break;

				case 'l':
					lflag++;
					break;

				case 's':
					lflag = 0;
					break;

				case 'u':
				case 'w':
					firstchar = argv[1][i];
					break;

				default:
					printf("Bad flag %s\n", argv[1]);
					exit(1);
				}
			}
		} else {
			if (!isalnum(argv[1][0]) || argc > 2) {
				printf("Usage: %s [ -hlsuw ] [ user ]\n", cp);
				exit(1);
			} else
				sel_user = argv[1];
		}
		argc--; argv++;
	}

	if ((kmem = open("/dev/kmem", 0)) < 0) {
		fprintf(stderr, "No kmem\n");
		exit(1);
	}
	cnlist("/vmsymbols", nl);
	if (nl[0].n_type==0) {
		fprintf(stderr, "No namelist\n");
		exit(1);
	}

	if (firstchar != 'u')
		readpr();

	ut = fopen("/etc/utmp","r");
	if (header) {
		/* Print time of day */
		time(&now);
		nowt = localtime(&now);
		prtat(nowt);

		/*
		 * Print how long system has been up.
		 * (Found by looking for "bootime" in kernel)
		 */
		lseek(kmem, (long)nl[X_BOOTIME].n_value, 0);
		read(kmem, &bootime, sizeof (bootime));

		uptime = now - bootime;
		days = uptime / (60*60*24);
		uptime %= (60*60*24);
		hrs = uptime / (60*60);
		uptime %= (60*60);
		mins = DIV60(uptime);

		printf("  up");
		if (days > 0)
			printf(" %d day%s,", days, days>1?"s":"");
		if (hrs > 0 && mins > 0) {
			printf(" %2d:%02d,", hrs, mins);
		} else {
			if (hrs > 0)
				printf(" %d hr%s,", hrs, hrs>1?"s":"");
			if (mins > 0)
				printf(" %d min%s,", mins, mins>1?"s":"");
		}

		/* Print number of users logged in to system */
		while (fread(&utmp, sizeof(utmp), 1, ut)) {
			if (utmp.ut_name[0] != '\0')
				nusers++;
		}
		rewind(ut);
		printf("  %d user%s", nusers, (nusers != 1) ? "s" : "");

		/*
		 * Print 1, 5, and 15 minute load averages.
		 * (Found by looking in kernel for avenrun).
		 */
		printf(",  load average:");
		lseek(kmem, (long)nl[X_AVENRUN].n_value, 0);
		read(kmem, avenrun, sizeof(avenrun));
		for (i = 0; i < (sizeof(avenrun)/sizeof(avenrun[0])); i++) {
			if (i > 0)
				printf(",");
			/*printf(" %.2f", avenrun[i]);*/
			printavenrun(avenrun[i]);	/* no floating point */
		}
		printf("\n");
		if (firstchar == 'u')
			exit(0);

		/* Headers for rest of output */
		if (lflag)
			printf("User     tty       login@  idle   JCPU   PCPU  what\n");
		else
			printf("User    tty  idle  what\n");
		fflush(stdout);
	}


	for (;;) {	/* for each entry in utmp */
		if (fread(&utmp, sizeof(utmp), 1, ut) == NULL) {
			fclose(ut);
			exit(0);
		}
		if (utmp.ut_name[0] == '\0')
			continue;	/* that tty is free */
		if (sel_user && strcmpn(utmp.ut_name, sel_user, NMAX) != 0)
			continue;	/* we wanted only somebody else */

		gettty();
		jobtime = 0;
		proctime = 0;
		strcpy(doing, "-");	/* default act: normally never prints */
		empty = 1;
		curpid = -1;
		idle = findidle();
		for (i=0; i<np; i++) {	/* for each process on this tty */
			if (!(TTYEQ))
				continue;
			jobtime += pr[i].w_time + pr[i].w_ctime;
			proctime += pr[i].w_time;
			if (debug) {
				printf("\t\t%d\t%s", pr[i].w_pid, pr[i].w_args);
				if ((j=pr[i].w_igintr) > 0)
					if (j==IGINT)
						printf(" &");
					else
						printf(" & %d %d", j%3, j/3);
				printf("\n");
			}
			if (empty && pr[i].w_igintr!=IGINT) {
				empty = 0;
				curpid = -1;
			}
			if(pr[i].w_pid>curpid && (pr[i].w_igintr!=IGINT || empty)){
				curpid = pr[i].w_pid;
				strcpy(doing, lflag ? pr[i].w_args : pr[i].w_comm);
#ifdef notdef
				if (doing[0]==0 || doing[0]=='-' && doing[1]<=' ' || doing[0] == '?') {
					strcat(doing, " (");
					strcat(doing, pr[i].w_comm);
					strcat(doing, ")");
				}
#endif
			}
		}
		putline();
	}
}

/* figure out the major/minor device # pair for this tty */
gettty()
{
	char ttybuf[20];
	struct stat statbuf;

	ttybuf[0] = 0;
	strcpy(ttybuf, "/dev/");
	strcat(ttybuf, utmp.ut_line);
	stat(ttybuf, &statbuf);
	tty = statbuf.st_rdev;
}

/*
 * putline: print out the accumulated line of info about one user.
 */
putline()
{
	register int tm;

	/* print login name of the user */
	printf("%-*.*s ", NMAX, NMAX, utmp.ut_name);

	/* print tty user is on */
	if (lflag)
		/* long form: all (up to) LMAX chars */
		printf("%-*.*s", LMAX, LMAX, utmp.ut_line);
	else {
		/* short form: 2 chars, skipping 'tty' if there */
		if (utmp.ut_line[0]=='t' && utmp.ut_line[1]=='t' && utmp.ut_line[2]=='y')
			printf("%-2.2s", &utmp.ut_line[3]);
		else
			printf("%-2.2s", utmp.ut_line);
	}

	if (lflag)
		/* print when the user logged in */
		prtat(localtime(&utmp.ut_time));

	/* print idle time */
	prttime(idle," ");

	if (lflag) {
		/* print CPU time for all processes & children */
		prttime(DIV60(jobtime)," ");
		/* print cpu time for interesting process */
		prttime(DIV60(proctime)," ");
	}

	/* what user is doing, either command tail or args */
	printf(" %-.32s\n",doing);
	fflush(stdout);
}

/* find & return number of minutes current tty has been idle */
findidle()
{
	struct stat stbuf;
	long lastaction, diff;
	char ttyname[20];

	strcpy(ttyname, "/dev/");
	strcatn(ttyname, utmp.ut_line, LMAX);
	stat(ttyname, &stbuf);
	time(&now);
	lastaction = stbuf.st_atime;
	diff = now - lastaction;
	diff = DIV60(diff);
	if (diff < 0) diff = 0;
	return(diff);
}

/*
 * prttime prints a time in hours and minutes.
 * The character string tail is printed at the end, obvious
 * strings to pass are "", " ", or "am".
 */
prttime(tim, tail)
	time_t tim;
	char *tail;
{
	register int didhrs = 0;

	if (tim >= 60) {
		printf("%3d:", tim/60);
		didhrs++;
	} else {
		printf("    ");
	}
	tim %= 60;
	if (tim > 0 || didhrs) {
		printf(didhrs&&tim<10 ? "%02d" : "%2d", tim);
	} else {
		printf("  ");
	}
	printf("%s", tail);
}

/* prtat prints a 12 hour time given a pointer to a time of day */
prtat(p)
	struct tm *p;
{
	register int t, pm;

	t = p -> tm_hour;
	pm = (t > 11);
	if (t > 11)
		t -= 12;
	if (t == 0)
		t = 12;
	prttime(t*60 + p->tm_min, pm ? "pm" : "am");
}

/*
 * readpr finds and reads in the array pr, containing the interesting
 * parts of the proc and user tables for each live process.
 */
readpr()
{
	int pn, mf, addr, c;
	int szpt, pfnum, i, aproc;

	/*
	 * Locate proc table
	 */
	lseek(kmem, (long)nl[X_NPROC].n_value, 0);
	read(kmem, &nproc, sizeof(nproc));
	lseek(kmem, (long)nl[X_PROC].n_value, 0);
	read(kmem, &aproc, sizeof(aproc));
	pr = (struct pr *)calloc(nproc, sizeof (struct pr));
	np = 0;
	for (pn=0; pn<nproc; pn++) {
		lseek(kmem, aproc + (pn * sizeof(mproc)), 0);
		read(kmem, &mproc, sizeof mproc);
		/* decide if it's an interesting process */
		if (mproc.p_stat==0 || mproc.p_pgrp==0)
			continue;
		if (mproc.p_flag&SDETACH)
			continue;
		/* find & read in the user structure */
		lseek(kmem, (long)nl[X_U].n_value + ((pn + 1) * USIZE), 0);
		if (read(kmem, &up, sizeof(up)) != sizeof(up)) {
			continue;
		}
		if (up.u_ttyp == NULL)
			continue;

		/* save the interesting parts */
		pr[np].w_pid = mproc.p_pid;
		pr[np].w_flag = mproc.p_flag;
		pr[np].w_size = mproc.p_tdsize;
		pr[np].w_igintr = (((int)up.u_signal[2]==1) + 2*((int)up.u_signal[2]>1) + 3*((int)up.u_signal[3]==1)) + 6*((int)up.u_signal[3]>1);
		pr[np].w_time = up.u_vm.vm_utime + up.u_vm.vm_stime;
		pr[np].w_ctime = up.u_cvm.vm_utime + up.u_cvm.vm_stime;
		pr[np].w_tty = up.u_ttyd;
		up.u_comm[14] = 0;	/* Bug: This bombs next field. */
		strcpy(pr[np].w_comm, up.u_comm);
		/*
		 * Get args if there's a chance we'll print it.
		 * Cant just save pointer: getargs returns static place.
		 * Cant use strcpyn: that crock blank pads.
		 */
		pr[np].w_args[0] = 0;
		strcatn(pr[np].w_args,getargs(&pr[np]),ARGWIDTH);
		if (pr[np].w_args[0]==0 || pr[np].w_args[0]=='-' && pr[np].w_args[1]<=' ' || pr[np].w_args[0] == '?') {
			strcat(pr[np].w_args, " (");
			strcat(pr[np].w_args, pr[np].w_comm);
			strcat(pr[np].w_args, ")");
		}
		np++;
	}
}

/*
 * getargs: given a pointer to a proc structure, this tries to reconstruct
 * the arguments. This is straight out of ps.
 */
char *
getargs(p)
	struct pr *p;
{
	int c, addr, nbad;
	static int abuf[MCSIZE/sizeof(int)];
	register int *ip;
	register char *cp, *cp1;

	if (vmap(1, abuf, TOPUSER - MCSIZE, p->w_pid)) {
		return(p->w_comm);
	}
	abuf[sizeof(abuf)/sizeof(abuf[0])-1] = 0;
	for (ip = &abuf[sizeof(abuf)/sizeof(abuf[0])-2]; ip > abuf;) {
		/* Look from top for -1 or 0 as terminator flag. */
		if (*--ip == -1 || *ip == 0) {
			cp = (char *)(ip+1);
			if (*cp==0)
				cp++;
			nbad = 0;	/* up to 5 funny chars as ?'s */
			for (cp1 = cp; cp1 < (char *)&abuf[sizeof(abuf)/sizeof(abuf[0])]; cp1++) {
				c = *cp1&0177;
				if (c==0)  /* nulls between args => spaces */
					*cp1 = ' ';
				else if (c < ' ' || c > 0176) {
					if (++nbad >= 5) {
						*cp1++ = ' ';
						break;
					}
					*cp1 = '?';
				} else if (c=='=') {	/* Oops - found an
							 * environment var, back
							 * over & erase it. */
					*cp1 = 0;
					while (cp1>cp && *--cp1!=' ')
						*cp1 = 0;
					break;
				}
			}
			while (*--cp1==' ')	/* strip trailing spaces */
				*cp1 = 0;
			return(cp);
		}
	}
	return (p->w_comm);
}


/* Temporary routine to print fake floating point numbers for load averages */
#define	LASCALE 1000	/* scaling factor (1000 = load average of 1.00) */
printavenrun(n)
	register long	n;
{
	n += 5;				/* round up */
	printf(" %d.", n / LASCALE);	/* integer part */
	n = (n % LASCALE) * 10;
	printf("%d", n / LASCALE);	/* tenths */
	n = (n % LASCALE) * 10;
	printf("%d", n / LASCALE);	/* hundreths */
}
