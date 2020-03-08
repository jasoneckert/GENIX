
# ifndef NOSCCS
static char *sccsid = "@(#)dcusize.c	1.2	9/16/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

#include <sys/disk.h>

/* Parameters of the known disk types */

/* BASF 20 MB */
#define	S_T_BASF 24		/* sectors per track */
#define	T_C_BASF 3		/* tracks per cylinder */
#define	C_U_BASF 614		/* cylinders per unit */
#define	S_C_BASF (24*3)		/* sectors per cylinder */
#define	S_U_BASF (24*3*614)	/* sectors per unit */
#define	PZ_BASF 11		/* formatting data for sector zero */
#define	PNZ_BASF 17		/* formatting data for nonzero sectors */

/* 3M 20 MB */
#define	S_T_3M20 30		/* sectors per track */
#define	T_C_3M20 4		/* tracks per cylinder */
#define	C_U_3M20 280		/* cylinders per unit */
#define	S_C_3M20 (30*4)		/* sectors per cylinder */
#define	S_U_3M20 (30*4*280)	/* sectors per unit */
#define	PZ_3M20 29		/* formatting data for sector zero */
#define	PNZ_3M20 57		/* formatting data for nonzero sectors */

/* 3M 60 MB */
#define	S_T_3M60 32		/* sectors per track */
#define	T_C_3M60 4		/* tracks per cylinder */
#define	C_U_3M60 838		/* cylinders per unit */
#define	S_C_3M60 (32*4)		/* sectors per cylinder */
#define	S_U_3M60 (32*4*838)	/* sectors per unit */
#define	PZ_3M60	9		/* formatting data for sector zero */
#define	PNZ_3M60 21		/* formatting data for nonzero sectors */

/* IMI 20 MB */
#define	S_T_IMI20 18		/* sectors per track */
#define	T_C_IMI20 5		/* tracks per cylinder */
#define	C_U_IMI20 388		/* cylinders per unit */
#define	S_C_IMI20 (18*5)	/* sectors per cylinder */
#define	S_U_IMI20 (18*5*388)	/* sectors per unit */
#define	PZ_IMI20 29		/* formatting data for sector zero */
#define	PNZ_IMI20 61		/* formatting data for nonzero sectors */

/* IMI 60 MB */
#define	S_T_IMI60 18		/* sectors per track */
#define	T_C_IMI60 7		/* tracks per cylinder */
#define	C_U_IMI60 776		/* cylinders per unit */
#define	S_C_IMI60 (18*7)	/* sectors per cylinder */
#define	S_U_IMI60 (18*7*776)	/* sectors per unit */
#define	PZ_IMI60 29		/* formatting data for sector zero */
#define	PNZ_IMI60 61		/* formatting data for nonzero sectors */

/* PRIAM 70 MB */
#define	S_T_PRI70 22		/* sectors per track */
#define	T_C_PRI70 5		/* tracks per cylinder */
#define	C_U_PRI70 1049		/* cylinders per unit */
#define	S_C_PRI70 (22*5)	/* sectors per cylinder */
#define	S_U_PRI70 (22*5*1049)	/* sectors per unit */
#define	PZ_PRI70 23		/* formatting data for sector zero */
#define	PNZ_PRI70 73		/* formatting data for nonzero sectors */

struct partition pt_basf[8] = {	/* partitions for BASF drives */
	544*S_C_BASF,	0,	/* A = cylinders 0 through 543 */
	70*S_C_BASF,	544,	/* B = cylinders 544 through 613 */
	S_U_BASF,	0,	/* C = cylinders 0 through 613 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	partition pt_3m20[8] = { /* partitions for 3M20 drives */
	238*S_C_3M20,	0,	/* A = cylinders 0 through 237 */
	42*S_C_3M20,	238,	/* B = cylinders 238 through 279 */
	S_U_3M20,	0,	/* C = cylinders 0 through 279 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	partition pt_3m60[8] = {/* partitions for 3M60 drives */
	796*S_C_3M60,	0,	/* A = cylinders 0 through 795 */
	42*S_C_3M60,	796,	/* B = cylinders 796 through 837 */
	S_U_3M60,	0,	/* C = cylinders 0 through 837 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	partition pt_imi20[8] = { /* partitions for IMI20 drives */
	332*S_C_IMI20,	0,	/* A = cylinders 0 through 331 */
	56*S_C_IMI20,	332,	/* B = cylinders 332 through 387 */
	S_U_IMI20,	0,	/* C = cylinders 0 through 387 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	partition pt_imi60[8] = { /* partitions for IMI 60Mb drives */
	720*S_C_IMI60,	0,	/* A = cylinders 0 through 719 */
	56*S_C_IMI60,	720,	/* B = cylinders 720 through 775 */
	S_U_IMI60,	0,	/* C = cylinders 0 through 775 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	partition pt_pri70[8] = { /* partitions for PRIAM 70Mb drives */
	1000*S_C_PRI70,	0,	/* A = cylinders 0 through 999 */
	49*S_C_PRI70,	1000,	/* B = cylinders 1000 through 1048 */
	S_U_PRI70,	0,	/* C = cylinders 0 through 1048 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

/* The table of all known disk types */

#define	DD(name, s_t, t_c, c_u, ploz, plonz, pt) \
	 ploz, plonz, s_t, t_c, c_u, (s_t)*(t_c), (s_t)*(t_c)*(c_u), pt, name

struct	unitdata unitsizetable[] = {
    DD("3M60", S_T_3M60, T_C_3M60, C_U_3M60, PZ_3M60, PNZ_3M60,pt_3m60),
    DD("IMI20", S_T_IMI20, T_C_IMI20, C_U_IMI20, PZ_IMI20, PNZ_IMI20,pt_imi20),
    DD("3M20", S_T_3M20, T_C_3M20, C_U_3M20, PZ_3M20, PNZ_3M20, pt_3m20),
    DD("PRI70",S_T_PRI70,T_C_PRI70,C_U_PRI70,PZ_PRI70,PNZ_PRI70, pt_pri70),
    DD("IMI60",S_T_IMI60,T_C_IMI60,C_U_IMI60,PZ_IMI60,PNZ_IMI60, pt_imi60),
    DD("BASF", S_T_BASF, T_C_BASF, C_U_BASF, PZ_BASF, PNZ_BASF, pt_basf),
    0	/* end of table */
};
