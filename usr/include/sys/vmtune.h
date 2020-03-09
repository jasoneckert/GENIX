/*
 * @(#)vmtune.h	3.4	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Tunable performance parameters
 *
 * TENTATIVE!!!!
 */
#define	LOTSFREE	90	/* don't work hard at swapping */
#define	DESFREE		50	/* minimum desirable free memory */
#define	MINFREE		10	/* getting desperate, swap something */
#define	MAXSLP		5	/* max blocked time (in seconds) allowed
				   before being very swappable */
#define	MAXPGIO		12	/* max desired paging i/o per second */

#ifdef KERNEL
extern int	maxslp;
extern int	maxpgio;
extern int	minfree;
extern int	desfree;
extern int	lotsfree;

extern int	avefree;
extern int	avefree30;
extern int	deficit;	/* estimate of needs of new swapped in procs */
extern int	pagesfree;
#endif

/* Average new into old with aging factor time */
#define	ave(smooth, cnt, time) \
	smooth = ((time - 1) * (smooth) + (cnt)) / (time)
