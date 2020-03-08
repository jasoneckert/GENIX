/*
 * param.c: version 1.16 of 11/9/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)param.c	1.16 (NSC) 11/9/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/buf.h"
#include "../h/inode.h"
#include "../h/file.h"
#include "../h/callout.h"
#include "../h/mount.h"
#include "../h/vm.h"
#include "../h/vmswap.h"
#include "../h/clist.h"

extern	int	totalpages;

/*
 * System parameter formulae.
 *
 * This file is copied into each directory where we compile
 * the kernel; it should be modified there to suit local taste
 * if necessary.
 *
 * Formerly compiled with -DHZ=xx -DTIMEZONE=x -DDST=x -DMAXUSERS=xx
 */

#ifndef	HZ
#define	HZ 60						/* line frequency */
#endif	HZ

#ifndef	TIMEZONE
#define	TIMEZONE (8*60)					/* minutes from GMT */
#endif	TIMEZONE

#ifndef	DST
#define	DST 1						/* daylight saving on */
#endif	DST

#ifndef	MAXUSERS
#define	MAXUSERS 8					/* maximum users */
#endif	MAXUSERS

#define	NPROC (30 + 4 * MAXUSERS)			/* processes */
#define	NTEXT (20 + MAXUSERS)				/* shared texts */
#define	NMOUNT 5					/* file systems */
#define	NINODE ((NPROC + 16 + MAXUSERS) + 32)		/* inodes */
#define	NFILE (8 * (NPROC + 16 + MAXUSERS) / 10 + 32)	/* open files */
#define	NCALLOUT (16 + MAXUSERS)			/* callouts */
#define	NCLIST (100 + 16 * MAXUSERS)			/* character lists */
#define	MAXUPRC ((NPROC * 2) / MAXUSERS)		/* per user processes */
#define	NBUF 40						/* file buffers */
#define	NKALLOC	200					/* scratch pages */
#define	NSWAP 5000					/* swapping sectors */
#define	NSWAPDEV 1					/* swap devices */
#define	MAXPHYSIO (64 * MCSIZE)				/* largest phys io */

int	hz = HZ;
int	timezone = TIMEZONE;
int	dstflag = DST;
int	nproc = NPROC;
int	ntext = NTEXT;
int	nbuf = NBUF;
int	ninode	= NINODE;
int	nfile	= NFILE;
int	nmount = NMOUNT;
int	ncallout = NCALLOUT;
int	nclist = NCLIST;
int	nkalloc = NKALLOC;
int	nswap = NSWAP;
int	nswapdev = NSWAPDEV;
int	maxphysio = MAXPHYSIO;
int	maxuprc = MAXUPRC;
daddr_t	swplo = SWAPLO;
daddr_t	dumplo = NSWAP - (2*2048);


/*
 * These have to be allocated somewhere; allocating
 * them here forces loader errors if this file is omitted.
 */
struct	spt *spt, *sptNSPT, *sptFILESPTI;
struct	proc *proc, *procNPROC;
struct	file *file, *fileNFILE;
struct	inode *inode, *inodeNINODE;
struct	mount *mount, *mountNMOUNT;
struct	cst *cst, *cstNCST;
struct	swap *swapbuf, *swapbufNSWAPBUF;
struct	cblock *cfree;
struct	callout *callout;
struct	buf *buf;
char	*buffers;
char	*swapbittable;
char	*kalloctable;
pte	*botpte1;
pte	*toppte1;
int	filespti;
int	lastprocspti;
int	nswapbuf;
int	nspt;
int	ncst;


#define	valloc(name, type, num) \
	(name) = (type *)(v); (v) = (caddr_t)((name)+(num))
#define valloclim(name, type, num, lim) \
	(name) = (type *)(v); (v) = (caddr_t)((lim) = ((name)+(num)))


/* routine to allocate all parameterized tables */
alloctables()
{
	register caddr_t v;		/* next free address */

	ncst = totalpages;
	nspt = nproc + ntext;
	lastprocspti = nproc - 1;
	filespti = nproc;
	nswapbuf = (nswapdev * 2) + nmount;
	topsys = (topsys + (P1SIZE-1)) & ~(P1SIZE-1);	/* need 1K alignment */
	v = (caddr_t) topsys;
	valloclim(botpte1, pte, (P1SIZE/PTESIZE) * nspt, toppte1);
	valloc(buffers, char, BSIZE * nbuf);
	valloc(buf, struct buf, nbuf);
	valloclim(swapbuf, struct swap, nswapbuf, swapbufNSWAPBUF);
	valloclim(inode, struct inode, ninode, inodeNINODE);
	valloclim(file, struct file, nfile, fileNFILE);
	valloclim(mount, struct mount, nmount, mountNMOUNT);
	valloclim(proc, struct proc, nproc, procNPROC);
	valloclim(cst, struct cst, ncst, cstNCST);
	valloclim(spt, struct spt, nspt, sptNSPT);
	sptFILESPTI = &spt[filespti];
	valloc(cfree, struct cblock, nclist);
	valloc(callout, struct callout, ncallout);
	valloc(kalloctable, char, (nkalloc+7)/8);
	valloc(swapbittable, char, ((nswap/MCPAGES)+7)/8);
	bzero(topsys, ((int)v) - ((int)topsys));	/* clear memory */
	topsys = (((int)v) + MCOFSET) & ~MCOFSET;	/* topsys is next pg */
}
