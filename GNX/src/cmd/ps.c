/*
 * ps.c: version 1.22 of 10/3/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)ps.c	1.22 (NSC) 10/3/83";
# endif

/*
 * ps: show status of processes
 */

#include <stdio.h>
#include <ctype.h>
#include <nlist.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/tty.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vm.h>
#include <sys/pte.h>
#include <sys/stat.h>
#include <math.h>
#include <sys/vlimit.h>

#define	double	long		/* TEMPORARY, NO FLOATING POINT */
#define	float	long
#define	clrnd(a) (a)	
#define	ctopt(a) (a)
#define	AVENSCALE 1000		/* scale factor for fake floating point */

struct nlist nl[] = {
	{ "_proc" },
#define	X_PROC		0
	{ "_nproc" },
#define	X_NPROC		1
	{ "_u" },
#define	X_U		2
	{ "_spt" },
#define	X_SPT		3
	{ "_maxslp" },
#define	X_MAXSLP	4
	{ "_userpages" },
#define	X_USERPAGES	5
	{ "_hz" },
#define	X_HZ		6
	{ "_nspt" },
#define	X_NSPT		7
	{ 0 }
};

struct	savcom {
	union {
		struct	lsav *lp;
		float	u_pctcpu;
		struct	vsav *vp;
		int	s_ssiz;
	} sun;
	struct	asav *ap;
} *savcom;

struct	asav {
	char	*a_cmdp;
	int	a_flag;
	short	a_stat, a_uid, a_pid, a_nice, a_pri, a_slptime, a_time;
	size_t	a_size, a_rss, a_tsiz, a_txtrss, a_pgtab;
	short	a_xccount;
	char	a_tty[DIRSIZ+1];
	dev_t	a_ttyd;
	time_t	a_cpu;
	size_t	a_maxrss;
};

char	*lhdr;
struct	lsav {
	short	l_ppid;
	char	l_cpu;
	caddr_t	l_wchan;
};

char	*uhdr;
char	*shdr;

char	*vhdr;
struct	vsav {
	u_int	v_majflt;
	size_t	v_swrss, v_txtswrss;
	float	v_pctcpu;
};

struct	proc proc[8];		/* 8 = a few, for less syscalls */
struct	proc *mproc;

int	sptadr;			/* address of spt table in kernel */
struct	spt sptp;		/* spt data for process */
struct	spt sptt;		/* spt data for text */

union {
	struct	user user;
	char	upages[UPAGES][NBPG];
} user;
#define u	user.user

#define clear(x) 	((int)x & 0x7fffffff)

int	chkpid;
int	aflg, cflg, eflg, gflg, kflg, lflg, sflg, uflg, vflg, xflg;
char	*tptr;
char	*gettty(), *getcmd(), *getname(), *savestr(), *alloc(), *state();
double	pcpu(), pmem();
int	pscomp();
int	maxslp;
double	ccpu;
int	userpages;
int	nproc, nspt, procno, hz;

struct	ttys {
	char	name[DIRSIZ+1];
	dev_t	ttyd;
	struct	ttys *next;
	struct	ttys *cand;
} *allttys, *cand[16];

int	npr;

int	cmdstart;
int	twidth;
char	*kmemf, *memf, *nlistf;
int	kmem, mem;
int	rawcpu, sumcpu;

extern	char _sobuf[];

