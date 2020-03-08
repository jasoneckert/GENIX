; @(#)sigentry.s	3.1	9/11/83
; @(#)Copyright (C) 1983 by National Semiconductor Corp.
;
;Intermediate subroutine called by the kernel when sending any signal.
;It saves the psr flags and temporary registers, calls the higher-level signal
;handler, restores the psr flags and temporary registers, and then returns
;back to the place which was interruped from.  The routine "start" in crt0.s
;uses a special signal call to tell the kernel where this routine is.  This
;routine is replaced by a fancier one if sigset is in use.  To send a signal,
;the kernel effectively executes the following instructions:
;	movd	signal_number,tos	;store signal which occurred
;	movd	signal_handler,tos	;store routine to handle the signal
;	cxp	sigentry		;call main signal handler routine

sigentry::
	save	[r0,r1,r2,r3]		;save temporaries and an extra reg
	sprd	upsr,r3			;save psr flags in safe register
	movd	28(sp),tos		;copy signal number for routine to see
	cxpd	28(sp)			;call signal handler
	adjspb	-4			;remove signal number
	lprd	upsr,r3			;restore old psr flags
	restore	[r0,r1,r2,r3]		;restore accumulators
	rxp	8			;return to interrupted routine
