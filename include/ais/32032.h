/* 32032.h -- Hardware Definitions for NS 32032
 * copyright (c) American Information Systems Corporation
 *	February, 1985
 */

#ifndef PROC32032
#define PROC32032

/* bit definitions for psr */
#define	PSR_C	0x0001	/* carry */
#define PSR_T	0x0002	/* trace */
#define	PSR_L	0x0004	/* low */
#define	PSR_F	0x0020	/* flag */
#define	PSR_Z	0x0040	/* zero */
#define	PSR_N	0x0080	/* negative */
#define	PSR_U	0x0100	/* user mode */
#define	PSR_S	0x0200	/* stack */
#define	PSR_P	0x0400	/* pending trace trap */
#define	PSR_I	0x0800	/* interrupt enable */

#define	PSR_B_C		0	/* carry */
#define PSR_B_T		1	/* trace */
#define	PSR_B_L		2	/* low */
#define	PSR_B_F		5	/* flag */
#define	PSR_B_Z		6	/* zero */
#define	PSR_B_N		7	/* negative */
#define	PSR_B_U		8	/* user mode */
#define	PSR_B_S		9	/* stack */
#define	PSR_B_P		10	/* pending trace trap */
#define	PSR_B_I		11	/* interrupt enable */

/* bit definitions for msr */
#define MSR_NT	0x02000000	/* nonsequential trace */
#define MSR_UT	0x01000000	/* user trace */
#define MSR_FT	0x00800000	/* flow trace */
#define MSR_AI	0x00400000	/* abort / interrupt */
#define MSR_UB	0x00200000	/* user break */
#define MSR_BEN	0x00100000	/* breakpoint enable */
#define MSR_AO	0x00080000	/* access override */
#define MSR_DS	0x00040000	/* dual space */
#define MSR_TS	0x00020000	/* translate supervisor */
#define MSR_TU	0x00010000	/* translate user */
#define MSR_BD	0x00000200	/* breakpoint direction */
#define MSR_ED	0x00000100	/* error data direction */
#define	MSR_BN	0x00000040	/* breakpoint number */
#define	MSR_TET	0x00000038	/* error type */
#define	MSR_ERC	0x00000007	/* error class */

#define	MSR_TET_PROT	0x00000008	/* Protection error */
#define	MSR_TET_SEG	0x00000010	/* Segment Fault */
#define	MSR_TET_PTE	0x00000020	/* Page Fault */

#define	MSR_ERC0	0x00000001	/* error class bit 0 */
#define	MSR_ERC1	0x00000002	/* error class bit 1 */
#define	MSR_ERC2	0x00000004	/* error class bit 2 */

#define MSR_B_NT	25	/* nonsequential trace */
#define MSR_B_UT	24	/* user trace */
#define MSR_B_FT	23	/* flow trace */
#define MSR_B_AI	22	/* abort / interrupt */
#define MSR_B_UB	21	/* user break */
#define MSR_B_BEN	20	/* breakpoint enable */
#define MSR_B_AO	19	/* access override */
#define MSR_B_DS	18	/* dual space */
#define MSR_B_TS	17	/* translate supervisor */
#define MSR_B_TU	16	/* translate user */
#define MSR_B_BD	9	/* breakpoint direction */
#define MSR_B_ED	8	/* error data direction */
#define	MSR_B_BN	6	/* breakpoint number */

#define MSR_B_TET_PROT	3	/* Protection error */
#define MSR_B_TET_SEG	4	/* Segment Fault */
#define MSR_B_TET_PTE	5	/* Page Fault */

#define EIA_ADDR	0xffffff	/* Mask for address in EIA register */


/* bit definitions for breakpoint registers bpr0, bpr1 */
#define BRK_C	0x04000000	/* enable breakpoint counter (bpr0 only) */
#define BRK_W	0x08000000	/* break on write */
#define BRK_R	0x10000000	/* break on read */
#define BRK_X	0x20000000	/* break on execute */
#define BRK_P	0x40000000	/* match with physical address */
#define BRK_A	0x80000000	/* address space bit */

/* module table offsets */
#define MOD_SB	0x0		/* Static Base */
#define MOD_LB	0x4		/* Link Base */
#define MOD_PB	0x8		/* Program Base */

#define MOD_SIZE	16		/* module table entry size */

/* define interrupt vectors */
#define T_NVI	00		/* non-vectored interrupt */
#define T_NMI	01		/* non-maskable interrupt */
#define T_ABT	02		/* abort */
#define T_FPU	03		/* floating point exception */
#define T_ILL	04		/* illegal operation */
#define T_SVC	05		/* supervisor call */
#define T_DVZ	06		/* divide by zero */
#define T_FLG	07		/* flag bit trap */
#define T_BPT	08		/* breakpoint trap */
#define T_TRC	09		/* trace trap */
#define T_UND	10		/* undefined instruction */
#define T_RSV11	11		/* reserved trap */
#define T_RSV12	12		/* reserved trap */
#define T_RSV13	13		/* reserved trap */
#define T_RSV14	14		/* reserved trap */
#define T_RSV15	15		/* reserved trap */
#define T_VEC	16		/* start of vectored interrupts */

/* Define offsets in Interrupt Vector Table */
#define XPD_SIZE	4		/* external procedure entry size */

#define I_NVI	(T_NVI * XPD_SIZE)		/* non-vectored interrupt */
#define I_NMI	(T_NMI * XPD_SIZE)		/* non-maskable interrupt */
#define I_ABT	(T_ABT * XPD_SIZE)		/* abort */
#define I_FPU	(T_FPU * XPD_SIZE)		/* floating point exception */
#define I_ILL	(T_ILL * XPD_SIZE)		/* illegal operation */
#define I_SVC	(T_SVC * XPD_SIZE)		/* supervisor call */
#define I_DVZ	(T_DVZ * XPD_SIZE)		/* divide by zero */
#define I_FLG	(T_FLG * XPD_SIZE)		/* flag bit trap */
#define I_BPT	(T_BPT * XPD_SIZE)		/* breakpoint trap */
#define I_TRC	(T_TRC * XPD_SIZE)		/* trace trap */
#define I_UND	(T_UND * XPD_SIZE)		/* undefined instruction */
#define I_RSV11	(T_RSV11 * XPD_SIZE)		/* reserved trap */
#define I_RSV12	(T_RSV12 * XPD_SIZE)		/* reserved trap */
#define I_RSV13	(T_RSV13 * XPD_SIZE)		/* reserved trap */
#define I_RSV14	(T_RSV14 * XPD_SIZE)		/* reserved trap */
#define I_RSV15	(T_RSV15 * XPD_SIZE)		/* reserved trap */
#define I_VEC	(T_VEC * XPD_SIZE)		/* vectored interrupts */

#define REALTRAPS	11	/* number of real CPU traps */
#define TRAPS	16		/* number of CPU traps before vectored ints */

#define TRAP_SIZE	XPDSIZ*TRAPS	/* non-vectored interrupt table size */

#define SVC_LENGTH	1	/* number of bytes in an SVC instruction */

#endif
