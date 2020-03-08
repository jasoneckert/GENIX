;
; execve.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)execve.S	1.1 (NSC) 9/3/82
;

; C library -- execve

; execve(file, argv, env);
;
; where argv is a vector argv[0] ... argv[x], 0
; last vector element must be 0

exece:	.equ	59

	.import cerror

_execve::
	addr	@exece,r0
	addr	8(sp),r1
	svc
	cxp	cerror			;only get here on an error
	rxp	0
