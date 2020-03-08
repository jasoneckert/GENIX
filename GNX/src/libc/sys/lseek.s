;
; lseek.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)lseek.S	1.1 (NSC) 9/3/82
;

; C library -- lseek

; error = lseek(file, offset, ptr);
;


lseek:	.equ	19  	; system call table (sysent.c) index

	.import cerror

_lseek::
	addr	@lseek,r0  
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
