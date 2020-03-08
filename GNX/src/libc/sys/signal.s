; @(#)signal.s	1.6	9/11/83
; @(#)Copyright (C) 1983 by National Semiconductor Corp.
;
;	signal(n, 0);		/* default action on signal(n) */
;	signal(n, 1);		/* ignore signal(n) */
;	signal(n, label);	/* goto label on signal(n) */
;
;Returns old label, only one level.
;
;Notice: When generating a signal, the kernel does not transfer control
;directly to the specified label.  Instead, it calls an intermediate routine,
;"sigentry", which calls the desired routine after preserving the temporary
;registers.  This intermediate routine is in the module sigentry.s.


signal: 	.equ	48	;System call table (sysent.c) index

	.import cerror

_signal::
	addr	@signal,r0		;set up svc number
	addr    8(sp),r1		;point at arguments
	svc				;set new action, return old one
	bfc	noerr	  		;jump if no error
	cxp	cerror			;else return error code
noerr:	rxp     0			;done