main(argc, argv)
	char **argv;
{
	register int i, j;
	register char *ap;
	int uid;
	off_t procp;

	if (chdir("/dev") < 0) {
		perror("/dev");
		exit(1);
	}
	twidth = 75;
	setbuf(stdout, _sobuf);
	argc--, argv++;
	if (argc > 0) {
		ap = argv[0];
		while (*ap) switch (*ap++) {

		case 'C':
			rawcpu++;
			break;
		case 'S':
			sumcpu++;
			break;
		case 'a':
			aflg++;
			break;
		case 'c':
			cflg = !cflg;
			break;
		case 'e':
			eflg++;
			break;
		case 'g':
			gflg++;
			break;
		case 'k':
			kflg++;
			break;
		case 'l':
			lflg++;
			break;
		case 's':
			sflg++;
			break;
		case 't':
			if (*ap)
				tptr = ap;
			aflg++;
			gflg++;
			if (*tptr == '?')
				xflg++;
			while (*ap)
				ap++;
			break;
		case 'u':
			uflg++;
			break;
		case 'v':
			cflg = 1;
			vflg++;
			break;
		case 'w':
			if (twidth == 80)
				twidth = 132;
			else
				twidth = BUFSIZ;
			break;
		case 'x':
			xflg++;
			break;
		default:
			if (!isdigit(ap[-1]))
				break;
			chkpid = atoi(--ap);
			*ap = 0;
			aflg++;
			xflg++;
			break;
		}
	}
	openfiles(argc, argv);
	getkvars(argc, argv);
	getdev();
	uid = getuid();
	printhdr();
	procp = getw(nl[X_PROC].n_value);
	nproc = getw(nl[X_NPROC].n_value);
	hz = getw(nl[X_HZ].n_value);
	sptadr = getw(nl[X_SPT].n_value);
	nspt = getw(nl[X_NSPT].n_value);
	savcom = (struct savcom *)calloc(nproc, sizeof (*savcom));
	for (i=0; i<nproc; i += 8) {
		lseek(kmem, (char *)procp, 0);
		j = nproc - i;
		if (j > 8)
			j = 8;
		j *= sizeof (struct proc);
		if (read(kmem, (char *)proc, j) != j)
			cantread("proc table", kmemf);
		procp += j;
		for (j = j / sizeof (struct proc) - 1; j >= 0; j--) {
			mproc = &proc[j];
			procno = i + j;
			if (mproc->p_stat == 0 ||
			    mproc->p_pgrp == 0 && xflg == 0)
				continue;
			if (tptr == 0 && gflg == 0 && xflg == 0 &&
			    mproc->p_ppid == 1 && (mproc->p_flag&SDETACH) == 0)
				continue;
			if (uid != mproc->p_uid && aflg==0 ||
			    chkpid != 0 && chkpid != mproc->p_pid)
				continue;
			if (vflg && gflg == 0 && xflg == 0) {
				if (mproc->p_stat == SZOMB ||
				    mproc->p_flag&SWEXIT)
					continue;
				if (/*mproc->p_slptime > MAXSLP && */
				    (mproc->p_stat == SSLEEP ||
				     mproc->p_stat == SSTOP))
				continue;
			}
			save();
		}
	}
	qsort(savcom, npr, sizeof(savcom[0]), pscomp);
	for (i=0; i<npr; i++) {
		register struct savcom *sp = &savcom[i];
		if (lflg)
			lpr(sp);
		else if (vflg)
			vpr(sp);
		else if (uflg)
			upr(sp);
		else
			spr(sp);
		if (sp->ap->a_flag & SWEXIT)
			printf(" <exiting>");
		else if (sp->ap->a_stat == SZOMB)
			printf(" <defunct>");
		else if (sp->ap->a_pid == 0)
			printf(" swapper");
		else
			printf(" %.*s", twidth - cmdstart - 2, sp->ap->a_cmdp);
		printf("\n");
	}
	exit(npr == 0);
}

getw(loc)
	off_t loc;
{
	long word;

	lseek(kmem, loc, 0);
	if (read(kmem, &word, sizeof (word)) != sizeof (word))
		printf("error reading kmem at %x\n", loc);
	return (word);
}

openfiles(argc, argv)
	char **argv;
{

	kmemf = "kmem";
	if (kflg) {
		kmemf = argc > 1 ? argv[1] : "/vmcore";
		if (access(kmemf, 4)) {		/* test for readability */
			perror(kmemf);
			exit(1);
		}
	}
	kmem = open(kmemf, 0);
	if (kmem < 0) {
		perror(kmemf);
		exit(1);
	}
	if (kflg)  {
		mem = kmem;
		memf = kmemf;
	} else {
		memf = "mem";
		mem = open(memf, 0);
		if (mem < 0) {
			perror(memf);
			exit(1);
		}
	}
}

