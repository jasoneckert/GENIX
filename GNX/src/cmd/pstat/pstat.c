/*
 * pstat.c: version 1.25 of 10/3/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)pstat.c	1.25 (NSC) 10/3/83";
# endif

/*
 * Print system stuff
 */

#include <sys/param.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/inode.h>
#include <sys/tty.h>
#include <sys/vm.h>
#include <sys/pte.h>
#include <sys/devvm.h>
#include <nlist.h>

char	*fcore	= "/dev/kmem";
char	*fnlist	= "/vmunix";
char	*fastsymbols = "/vmsymbols";
int	fc;

struct nlist nl[] = {
#define	SINODE	0
	{ "_inode" },
#define	SNINODE	1
	{ "_ninode" },
#define	SFIL	2
	{ "_file" },
#define	SNFILE	3
	{ "_nfile" },
#define	SPROC	4
	{ "_proc" },
#define	SNPROC	5
	{ "_nproc" },
#define	SSPT	6
	{ "_spt" },
#define SNSPT	7
	{ "_nspt" },
#define SNTEXT	8
	{ "_ntext" },
#define	SNSWAP	9
	{ "_nswap" },
#define SSWAPBITTABLE	10
	{ "_swapbittable" },
#define SBITMAP 11
	{ "_bitmap"},
#define STTYS 12
	{ "_rs_tty"},
#define	SNTTYS 13
	{ "_nttys"},
#define	SSIO 14
	{ "_sio"},
#define	SUDOT 15
	{ "_u"},
	0,
};

int	inof;
int	txtf;
int	prcf;
int	ttyf;
int	usrf;
long	upid;
int	filf;
int	swpf;
int	totflg;
int	allflg;
int	kflg;
int	procadr;
int	nproc;
int	sptadr;
int	nspt;

main(argc, argv)
char **argv;
{
	register char *argp;

	argc--, argv++;
	while (argc > 0 && **argv == '-') {
		argp = *argv++;
		argp++;
		argc--;
		while (*argp++)
		switch (argp[-1]) {

		case 'T':
			totflg++;
			break;

		case 'a':
			allflg++;
			break;

		case 'i':
			inof++;
			break;

		case 'k':
			kflg++;
			fcore = "/vmcore";
			break;

		case 'x':
			txtf++;
			break;

		case 'p':
			prcf++;
			break;

		case 't':
			ttyf++;
			break;

		case 'u':
			if (argc == 0)
				break;
			argc--;
			usrf++;
			sscanf( *argv++, "%d", &upid);
			break;

		case 'f':
			filf++;
			break;
		case 's':
			swpf++;
			break;
		}
	}
	if (argc>0) {
		fcore = argv[0];
		if (access(fcore, 4)) {		/* check access in case root */
			perror(fcore);
			exit(1);
		}
	}
	if ((fc = open(fcore, 0)) < 0) {
		printf("Can't find %s\n", fcore);
		exit(1);
	}
	if (argc>1) {
		fnlist = argv[1];
		if (access(fnlist, 4)) {	/* check access */
			perror(fnlist);
			exit(1);
		}
		nlist(fnlist, nl);
	}
	else cnlist(fastsymbols, nl);
	if (nl[0].n_type == 0) {
		printf("no namelist\n");
		exit(1);
	}
	procadr = getw(nl[SPROC].n_value);
	nproc = getw(nl[SNPROC].n_value);
	sptadr = getw(nl[SSPT].n_value);
	nspt = getw(nl[SNSPT].n_value);
	if (filf||totflg)
		dofile();
	if (inof||totflg)
		doinode();
	if (prcf||totflg)
		doproc();
	if (txtf||totflg)
		dotext();
	if (ttyf)
		dotty();
	if (usrf)
		dousr();
	if (swpf||totflg)
		doswap();
}

