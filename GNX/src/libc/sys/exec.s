;
; exec.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)exec.S	1.1 (NSC) 9/3/82
;

; C library -- exec

; exec(file, argv);
;
; where argv is a vector argv[0] ... argv[x], 0
; last vector element must be 0

exec:	.equ	11  	; system call table (sysent.c) index

	.import cerror

_exec::
	addr	@exec,r0  
	addr	8(sp),r1
	svc
	cxp 	cerror		;only get here on an error
	rxp	0
