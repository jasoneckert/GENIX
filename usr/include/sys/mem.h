/*
 * @(#)mem.h	3.3	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Memory controller registers
 */
struct	mcr {
	short	m16_mode0;	/* (0x000) Set memory to mode 0 */
	short	m16_mode1;	/* (0x002) Set memory to mode 1 */
	short	m16_mode2;	/* (0x004) Set memory to mode 2 */
	short	m16_foo1;
	short	m16_mode3;	/* (0x008) Set memory to mode 3 */
	char	m16_foo2[0x36];
	short	m16_esrlo;	/* (0x040) low order half, error status reg. */
	char	m16_foo3[0x3e];
	short	m16_esrhi;	/* (0x080) hi order half,  error status reg. */
	char	m16_foo4[0x7e];
	short	m16_mcr;	/* (0x100) mode control register */
};

/*
 * Compute maximum possible number of memory controllers,
 * for sizing of the mcraddr array.
 */
#define	MAXNMCR		3

/* Offsets into memory controller register page */
#define	M16_MODE0	0x000	/* Set memory to mode 0 */
#define	M16_MODE1	0x002	/* Set memory to mode 1 */
#define	M16_MODE2	0x004	/* Set memory to mode 2 */
#define	M16_MODE3	0x008	/* Set memory to mode 3 */
#define	M16_ESRLO	0x040	/* Low order half of Error Status Register */
#define	M16_ESRHI	0x080	/* High order half of Error Status Register */
#define	M16_MCR		0x100	/* Mode Control Register */

/* Contents of Error Status Register */
#define	ECCADDR		0x00ffffff	/* Address of ECC error */
#define	ECCSYN		0x3f000000	/* Syndrome bits */
#define	ECCERR		0x40000000	/* Error flag */
#define	ECCOVFL		0x80000000	/* Overflow flag */

/* Macros to return parts of the Error Status Register */
#define	M16_ADDR(esr)	((esr) & ECCADDR)
#define M16_SYN(esr)	(((esr) & ECCSYN) >> 24)
#define	M16_ERR(esr)	((esr) & ECCERR)
#define M16_OVFL(esr)	((esr) & ECCOVFL)

/* Macros to set memory modes */
#define M16_SETM0(mcr)	((mcr)->m16_mode0 = 0)
#define M16_SETM1(mcr)	((mcr)->m16_mode1 = 0)
#define M16_SETM2(mcr)	((mcr)->m16_mode2 = 0)
#define M16_SETM3(mcr)	((mcr)->m16_mode3 = 0)

/* Macro to read memory modes */
#define M16_READM(mcr) ((((mcr)->m16_mcr) >> 1) & 0x3)

#ifdef	KERNEL
extern	int	nmcr;
extern	struct	mcr *mcraddr[MAXNMCR];
#endif
