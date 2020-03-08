/*
 * machdep.c: version 3.26 of 9/11/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)machdep.c	3.26 (NSC) 9/11/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/inode.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/mmu.h"
#include "../h/vm.h"
#include "../h/buf.h"
#include "../h/devvm.h"
#include "../h/reboot.h"
#include "../h/psr.h"
#include "../h/conf.h"
#include "../h/timer.h"
#include "../h/icu.h"

extern	long	*promintbase;		/* interrupt vector bases */
extern	long	*unixintbase;
int	nulldev();		/* to force optimizer to preserve references */

/*
 * Machine-dependent startup code
 */

startup()
{
	;
}

/*
 * Start the clock.
 */
clkstart()
{
	register TIMER *timer = (TIMER *) CP_Time;
	u_short clkcnt = C2VAL;
	register char *rp = (char *)&clkcnt;

	BYTE timer->mode  = C2MODE;
	BYTE timer->clock = *rp;		/* Lo byte... */
	BYTE timer->clock = rp[1];		/* Hi byte... */

        icuenable(TIMRMASK);
}

# ifdef notdef
/*
 * This routine is unused; its function is accomplished in icu.s,
 * where all graphics-board related interrupts are cleared at once
 * before returning from the interrupt.  The former call in clock()
 * has been removed in favor of this new interrupt dismissal strategy.
 */
/*
 * Restart the clock.
 */
clkreld()
{
	/*
	 * Clear the clock interrupt by writing a zero
	 * into the vsync bit position.
	 */
	*CLOCKCLR = ~CLKENABLE;
}
# endif

/*
 * undefined interrupt
 */

undint(vector,r2,r1,r0,ipl,ufp,upc,ups)
int vector;
{

	printf("STRAY INTERRUPT #%x\n",vector);
	printf("psr= %x, pc= %x, ipl= %x\n",ups,upc,ipl);
}


/*
 * Let a process handle a signal by simulating a CXPD call.  Read the CALL
 * description in the Software Modules chapter of the Programmers Reference
 * Manual to see what is going on.  The function address for a signal is an
 * external procedure descriptor, made up of a module and displacement pair.
 * Data pushed on the user's stack is the signal number, the actual routine
 * to be called, and the return pc and mod.
 */
sendsig(funcadr, signo)
	register caddr_t funcadr;	/* address for signal handler */
{
	register long	newpc;		/* new user pc */
	register long	calladr;	/* address to be called */
	register long	newsp;		/* new stack pointer */
	register int	count;		/* number of bytes of stack data */
	long	data[4];		/* data to push on user's stack */

	calladr = (int)u.u_sigcatch;	/* get global signal catcher */
	count = 16;			/* and number of bytes of data */
	if (calladr == 0) {		/* temporary: if no global catcher */
		calladr = (int)funcadr;	/* then use signal's catcher */
		count = 8;		/* and only 8 bytes of data */
	}
	newpc = fuword((calladr & 0xffff) + 8);	/* get program base of module */
	if (newpc == -1) goto bad;	/* failed */
	newpc += (((unsigned)calladr)>>16);	/* add offset to get real pc */
	if ((unsigned) newpc >= TOPUSER) /* range check pc */
		goto bad;		/* bad */
	data[0] = u.u_ar0[RPC];		/* store old pc */
	data[1] = u.u_ar0[RPSRMOD];	/* store old mod */
	data[2] = (int)funcadr;		/* store procedure descriptor */
	data[3] = signo;		/* store signal number */
	newsp = u.u_ar0[RSP] - count;	/* compute new stack pointer */
	if (copyout(data,newsp,count))	/* try to copy data to stack */
		goto bad;		/* failed */
	u.u_ar0[RSP] = newsp;		/* set new stack pointer */
	u.u_ar0[RPSRMOD] &= ~0xffff;	/* remove old mod */
	u.u_ar0[RPSRMOD] |= (calladr & 0xffff);	/* insert new mod */
	u.u_ar0[RPC] = newpc;		/* set new pc */
	return;				/* all done */

	/* Here if the signal cannot be sent.  Cause the process to fail
	 * with an untrappable segmentation violation.
	 */

bad:	u.u_signal[SIGSEGV] = SIG_DFL;		/* make signal untrappable */
	psignal(u.u_procp, SIGSEGV);		/* post it */
}


