; @(#)mcrt0.s	1.4	9/11/83
; @(#)Copyright (C) 1983 by National Semiconductor Corp.
;
;	Startup code for user mode programs with monitoring.
;	Transfers to main when done.

	.program
	.routine _main, %initfun, _sbrk, _monitor, sigentry
	.import _etext
	.comm   countbase,4
cbufs:	.equ	10			; the number of entry counters
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
	addr	@signal,r0		;svc for signal
	addr	tos,r1			;address of arguments
	svc				;tell kernel where signal handler is
	adjspb	-8			;remove arguments
	cxp	%initfun		;initialize runtime addresses

;  Set up monitoring
	addr	_etext-9,r3		;etext-16+7
	ashd	-3,r3			;size of array
	addd	r3,r3			;tally size is 2
	addr	8*cbufs+12(r3),r3	;entrance counters plus header
	addr	@cbufs,tos		; # of entrance counters
	movd	r3,tos
	movd	r3,tos			; for _sbrk
	cxp	_sbrk			; allocate buffer
	adjspb	-4
	cmpqd	-1,r0			; leave size of buffer on stack
	beq	nospace
	movd	r0,tos			; ptr to buffer
	addr	12(r0),countbase
	addr	_etext,tos
	addr	@16,tos			; begin at 16.
	cxp	_monitor
	adjspb	-20
;  Finished setting up monitoring

	cxp	_main			;call user program
	movd	r0,tos			; pass the exit value to exit
	cxp	_exit			;then exit
	bpt				;should not return

	.static

_environ::	.blkd			; the environment pointer

	.endseg

	.routine _write, __cleanup

nospace:
	addr	@(em1-emsg),tos
	addr	emsg,tos
	movqd	2,tos
	cxp	_write
	adjspb	-12		; then exit

; exit(code)
; code is return in r0 to system

exit:	.equ	1		;system call index

_exit::
	cxp	__cleanup
	movqd	0,tos
	cxp	_monitor
	adjspb	-4
	movqd	exit,r0
	addr	8(sp),r1
	svc
	bpt			;should never get here

emsg:	.byte "No space for monitor buffer"
	.byte x'a,x'0
em1:
	.endseg
