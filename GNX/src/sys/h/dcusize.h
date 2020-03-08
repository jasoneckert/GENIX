/*
 * @(#)dcusize.h	3.10	9/6/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/* Data structures used for referencing disk parameters */

struct	unitsizes {		/* structure for unit size data */
	struct unitsubsizes *subsizes;	/* pointer to subunit size table */
	short	secpertrk;	/* number of sectors per track */
	short	trkpercyl;	/* number of tracks per cylinder */
	short	secpercyl;	/* number of sectors per cylinder */
	short	cylperunit;	/* number of cylinders per unit */
	long	secperunit;	/* sectors per unit */
	short	ploz;		/* formatting data for sector zero */
	short	plonz;		/* formatting data for non-zero sectors */
	char	*name;		/* name of disk type */
};

struct unitsubsizes {		/* structure for subunit size data */
	daddr_t	nblocks;	/* number of sectors in subunit */
	short	cyloff;		/* starting cylinder offset for subunit */
};


#ifdef	DUMPUNITSIZES		/* define the remainder only on demand */

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
#define	S_T_3M 30		/* sectors per track */
#define	T_C_3M 4		/* tracks per cylinder */
#define	C_U_3M 280		/* cylinders per unit */
#define	S_C_3M (30*4)		/* sectors per cylinder */
#define	S_U_3M (30*4*280)	/* sectors per unit */
#define	PZ_3M 29		/* formatting data for sector zero */
#define	PNZ_3M 57		/* formatting data for nonzero sectors */

/* 3M 60 MB */
#define	S_T_3M60 32		/* sectors per track */
#define	T_C_3M60 4		/* tracks per cylinder */
#define	C_U_3M60 838		/* cylinders per unit */
#define	S_C_3M60 (32*4)		/* sectors per cylinder */
#define	S_U_3M60 (32*4*838)	/* sectors per unit */
#define	PZ_3M60	9		/* formatting data for sector zero */
#define	PNZ_3M60 21		/* formatting data for nonzero sectors */

/* IMI 20 MB */
#define	S_T_IMI 18		/* sectors per track */
#define	T_C_IMI 5		/* tracks per cylinder */
#define	C_U_IMI 388		/* cylinders per unit */
#define	S_C_IMI (18*5)		/* sectors per cylinder */
#define	S_U_IMI (18*5*388)	/* sectors per unit */
#define	PZ_IMI 29		/* formatting data for sector zero */
#define	PNZ_IMI 61		/* formatting data for nonzero sectors */

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

struct unitsubsizes ss_basf[8] = {	/* subunits for BASF drives */
	543*S_C_BASF,	0,	/* A = cylinders 0 through 543 */
	70*S_C_BASF,	544,	/* B = cylinders 544 through 613 */
	S_U_BASF,	0,	/* C = cylinders 0 through 613 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	unitsubsizes ss_3m[8] = {	/* subunits for 3M drives */
	237*S_C_3M,	0,	/* A = cylinders 0 through 237 */
	42*S_C_3M,	238,	/* B = cylinders 238 through 279 */
	S_U_3M,		0,	/* C = cylinders 0 through 279 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	unitsubsizes ss_3m60[8] = {	/* subunits for 3M60 drives */
	795*S_C_3M60,	0,	/* A = cylinders 0 through 795 */
	42*S_C_3M60,	796,	/* B = cylinders 796 through 837 */
	S_U_3M60,	0,	/* C = cylinders 0 through 837 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	unitsubsizes ss_imi[8] = {	/* subunits for IMI drives */
	331*S_C_IMI,	0,	/* A = cylinders 0 through 331 */
	56*S_C_IMI,	332,	/* B = cylinders 332 through 387 */
	S_U_IMI,	0,	/* C = cylinders 0 through 387 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	unitsubsizes ss_imi60[8] = {	 /* subunits for IMI 60Mb drives */
	719*S_C_IMI60,	0,	/* A = cylinders 0 through 719 */
	56*S_C_IMI60,	720,	/* B = cylinders 720 through 775 */
	S_U_IMI60,	0,	/* C = cylinders 0 through 775 */
	0,		0,
	0,		0,
	0,		0,
	0,		0,
	0,		0
};

struct	unitsubsizes ss_pri70[8] = {	 /* subunits for PRIAM 70Mb drives */
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

#define	DD(sub, s_t, t_c, c_u, ploz, plonz, name) \
	sub, s_t, t_c, (s_t)*(t_c), c_u, (s_t)*(t_c)*(c_u), ploz, plonz, name

struct	unitsizes unitsizetable[] = {
	DD(ss_basf, S_T_BASF, T_C_BASF, C_U_BASF, PZ_BASF, PNZ_BASF, "BASF"),
	DD(ss_3m, S_T_3M, T_C_3M, C_U_3M, PZ_3M, PNZ_3M, "3M"),
	DD(ss_3m60, S_T_3M60, T_C_3M60, C_U_3M60, PZ_3M60, PNZ_3M60, "3M60"),
	DD(ss_imi, S_T_IMI, T_C_IMI, C_U_IMI, PZ_IMI, PNZ_IMI, "IMI"),
	DD(ss_imi60,S_T_IMI60,T_C_IMI60,C_U_IMI60,PZ_IMI60,PNZ_IMI60,"IMI60"),
	DD(ss_pri70,S_T_PRI70,T_C_PRI70,C_U_PRI70,PZ_PRI70,PNZ_PRI70,"PRI70"),
	0	/* end of table */
};

#endif	DUMPUNITSIZES	/* end of data defined only on demand */
