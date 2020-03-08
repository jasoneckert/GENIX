/*
 * vmstat.c: version 1.15 of 9/7/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)vmstat.c	1.15 (NSC) 9/7/83";
# endif

#include <stdio.h>
#include <sys/param.h>
#include <sys/dk.h>
#include <nlist.h>
#include <sys/vmmeter.h>
#include <sys/vmsystm.h>

struct nlist nl[] = {
#define	X_FORKSTAT	0
	{ "_forkstat" },
#define X_SUM		1
	{ "_sum" },
#define	X_BOOTIME	2
	{ "_bootime" },
#define	X_RATE		3
	{ "_rate" },
#define X_TOTAL		4
	{ "_total" },
#define	X_DEFICIT	5
	{ "_deficit" },
#define X_HZ		6
	{ "_hz" },
#define	X_CPTIME	7
	{ "_cp_time" },
#define	X_DCXFER	8
	{ "_dc_xfer" },
	{ 0 },
};

#define	nz(x)	((x) ? (x) : 1)

#ifdef notdef
char dr_name[DK_NDRIVE][10];
char dr_unit[DK_NDRIVE];
int	firstfree, maxfree;
#endif

long	stat1();
int	hz;

struct
{
	long	xfer[DK_NDRIVE];
	long	time[CPUSTATES];
	struct	vmmeter Rate;
	struct	vmtotal	Total;
	struct	vmmeter Sum;
	struct	forkstat Forkstat;
} s, s1, z;

#define	rate		s.Rate
#define	total		s.Total
#define	sum		s.Sum
#define	forkstat	s.Forkstat

long	etime;
int	deficit;
int 	mf;
char	meanf;

main(argc, argv)
char **argv;
{
	time_t now;
	int lines;
	register i;
	int iter, nintv;
	time_t bootime;
	long t;
	long f;
	extern char _sobuf[];

	setbuf(stdout, _sobuf);
	cnlist("/vmsymbols", nl);
	if(nl[0].n_type == 0) {
		printf("no /vmsymbols namelist\n");
		exit(1);
	}
	mf = open("/dev/kmem", 0);
	if(mf < 0) {
		printf("cannot open /dev/kmem\n");
		exit(1);
	}
	iter = 0;
	argc--, argv++;
	while (argc>0 && argv[0][0]=='-') {
		char *cp = *argv++;
		argc--;
		while (*++cp) switch (*cp) {

		case 'z':
			close(mf);
			mf = open("/dev/kmem", 2);
			lseek(mf, (long)nl[X_SUM].n_value, 0);
			write(mf, &z.Sum, sizeof z.Sum);
			lseek(mf, (long)nl[X_FORKSTAT].n_value, 0);
			write(mf, &z.Forkstat, sizeof z.Forkstat);
			exit(0);

		case 'f':
			doforkst();
			exit(0);

		case 'm':
			meanf = 1;
			continue;

		case 's':
			dosum();
			exit(0);

		default:
			fprintf(stderr, "usage: vmstat [ -fms ] [ interval ] [ count]\n");
			exit(1);
		}
	}
	if (meanf) {
		nice(-40);		/* set high priority */
		for (i = sbrk(0); i >= 0; i -= 1024) {
			vlock(1, i);	/* lock text and data */
		}
		vlock(1, &argc);	/* lock stack */
	}
	if(argc > 1)
		iter = atoi(argv[1]);
	lseek(mf, (long)nl[X_BOOTIME].n_value, 0);
	read(mf, &bootime, sizeof bootime);
	lseek(mf, (long)nl[X_HZ].n_value, 0);
	read(mf, &hz, sizeof hz);
#ifdef notdef
	for (i = 0; i < DK_NDRIVE; i++) {
		strcpy(dr_name[i], "xx");
		dr_unit[i] = i;
	}
	read_names();
#endif
	time(&now);
	nintv = now - bootime;
	if (nintv <= 0 || nintv > 60*60*24*365*10) {
		printf("Time makes no sense... namelist must be wrong.\n");
		exit(1);
	}
reprint:
	lines = 20;
#ifdef notdef
printf("\
 procs     memory           page                  disk     faults       cpu\n\
 r b w   avm  fre  pf  pi  po  fo  zr  de %c%d %c%d %c%d %c%d  in  sy  cs us sy id\n\
", dr_name[0][0], dr_unit[0], dr_name[1][0], dr_unit[1], dr_name[2][0], dr_unit[2], dr_name[3][0], dr_unit[3]);
#else
printf("\
 procs     memory           page               disk     faults       cpu\n\
 r b w   avm  fre  pf  pi  po  fo  zr  de d0 d1 d2 d3  in  sy  cs us sy id\n\
");
#endif


