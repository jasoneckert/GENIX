/*
 * @(#)proc.h	3.23	8/30/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * One structure allocated per active
 * process. It contains all data needed
 * about the process while the
 * process may be swapped out.
 * Other per process data (user.h)
 * is swapped with the process.
 */
struct	proc {
	struct	proc *p_link;	/* linked list of running processes */
	struct	proc *p_rlink;
	short	p_flag;
	char	p_usrpri;	/* user-priority based on p_cpu and p_nice */
	char	p_pri;		/* priority, negative is high */
	char	p_cpu;		/* cpu usage for scheduling */
	char	p_stat;
	char	p_time;		/* resident time for scheduling */
	char	p_nice;		/* nice for cpu usage */
	char	p_slptime;	/* time since last block */
	long	p_sig;		/* signals pending to this process */
	long	p_siga0;	/* low bit of 2 bit signal action */
	long	p_siga1;	/* high bit of 2 bit signal action */
#define	p_ignsig p_siga0	/* ignored signal mask */
	char	p_cursig;
	short	p_uid;		/* user id, used to direct tty signals */
	short	p_pgrp;		/* name of process group leader */
	short	p_pid;		/* unique process id */
	short	p_ppid;		/* process id of parent */
	struct	proc *p_pptr;	/* pointer to process structure of parent */
	short	p_cpticks;	/* ticks of cpu time */
#ifdef	float
	float	p_pctcpu;	/* %cpu for this process during p_time */
#else
	long	p_pctcpu;	/* %cpu for this process during p_time */
#endif
	long	p_infopc;	/* user mode pc for typeout */
	long	p_infotime;	/* runtime for typeout */
	long	p_infoage;	/* counter for determining recent running */
	char	p_infoname[15];	/* program name for typeout */
	union {
		struct {
			short	P_idhash;	/* hashed based on p_pid
						 *  for kill+exit+... */
			caddr_t P_wchan;	/* event process is awaiting */
			int	P_clktim;	/* time to alarm clock signal */
			long	P_ssize;	/* size of stack for swapping */
			long	P_tdsize;	/* size of text and data
						 *  for swapping */
			long	P_maxrss;	/* copy of u.u_limit[LIM_MAXRSS]
						 *  with unit pages */
			long	P_spti;		/* spt index of this process */
		} p_p;
#define p_idhash	p_up.p_p.P_idhash
#define	p_wchan		p_up.p_p.P_wchan
#define p_clktim	p_up.p_p.P_clktim
#define p_ssize		p_up.p_p.P_ssize
#define p_tdsize	p_up.p_p.P_tdsize
#define p_maxrss	p_up.p_p.P_maxrss
#define p_spti		p_up.p_p.P_spti
/*
 * replace part with times to be passed to parent process
 * in ZOMBIE state
 */
		struct {
			short	XP_xstat;	/* Exit status for wait */
			struct	vtimes	XP_vm;	/* stats */
		} p_x;
#define xp_xstat	p_up.p_x.XP_xstat
#define xp_vm		p_up.p_x.XP_vm
	} p_up;
};

#define	PIDHSZ		23			/* hash size */
#define	PIDHASH(pid)	((pid) % PIDHSZ)	/* function to compute size */

#ifdef KERNEL
struct	proc *pfind();
short	pidhash[PIDHSZ];	/* pid hash table */
extern struct proc *proc;	/* the proc table itself */
extern struct proc *procNPROC;	/* pointer to end of table */
extern int nproc;
#ifdef FASTVAX
#define	NQS	32		/* 32 run queues */
extern struct	prochd {
	struct	proc *ph_link;	/* linked list of running processes */
	struct	proc *ph_rlink;
} qs[NQS];
extern int	whichqs;	/* bit mask summarizing non-empty qs's */
#else
extern struct	proc *runq;
#endif
#endif

/* stat codes */
#define	SSLEEP	1		/* awaiting an event */
#define	SWAIT	2		/* (abandoned state) */
#define	SRUN	3		/* running */
#define	SIDL	4		/* intermediate state in process creation */
#define	SZOMB	5		/* intermediate state in process termination */
#define	SSTOP	6		/* process being traced */

/* flag codes */
#define	SLOAD	01		/* in core */
#define	SSYS	02		/* scheduling process */
#define	SLOCK	04		/* process cannot be swapped */
#define	SSWAP	010		/* process is being swapped out */
#define	STRC	020		/* process is being traced */
#define	SWTED	040		/* another tracing flag */
#define	SVFORK	0100		/* fork created by vfork */
#define	SNUSIG	0200		/* using new signal mechanism */
#define	SDETACH	0400		/* detached inherited by init */
#define	SWEXIT	01000		/* working on exiting */
#define	SPAGE	02000		/* process in page wait state */
#define SGRCGVN 04000		/* CPU grace time has been given once */
#define	SFPU	010000		/* floating point instructions enabled */
#define	SMMU	020000		/* mmu debug block enabled */
