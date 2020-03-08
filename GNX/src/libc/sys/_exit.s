;
; _exit.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)_exit.S	1.1 (NSC) 9/3/82
;

; C library -- exit

; exit(arg);

exit:	.equ	1  	; system call table (sysent.c) index

__exit::
	movqd	exit,r0  
	addr	8(sp),r1
	svc
	bpt			;should not return