doinode()
{
	register struct inode *ip;
	struct inode *xinode, *ainode;
	register int nin;
	int ninode;

	nin = 0;
	ninode = getw(nl[SNINODE].n_value);
	xinode = (struct inode *)calloc(ninode, sizeof (struct inode));
	ainode = (struct inode *)getw(nl[SINODE].n_value);
	lseek(fc, (int)ainode, 0);
	read(fc, xinode, ninode * sizeof(struct inode));
	for (ip = xinode; ip < &xinode[ninode]; ip++)
		if (ip->i_count)
			nin++;
	if (totflg) {
		printf("%3d/%3d inodes\n", nin, ninode);
		return;
	}
	printf("%d/%d active inodes\n", nin, ninode);
	printf("   LOC    FLAGS    CNT DEVICE   INO  MODE  NLK  UID   SIZE/DEV\n");
	for (ip = xinode; ip < &xinode[ninode]; ip++) {
		if (ip->i_count == 0)
			continue;
		printf("%8.1x ", ainode + (ip - xinode));
		putf(ip->i_flag&ILOCK, 'L');
		putf(ip->i_flag&IUPD, 'U');
		putf(ip->i_flag&IACC, 'A');
		putf(ip->i_flag&IMOUNT, 'M');
		putf(ip->i_flag&IWANT, 'W');
		putf(ip->i_flag&ITEXT, 'T');
		putf(ip->i_flag&ICHG, 'C');
		putf(ip->i_flag&IPIPE, 'P');
		printf("%4d", ip->i_count&0377);
		printf("%4d,%3d", major(ip->i_dev), minor(ip->i_dev));
		printf("%6d", ip->i_number);
		printf("%6x", ip->i_mode & 0xffff);
		printf("%4d", ip->i_nlink);
		printf(" %4d", ip->i_uid);
		if ((ip->i_mode&IFMT)==IFBLK || (ip->i_mode&IFMT)==IFCHR)
			printf("%6d,%3d", major(ip->i_un.i_rdev), minor(ip->i_un.i_rdev));
		else
			printf("%10ld", ip->i_size);
		printf("\n");
	}
	free(xinode);
}

getw(loc)
	off_t loc;
{
	int word;

	if (kflg)
		loc &= 0x7fffffff;
	lseek(fc, loc, 0);
	read(fc, &word, sizeof (word));
	if (kflg)
		word &= 0x7fffffff;
	return (word);
}

putf(v, n)
{
	if (v)
		printf("%c", n);
	else
		printf(" ");
}

#undef	SPTISFILE
#define	SPTISFILE(i) ((i) >= nproc)

dotext()
{
	register struct spt *xp;
	struct spt *xtext, *atext;
	register int ntext;
	register int ntx;
	register int i;

	ntx = 0;
	ntext = getw(nl[SNTEXT].n_value);

	xtext = (struct spt *)calloc(nspt, sizeof (struct spt));
	lseek(fc, (int)(atext = (struct spt *)sptadr), 0);
	read(fc, xtext, nspt * sizeof (struct spt));
	for (i = 0; i < nspt; i++) {
		xp = &xtext[i];
		if (SPTISFILE(i) && (xp->spt_flags || xp->spt_inode
			|| xp->spt_proccount || xp->spt_share))
				ntx++;
	}
	if (totflg) {
		printf("%3d/%3d texts\n", ntx, ntext);
		return;
	}
	printf("%d/%d active texts\n", ntx, ntext);
	printf("\
SPTI   LOC  FLAGS  RSS TRSS SIZE PTPGS PROCS SHARE  IPTR   PTE1     AGE\n");
	for (i = 0; i < nspt; i++ ) {
		if ((allflg == 0) && (!SPTISFILE(i))) continue; 
		xp = &xtext[i];
		if ((allflg == 0) && (xp->spt_inode == NULL)
			&& (xp->spt_proccount == 0) && (xp->spt_share == 0)
			&& (xp->spt_flags == 0)) {
				continue;
		}
		printf(" %3d", i);
		printf(" %6x", atext + (xp - xtext));
		printf("  ");
		putf(xp->spt_flags&SPT_INIT, 'I');
		putf(xp->spt_flags&SPT_WANT, 'W');
		putf(xp->spt_flags&SPT_DEAD, 'D');
		putf(xp->spt_flags&SPT_XSRSS, 'X');
		printf(" %4d", xp->spt_mempages);
		printf(" %4d", xp->spt_tmempages);
		printf(" %4d", xp->spt_usedpages);
		printf(" %4d", xp->spt_tablepages);
		printf(" %5d", xp->spt_proccount);
		printf(" %5d", xp->spt_share);
		printf(" %6x", (int)xp->spt_inode);
		printf(" %6x", (int)xp->spt_pte1);
		printf(" %7u", xp->spt_age);
		printf("\n");
	}
	free(xtext);
}

