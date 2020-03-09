/*
 * @(#)vmswap.h	1.2	9/2/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

#define	SWAPSIZE 32		/* maximum pages swapped in one operation */

/* Header for swapping operations (or demand-paged text reads).  Each swap
 * structure holds a list of all physical pages that want to be read from
 * a particular device, or written to a particular device.
 *
 * WARNING:	The first entry in the swap structure MUST be the swap
 *		buffer sw_buf (assummed by swapint).
 */

struct	swap	{
	struct	buf	sw_buf;		/* buffer for I/O (MUST BE FIRST) */
	struct	cst	*sw_queue;	/* list of pages waiting to transfer */
	struct	cst	*sw_active;	/* pages actively being transferred */
	long	sw_inval;		/* counter for swapinsert retries */
	short	sw_count;		/* count of pages queued */
	short	sw_dir;			/* direction of I/O */
	long	sw_pages[SWAPSIZE];	/* page addresses for current I/O */
};

#ifdef	KERNEL
extern	int	nswapbuf;		/* number of swapping buffers */
extern	struct	swap	*swapbuf;	/* address of swapping buffers */
extern	struct	swap	*swapbufNSWAPBUF;	/* end of swapping buffers */
#endif