getkvars(argc, argv)
	char **argv;
{
	register struct nlist *nlp;

	nlistf = argc > 3 ? argv[3] : "/vmsymbols";
	cnlist(nlistf, nl);
	if (nl[0].n_type == 0) {
		fprintf(stderr, "%s: No namelist\n", nlistf);
		exit(1);
	}
	maxslp = 20;
	/*
	lseek(kmem, (long)nl[X_MAXSLP].n_value, 0);
	if (read(kmem, &maxslp, sizeof (maxslp)) != sizeof (maxslp)) {
		cantread("maxslp", kmemf);
		exit(1);
	}
	lseek(kmem, (long)nl[X_CCPU].n_value, 0);
	if (read(kmem, &ccpu, sizeof (ccpu)) != sizeof (ccpu)) {
		cantread("ccpu", kmemf);
		exit(1);
	}
	*/
	lseek(kmem, (long)nl[X_USERPAGES].n_value, 0);
	if (read(kmem, &userpages, sizeof (userpages)) != sizeof (userpages)) {
		cantread("userpages", kmemf);
		exit(1);
	}
}

printhdr()
{
	char *hdr;

	if (sflg+lflg+vflg+uflg > 1) {
		fprintf(stderr, "ps: specify only one of s,l,v and u\n");
		exit(1);
	}
	hdr = lflg ? lhdr : (vflg ? vhdr : (uflg ? uhdr : shdr));
	if (lflg+vflg+uflg+sflg == 0)
		hdr += strlen("SSIZ ");
	cmdstart = strlen(hdr);
	printf("%s COMMAND\n", hdr);
	fflush(stdout);
}

cantread(what, fromwhat)
	char *what, *fromwhat;
{

	fprintf(stderr, "ps: error reading %s from %s", what, fromwhat);
}

struct	direct dbuf;
int	dialbase;

getdev()
{
	register FILE *df;
	register struct ttys *dp;

	dialbase = -1;
	if ((df = fopen(".", "r")) == NULL) {
		fprintf(stderr, "Can't open . in /dev\n");
		exit(1);
	}
	while (fread((char *)&dbuf, sizeof(dbuf), 1, df) == 1) {
		if (dbuf.d_ino == 0)
			continue;
		maybetty(dp);
	}
	fclose(df);
}

/*
 * Attempt to avoid stats by guessing minor device
 * numbers from tty names.  Console is known,
 * know that r(hp|up|mt) are unlikely as are different mem's,
 * null, tty, etc.
 */
