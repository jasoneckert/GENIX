/*
 * @(#)panic.h	1.6	7/31/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/* Structure to hold interesting data collected during a panic.
 * Those fields flagged by an "#" are initialized at system startup time,
 * and are thus readable even before a crash.
 */

struct panic {
	long	ps_gen[8];	/* general registers r0 - r7 */
	long	ps_pc;		/* pc at time of panic */
	long	ps_psrmod;	/* psr and mod at time of panic */
	long	ps_sp;		/* stack pointer */
	long	ps_fp;		/* frame pointer */
	long	ps_intbase;	/* interrupt base */
	long	ps_ptb0;	/* mmu ptb0 register */
	long	ps_ptb1;	/* mmu ptb1 register */
	long	ps_msr;		/* mmu status register */
	long	ps_eia;		/* mmu eia register */
	long	ps_bpr0;	/* first mmu breakpoint register */
	long	ps_bpr1;	/* second mmu breakpoint register */
	long	ps_nest;	/* counter for nested panics */
	char	*ps_string;	/* address of panic string */
	long	*ps_args;	/* address of arguments to panic string */
	char	*ps_version;	/* # address of system version string */
	struct	proc *ps_proc;	/* # address of proc table */
	struct	spt *ps_spt;	/* # address of spt table */
};


/* The following definition is never changed, and points to the panic block.
 * This particular address is always available because it is one of the
 * reserved words in the kernel's mod table.
 */

#define	PANICADDR	12	/* address of pointer to panic block */
