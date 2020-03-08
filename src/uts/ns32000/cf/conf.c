/*
 * %M%: version %I% of %H%
 */
#ifdef SCCS
static char *sccsid = "%W%";
#endif

/*
 *  Configuration information
 */

#include	"config.h"
#include	"sys/param.h"
#include	"sys/types.h"
#include	"sys/sysmacros.h"
#include	"sys/space.h"
#include	"sys/conf.h"

#include	"ais/osiopkt.h"
#include	"ais/vinfo.h"
#include	"ais/vblk.h"
#include	"ais/vchar.h"
#include	"ais/vtape.h"
#include	"ais/vfile.h"
/* #include	"ais/vhmem.h"	*/

extern nodev(), nulldev();
extern vbopen(), vbclose(), vbread(), vbwrite(), vbstrategy();
extern vcopen(), vcclose(), vcread(), vcwrite(), vcstrategy(), vcioctl();
extern vtopen(), vtclose(), vtread(), vtwrite(), vtstrategy(), vtioctl();
extern sxtopen(), sxtclose(), sxtread(), sxtwrite(), sxtstrategy(), sxtioctl();
extern mmread(), mmwrite();

#if HOSTMEM==1
extern hmopen(), hmread(), hmwrite(), hmclose() /*hmioctl()*/;
#endif

extern ftopen(), ftread(), ftwrite(), ftclose(), ftioctl();
extern syopen(), syread(), sywrite(), syioctl();
extern erropen(), errclose(), errread();
extern	struct	tty	vc_tty[];

struct bdevsw bdevsw[] = {
/* 0*/	vbopen, vbclose, vbstrategy, nulldev,
/* 1*/	vtopen, vtclose, vtstrategy, vtioctl,
/* 2*/	nulldev, nulldev, nulldev, nulldev,
};

struct cdevsw cdevsw[] = {
/* 0*/	vcopen,	vcclose, vcread, vcwrite, vcioctl, vc_tty,
/* 1*/	sxtopen, sxtclose, sxtread, sxtwrite, sxtioctl, 0,
/* 2*/	syopen,	nulldev, syread, sywrite, syioctl, 0,
/* 3*/	nulldev, nulldev, mmread, mmwrite, nodev, 0,
/* 4*/	vbopen, vbclose, vbread, vbwrite, nodev, 0,
/* 5*/	vtopen, vtclose, vtread, vtwrite, vtioctl, 0,
/* 6*/	ftopen, ftclose, ftread, ftwrite, ftioctl, 0,

#if HOSTMEM==1
/* 7*/	hmopen, hmclose, hmread, hmwrite, nulldev /*hmioctl*/, 0,
#else
/* 7*/	nodev, nodev, nodev, nodev, nodev, 0,
#endif

/* 8*/	erropen, errclose, errread, nodev, nodev, 0,
};

int	bdevcnt = 3;
int	cdevcnt = 9;

dev_t	rootdev = makedev(0x20, 0);	/* 1K file system */
dev_t	pipedev = makedev(0x20, 0);
dev_t	swapdev = makedev(0, 1);
daddr_t	swplo = 0;
int	nswap = 0;

int sxt_cnt = 32;


short vc_cnt = NVCHRD;
struct tty vc_tty[NVCHRD];
struct vcdev vc_dev[NVCHRD];
TTY_STAT vc_stat[NVCHRD][3];


short vb_cnt = NVBLKD;
short vb_rcnt = NVBLKD*NREQPD;
short vb_qdepth = NREQPD;
struct vbdev vb_dev[NVBLKD];
struct  request requests[NVBLKD*NREQPD];


short vt_cnt = NVTAPE;
struct vtdev vt_dev[NVTAPE];

short ft_cnt = NVFILE;
struct ftdev ft_dev[NVFILE];
short ft_pnamlen = MAXFTPNAME;
char ft_pname[MAXFTPNAME+1];

/*
short hm_cnt = NVHMEM;
struct hmdev hm_dev[NVHMEM];
short hm_vnamlen = MAXHMNAME;
char hm_vname[MAXHMNAME+1];
*/

