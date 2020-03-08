/*
 * @(#)fsr.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/* Definitions for the floating point status register */
struct	fsr	{
unsigned int	fsr_trap:3,		/* trap which occurred (see below) */
		fsr_ufenb:1,		/* underflow trap enabled */
		fsr_underflow:1,	/* underflow occurred */
		fsr_ienb:1,		/* inexact result trap enabled */
		fsr_inexact:1,		/* inexact result occurred */
		fsr_round:2,		/* rounding mode (see below) */
		fsr_software:7,		/* reserved for software */
		fsr_reserved:16;	/* reserved for hardware */
};

/* Floating point trap types */
#define	FTT_NONE	0		/* no trap occurred */
#define	FTT_UNDERFLOW	1		/* underflow */
#define	FTT_OVERFLOW	2		/* overflow */
#define	FTT_DIVZERO	3		/* divide by zero */
#define	FTT_ILLINSTR	4		/* illegal instruction */
#define	FTT_ILLOPER	5		/* illegal operands */
#define	FTT_INEXACT	6		/* inexact result */
#define	FTT_UNUSED	7		/* unused */

/* Floating point rounding modes */
#define	FRM_NEAREST	0		/* round to nearest value */
#define	FRM_ZERO	1		/* round towards zero */
#define	FRM_POSITIVE	2		/* round towards positive infinity */
#define	FRM_NEGATIVE	3		/* round towards negative infinity */
#define	FRM_DEFAULT	FRM_NEAREST	/* default type of rounding */
