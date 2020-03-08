/*
 * trap.c: version 3.22 of 10/3/83
 * Mesa Unix Kernel Source
 */
# ifdef SCCS
static char *sccsid = "@(#)trap.c	3.22 (NSC) 10/3/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/trap.h"
#include "../h/psr.h"
#include "../h/mmu.h"

extern	long	floatf;		/* nonzero if floating point is allowed */
int	svcinfoflag;		/* debugging */

/*
 * In order to stop the system from destroying
 * kernel data space any further, allow only one
 * terminal trap. After once_thru is set, any
 * futher traps will be handled by looping in place.
 */
int	once_thru = 0;

/*
 * Offsets of the user's registers relative to
 * the saved r0. See reg.h
 */
char	regloc[] = {
	R0, R1, R2, R3, R4, R5, R6, R7, RSP, RFP, RPC, RPS
};

/*
 * The mapping of traps to signals
 */
char	signals[] = {
	0,		/* NVI */
	SIGNMI,		/* NMI */
	SIGSEGV,	/* ABT - mmu abort */
	SIGFPE,		/* FPU */
	SIGILL,		/* ILL */
	0,		/* SVC */
	SIGDVZ,		/* DVZ */
	SIGIOT,		/* FLG */
	SIGBPT,		/* BPT */
	SIGTRAP,	/* TRC */
	SIGEMT,		/* UND */
	0,		/* AST */
	0,		/* SWTCH */
	SIGSEGV		/* SEGV - forced segment fault */
};
extern	int fuword();

struct	user *UDOT = &u;

#undef USERMODE
#define USERMODE(ps)	((ps) & 0x01000000)

#define	ISSIG(p)	((p)->p_sig && \
	((p)->p_flag&STRC || ((p)->p_sig &~ (p)->p_ignsig)) && issig())
/*
 * Called from icu.s when a processor trap occurs.
 * The arguments are the words saved on the system stack
 * by the hardware and software during the trap processing.
 * Their order is dictated by the hardware and the details
 * of C's calling sequence. They are peculiar in that
 * this call is not 'by value' and changed user registers
 * get copied back on return.
 * type is the kind of trap that occurred.
 */
trap (type, sp, r2, r1, r0, ppl, fp, pc, psl)
	caddr_t	r1;
	char	*pc;
	union {
		int PSLINT;
		struct modpsr PSRMOD;
	} psl;
#define pslint psl.PSLINT
#define psrmod psl.PSRMOD
{
	register int *locr0 = ((int *)&r0);
	register struct user *up = UDOT;
	register int i;
	register struct proc *p;
	/*	Padding to make call frames the same between traps
	 *	and syscalls
	 */
	register t2;
	long	 t3;
	/*	End kludge code		*/
	time_t syst;

	syst = up->u_vm.vm_stime;
	if (up->u_procp->p_flag & SMMU) clearben();
	if (USERMODE(locr0[RPS])){
		up->u_ar0 = locr0;		/* only change if usermode */
		i = signals[type];		/* set the signal type */

		switch(type) {

		case ABTTRAP:		/* user page fault */
		    up->u_abtar0 = locr0;	/* point to registers of trap */
		    i = aborttrap(pc,pslint,0);	/* call abort routine */
		    if (i) break;		/* cause signal */
		    goto out;			/* else exit */

		case SWTCHTRAP:		/* process switch */
		    goto out;

		case TRCTRAP:		/* instruction trace */
		    PSR(psrmod) &= ~PSR_T;
		    break;

		case UNDTRAP:		/* undefined instruction */
		    /* Here to handle undefined instructions.  To speed up
		     * context switching, the floating point registers are not
		     * saved or restored until their first use by the process.
		     * Their first use will cause an undefined instruction trap.
		     * We then enable floating point and return to the user.
		     * If the undefined instruction was really something besides
		     * a floating point instruction, the trap will occur again
		     * and a signal will then be generated.
		     */
		    if ((floatf == 0) || (u.u_procp->p_flag & SFPU)) break;
		    u.u_procp->p_flag |= SFPU;	/* floating point now in use */
		    u.u_fsr = 0;
		    enablefpu();		/* do config to enable fpu */
		    restorefpu();		/* initialize registers */
		    goto out;

		case SEGVTRAP:		/* forced segmentation fault */
		    u.u_signal[SIGSEGV] = SIG_DFL;	/* restore default */
		    break;

		case NMITRAP:		/* machine check */
		    machine_check(pc,pslint);
		    break;

		}
	} else 		 /* kernel mode traps */

		switch (type) {	

		case ABTTRAP:		/* kernel page fault */
		    i = (int) up->u_abtar0;	/* save previous pointer */
		    up->u_abtar0 = locr0;	/* point to registers of trap */
		    aborttrap(pc,pslint,ppl);	/* handle it */
		    up->u_abtar0 = (int *) i;	/* restore previous pointer */
		    return;			/* and return */

		case NMITRAP:		/* kernel mode machine check */
		    machine_check(pc,pslint);
		    break;
	       /*
	 	* Trap not expected.
	 	* Usually a kernel mode bus error.
	 	*/
		default:
		    asm("bpbadtrap::");
		    if( once_thru )
			    for(;;);
		    once_thru++;
		    printf("ps = %x pc = %x ipl=%x\n", pslint, pc, ppl);
		    printf("trap type %x\n", (u_int)type);
		    panic("trap");

	}
	psignal(up->u_procp, i);

out:
	p = up->u_procp;
	if(ISSIG(p))
		psig();
	p->p_pri = p->p_usrpri;
	if (runrun) {
		(void) spl6();
		setrq(p);
		swtch();
	}
#ifdef notyet		/* Profiling code not yet enabled */
	if (up->u_prof.pr_scale && (syst -= up->u_vm.vm_stime))
		addupc(locr0[PC], &up->u_prof, (int)-syst);
#endif
	curpri = p->p_pri;
	if (p->p_flag & SMMU) setben();
}

