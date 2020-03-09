/*
 * @(#)mmu.h	3.3	8/29/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/* Memory management register definitions */

/* The EIA register - returns address of memory fault.  Also used to invalidate
 * and address in the translation buffer.
 */
#define	EIA_ADDR	0xefffffff	/* address causing memory fault */
#define	EIA_PTB1	0x80000000	/* translated by PTB1 instead of PTB0 */


/* The MSR register - The MMU status register.  Gives reasons for an abort,
 * and is used to control the MMU.
 */
#define	MSR_TRANSERR	0x00000001	/* error due to address translation */
#define	MSR_BRKERR	0x00000004	/* error due to breakpoint */
#define	MSR_PROTERR	0x00000008	/* protection level error */
#define	MSR_PTE1ERR	0x00000010	/* invalid level-1 page table entry */
#define	MSR_PTE2ERR	0x00000020	/* invalid level-2 page table entry */
#define	MSR_BPT1	0x00000040	/* breakpoint was BPR register 1 */
#define	MSR_READERR	0x00000100	/* 1 = read error, 0 = write failure */
#define	MSR_READBPT	0x00000200	/* 1 = read breakpoint, 0 = write */
#define	MSR_TRANSSTAT	0x00003400	/* bus cycle at translation error */
#define	MSR_BPTSTAT	0x0000e000	/* bus cycle at breakpoint */
#define	MSR_TRANSUSER	0x00010000	/* translate user addresses */
#define	MSR_TRANSSUP	0x00020000	/* translate supervisor addresses */
#define	MSR_USEPTB1	0x00040000	/* use PTB1 for user translations */
#define	MSR_OVERRIDE	0x00080000	/* use supervisor protection for user */
#define	MSR_BRKENABLE	0x00100000	/* breakpoints enabled */
#define	MSR_BRKUSER	0x00200000	/* breakpoint only in user mode */
#define	MSR_AI		0x00400000	/* abort or NMI trap for debug blk */
#define	MSR_FLOWENABLE	0x00800000	/* flow tracing is enabled */
#define	MSR_FLOWUSER	0x01000000	/* flow trace only in user mode */
#define	MSR_SEQENABLE	0x02000000	/* nonsequential flow trap enabled */


/* the mmu registers */

enum mmreg {eia, msr, ptb0, ptb1, bpr0, bpr1, bcnt, pf0, pf1, sc};

/*
   the entire state of the mmu. a pointer to this structure may
   be passed to the function "readmmu" to get the mmu status.
*/

struct mmu {
	int	eia;
	int	msr;
	int	ptb0;
	int	ptb1;
	int	bpr0;
	int	bpr1;
	int	bcnt;
	short	sc0;
	short	sc1;
	int	pf0;
	int	pf1;
};

/*
 * ptrace arg value for the users'
 * mmu msr and eia registers.
 */

#define UMSR	-200
#define UEIA	-201

