;
; setpgrp.S: version 1.1 of 11/29/82
; 
;
; @(#)setpgrp.S	1.1 (NSC) 11/29/82
;

; C library -- setpgrp, getpgrp

; setpgrp(pid, pgrp);	/* set pgrp of pid and descendants to pgrp */
; if pid==0 use current pid
;
; getpgrp(pid)
; implemented as setpgrp(pid, -1)

setpgrp:	.equ	39	;system call table (sysent.c) index

	.import	cerror

_setpgrp::
	addr	@setpgrp,r0
	addr	8(sp),r1
	svc
	bfc	noerror
	cxp	cerror
noerror:
	rxp	0

_getpgrp::
	addr	@setpgrp,r0	;share system call with setpgrp
	movqd	-1,tos		;use pgrp = -1 to denote getpgrp
	movd	12(sp),tos	;copy pid
	addr	0(sp),r1	;get address of new arguments
	svc
	adjspb	-8		;get rid of copied arguments
	bfc	noerror
	cxp	cerror
	rxp	0
