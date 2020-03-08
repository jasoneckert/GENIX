;
; killpg.S: version 1.1 of 11/29/82
; 
;
; @(#)killpg.S	1.1 (NSC) 11/29/82
;

; C library -- killpg

; result = killpg(pgrp, sig);
;
; result == 0  if all processes in the group are sent the signals
; result == -1 if any process in the process group cannot be sent the
;              signal, or if the group is empty

kill:	.equ	37		;share system call with kill
				;we negate the signal number to denote killpg

	.import	cerror

_killpg::
	addr	@kill,r0
	negd	12(sp),tos	;push -sig on stack to denote killpg
	movd	12(sp),tos	;move process group
	addr	0(sp),r1	;get new address of args
	svc
	adjspb	-8		;clear copied args off stack
	bfc 	noerror
	cxp	cerror
noerror:
	rxp	0