maybetty()
{
	register char *cp = dbuf.d_name;
	register struct ttys *dp;
	int x;
	struct stat stb;

	switch (cp[0]) {

	case 'c':
		if (!strcmp(cp, "console")) {
			x = 0;
			goto donecand;
		}
		/* cu[la]? are possible!?! don't rule them out */
		break;

	case 'd':
		if (!strcmp(cp, "drum"))
			return (0);
		break;

	case 'k':
		cp++;
		if (*cp == 'U')
			cp++;
		goto trymem;

	case 'r':
		cp++;
		if (*cp == 'r' || *cp == 'u' || *cp == 'h')
			cp++;
#define is(a,b) cp[0] == 'a' && cp[1] == 'b'
		if (is(r,p) || is(u,p) || is(r,k) || is(r,m) || is(m,t)) {
			cp += 2;
			if (isdigit(*cp) && cp[2] == 0)
				return (0);
		}
		break;

	case 'm':
trymem:
		if (cp[0] == 'm' && cp[1] == 'e' && cp[2] == 'm' && cp[3] == 0)
			return (0);
		if (cp[0] == 'm' && cp[1] == 't')
			return (0);
		break;

	case 'n':
		if (!strcmp(cp, "null"))
			return (0);
		break;

	case 'v':
		if ((cp[1] == 'a' || cp[1] == 'p') && isdigit(cp[2]) &&
		    cp[3] == 0)
			return (0);
		break;
	}
mightbe:
	cp = dbuf.d_name;
	while (cp < &dbuf.d_name[DIRSIZ] && *cp)
		cp++;
	--cp;
	x = 0;
	if (cp[-1] == 'd') {
		if (dialbase == -1) {
			if (stat("ttyd0", &stb) == 0)
				dialbase = stb.st_rdev & 017;
			else
				dialbase = -2;
		}
		if (dialbase == -2)
			x = 0;
		else
			x = 11;
	}
	if (cp > dbuf.d_name && isdigit(cp[-1]) && isdigit(*cp))
		x += 10 * (cp[-1] - ' ') + cp[0] - '0';
	else if (*cp >= 'a' && *cp <= 'f')
		x += 10 + *cp - 'a';
	else if (isdigit(*cp))
		x += *cp - '0';
	else
		x = -1;
donecand:
	dp = (struct ttys *)alloc(sizeof (struct ttys));
	strncpy(dp->name, dbuf.d_name, DIRSIZ);
	dp->next = allttys;
	dp->ttyd = -1;
	allttys = dp;
	if (x == -1)
		return;
	x &= 017;
	dp->cand = cand[x];
	cand[x] = dp;
}

char *
gettty()
{
	register char *p;
	register struct ttys *dp;
	struct stat stb;
	int x;

	if (u.u_ttyp == 0)
		return("?");
	x = u.u_ttyd & 017;
	for (dp = cand[x]; dp; dp = dp->cand) {
		if (dp->ttyd == -1) {
			if (stat(dp->name, &stb) == 0 &&
			   (stb.st_mode&S_IFMT)==S_IFCHR)
				dp->ttyd = stb.st_rdev;
			else
				dp->ttyd = -2;
		}
		if (dp->ttyd == u.u_ttyd)
			goto found;
	}
	/* ick */
	for (dp = allttys; dp; dp = dp->next) {
		if (dp->ttyd == -1) {
			if (stat(dp->name, &stb) == 0 &&
			   (stb.st_mode&S_IFMT)==S_IFCHR)
				dp->ttyd = stb.st_rdev;
			else
				dp->ttyd = -2;
		}
		if (dp->ttyd == u.u_ttyd)
			goto found;
	}
	return ("?");
found:
	p = dp->name;
	if (p[0]=='t' && p[1]=='t' && p[2]=='y')
		p += 3;
	return (p);
}

