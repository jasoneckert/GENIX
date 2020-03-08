/*
 * @(#)pagsiz.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

#define	NBPG	512
#define	PGOFSET	511
#define	CLSIZE	2
#define	CLOFSET	1023
#define	BSIZE	1024
#define	BMASK	1023
#define	BSHIFT	10
#define	PAGSIZ	(NBPG*CLSIZE)
#define	PAGRND	((PAGSIZ)-1)
