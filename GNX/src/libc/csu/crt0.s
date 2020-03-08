; @(#)crt0.s	1.4	9/11/83
; @(#)Copyright (C) 1983 by National Semiconductor Corp.
;
;	Startup code for user mode programs.  Transfers to main when done.

	.routine _main, _exit, %initfun, sigentry
	.program

signal:		.equ	48		;svc number for signal
sigcatchall:	.equ	h'400		;magic signal number

start::	
	adjspb	8
	movd	8(sp),0(sp)		; push argc
	addr	12(sp),r0
	movd	r0,4(sp)		; push argv
L1:
	cmpqd	0,0(r0)			; null argp?
	addqd	4,r0			; post increment list pointer...
	bne	L1
	cmpd	r0,0(4(sp))		; any envp's?
	bls	L2
	addqd	-4,r0			; nope... backup to the null
L2:
	movd	r0,8(sp)		; 0 if no env, not 0 if env
	movd	r0,_environ
	lxpd	sigentry,tos		;get descriptor for signal handler
	addr	@sigcatchall,tos	;and magic signal number
	addr	@signal,r0		;number for signal
	addr	tos,r1			;address of arguments
	svc				;tell kernel where signal handler is
	adjspb	-8			;remove arguments
	cxp	%initfun		;initialize runtime addresses
	cxp	_main			;call user program
	movd	r0,tos			; pass the exit value to exit
	cxp	_exit			;then exit
	bpt				;should not return

	.endseg

	.static

_environ::	.blkd			; the environment pointer

	.endseg