save()
{
	register struct savcom *sp;
	register struct asav *ap;
	register char *cp;
	char *ttyp, *cmdp;

	if (mproc->p_stat != SZOMB && getu() == 0)
		return;
	ttyp = gettty();
	if (xflg == 0 && ttyp[0] == '?' || tptr && strcmpn(tptr, ttyp, 2))
		return;
	sp = &savcom[npr];
	cmdp = getcmd();
	if (cmdp == 0)
		return;
	if (getspt() == 0)
		return;
	sp->ap = ap = (struct asav *)alloc(sizeof (struct asav));
	sp->ap->a_cmdp = cmdp;
#define e(a,b) ap->a = mproc->b
	e(a_flag, p_flag); e(a_stat, p_stat); e(a_nice, p_nice);
	e(a_uid, p_uid); e(a_pid, p_pid); e(a_pri, p_pri);
	e(a_slptime, p_slptime); e(a_time, p_time);
	ap->a_tty[0] = ttyp[0];
	ap->a_tty[1] = ttyp[1] ? ttyp[1] : ' ';
	/* NOT RIGHT
	if (ap->a_stat == SZOMB) {
		register struct xproc *xp = (struct xproc *)mproc;

		ap->a_cpu = xp->xp_vm.vm_utime + xp->xp_vm.vm_stime;
	} else {
	*/
	{
		ap->a_size = (sptp.spt_usedpages * MCPAGES) / 2;
		ap->a_rss = (sptp.spt_mempages * MCPAGES) / 2;
		ap->a_pgtab = (sptp.spt_tablepages * MCPAGES) / 2;
		ap->a_ttyd = u.u_ttyd;
		ap->a_cpu = u.u_vm.vm_utime + u.u_vm.vm_stime;
		if (sumcpu)
			ap->a_cpu += u.u_cvm.vm_utime + u.u_cvm.vm_stime;
		if (u.u_xspti >= 0) {
			ap->a_tsiz = (sptt.spt_usedpages * MCPAGES) / 2;
			ap->a_txtrss = (sptt.spt_mempages * MCPAGES) / 2;
			ap->a_xccount = 0;	/* temporary */
		}
	}
	if (ap->a_pid == 0) {	/* swapper process has no memory really */
		ap->a_size = 0;
		ap->a_rss = 0;
		ap->a_tsiz = 0;
		ap->a_txtrss = 0;
		ap->a_pgtab = 0;
	}
#undef e
	ap->a_cpu /= hz;
	ap->a_maxrss = (mproc->p_maxrss * MCPAGES) / 2;
	if (lflg) {
		register struct lsav *lp;

		sp->sun.lp = lp = (struct lsav *)alloc(sizeof (struct lsav));
#define e(a,b) lp->a = mproc->b
		e(l_ppid, p_ppid); e(l_cpu, p_cpu);
		if (ap->a_stat != SZOMB)
			e(l_wchan, p_wchan);
#undef e
	} else if (vflg) {
		register struct vsav *vp;

		sp->sun.vp = vp = (struct vsav *)alloc(sizeof (struct vsav));
#define e(a,b) vp->a = mproc->b
		if (ap->a_stat != SZOMB) {
			/*e(v_swrss, p_swrss);*/
			vp->v_majflt = u.u_vm.vm_majflt;
			/*
			if (mproc->p_textp)
				vp->v_txtswrss = xp->x_swrss;
			*/
		}
		vp->v_pctcpu = pcpu();
#undef e
	} else if (uflg)
		sp->sun.u_pctcpu = pcpu();
	else if (sflg) {
		if (ap->a_stat != SZOMB) {
			for (cp = (char *)u.u_stack;
			    cp < &user.upages[UPAGES][0]; )
				if (*cp++)
					break;
			sp->sun.s_ssiz = (&user.upages[UPAGES][0] - cp);
		}
	}
	npr++;
}

double
pmem(ap)
	register struct asav *ap;
{
	double fracmem;
	int szptudot;

	if ((ap->a_flag&SLOAD) == 0)
		fracmem = 0;
	else {
		szptudot = (UPAGES/MCSIZE) + ap->a_pgtab;
		fracmem = (((float)ap->a_rss+szptudot)*1000)/userpages;
	}
	return (fracmem);
}

double
pcpu()
{
	time_t time;

	time = mproc->p_time;
	if (time == 0 || (mproc->p_flag&SLOAD) == 0)
		return (0);
	return ((mproc->p_pctcpu + (AVENSCALE / 20)) / (AVENSCALE / 10));
}


getu()
{
	int size, phyu, viru, i;
	char *uptr = (char *)&user;

	if (kflg) {
	    viru = (long)nl[X_U].n_value + ((procno + 1) * USIZE);
	    for (i = 0; i < USIZE; i+=MCSIZE) {

	    phyu = physaddr(SYSSPTI, viru);
	    if (phyu < 0) {
		fprintf(stderr, "ps: cant lseek u for vaddr %x\n", viru);
		return (0);
	    }
	    lseek(kmem, phyu, 0);
	    if (read(kmem, uptr, MCSIZE) != MCSIZE) {
		fprintf(stderr, "ps: cant read u for pid %d\n", mproc->p_pid);
		return (0);
	    }
	    uptr += MCSIZE;
	    viru += MCSIZE;

	    }
	    return (1);
	}
	size = sflg ? (USIZE) : sizeof (struct user);
	lseek(kmem, (long)nl[X_U].n_value + ((procno + 1) * USIZE), 0);
	if (read(kmem, &user, size) != size) {
	    fprintf(stderr, "ps: cant read u for pid %d\n", mproc->p_pid);
	    return (0);
	}
	return (1);
}


