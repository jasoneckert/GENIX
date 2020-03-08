static char *sccsid = "@(#)main.c	3.32	11/9/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/filsys.h"
#include "../h/mount.h"
#include "../h/proc.h"
#include "../h/inode.h"
#include "../h/conf.h"
#include "../h/buf.h"
#include "../h/mmu.h"
#include "../h/vm.h"
#include "../h/vlimit.h"
#include "../h/devvm.h"		/* TEMPORARY for runtime options */

extern	char	version[];	/* name of system */
extern	int	bmmajor;	/* initial major device */
extern	int	consmajor;	/* initial console device */
extern	dev_t	consdev;	/* console device */
extern	time_t	bootime;	/* time of boot */
extern	char	*icode;		/* address of process 0 init code */
extern	int	szicode;	/* size of code */
extern	char	*buffers;	/* buffer allocation area */
long	bmconsf;		/* nonzero if bitmap device is console */
long	suusf;			/* nonzero to allow use of movsu and movus */
long	floatf = 1;		/* nonzero to allow use of floating point */
long	dummy;			/* a dummy for general assignments */

/*
 * Initialization code.
 * Called from cold start routine as
 * soon as a stack
 * has been established.
 * Functions:
 *	clear and free user core
 *	turn on clock
 *	hand craft 0th process
 *	call all initialization routines
 *	load - process 0 execute bootstrap
 *
 * loop at low address in user mode -- /etc/init
 *	cannot be executed.
 */
main()
{
	register int addr, i;
	register struct	proc	*procp;
	struct	cdevsw	tempdev;

	/* The following reads the hardware option switch.  We use a few of
	 * the switch bits to enable certain software features.
	 */
	i = *((short *) CP_OptBuffer);
	suusf = i & 0x200;	/* BIT 2: TEMPORARY.  Allow movsu in copyout */
#if NBM > 0
#if NRS > 0
	bmconsf = ((i&0x400)==0);/* BIT 3: Make bitmap be console if off */
#else
	bmconsf = 1;
#endif
	if (bmconsf) {		/* swap table entries so console is bitmap */
		tempdev = cdevsw[consmajor];
		cdevsw[consmajor] = cdevsw[bmmajor];
		cdevsw[bmmajor] = tempdev;
	}
#endif

	updlock = 0;
	rqinit();
	startup();

	/*
	 * set up system process
	 */

	procp = &proc[0];
	procp->p_stat = SRUN;
	procp->p_flag |= SLOAD|SSYS;
	procp->p_nice = NZERO;
	procp->p_maxrss = INFINITY/NBPG;
	procp->p_spti = SYSSPTI;
	u.u_procp = procp;
	u.u_spti = SYSSPTI;
	u.u_xspti = -1;
	u.u_cmask = CMASK;
	for (i=1; i < sizeof(u.u_limit)/sizeof(u.u_limit[0]); i++) {
		switch (i) {

		case LIM_STACK:
			u.u_limit[i] = USERCORE;
			continue;
		case LIM_DATA:
			u.u_limit[i] = USERCORE;
			continue;
		default:
			u.u_limit[i] = INFINITY;
			continue;
		}
	}

	/*
	 * Initialize devices and
	 * set up 'known' i-nodes
	 */

	ihinit();
	bhinit();
	cinit();
	callinit();			/* init callouts */
#	if NBM > 0
	init_termem();			/* temp - start screen */
#	endif
	printf(version);		/* say howdy!! */
	binit();
	clkstart();
	iinit();
	rootdir = iget(rootdev, (ino_t)ROOTINO);
	if (rootdir == NULL) panic("NOROOTDIR");
	rootdir->i_flag &= ~ILOCK;
	u.u_cdir = iget(rootdev, (ino_t)ROOTINO);
	u.u_cdir->i_flag &= ~ILOCK;
	u.u_rdir = NULL;
	mtpr(ptb1, spt[1].spt_pte1);
	(*cdevsw[consmajor].d_open)(consdev, 1); /* open console to see init */
	if (newproc(0)) {
		/* in process 1
		 *
 	 	 * Icode is the bootstrap program executed in user mode
 	 	 * to bring up the system.
 	 	 */

		while ((addr = allocpage(1, 0, PTY_DATA)) <= 0) {
			if (addr < 0) panic("noinitpage");
		}
		setmap(1, 0, PTEMEM(addr, PG_RWRW));	/* copyout needs RW */
		copyout(&icode, 0, szicode);
		unlockaddr(addr);
		return;
		/*
	 	 * Return goes to loc. icode of user init
	 	 * code .
	 	 */
	}
	else sched();
}
extern panic();
extern brelse(),bcopy();
/*
 * iinit is called once (from main)
 * very early in initialization.
 * It reads the root's super block
 * and initializes the current date
 * from the last modified date.
 *
 * panic: iinit -- cannot read the super
 * block. Usually because of an IO error.
 */
iinit()
{
	register struct buf *bp;
	register struct filsys *fp;
	register int i;

	u.u_error = 0;
	(*bdevsw[major(rootdev)].d_open)(rootdev, 1);
	bp = bread(rootdev, SUPERB);
	if(u.u_error)
		panic("iinit");
	bp->b_flags |= B_LOCKED;		/* block can never be re-used */
	brelse(bp);
	mount[0].m_dev = rootdev;
	mount[0].m_bufp = bp;
	fp = bp->b_un.b_filsys;
	fp->s_flock = 0;
	fp->s_ilock = 0;
	fp->s_ronly = 0;
	fp->s_lasti = 1;
	fp->s_nbehind = 0;
	fp->s_fname[0] = '/';
	for (i = 1; i < sizeof(fp->s_fname); i++)
		fp->s_fname[i] = 0;
	time = fp->s_time;
	bootime = time;
}

/*
 * Initialize the buffer I/O system by freeing
 * all buffers and setting all device buffer lists to empty.
 */
binit()
{
	register struct buf *bp;
	register struct buf *dp;
	register int i;
	register struct bdevsw *bdp;

	for (dp = bfreelist; dp < &bfreelist[BQUEUES]; dp++) {
		dp->b_forw = dp->b_back = dp->av_forw = dp->av_back = dp;
		dp->b_flags = B_HEAD;
	}
	dp--;				/* dp = &bfreelist[BQUEUES-1]; */
	for (i=0; i<nbuf; i++) {
		bp = &buf[i];
		bp->b_dev = NODEV;
		bp->b_un.b_addr = buffers + (BSIZE * i);
		bp->b_back = dp;
		bp->b_forw = dp->b_forw;
		dp->b_forw->b_back = bp;
		dp->b_forw = bp;
		bp->b_flags = B_BUSY|B_INVAL;
		brelse(bp);
	}
	for (bdp = bdevsw; bdp->d_open; bdp++) {
		dp = bdp->d_tab;
		if(dp) {
			dp->b_forw = dp;
			dp->b_back = dp;
		}
		nblkdev++;
	}
}