doproc()
{
	struct proc *xproc, *aproc;
	struct spt *xspt, *aspt;
	register struct proc *pp;
	register np;

	xproc = (struct proc *)calloc(nproc, sizeof (struct proc));
	lseek(fc, (int)(aproc = (struct proc *)procadr), 0);
	read(fc, xproc, nproc * sizeof (struct proc));

	xspt = (struct spt *)calloc(nspt, sizeof (struct spt));
	lseek(fc, (int)(aspt = (struct spt *)sptadr), 0);
	read(fc, xspt, nspt * sizeof (struct spt));

	np = 0;
	for (pp=xproc; pp < &xproc[nproc]; pp++)
		if (pp->p_stat)
			np++;
	if (totflg) {
		printf("%3d/%3d processes\n", np, nproc);
		return;
	}
	printf("%d/%d processes\n", np, nproc);
	printf("  LOC  S    F PRI      SIG  UID SLP TIM  CPU  NI   PGRP    PID   PPID  RSS SRSS  SIZE  WCHAN   LINK SPTI  PTE1   CLKT\n");
	for (pp=xproc; pp<&xproc[nproc]; pp++) {
		if (pp->p_stat==0 && allflg==0)
			continue;
		printf("%6x", aproc + (pp - xproc));
		printf(" %1d", pp->p_stat);
		printf(" %4x", pp->p_flag & 0xffff);
		printf(" %3d", pp->p_pri);
		printf(" %8x", pp->p_sig);
		printf(" %4d", pp->p_uid);
		printf(" %3d", pp->p_slptime);
		printf(" %3d", pp->p_time);
		printf(" %4d", pp->p_cpu&0377);
		printf(" %3d", pp->p_nice);
		printf(" %6d", pp->p_pgrp);
		printf(" %6d", pp->p_pid);
		printf(" %6d", pp->p_ppid);
		printf(" %4d", xspt[pp->p_spti].spt_mempages);
		printf(" %4d", (pp->p_tdsize + pp->p_ssize)/PGSIZE);
		printf(" %5d", xspt[pp->p_spti].spt_usedpages);
		printf(" %6x", pp->p_wchan);
		printf(" %6x", pp->p_rlink);
		printf(" %4d", pp->p_spti);
		printf(" %6x", xspt[pp->p_spti].spt_pte1);
		printf("    %u", pp->p_clktim);
		printf("\n");
	}
	free(xspt);
	free(xproc);
}

dotty()
{
	struct tty ttydat;
	int nlines;
	register struct tty *xp, *tp;

	printf(" # RAW CAN OUT    MODE    ADDR  DEL COL  STATE   PGRP DISC\n");
	if (nl[SBITMAP].n_type != 0) {
		printf("1 bitmap\n");
		lseek(fc, (long)nl[SBITMAP].n_value, 0);
		read(fc, &ttydat, sizeof(struct tty));
		ttyprt(&ttydat, 0);
	}

	if (nl[STTYS].n_type != 0) {
		printf("1 RS232\n");
		lseek(fc, (long)nl[STTYS].n_value, 0);
		read(fc, &ttydat, sizeof(struct tty));
		ttyprt(&ttydat, 0);
	}

	if ((nl[SSIO].n_type == 0) || (nl[SNTTYS].n_type == 0)) return;
	nlines = getw(nl[SNTTYS].n_value);
	if (nlines == 0) return;
	xp = (struct tty *)calloc(nlines, sizeof(struct tty));
	lseek(fc, (long)nl[SSIO].n_value, 0);
	read(fc, xp, nlines * sizeof(struct tty));
	printf("%d sio lines\n", nlines);
	for (tp = xp; tp < &xp[nlines]; tp++) {
		ttyprt(tp, tp - xp);
	}
	free(xp);
}

ttyprt(atp, line)
struct tty *atp;
{
	register struct tty *tp;

	printf("%2d", line);
	tp = atp;
	switch (tp->t_line) {

	case NETLDISC:
		if (tp->t_rec)
			printf("%4d%4d", 0, tp->t_inbuf);
		else
			printf("%4d%4d", tp->t_inbuf, 0);
		break;

	default:
		printf("%4d", tp->t_rawq.c_cc);
		printf("%4d", tp->t_canq.c_cc);
	}
	printf("%4d", tp->t_outq.c_cc);
	printf("%8.1o", tp->t_flags);
	printf(" %8.1x", tp->t_addr);
	printf("%3d", tp->t_delct);
	printf("%4d ", tp->t_col);
	putf(tp->t_state&TIMEOUT, 'T');
	putf(tp->t_state&WOPEN, 'W');
	putf(tp->t_state&ISOPEN, 'O');
	putf(tp->t_state&CARR_ON, 'C');
	putf(tp->t_state&BUSY, 'B');
	putf(tp->t_state&ASLEEP, 'A');
	putf(tp->t_state&XCLUDE, 'X');
	putf(tp->t_state&HUPCLS, 'H');
	printf("%6d", tp->t_pgrp);
	switch (tp->t_line) {

	case NTTYDISC:
		printf(" ntty");
		break;

	case NETLDISC:
		printf(" net");
		break;
	}
	printf("\n");
}