/* Read spt data for current process, and for text it is attached to */
getspt()
{
	lseek(kmem, sptadr + (sizeof(sptp) * mproc->p_spti), 0);
	if (read(kmem, &sptp, sizeof(sptp)) != sizeof(sptp)) {
		fprintf(stderr, "ps: cant read spt for pid %d\n", mproc->p_pid);
		return(0);
	}
	if (u.u_xspti < 0) return(1);
	lseek(kmem, sptadr + (sizeof(sptt) * u.u_xspti), 0);
	if (read(kmem, &sptt, sizeof(sptt)) != sizeof(sptt)) {
		fprintf(stderr, "ps: cant read text spt for pid %d\n", mproc->p_pid);
		return(0);
	}
	return(1);
}

char *
getcmd()
{
	char cmdbuf[BUFSIZ];
	union {
		char	argc[MCSIZE];
		int	argi[MCSIZE/sizeof (int)];
	} argspac;
	register char *cp;
	register int *ip;
	char c;
	int nbad;
	int phyu, viru;

	if (mproc->p_stat == SZOMB || mproc->p_flag&(SSYS|SWEXIT))
		return ("");
	if (cflg) {
		strncpy(cmdbuf, u.u_comm, sizeof (u.u_comm));
		return (savestr(cmdbuf));
	}
	if (kflg) {
	    viru = TOPUSER - MCSIZE;
	    phyu = physaddr(u.u_spti, viru);
	    if (phyu < 0) goto bad;
	    lseek(kmem, phyu, 0);
	    if (read(kmem, &argspac, sizeof (argspac)) != sizeof (argspac))
		goto bad;
	}
	else
	if (vmap(1, &argspac, (TOPUSER-MCSIZE), mproc->p_pid)) {
		goto bad;
	}
	ip = &argspac.argi[MCSIZE/sizeof (int)];
	ip -= 2;		/* last arg word and .long 0 */
	while (*--ip)
		if (ip == argspac.argi)
			goto retucomm;
	*(char *)ip = ' ';
	ip++;
	nbad = 0;
	for (cp = (char *)ip; cp < &argspac.argc[MCSIZE]; cp++) {
		c = *cp & 0177;
		if (c == 0)
			*cp = ' ';
		else if (c < ' ' || c > 0176) {
			if (++nbad >= 5*(eflg+1)) {
				*cp++ = ' ';
				break;
			}
			*cp = '?';
		} else if (eflg == 0 && c == '=') {
			while (*--cp != ' ')
				if (cp <= (char *)ip)
					break;
			break;
		}
	}
	*cp = 0;
	while (*--cp == ' ')
		*cp = 0;
	cp = (char *)ip;
	strncpy(cmdbuf, cp, &argspac.argc[MCSIZE] - cp);
	if (cp[0] == '-' || cp[0] == '?' || cp[0] <= ' ') {
		strcat(cmdbuf, " (");
		strncat(cmdbuf, u.u_comm, sizeof(u.u_comm));
		strcat(cmdbuf, ")");
	}
	return (savestr(cmdbuf));

bad:
	fprintf(stderr, "ps: error locating command name for pid %d\n",
	    mproc->p_pid);
retucomm:
	strcpy(cmdbuf, " (");
	strncat(cmdbuf, u.u_comm, sizeof (u.u_comm));
	strcat(cmdbuf, ")");
	return (savestr(cmdbuf));
}

char	*lhdr =
"     F UID   PID  PPID CP PRI NI  SZ  RSS  WCHAN STAT TT  TIME";
lpr(sp)
	struct savcom *sp;
{
	register struct asav *ap = sp->ap;
	register struct lsav *lp = sp->sun.lp;

