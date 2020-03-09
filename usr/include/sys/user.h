/*
 * @(#)user.h	3.11	9/9/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

#include "pte.h"
#include "vtimes.h"

/*
 * The user structure.
 * One allocated per process.
 * Contains all per process data
 * that doesn't need to be referenced
 * while the process is swapped.
 * The user block is USIZE*64 bytes
 * long; resides at virtual kernel
 * loc 140000; contains the system
 * stack per user; is cross referenced
 * with the proc structure for the
 * same process.
 */

#define	SHSIZE	32

struct	user
{
	long	u_msrreg;		/* mmu msr register at last fault */
	long	u_eiareg;		/* mmu eia register at last fault */
	long	u_bpr0;			/* mmu bpr0 register */
	long	u_bpr1;			/* mmu bpr1 register */
	long	u_bcnt;			/* mmu bcnt register */
	long	u_pf0;			/* mmu pf0 register */
	long	u_pf1;			/* mmu pf1 register */
	long	u_sc;			/* mmu sc register */
	char	u_segflg;		/* 0:user D; 1:system; 2:user I */
	char	u_error;		/* return error code */
	short	u_uid;			/* effective user id */
	short	u_gid;			/* effective group id */
	short	u_ruid;			/* real user id */
	short	u_rgid;			/* real group id */
	struct proc *u_procp;		/* pointer to proc structure */
	unsigned long 	u_spti;		/* spt index */
	long	u_xspti;		/* spt index of shared text segment */
	int	*u_ap;			/* pointer to arglist */
	union {				/* syscall return values */
		struct	{
			int	R_val1;
			int	R_val2;
		} u_rv;
#define	r_val1	u_rv.R_val1
#define	r_val2	u_rv.R_val2
		off_t	r_off;
		time_t	r_time;
	} u_r;
	caddr_t	u_base;			/* base address for IO */
	unsigned int u_count;		/* bytes remaining for IO */
	off_t	u_offset;		/* offset in file for IO */
	struct inode *u_cdir;		/* pointer to inode of current directory */
	struct inode *u_rdir;		/* root directory of current process */
	char	u_dbuf[DIRSIZ];		/* current pathname component */
	caddr_t	u_dirp;			/* pathname pointer */
	struct direct u_dent;		/* current directory entry */
	struct inode *u_pdir;		/* inode of parent directory of dirp */
	struct file *u_ofile[NOFILE];	/* pointers to file structures of open files */
	char	u_pofile[NOFILE];	/* per-process flags of open files */
	int	u_arg[5];		/* arguments to current system call */
	unsigned u_tsize;		/* text size (clicks) */
	unsigned u_dsize;		/* data size (clicks) */
	unsigned u_ssize;		/* stack size (clicks) */
	label_t	u_qsav;			/* label variable for quits and interrupts */
#define	EXCLOSE 01		/* auto-close on exec */
	label_t u_ssav;			/* label variable for swapping */
	int	(*u_signal[NSIG])();	/* disposition of signals */
	int	(*u_sigcatch)();	/* the REAL signal catcher routine */
	int	u_code;			/* ``code'' to trap */
	int	*u_ar0;			/* address of users saved R0 */
	int	*u_abtar0;		/* saved r0 on last page fault */
	long	u_fsr;			/* floating point status register */
	long	u_fregs[8];		/* floating point general registers */
	char	u_eosys;		/* special action on end of syscall */
	char	u_intflg;		/* catch intr from sys */
	struct tty *u_ttyp;		/* controlling tty pointer */
	dev_t	u_ttyd;			/* controlling tty dev */
	union {
	   struct {			/* header of executable file */
		long	Ux_magic;		/* magic number */
		unsigned long	Ux_text;	/* size of text segment */
		unsigned long	Ux_data;	/* size of initialized data */
		unsigned long	Ux_bss;		/* size of uninitialized data */
		unsigned long	Ux_syms;	/* size of symbol table */
		unsigned long	Ux_entry;	/* entry point address */
		unsigned long	Ux_entry_mod;	/* entry point mod number */
						/* further fields unneeded */
	   } Ux_A;
	   char ux_shell[SHSIZE];	/* #! and name of interpreter */
	} u_exdata;
#define	ux_magic	Ux_A.Ux_magic
#define	ux_text		Ux_A.Ux_text
#define	ux_data		Ux_A.Ux_data
#define	ux_bss		Ux_A.Ux_bss
#define	ux_syms		Ux_A.Ux_syms
#define	ux_entry	Ux_A.Ux_entry
#define	ux_entry_mod	Ux_A.Ux_entry_mod
#define	OMAGIC	0407			/* old impure format */
#define	NMAGIC	0410			/* read-only text format */
#define	ZMAGIC	0413			/* demand load format */
#define	XMAGIC	0414			/* demand load (0-1023 unmapped) */

	char	u_comm[DIRSIZ];		/* command name */
	time_t	u_start;
	char	u_acflag;
	short	u_cmask;		/* mask for file creation */
	struct	vtimes	u_vm;		/* stats for this proc */
	struct	vtimes	u_cvm;		/* sum of stats for reaped children */
	int	u_limit[8];		/* see <sys/limit.h> */
	int	u_stack[1];
					/* kernel stack per user
					 * extends from u + USIZE*64
					 * backward not to reach here
					 */
};

extern struct user u;

/* u_eosys values */
#define	JUSTRETURN	0
#define	RESTARTSYS	1
#define	SIMULATERTI	2

/* u_error codes */
#define	EPERM	1
#define	ENOENT	2
#define	ESRCH	3
#define	EINTR	4
#define	EIO	5
#define	ENXIO	6
#define	E2BIG	7
#define	ENOEXEC	8
#define	EBADF	9
#define	ECHILD	10
#define	EAGAIN	11
#define	ENOMEM	12
#define	EACCES	13
#define	EFAULT	14
#define	ENOTBLK	15
#define	EBUSY	16
#define	EEXIST	17
#define	EXDEV	18
#define	ENODEV	19
#define	ENOTDIR	20
#define	EISDIR	21
#define	EINVAL	22
#define	ENFILE	23
#define	EMFILE	24
#define	ENOTTY	25
#define	ETXTBSY	26
#define	EFBIG	27
#define	ENOSPC	28
#define	ESPIPE	29
#define	EROFS	30
#define	EMLINK	31
#define	EPIPE	32
#define	EDOM	33
#define	ERANGE	34

