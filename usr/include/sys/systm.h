/*
 * @(#)systm.h	3.6	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Random set of variables
 * used by more than one
 * routine.
 */
/* extern char	canonb[CANBSIZ];	/* buffer for erase and kill (#@) */
extern	struct	inode	*rootdir;	/* pointer to inode of root directory */
extern	int	lbolt;			/* time of day in 60th not in time */
extern	int	hz;			/* clock frequency */
extern	time_t	time;			/* time in sec from 1970 */

/*
 * Nblkdev is the number of entries
 * (rows) in the block switch. It is
 * set in binit/bio.c by making
 * a pass over the switch.
 * Used in bounds checking on major
 * device numbers.
 */
extern	int	nblkdev;

/*
 * Number of character switch entries.
 * Set by cinit/prim.c
 */
extern	int	nchrdev;
extern	char	runrun;			/* scheduling flag */
extern	char	runin;			/* scheduling flag */
extern	char	runout;			/* scheduling flag */
extern	char	curpri;			/* more scheduling */
extern	char	noproc;			/* no process running */
extern	int	wantin;			/* more scheduling */
extern daddr_t	swplo;			/* block number of swap space */
extern	int	nswap;			/* size of swap space */
extern	int	updlock;		/* lock for sync */
extern	daddr_t	rablock;		/* block for read ahead */
extern	char	regloc[];		/* locs. of saved user regs (trap.c) */
extern dev_t	rootdev;		/* device of the root */
extern dev_t	pipedev;		/* pipe device */

extern	daddr_t	bmap();
extern	struct inode *ialloc();
extern	struct inode *ifind();
extern	struct inode *iget();
extern	struct inode *owner();
extern	struct inode *maknode();
extern	struct inode *namei();
extern	struct buf *alloc();
extern	struct buf *getblk();
extern	struct buf *geteblk();
extern	struct buf *bread();
extern	struct filsys *getfs();
extern	struct file *getf();
extern	struct file *falloc();
extern	int	uchar();

/*
 * Structure of the system-entry table
 */
extern struct sysent {			/* system call entry table */
	char	sy_narg;		/* total number of arguments */
	int	(*sy_call)();		/* handler */
} sysent[];

#define	SYSINDIR	0		/* ordinal of indirect sys call */