	printf("%6x%4d%6u%6u%3d%4d%3d%4d%5d",
	    ap->a_flag, ap->a_uid,
	    ap->a_pid, lp->l_ppid, lp->l_cpu&0377, ap->a_pri-PZERO,
	    ap->a_nice-NZERO, ap->a_size, ap->a_rss);
	printf(lp->l_wchan ? " %6x" : "       ", (int)lp->l_wchan&0xffffff);
	printf(" %4.4s ", state(ap));
	ptty(ap->a_tty);
	ptime(ap);
}

ptty(tp)
	char *tp;
{

	printf("%-2.2s", tp);
}

ptime(ap)
	struct asav *ap;
{

	printf("%3ld:%02ld", ap->a_cpu / hz, ap->a_cpu % hz);
}

char	*uhdr =
"USER       PID %CPU %MEM   SZ  RSS TT STAT  TIME";
upr(sp)
	struct savcom *sp;
{
	register struct asav *ap = sp->ap;
	int vmsize, rmsize;
	int pc, pm;

	vmsize = ap->a_size;
	rmsize = ap->a_rss;
	pc = sp->sun.u_pctcpu;
	pm = pmem(ap);
	if (ap->a_xccount)
		rmsize += ap->a_txtrss/ap->a_xccount;
	printf("%-8.8s %5d%3d.%1d%3d.%1d%5d%5d",
	    getname(ap->a_uid), ap->a_pid, pc/10, pc%10, pm/10, pm%10,
	    vmsize, rmsize);
	putchar(' ');
	ptty(ap->a_tty);
	printf(" %4.4s", state(ap));
	ptime(ap);
}

char *vhdr =
"  PID TT STAT  TIME SL RE PAGEIN SIZE  RSS  LIM TSIZ TRS %CPU %MEM";
vpr(sp)
	struct savcom *sp;
{
	register struct vsav *vp = sp->sun.vp;
	register struct asav *ap = sp->ap;
	int	pc, pm;

	printf("%5u ", ap->a_pid);
	ptty(ap->a_tty);
	printf(" %4.4s", state(ap));
	ptime(ap);
	printf("%3d%3d%7d%5d%5d",
	   ap->a_slptime > 99 ? 99 : ap-> a_slptime,
	   ap->a_time > 99 ? 99 : ap->a_time, vp->v_majflt,
	   ap->a_size, ap->a_rss);
	if (ap->a_maxrss == (INFINITY/NBPG))
		printf("   xx");
	else
		printf("%5d", ap->a_maxrss);
	pc = vp->v_pctcpu;
	pm = pmem(ap);
	printf("%5d%4d%3d.%1d%3d.%1d",
	   ap->a_tsiz, ap->a_txtrss, pc/10, pc%10, pm/10, pm%10);
}

char	*shdr =
"SSIZ   PID TT STAT  TIME";
spr(sp)
	struct savcom *sp;
{
	register struct asav *ap = sp->ap;

	if (sflg)
		printf("%4d ", sp->sun.s_ssiz);
	printf("%5u", ap->a_pid);
	putchar(' ');
	ptty(ap->a_tty);
	printf(" %4.4s", state(ap));
	ptime(ap);
}

char *
state(ap)
	register struct asav *ap;
{
	char stat, load, nice, anom;
	static char res[5];

	switch (ap->a_stat) {

	case SSTOP:
		stat = 'T';
		break;

	case SSLEEP:
		if (ap->a_pri >= PZERO)
			/*
			if (ap->a_slptime >= MAXSLP)
				stat = 'I';
			else
			*/
				stat = 'S';
		else if (ap->a_flag & SPAGE)
			stat = 'P';
		else
			stat = 'D';
		break;

	case SWAIT:
	case SRUN:
	case SIDL:
		stat = 'R';
		break;

	case SZOMB:
		stat = 'Z';
		break;

	default:
		stat = '?';
	}
	load = ap->a_flag & SLOAD ? (ap->a_rss>ap->a_maxrss ? '>' : ' ') : 'W';
	if (ap->a_nice < NZERO)
		nice = '<';
	else if (ap->a_nice > NZERO)
		nice = 'N';
	else
		nice = ' ';
	anom = ' ';
	/*anom = (ap->a_flag&SUANOM) ? 'A' : ((ap->a_flag&SSEQL) ? 'S' : ' ');*/
	res[0] = stat; res[1] = load; res[2] = nice; res[3] = anom;
	return (res);
}

