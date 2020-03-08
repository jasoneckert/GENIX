/*
 * @(#)dbm.h	1.3	8/31/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

#define	PBLKSIZ	1024
#define	DBLKSIZ	4096
#define	BYTESIZ	8
#define	NULL	((char *) 0)

static long	bitno;
static long	maxbno;
static long	blkno;
static long	hmask;

static char	pagbuf[PBLKSIZ];
static char	dirbuf[DBLKSIZ];

static int	dirf;
static int	pagf;
static int	dbrdonly;

typedef	struct
{
	char	*dptr;
	int	dsize;
} datum;

datum	fetch();
datum	makdatum();
datum	firstkey();
datum	nextkey();
datum	firsthash();
long	calchash();
long	hashinc();

