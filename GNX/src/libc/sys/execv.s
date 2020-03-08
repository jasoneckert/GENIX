;
; execv.S: version 1.3 of 10/9/82
; Mesa Unix C Library Source File
;
; @(#)execv.S	1.3 (NSC) 10/9/82
;

; C library -- execv

; execv(file, argv);
;
; where argv is a vector argv[0] ... argv[x], 0
; last vector element must be 0
; the environment is defaulted to the standard one

exece:	.equ	59  	; system call table (sysent.c) index

	.import cerror
	.import	_environ

_execv::
	movd	_environ,tos	;get the default environment pointer
	movd	16(sp),tos	;and argument vector
	movd	16(sp),tos	;and file name
	addr	@exece,r0	;svc index
	addr	0(sp),r1	;point at arguments
	svc			;execute the program
	cxp 	cerror		;only get here on an error
	adjspb	-12		;pop arguments off of stack
	rxp	0		;return with error code on r0