extern unsigned int sysentsize;		/* number of valid svcs */

syscall (type, sp, r2, r1, r0, ppl, fp, pc, psl)
	int	*r1;
	caddr_t	pc;
	struct modpsr psl;
{
	register int *locr0 = ((int *)&r0);
	register struct user *up = UDOT;
	register int *args;
	register struct sysent *callp;
	register struct proc *p;
	register int i;
	time_t syst;
	caddr_t opc;

	syst = up->u_vm.vm_stime;
	if (!USERMODE(locr0[RPS])){
		panic("kernel mode syscall");
	}
	up->u_ar0 = locr0;
	up->u_error = 0;
	args = r1;
	opc = pc;
	pc += 1;
	callp = &sysent[0];	/* MUST be nosys entry */
	if ((u_int) r0 < sysentsize)
		callp += r0;
	if (up->u_procp->p_flag & SMMU) clearben();
	for(i=0; i<callp->sy_narg; i++)
		up->u_arg[i] = fuword(args++);
	up->u_ap = up->u_arg;
	up->u_dirp = (caddr_t)up->u_arg[0];
	up->u_r.r_val1 = 0;
	up->u_r.r_val2 = locr0[R1];
	if (save(&up->u_qsav)) {
		if (up->u_error==0 && up->u_eosys == JUSTRETURN)
			up->u_error = EINTR;
	} else {
		up->u_eosys = JUSTRETURN;
		(*callp->sy_call)();
	}
	if (up->u_eosys == RESTARTSYS)
		pc = opc;	/* back up pc to restart syscall */
	else if (up->u_eosys == SIMULATERTI)
		dorti(callp->sy_narg);
	else if(up->u_error) {
		PSR(psl) |= PSR_F;
		locr0[R0] = up->u_error;
		if (svcinfoflag) {	/* describe svc failure */
			cprintf("pid %d: svc %d failed, errno %d",
				up->u_procp->p_pid, callp-sysent, up->u_error);
			for (i = 0; i < callp->sy_narg; i++)
				cprintf(", %s%d", i ? "":"args ", up->u_arg[i]);
			cprintf("\n");
		}
	} else {
		PSR(psl) &= ~PSR_F;
		locr0[R0] = up->u_r.r_val1;
		locr0[R1] = up->u_r.r_val2;
	}
	p = up->u_procp;
	if(p->p_cursig || ISSIG(p))
		psig();
	p->p_pri = p->p_usrpri;
	if (runrun) {
		(void) spl6();
		setrq(p);
		swtch();
	}
#ifdef notyet		/* Profiling code not yet enabled */
	if (up->u_prof.pr_scale && (syst -= up->u_vm.vm_stime))
		addupc(locr0[PC], &up->u_prof, (int)-syst);
#endif
	curpri = p->p_pri;
	if (p->p_flag & SMMU) setben();
}

/*
 * nonexistent system call-- set fatal error code.
 */
nosys() { u.u_error = 100; }

/*
 * Ignored system call
 */
nullsys() { }

/*
 *	Analyze the cause of a NMI delivery.
 */

static char *mcname[] = {
	"POWER",
	"MEMORY",
	"NMI",
	"HEAT"
};

extern short erreg;

machine_check(pc,psl)
u_int pc;
u_int psl;
{
	register e = erreg;
	register char **s;
	register int i;

	spl7();
	printf("\nmachine check: pc=%x ps=%x * ",pc,psl);
	if ((e & 0xf) == 0) {
		panic("UNKNOWN");
	}
	s = mcname;
	for (i=0;i<4;i++) {
		if (e & 1) {
			printf("%s ",*s++);
		}
		e >>= 1;
	}
	panic("MACHINE CHECK");
}