dousr()
{
	struct proc *xproc, *aproc;
	int found;
	register struct proc *pp;
	register struct user *U;
	register i, j, *ip;

	xproc = (struct proc *)calloc(nproc, sizeof (struct proc));
	lseek(fc, (int)(aproc = (struct proc *)procadr), 0);
	read(fc, xproc, nproc * sizeof (struct proc));

	found = 0;
	for (pp = xproc; pp < &xproc[nproc]; pp++) {
		if (pp->p_pid == upid) {
			found++;
			break;
		}
	}
	if (!found) {
		printf("Could not find pid %d\n",upid);
		exit(1);
	}
	free(xproc);
	U = (struct user *) calloc(1, USIZE);
	i = (pp - xproc);
	if (getu(i, U) == 0) {
		exit(1);
	}
	if (U->u_procp != (aproc + i)) {
		printf("Inconsistent u. area for pid %d\n", upid);
		exit(1);
	}

	printf("msrreg\t%08x\n", U->u_msrreg);
	printf("eiareg\t%08x\n", U->u_eiareg);
	printf("segflg\t%d\nerror\t%d\n", U->u_segflg, U->u_error);
	printf("uids\t%d,%d,%d,%d\n", U->u_uid,U->u_gid,U->u_ruid,U->u_rgid);
	printf("procp\t%.1x\n", U->u_procp);
	printf("spti\t%d\nxspti\t%d\n", U->u_spti, U->u_xspti);
	printf("ap\t%.1x\n", U->u_ap);
	printf("r_val\t%.1x %.1x\n", U->u_r.r_val1, U->u_r.r_val2);
	printf("base, count, offset %.1x %.1x %ld\n", U->u_base,
		U->u_count, U->u_offset);
	printf("cdir rdir %.1x %.1x\n", U->u_cdir, U->u_rdir);
	printf("dbuf\t%.14s\n", U->u_dbuf);
	printf("dirp\t%.1x\n", U->u_dirp);
	printf("dent\t%d %.14s\n", U->u_dent.d_ino, U->u_dent.d_name);
	printf("pdir\t%.1o\n", U->u_pdir);
	printf("file");
	for (i=0; i<NOFILE; i++) {
		if (i%8 == 0) printf("\t");
		printf("%8x", U->u_ofile[i]);
		if (i%8 == 7) printf("\n");
		else printf(" ");
	}
	if (i%8) printf("\n");
	printf("pofile");
	for (i=0; i<NOFILE; i++) {
		if (i%8 == 0) printf("\t");
		printf("%8x", U->u_pofile[i]);
		if (i%8 == 7) printf("\n");
		else printf(" ");
	}
	if (i%8) printf("\n");
	printf("arg\t");
	for (i=0; i<5; i++)
		printf("%.1x ", U->u_arg[i]);
	printf("\n");
	printf("sizes\t%.1x %.1x %.1x\n", U->u_tsize, U->u_dsize, U->u_ssize);
	printf("qsav");
	ip = (int *)&U->u_qsav;
	for (i=0; i<sizeof(label_t)/sizeof(int); i++) {
		if (i%5==0)
			printf("\t");
		printf("%9.1x", *ip++);
		if (i%5==4)
			printf("\n");
	}
	if (i%5)
		printf("\n");
	printf("ssav");
	ip = (int *)&U->u_ssav;
	for (i=0; i<sizeof(label_t)/sizeof(int); i++) {
		if (i%5==0)
			printf("\t");
		printf("%9.1x", *ip++);
		if (i%5==4)
			printf("\n");
	}
	if (i%5) printf("\n");
	printf("sigcatcher\t%x\n", U->u_sigcatch);
	printf("sigs");
	for (i=0; i<NSIG; i++) {
		if (i%16 == 0) printf("\t");
		printf("%.1x", U->u_signal[i]);
		if (i%16 == 15) printf("\n");
		else printf(" ");
	}
	if (i%16) printf("\n");
	printf("code\t%.1x\n", U->u_code);
	printf("ar0\t%.1x\n", U->u_ar0);
	printf("abtar0\t%.1x\n", U->u_abtar0);
	printf("eosys\t%d\n", U->u_eosys);
	printf("intflg\t%d\n", U->u_intflg);
	printf("ttyp\t%.1x\n", U->u_ttyp);
	printf("ttyd\t%d,%d\n", major(U->u_ttyd), minor(U->u_ttyd));
	printf("exdata\t");
	ip = (int *)&U->u_exdata;
	for (i = 0; i < sizeof(U->u_exdata)/sizeof(int); i++)
		printf("%.1D ", *ip++);
	printf("\n");
	printf("comm\t%.14s\n", U->u_comm);
	printf("start\t%D\n", U->u_start);
	printf("acflag\t%D\n", U->u_acflag);
	printf("cmask\t%D\n", U->u_cmask);
	printf("vm\t");
	ip = (int *)&U->u_vm;
	for (i = 0; i < sizeof(U->u_vm)/sizeof(int); i++)
		printf("%D ", ip[i]);
	printf("\n");
	ip = (int *)&U->u_cvm;
	printf("cvm\t");
	for (i = 0; i < sizeof(U->u_vm)/sizeof(int); i++)
		printf("%D ", ip[i]);
	printf("\n");
	printf("limit\t");
	for (i = 0; i < sizeof(U->u_limit)/sizeof(int); i++)
		printf("%x ",U->u_limit[i]);
	printf("\n");

	i =  (int)((int *)&U->u_stack[0] - (int *)U);
	printf("stack limit at u+%x\n", i<<2);
	ip = (int *)U;
	while (ip[++i] == 0);
	i &= ~07;
	while (i < USIZE/NBPW) {
		printf("u+%03x/",i<<2);
		for (j=0; j<8; j++)
			printf("%9x", ip[i++]);
		printf("\n");
	}
	free(U);
}

