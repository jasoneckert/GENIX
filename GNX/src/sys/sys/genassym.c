/*
 * genassym.c: version 1.10 of 8/22/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)genassym.c	1.10 (NSC) 8/22/83";
# endif

/*
	This file generates offsets for some of the fields of "interesting"
	structs. This is a holdover from Berzerkley, and is a REAL kludge..
*/
#include "../h/param.h"
#include "../h/buf.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/vmmeter.h"
#include "../h/pte.h"
#include "../h/vm.h"
#include "../h/panic.h"
#include "../h/pdma.h"

#define	X(type,field)	((long) (&((struct type *)(0))->field))

main()
{
	printf("#ifdef LOCORE\n");
/*
 *	For the schedular
 */
	printf("#define\tP_LINK %d\n", X(proc,p_link));
	printf("#define\tP_RLINK %d\n", X(proc,p_rlink));
	/*printf("#define\tP_ADDR %d\n", X(proc,p_addr));*/
	printf("#define\tP_PRI %d\n", X(proc,p_pri));
	printf("#define\tP_STAT %d\n", X(proc,p_stat));
	printf("#define\tP_WCHAN %d\n", X(proc,p_wchan));
	printf("#define\tSSLEEP %d\n", SSLEEP);
	printf("#define\tSRUN %d\n", SRUN);

	printf("#define\tP_DEV %d\n",X(pdma,p_addr));
	printf("#define\tP_MEM %d\n",X(pdma,p_mem));
	printf("#define\tP_END %d\n",X(pdma,p_end));
	printf("#define\tP_ARG %d\n",X(pdma,p_arg));

	printf("#define\tU_SPTI %d\n", X(user,u_spti));
	printf("#define\tU_FSR %d\n", X(user,u_fsr));
	printf("#define\tU_FREGS %d\n", X(user,u_fregs[0]));

	printf("#define\tV_SWTCH %d\n", X(vmmeter,v_swtch));
	printf("#define\tV_TRAP %d\n", X(vmmeter,v_trap));
	printf("#define\tV_SYSCALL %d\n", X(vmmeter,v_syscall));
	printf("#define\tV_INTR %d\n", X(vmmeter,v_intr));

	printf("#define\tPS_GEN %d\n", X(panic,ps_gen[0]));
	printf("#define\tPS_PC %d\n", X(panic,ps_pc));
	printf("#define\tPS_PSRMOD %d\n", X(panic,ps_psrmod));
	printf("#define\tPS_SP %d\n", X(panic,ps_sp));
	printf("#define\tPS_FP %d\n", X(panic,ps_fp));
	printf("#define\tPS_INTBASE %d\n", X(panic,ps_intbase));
	printf("#define\tPS_PTB0 %d\n", X(panic,ps_ptb0));
	printf("#define\tPS_PTB1 %d\n", X(panic,ps_ptb1));
	printf("#define\tPS_MSR %d\n", X(panic,ps_msr));
	printf("#define\tPS_EIA %d\n", X(panic,ps_eia));
	printf("#define\tPS_BPR0 %d\n", X(panic,ps_bpr0));
	printf("#define\tPS_BPR1 %d\n", X(panic,ps_bpr1));
	printf("#define\tPS_STRING %d\n", X(panic,ps_string));
	printf("#define\tPS_ARGS %d\n", X(panic,ps_args));
	printf("#define\tPS_NEST %d\n", X(panic,ps_nest));
	printf("#define\tPS_VERSION %d\n", X(panic,ps_version));
	printf("#define\tPS_PROC %d\n", X(panic,ps_proc));
	printf("#define\tPS_SPT %d\n", X(panic,ps_spt));
	printf("#define\tPANICADDR %d\n", PANICADDR);
	printf("#define\tPTLEVEL2 %d\n", PTLEVEL2);

	printf("#endif\n");
}
