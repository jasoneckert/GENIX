/*
 * @(#)param.h	3.14	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

#define	NOFILE	20		/* max open files per process */
#define	CANBSIZ	256		/* max size of typewriter line */
#define	MSGBUFS	128		/* Characters saved from error messages */
#define	NCARGS	10240		/* # characters in exec arglist */
#define PGSIZE	512		/* bytes per addressable disk sector */

/*
 * Magic system offsets
 */

#define UPAGES	4		/* Number of PGSIZE blocks for u_. */
#define USIZE	(UPAGES*PGSIZE)	/* Size of _u. (in bytes) */
#define SSIZE	(PGSIZE)	/* Stack size */
#define	P1PAGES	2		/* pages for pte1 page tables (hardware set) */
#define	P1SIZE	(P1PAGES*PGSIZE)/* size of level 1 page tables */

#define TOPUSER	((128*256-2)*PGSIZE)	/* end of user virtual address space */
#define KERNELSTACK	256	/* amount of space to leave for kernel stack */
#define SWAPLO	0		/* beginning of swap area in blocks */
#define USERCORE TOPUSER	/* Abs. max user proc size */

#define TOPPHYMEM	0x380000	/* Maximum bytes of physical memory */
#define	MEMCHUNK	0x40000		/* "Quantum" amt of physical memory */
#define	MEMOFFSET	0x3ffff
#define	MEMSHIFT	18		/* LOG2(MEMCHUNK) */
#define MEMCHKS	(TOPPHYMEM/MEMCHUNK)	/* Maximum physical memory quanta */

/*
 * priorities
 * probably should not be
 * altered too much
 */

#define	PSWP	0
#define	PINOD	10
#define	PRIBIO	20
#define	PZERO	25
#define	NZERO	20
#define	PPIPE	26
#define	PWAIT	30
#define	PSLEP	40
#define	PUSER	50

/*
 * signals
 * dont change
 */
#ifndef LOCORE

# ifndef NSIG
#include	<signal.h>
# endif
#endif

/*
 * fundamental constants of the implementation--
 * cannot be changed easily
 */

#define	NBPW	sizeof(int)	/* number of bytes in an integer */
#define	NBPG	PGSIZE		/* size of basic MMU unit (one page) */
#define PGOFSET	(NBPG-1)	/* byte offset into page */

#define	MCPAGES	2		/* pages in a memory cluster (min allocation) */
#define	MCSHIFT	10		/* shift from address to memory cluster */
#define	MCSIZE	((PGSIZE) * (MCPAGES))	/* bytes in a memory cluster */
#define	MCOFSET	((MCSIZE) - 1)	/* maximum offset in a cluster */

#ifndef LOCORE

#define	BSIZE	1024		/* size of secondary block (bytes) */
			/* BSLOP can be 0 unless you have a TIU/Spider */
#define	BSLOP	0		/* In case some device needs bigger buffers */
#define	NINDIR	(BSIZE/sizeof(daddr_t))
#define	BMASK	01777		/* BSIZE-1 */
#define	BSHIFT	10		/* LOG2(BSIZE) */
#define	NMASK	0377		/* NINDIR-1 */
#define	NSHIFT	8		/* LOG2(NINDIR) */

#define	NULL	0
#define	CMASK	0		/* default mask for file creation */
#define	NODEV	(dev_t)(-1)
#define	ROOTINO	((ino_t)2)	/* i number of all roots */
#define	SUPERB	((daddr_t)1)	/* block number of the super block */
#define	DIRSIZ	14		/* max characters per directory */

#define	NICINOD	100		/* number of superblock inodes */
#define	NICFREE	178		/* number of superblock free blocks */

#define	CBSIZE	28		/* number of chars in a clist block */
#define	CROUND	0x1f		/* clist rounding: sizeof(int *) + CBSIZE - 1*/

#define PGSHIFT	9		/* LOG2(PGSIZE) */

/*
 * Some macros for units conversion
 */
#define btoc(s)	s
#define	ptoc(n) (((n) + MCPAGES - 1) / MCPAGES)		/* pages to clusters */

# ifndef MIN
#  define	MIN(a,b)	(((a)<(b))? (a):(b))
# endif
#define		MAX(a,b)	(((a)>(b))? (a):(b))

/* inumber to disk address */
#define itod(x) ((daddr_t)((((unsigned)(x)+2*INOPB-1)/INOPB)))

/* inumber to disk offset */
#define itoo(x) ((int)(((x)+2*INOPB-1)%INOPB))

/* file system blocks to disk blocks and back */
#define fsbtodb(b)	((daddr_t)((daddr_t)(b)<<1))
#define dbtofsb(b)	((daddr_t)((daddr_t)(b)>>1))

/* disk block and unit accessing */
#define	dkblock(bp)	((bp)->b_blkno)
#define	dkunit(bp)	(minor((bp)->b_dev) >> 3)

typedef	struct { int r[1]; } *	physadr;

# ifdef KERNEL
# include "../h/types.h"
# else
# include <sys/types.h>
# endif

/*
 * Machine-dependent bits and macros
 */


#define	UMODE	0x100		/* usermode bits */
#define	USERMODE(ps)	((ps.psr & UMODE)==UMODE)

#define	INTPRI	0x800		/* Interrupt  bit */
#define	BASEPRI(ps)	((ps.psr & INTPRI) != 0)

#endif	LOCORE
