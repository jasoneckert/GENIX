; @(#)signal.s	1.5	9/11/83
; @(#)Copyright (C) 1983 by National Semiconductor Corp.
;
; jobs library -- sigsys
;
; sigsys(n, SIG_DFL);		/* default action on signal(n) */
; sigsys(n, SIG_HOLD);		/* block signal temporarily */
; sigsys(n, SIG_IGN);		/* ignore signal(n) */
; sigsys(n, label);		/* goto label on signal(n) */
; sigsys(n, DEFERSIG(label));	/* goto label with signal SIG_HOLD */
;
; returns old label, only one level.


SIGDORTI:	.equ	h'200	;bit telling kernel to simulate procedure return
signal:		.equ	48	;System call table (sysent.c) index

	.import cerror

_sigsys::
	addr	@signal,r0		;get svc code
	addr    8(sp),r1		;point at arguments
	svc				;set the new signal address
	bfc	doret			;skip if successful
	cxp     cerror			;else set error code
doret:	rxp     0			;done


;Intermediate subroutine called by the kernel when sending any signal.
;It saves the psr flags and some registers, calls the higher-level signal
;handler, restores the psr flags and registers, and then returns back to
;the place which was interruped from.  The routine "start" in crt0.s uses
;a special signal call to tell the kernel where this routine is.  To send
;a signal, the kernel effectively executes the following instructions:
;	movd	signal_number,tos	;store signal which occurred
;	movd	signal_handler,tos	;store routine to handle the signal
;	cxp	sigentry		;call main signal handler routine
;
;This entry routine differs from the standard one in the old-style signal
;module, in that more state is saved so that sigpeel can work correctly.
;WARNING:  It is assumed that sigcatch trashes only r0, r1, r2, and r7.

sigentry::
	enter	[r0,r1,r2,r3,r7],0	;save registers and frame pointer
	sprd	upsr,r3			;save psr flags
	movd	16(fp),tos		;copy signal number for routine to see
	cxpd	12(fp)			;call signal handler
	adjspb	-4			;remove signal number
	lprd	upsr,r3			;restore old psr flags
	exit	[r0,r1,r2,r3,r7]	;restore accumulators
	rxp	8			;return to interrupted routine
;
; sigpeel(n, newact)
;
; This routine, called from _sigcatch, clears the stack of most of the
; data pushed during signal handling, and then calls the kernel to reenable
; the signal with the action specified.  The SIGDORTI flag is or'ed into
; the signal number.  This causes the kernel to finish the stack cleanup
; so as to remove the remaining items pushed during signal handling,
; to restore user registers R0 and R1, which normally would be altered
; by the supervisor call, to restore the user's stack and frame pointers,
; and to return to the user's mainline code which was interrupted by the
; signal.  The kernel does NOT return to this routine, as would normally be
; the case on a system call.  Between _sigpeel and the kernel's code, the
; effect is to return to the user's mainline code as if from an interrupt
; routine, with the state restored as it was when the interrupt (signal)
; happened.  Of course, any special data set by the signal handler remain,
; hopefully to be discovered by the user's code.
; 
; This is not a general "clear the stack and reenable the signal" routine --
; it is tailored specifically to the way the kernel calls signal routines
; and handles the SIGDORTI flag, and to the flow of control and stack
; manipulation in the jobs library, particularly by the routines in this
; module.  That flow of control is:
;
;	. the kernel calls sigentry
;	. sigentry cxp's to __sigcatch
;	. __sigcatch calls to the user's signal handler, which does whatever
;	  it wants and then returns to __sigcatch
;	. __sigcatch calls sigpeel
;
; When _sigpeel begins execution,    When _sigpeel executes the svc to
; the stack looks like this:         return control to the kernel, the
;                                    stack looks like this:
;
;(user's|///////////////////////|             (user's|/// user's //|
;  old  |/// user's old stack //|               old  |/ old stack /|
;  sp)->|///////////////////////|               sp)->|/////////////|
;	+-----------------------+ -                  +-------------+
;	|        signum         |  \                 |   newact    |
;	+-----------------------+   |                +-------------+
;	|    routine to call    |   |                |   signum    |
;       +-----------+-----------+    } pushed by     +------+------+
;       |    psr    |    mod    |   |  kernel to     | psr  | mod  |
;       +-----------+-----------+   |  send signal   +------+------+
;       |      return addr      |  /                 | return addr |
;       +-----------------------+ -                  +-------------+
;    +->|     user's old fp     |  \    (1)      fp->|  user's fp  |
;    |  +-----------------------+   |                +-------------+
;    |  |       user's R0       |   |                |  user's R0  |
;    |  +-----------------------+   |                +-------------+
;    |  |       user's R1       |   |            sp->|  user's R1  |
;    |  +-----------------------+   |                +-------------+
;    |  |       user's R2       |    } pushed by
;    |  +-----------------------+   |  sigentry
;    |  |       user's R3       |   |
;    |  +-----------------------+   |
;    |  |       user's R7       |   |   (2)
;    |  +-----------------------+   |
;    |  |        signum         |  /
;    |  +-----------+-----------+ -
;    |  |    psr    |    mod    |  \
;    |  +-----------+-----------+   } pushed by cxp from
;    |  |      return addr      |  /  sigentry to __sigcatch
;    |  +-----------------------+ -
;fp->+--|        saved fp       |  \
;       +-----------------------+   |
;       |                       |   |
;       /    local variables    /    } pushed by enter
;       /  and saved registers  /   |  in __sigcatch
;       |                       |  /
;       +-----------------------+ -
;       |        newact         |  \
;       +-----------------------+   } args to _sigpeel
;       |        signum         |  /
;       +-----------+-----------+ -
;       |    psr    |    mod    |  \
;       +-----------+-----------+   } pushed by cxp from
;sp-->  |      return addr      |  /  __sigcatch to _sigpeel
;       +-----------------------+ -
;
;Some of _sigpeel's code is a duplicate of sigentry, above.

_sigpeel::
	movd	8(sp),r0		;get signal number argument
	movd	12(sp),r1		;and new action argument
	lprd	fp,0(fp)		;back up the frame pointer to (1)
	addr	-5*4(fp),r2		;calculate proper stack pointer address
	lprd	sp,r2			;back up the stack pointer to (2)
	cmpqd	0,r1			;any action to set?
	bne	dorti			;yes, must call kernel
	lprd	upsr,r3			;restore psr flags
	exit	[r0,r1,r2,r3,r7]	;restore user registers
	rxp	8			;return

dorti:	lprd	upsr,r3			;restore psr flags
	restore	[r2,r3,r7]		;restore most user registers
					;(the kernel restores r0, r1, and fp)
	ord	SIGDORTI,r0		;set flag telling kernel "rti" wanted
	movd	r0,12(fp)		;store signal number
	movd	r1,16(fp)		;store action
	addr	@signal,r0		;denote signal call
	addr	12(fp),r1		;get address of arguments
	svc				;reenable signal and return to user
	bpt				;should never get here
