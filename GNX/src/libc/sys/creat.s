;
; creat.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)creat.S	1.1 (NSC) 9/3/82
;

; C library -- creat
;
;   file = creat(string, mode)
;  
;   file == -1 means error

creat:	.equ    8	; system call table (sysent.c) index

	.import cerror

_creat::
	addr	@creat,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