/*
 * Simulate a return from interrupt on return from the syscall,
 * then popping n words off the stack.
 * Actually, the stack is set up so that we simulate:
 *	exit	[r0,r1]		;restore user r0, r1, and fp
 *	rxp	0		;restore user psr, mod, sp, and pc
 *	adjspb	-(n*4)		;pop off n words (arguments to syscall)
 *
 * This code is typically invoked out of the libjobs library, where
 * we are resetting a signal at the very end of signal processing.  We do
 * not want to return to the libjobs code which is executing the svc,
 * but to the user's mainline code which was interrupted by the signal.
 */
dorti(n)
{
	register long	*dp;		/* pointer to data */
	long	data[7];		/* data to set */

	if (copyin(u.u_ar0[RSP], data, 7*4)) {	/* read data from stack */
		u.u_signal[SIGSEGV] = SIG_DFL;	/* failed */
		psignal(u.u_procp, SIGSEGV);
		return;
	}
	dp = data;
	if (u.u_sigcatch == SIG_DFL) dp = &data[2];	/* temporary */
	u.u_ar0[R1] = dp[0];			/* restore r1 */
	u.u_ar0[R0] = dp[1];			/* restore r0 */
	u.u_ar0[RFP] = dp[2];			/* set new frame pointer */
	u.u_ar0[RPC] = dp[3];			/* set new pc */
	u.u_ar0[RPSRMOD] &= ~0xffff;		/* clear out old mod */
	u.u_ar0[RPSRMOD] |= (dp[4] & 0xffff);	/* insert new one */
	u.u_ar0[RSP] += 4*(n+5);		/* pop stack */
}


/***********************************************************************

	Utility routines for accesing the mmu registers. Other
	processor registers can be added as required.

	mtpr -- move to processor register. Store val into reg.

	mfpr -- move from processor register. Fetch data from reg.

	These routines "know" about allocations. Val is expected to
	be located at r7, and the variable r in mfpr is expected
	to be at -4(fp). The variable "r" is used as a bug workaround
	in this rev of the mmu. It can go directly into r0 as the
	return value when the bug is fixed...

***********************************************************************/

mtpr(reg,val)
enum mmreg reg;
register int val;	/* r7 */
{
	switch(reg) {

	default:
		return(0);		/* Failed call */
	case eia:
		asm("	lmr	eia,r7");
		break;
	case msr :
		asm("	lmr	msr,r7");
		break;
	case ptb0:	/* changing our own mapping.  The address of the
			 * new value cannot be virtual (MMU bug?)
			 */
 		asm("	lmr	ptb0,r7");
		break;
	case ptb1:
		asm("	lmr	ptb1,r7");
		break;
	case bpr0:
		asm("	lmr	bpr0,r7");
		break;
	case bpr1:
		asm("	lmr	bpr1,r7");
		break;
	case bcnt:
		asm("	lmr	bcnt,r7");
		break;
	case pf0 :
		asm("	lmr	pf0,r7");
		break;
	case pf1 :
		asm("	lmr	pf1,r7");
		break;
	case sc  :
		asm("	lmr	sc,r7");
	}
	return(1);		/* Successful return */
}

mfpr(reg)
enum mmreg reg;
{
	int	r;		/* Workaround due to mmu bug */

	switch(reg) {

	default:
		return;
	case eia:
		asm("	smr	eia,-4(fp)");
		break;
	case msr:
		asm("	smr	msr,-4(fp)");
		break;
	case ptb0:
		asm("	smr	ptb0,-4(fp)");
		break;
	case ptb1:
		asm("	smr	ptb1,-4(fp)");
		break;
	case bpr0:
		asm("	smr	bpr0,-4(fp)");
		break;
	case bpr1:
		asm("	smr	bpr1,-4(fp)");
		break;
	case bcnt:
		asm("	smr	bcnt,-4(fp)");
		break;
	case pf0:
		asm("	smr	pf0,-4(fp)");
		break;
	case pf1:
		asm("	smr	pf1,-4(fp)");
		break;
	case sc:
		asm("	smr	sc,-4(fp)");
	}
	return(r);
}

