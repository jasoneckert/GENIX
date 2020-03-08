;
; exit.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)exit.S	1.1 (NSC) 9/3/82
;

; C library -- exit

; exit(code)
; code is return in r0 to system

exit:	.equ	1		;system call index

	.import	__cleanup

_exit::
	cxp	__cleanup
	movqd	exit,r0
	addr	8(sp),r1
	svc
	bpt			;should never get here