dofile()
{
	int nfile;
	struct file *xfile, *afile;
	register struct file *fp;
	register nf;

	nf = 0;
	nfile = getw(nl[SNFILE].n_value);
	xfile = (struct file *)calloc(nfile, sizeof (struct file));
	lseek(fc, (int)(afile = (struct file *)getw(nl[SFIL].n_value)), 0);
	read(fc, xfile, nfile * sizeof (struct file));
	for (fp=xfile; fp < &xfile[nfile]; fp++)
		if (fp->f_count)
			nf++;
	if (totflg) {
		printf("%3d/%3d files\n", nf, nfile);
		return;
	}
	printf("%d/%d open files\n", nf, nfile);
	printf("   LOC   FLG  CNT   INO    OFFS\n");
	for (fp=xfile; fp < &xfile[nfile]; fp++) {
		if (fp->f_count==0)
			continue;
		printf("%8x ", afile + (fp - xfile));
		putf(fp->f_flag&FREAD, 'R');
		putf(fp->f_flag&FWRITE, 'W');
		putf(fp->f_flag&FPIPE, 'P');
		printf("%4d", fp->f_count);
		printf("%9.1x", fp->f_inode);
		printf("  %ld\n", fp->f_un.f_offset);
	}
}

doswap()
{
	int *xswap, *aswap;
	int nswap, nswapwords;
	int swapfree, swapused;

	nswap = getw(nl[SNSWAP].n_value) / MCPAGES;
	nswapwords = (nswap + sizeof(int) - 1)/(sizeof(int));

	xswap = (int *)calloc(nswapwords, sizeof (int));
	lseek(fc, (int)(aswap = (int *)getw(nl[SSWAPBITTABLE].n_value)), 0);
	read(fc, xswap, nswapwords);

	swapfree = bitcount(xswap, nswap);
	swapused = nswap - swapfree;
	if (totflg) {
		printf("%3d/%3d pgs swap\n", swapused, nswap);
		return;
	}
	printf("%d used, %d free\n", swapused, swapfree);
	free(xswap);
}

getu(procno, uptr)
int procno;
char *uptr;
{
	int phyu, viru, i;

	viru = nl[SUDOT].n_value + ((procno + 1) * USIZE);
	if (kflg) {
	    for (i = 0; i < UPAGES; i++) {
		    phyu = physaddr(SYSSPTI, viru);
		    if ((phyu < 0) || (lseek(fc, phyu, 0) < 0) ||
			read(fc, uptr, MCSIZE) != MCSIZE) {
				printf("pstat: can't read u for pid %d\n",
					procno);
				return (0);
		    }
		    uptr += MCSIZE;
		    viru += MCSIZE;
	    }
	    return (1);
	}
	if ((lseek(fc, viru, 0) < 0) || (read(fc, uptr, USIZE) != USIZE)) {
		printf("pstat: can't read u for pid %d\n", procno);
		return (0);
	}
	return (1);
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

	if ((addr >= MAXADDRS) || (spti >= nspt)) return(-1); 
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