int	waittime = -1;

boot(paniced, arghowto)
	int paniced, arghowto;
{
	register int howto;		/* r7 == how to boot */
	register int devtype;		/* r6 == major of root dev */

	howto = arghowto;
	if ((howto&RB_NOSYNC)==0 && waittime < 0 && bfreelist[0].b_forw) {
		waittime = 0;
		update();
		printf("syncing disks... ");
		/* we'll try some future stuff */
		{ register struct buf *bp;
		  int iter, nbusy;

		  for (iter = 0; iter < 10; iter++) {
			nbusy = 0;
			for (bp = &buf[nbuf]; --bp >= buf; )
				if (bp->b_flags & B_BUSY)
					nbusy++;
			if (nbusy == 0)
				break;
			printf("%d", nbusy);
		  }
		}
		while (++waittime <= 3)
			sleep((caddr_t)&lbolt, PZERO);
		printf("done\n");
	}
	splx(7);			/* extreme priority */
	devtype = major(rootdev);
	if (howto&RB_HALT) {
		printf("spinning...");
		asm("bicpsrw	h'800");
		asm("movb	h'39,@h'ffee00");	/* disable interrupts */
		asm("wait");			/* really wedge machine */
		while (1) nulldev();
	} else {
		if (paniced == RB_PANIC)
			if (*((short *) CP_OptBuffer) & 0x1000)
				doadump();	/* BIT 4: Allow dump */
			else	dobpt();
		}
	for (;;)
		halt();
#ifdef lint
	printf("howto %d, devtype %d\n", howto, devtype);
#endif
	/*NOTREACHED*/
}

/* check if already called(then just halt)
   could turn off translation and but have to have to a real stack,
   we might not do this; right now just dump in virtual */
doadump()
{
	asm("bicpsrw	h'800");
	dumpsys();
	halt();
}

/* These comments are false!
 * Doadump comes here after turning off memory management and
 * getting on the dump stack, either when called above, or by
 * the auto-restart code.
 */
dumpsys()
{
	extern int dumpdev, dumplo;
/* don't drag rpb block in,
	rpb.rp_flag = 1;
*/
	if ((minor(dumpdev)&07) != 1)
		return;
	printf("\ndumping to dev %x, offset %d\n", dumpdev, dumplo);
	printf("dump ");
	switch ((*bdevsw[major(dumpdev)].d_dump)(dumpdev)) {

	case ENXIO:
		printf("device bad\n");
		break;

	case EFAULT:
		printf("device not ready\n");
		break;

	case EINVAL:
		printf("area improper\n");
		break;

	case EIO:
		printf("i/o error");
		break;

	default:
		printf("succeeded");
		break;
	}
}

/* Halt instruction: means do a BOOT UNIX */
halt()
{
int i;
/* r6 and r7 from caller, are to be passed to boot program */
	
	asm("lprw	psr,0");	/* reset psr */
	asm("lprd	sp,h'3f000");	/* reset sp */
	asm("lmr	msr,2");	/* reset msr, turn off translate */
	asm("lprd	intbase,@h'e00014");	/* reset intbase to monitor */
	asm("movmd	@h'e00010,@h'10,3");	/* recopy monitor module table*/
	asm("movd	h'e000,r1");		 /* clear boot bss */
	asm("movd	0,0(r1)");	
	asm("addr	4(r1),r2");	
	asm("movd	40000,r0");
	asm("movsd		");
	asm("jump	@h'e02000");	/* jump to boot program in ROM */

}


/* Call the prom monitor with a breakpoint.  We must write-enable
 * the BPT entry in our intbase table, and set it back to the
 * original prom entry so that the bpt will get there.
 */
dobpt()
{
	register long	*addr;		/* address of BPT vector */
	register long	savaddr;	/* previous vector value */

	printf("BPT\n");
	addr = &unixintbase[8];
	changemapprot(SYSSPTI, addr, PG_RWNA);
	changemapprot(SYSSPTI, (int)addr + 3, PG_RWNA);
	savaddr = *addr;
	*addr = promintbase[8];
	asm("bpt");
	nulldev();			/* force optimizer to keep references */
	*addr = savaddr;
}