pscomp(s1, s2)
	struct savcom *s1, *s2;
{
	register int i;

	if (uflg)
		return (s2->sun.u_pctcpu > s1->sun.u_pctcpu ? 1 : -1);
	if (vflg)
		return (vsize(s2) - vsize(s1));
	i = s1->ap->a_ttyd - s2->ap->a_ttyd;
	if (i == 0)
		i = s1->ap->a_pid - s2->ap->a_pid;
	return (i);
}

vsize(sp)
	struct savcom *sp;
{
	register struct asav *ap = sp->ap;
	register struct vsav *vp = sp->sun.vp;
	
	if (ap->a_flag & SLOAD)
		return (ap->a_rss);
	return (vp->v_swrss);
}

#define	NMAX	8
#define	NUID	2048

char	names[NUID][NMAX+1];

/*
 * Stolen from ls...
 */
char *
getname(uid)
{
	register struct passwd *pw;
	static init;
	struct passwd *getpwent();

	if (uid >= 0 && uid < NUID && names[uid][0])
		return (&names[uid][0]);
	if (init == 2)
		return (0);
	if (init == 0)
		setpwent(), init = 1;
	while (pw = getpwent()) {
		if (pw->pw_uid >= NUID)
			continue;
		if (names[pw->pw_uid][0])
			continue;
		strncpy(names[pw->pw_uid], pw->pw_name, NMAX);
		if (pw->pw_uid == uid)
			return (&names[uid][0]);
	}
	init = 2;
	endpwent();
	return (0);
}

char	*freebase;
int	nleft;

char *
alloc(size)
	int size;
{
	register char *cp;
	register int i;

	if (size > nleft) {
		freebase = (char *)sbrk(i = size > 2048 ? size : 2048);
		if (freebase == 0) {
			fprintf(stderr, "ps: ran out of memory\n");
			exit(1);
		}
		nleft = i - size;
	} else
		nleft -= size;
	cp = freebase;
	for (i = size; --i >= 0; )
		*cp++ = 0;
	freebase = cp;
	return (cp - size);
}

char *
savestr(cp)
	char *cp;
{
	register int len;
	register char *dp;

	len = strlen(cp);
	dp = (char *)alloc(len+1);
	strcpy(dp, cp);
	return (dp);
}

/* Return the physical memory address corresponding to a given virtual
 * address.  Returns -1 if physical address does not exist.
 */
physaddr(spti, addr)
	register spt_t	spti;		/* spt index */
	register unsigned long addr;	/* virtual address */
{
	pte	*curpte;		/* current pte entry */
	pte	thepte;			/* the pte entry */
	struct	spt	*sptn;		/* spt[spti] entry */
	pte	*sptpte1;		/* spt_pte1 address */

	
	if (addr >= MAXADDRS) return(-1);
	if (spti >= nspt) return(-1); 
	thepte = sptadr + (sizeof(struct spt) * spti);
	sptn = (struct spt *) thepte;
	sptpte1 = (pte *) getw((int) &(sptn->spt_pte1));
	if (sptpte1 == NULL) return(-1); 
	curpte = sptpte1 + GETVI1(addr);
	thepte = getw(curpte);
	if (PTEINVALID(thepte)) return(-1);
	curpte = (pte *) PTEMEM_ADDR(thepte);
	curpte += GETVI2(addr);
	thepte = getw(curpte);
	if (PTEINVALID(thepte)) return (-1);
	return (PTEMEM_ADDR(thepte) | GETVOFF(addr));
}

