/*
 * @(#)dk.h	3.3	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Instrumentation
 */
#define	CPUSTATES	4

#define	CP_USER		0
#define	CP_NICE		1
#define	CP_SYS		2
#define	CP_IDLE		3

#define	DK_NDRIVE	4

#ifdef KERNEL
extern	long	cp_time[CPUSTATES];
#ifdef notdef
int	dk_busy;
long	dk_time[DK_NDRIVE];
long	dk_seek[DK_NDRIVE];
long	dk_xfer[DK_NDRIVE];
long	dk_wds[DK_NDRIVE];
float	dk_mspw[DK_NDRIVE];

long	tk_nin;
long	tk_nout;
#endif
#endif
