/*
 * @(#)vmmeter.h	3.10	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/* Note: when adding new fields, be sure to update v_first or v_last
 * if necessary
 */
struct vmmeter
{
#define	v_first	v_swtch
/* from here to v_last, rate is calculated once a second */
	unsigned v_swtch;	/* context switches */
	unsigned v_trap;	/* traps */
	unsigned v_syscall;	/* syscalls */
	unsigned v_intr;	/* device interrupts */
	unsigned v_pgin;	/* pageins */
	unsigned v_pgout;	/* pageouts */
	unsigned v_fpgout;	/* free pageouts */
	unsigned v_faults;	/* total page faults taken */
	unsigned v_pgszrod;	/* pages zeroed in idle loop */
#define v_last v_pgszrod
#define v_rfirst v_swpin
/* from here to v_rlast, rate is calculated once every five seconds */
	unsigned v_swpin;	/* swapins */
	unsigned v_swpout;	/* process swapouts */
	unsigned v_tswpout;	/* text swapouts */
	unsigned v_pswpout;	/* partial swapouts */
#define v_rlast v_pswpout
};
#ifdef KERNEL
extern	struct	vmmeter cnt, rate, sum;
#endif

/* systemwide totals computed every five seconds */
struct vmtotal
{
	short	t_rq;		/* length of the run queue */
	short	t_dw;		/* jobs in ``disk wait'' (neg priority) */
	short	t_pw;		/* jobs in page wait */
	short	t_sl;		/* jobs sleeping in core */
	short	t_sw;		/* swapped out runnable/short block jobs */
	int	t_vm;		/* total virtual memory */
	int	t_avm;		/* active virtual memory */
	short	t_rm;		/* total real memory in use */
	short	t_arm;		/* active real memory */
	int	t_vmtxt;	/* virtual memory used by text */
	int	t_avmtxt;	/* active virtual memory used by text */
	short	t_rmtxt;	/* real memory used by text */
	short	t_armtxt;	/* active real memory used by text */
	short	t_free;		/* free memory pages */
};
#ifdef KERNEL
extern	struct	vmtotal total;
#endif
