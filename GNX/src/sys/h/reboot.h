/*
 * @(#)reboot.h	1.3	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */


/*
 * Arguments to reboot system call.
 * These are passed to boot program in r7,
 * and on to init.
 */
#define	RB_AUTOBOOT	0	/* flags for system auto-booting itself */

#define	RB_ASKNAME	1	/* ask for file name to reboot from */
#define	RB_SINGLE	2	/* reboot to single user only */
#define	RB_NOSYNC	4	/* dont sync before reboot */
#define	RB_HALT		8	/* don't reboot, just halt */

#define	RB_PANIC	0	/* reboot due to panic */
#define	RB_BOOT		1	/* reboot due to boot() */
