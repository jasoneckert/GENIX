/*
 * conf.c: version 3.22 of 11/9/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)conf.c	3.22 (NSC) 11/9/83";
# endif


#include "../h/param.h"
#include "../h/systm.h"
#include "../h/buf.h"
#include "../h/tty.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/file.h"
#include "../h/inode.h"

int	nulldev();
int	nodev();

int	dcustrategy(), dcuopen(), dcuread(), dcuwrite(), dcudump();
int	tcustrategy(), tcuopen(), tcuclose(), tcuread(), tcuwrite(), tcuioctl();
struct	buf	dcutab;
struct	buf	tcutab;

struct	bdevsw	bdevsw[] =		/* BLOCK DEVICES */
{
	dcuopen, nulldev, dcustrategy, dcudump, &dcutab,	/* disk=0 */
	tcuopen, tcuclose, tcustrategy, nodev, &tcutab,		/* tape=1 */
	0
};


/*
 * The following driver routines are always present.
 */
int	syopen(), syread(), sywrite(), syioctl();
int	ptsopen(), ptsclose(), ptsread(), ptswrite(), ptyioctl();
int	ptcopen(), ptcclose(), ptcread(), ptcwrite();
int	mmread(), mmwrite(), nullioctl();

/*
 * The following driver routines may or may not be present,
 * depending on configuration.
 */

# if NBM > 0
int	bmopen(), bmclose(), bmread(), bmwrite(), bmioctl();
# else
# define bmopen		nodev
# define bmclose	nulldev
# define bmread		nulldev
# define bmwrite	nulldev
# define bmioctl	nulldev
# endif NBM

# if NRS > 0
int	rsopen(), rsclose(), rsread(), rswrite(), rsioctl(), rsstop();
# else
# define rsopen		nodev
# define rsclose	nulldev
# define rsread		nulldev
# define rswrite	nulldev
# define rsioctl	nulldev
# define rsstop		nulldev
# endif NRS

# if NSIO > 0
int	sio_open(), sio_close(), sio_read(), sio_write(),
	sio_ioctl(), sio_stop();
# else
# define sio_open	nodev
# define sio_close	nulldev
# define sio_read	nulldev
# define sio_write	nulldev
# define sio_ioctl	nulldev
# define sio_stop	nulldev
# endif NSIO

# if NLP > 0
int	lpopen(), lpclose(), lpwrite();
# else
# define lpopen		nodev
# define lpclose	nulldev
# define lpwrite	nulldev
# endif

struct	cdevsw	cdevsw[] =		/* CHARACTER DEVICES */
{
	rsopen,		rsclose,	rsread,		rswrite,
	rsioctl,	rsstop,		0,			/* console=0 */
	nulldev,	nulldev,	mmread,		mmwrite,
	nullioctl,	nulldev,	0,		 	/* memory=1 */
	nulldev,	nulldev,	dcuread,	dcuwrite,
	nullioctl,	nulldev,	0,			/* disk=2 */
	syopen,		nulldev,	syread,		sywrite,
	syioctl,	nulldev,	0,		/* generic tty=3 */
	ptsopen,	ptsclose,	ptsread,	ptswrite,
	ptyioctl,	nulldev,	0,			/* pty tty=4 */
	ptcopen,	ptcclose,	ptcread,	ptcwrite,
	ptyioctl,	nulldev,	0,			/* pty ctrl=5 */
	tcuopen,	tcuclose,	tcuread,	tcuwrite,
	tcuioctl,	nulldev,	0,			/* tape=6 */
	bmopen,		bmclose,	bmread,		bmwrite,
	bmioctl,	nulldev,	0,			/* bitmap=7 */
	sio_open,	sio_close,	sio_read,	sio_write,
	sio_ioctl,	sio_stop,	0,			/* sio=8 */
	lpopen,		lpclose,	nulldev,	lpwrite,
	nulldev,	nulldev,	0,			/* lp=9 */
	0
};

int	ttstart();
#ifdef UCB_NTTY
int	ntyopen(),ntyclose(),ntread();
char	*ntwrite();
int	ntyinput(),ntyrend();
#endif

struct	linesw linesw[] =
{
#ifndef NO_OLDTTY
	ttyopen, nulldev, ttread, ttwrite, nodev,
	ttyinput, ttstart, nulldev, nulldev, nulldev,	/* 0 */
#else
	ntyopen, ntyclose, ntread, ntwrite, nullioctl,
	ntyinput, ntyrend, nulldev, ttstart, nulldev,	/* 0 */
#endif
/* Berkeley network stuff would go here -- hold its place */
	nulldev, nulldev, nulldev, (char *(*)())nulldev, nulldev,
	nulldev, nodev, nulldev, nulldev, nulldev,	/* 1 */
/* New line discipline */
	ntyopen, ntyclose, ntread, ntwrite, nullioctl,
	ntyinput, ntyrend, nulldev, ttstart, nulldev,	/* 2 */
};

dev_t	rootdev	= makedev(0,8);		/* unit 1, subunit 0 */
dev_t	pipedev	= makedev(0,8);		/* unit 1, subunit 0 */
dev_t	swapdev	= makedev(0,9);		/* unit 1, subunit 1 */
dev_t	dumpdev	= makedev(0,9);		/* unit 1, subunit 1 */
dev_t	consdev = makedev(0,0);		/* console device */
int	consmajor = 0;			/* major device of console */
int	bmmajor = 7;			/* major device of bitmap display */