loop:
	lseek(mf, (long)nl[X_CPTIME].n_value, 0);
 	read(mf, s.time, sizeof s.time);
	if (nl[X_DCXFER].n_value) {
		lseek(mf, (long)nl[X_DCXFER].n_value, 0);
		read(mf, s.xfer, sizeof s.xfer);
	}
	if (nintv != 1) {
		lseek(mf, (long)nl[X_SUM].n_value, 0);
		read(mf, &rate, sizeof rate);
	} else {
		lseek(mf, (long)nl[X_RATE].n_value, 0);
		read(mf, &rate, sizeof rate);
	}
	lseek(mf, (long)nl[X_TOTAL].n_value, 0);
	read(mf, &total, sizeof total);
	lseek(mf, (long)nl[X_DEFICIT].n_value, 0);
	read(mf, &deficit, sizeof deficit);
	etime = 0;
	for (i=0; i < DK_NDRIVE; i++) {
		t = s.xfer[i];
		s.xfer[i] -= s1.xfer[i];
		s1.xfer[i] = t;
	}
	for (i=0; i < CPUSTATES; i++) {
		t = s.time[i];
		s.time[i] -= s1.time[i];
		s1.time[i] = t;
		etime += s.time[i];
	}
	if(etime == 0)
		etime = 1;
	printf("%2d%2d%2d", total.t_rq, total.t_dw+total.t_pw, total.t_sw);
	printf("%6d%5d", total.t_avm, total.t_free);
	printf("%4d", rate.v_faults/nintv);
	printf("%4d", rate.v_pgin/nintv);
	printf("%4d%4d%4d%4d", rate.v_pgout/nintv, rate.v_fpgout/nintv,
	    rate.v_pgszrod/nintv, deficit);
	etime /= 60;
	for(i=0; i<4; i++)
		stats(i);
	printf("%4d%4d", (rate.v_intr/nintv) - hz, rate.v_syscall/nintv);
	printf("%4d", rate.v_swtch/nintv);
	for(i=0; i<CPUSTATES; i++) {
		f = stat1(i);
		if (i == 0) {		/* US+NI */
			i++;
			f += stat1(i);
		}
		printf("%3d", f);
	}
	printf("\n");
	fflush(stdout);
contin:
	nintv = 1;
	--iter;
	if(iter)
	if(argc > 0) {
		sleep(atoi(argv[0]));
		if (--lines <= 0)
			goto reprint;
		goto loop;
	}
}

dosum()
{

	lseek(mf, (long)nl[X_SUM].n_value, 0);
	read(mf, &sum, sizeof sum);

	printf("%9d swap ins\n", sum.v_swpin);
	printf("%9d swap outs\n", sum.v_swpout);
	printf("%9d dormant text swap outs\n", sum.v_tswpout);
	printf("%9d total address trans. faults taken\n", sum.v_faults);
	printf("%9d page ins\n", sum.v_pgin);
	printf("%9d page outs\n", sum.v_pgout);
	printf("%9d free page outs\n", sum.v_fpgout);
	printf("%9d pages zeroed at idle\n", sum.v_pgszrod);
	printf("%9d cpu context switches\n", sum.v_swtch);
	printf("%9d device interrupts\n", sum.v_intr);
	printf("%9d traps\n", sum.v_trap);
	printf("%9d system calls\n", sum.v_syscall);
}


doforkst()
{

	lseek(mf, (long)nl[X_FORKSTAT].n_value, 0);
	read(mf, &forkstat, sizeof forkstat);
	printf("%d forks, %d pages, average=%d.%02d\n",
		forkstat.cntfork, forkstat.sizfork,
		forkstat.sizfork / nz(forkstat.cntfork),
		((forkstat.sizfork * 100) / nz(forkstat.cntfork)) % 100);
	printf("%d vforks, %d pages, average=%d.%02d\n",
		forkstat.cntvfork, forkstat.sizvfork,
		forkstat.sizvfork / nz(forkstat.cntvfork),
		((forkstat.sizvfork * 100) / nz(forkstat.cntvfork)) % 100);
}

stats(dn)
{
	if (dn >= DK_NDRIVE) {
		printf("   ");
		return;
	}
	printf("%3d", s.xfer[dn]/(nz(etime)));
}

long
stat1(row)
{
	long t;
	register i;

	t = 0;
	for(i=0; i<CPUSTATES; i++)
		t += s.time[i];
	if(t == 0)
		t = 1;
	return(s.time[row]*100/t);
}

/*
 * Read the drive names out of kmem.
 * ARGH ARGH ARGH ARGH !!!!!!!!!!!!
 */

#ifdef notdef
#define steal(where, var) lseek(mf, where, 0); read(mf, &var, sizeof var);
read_names()
{
	struct mba_device mdev;
	register struct mba_device *mp;
	struct mba_driver mdrv;
	short two_char;
	char *cp = (char *) &two_char;
	struct uba_device udev, *up;
	struct uba_driver udrv;

	mp = (struct mba_device *) nl[X_MBDINIT].n_value;
	up = (struct uba_device *) nl[X_UBDINIT].n_value;
	if (up == 0) {
		fprintf(stderr, "iostat: Disk init info not in namelist\n");
		exit(1);
	}
	if (mp) for (;;) {
		steal(mp++, mdev);
		if (mdev.mi_driver == 0)
			break;
		if (mdev.mi_dk < 0 || mdev.mi_alive == 0)
			continue;
		steal(mdev.mi_driver, mdrv);
		steal(mdrv.md_dname, two_char);
		sprintf(dr_name[mdev.mi_dk], "%c%c", cp[0], cp[1]);
		dr_unit[mdev.mi_dk] = mdev.mi_unit;
	}
	for (;;) {
		steal(up++, udev);
		if (udev.ui_driver == 0)
			break;
		if (udev.ui_dk < 0 || udev.ui_alive == 0)
			continue;
		steal(udev.ui_driver, udrv);
		steal(udrv.ud_dname, two_char);
		sprintf(dr_name[udev.ui_dk], "%c%c", cp[0], cp[1]);
		dr_unit[udev.ui_dk] = udev.ui_unit;
	}
}
#endif
