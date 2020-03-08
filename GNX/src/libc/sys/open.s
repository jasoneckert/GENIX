;
; open.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)open.S	1.1 (NSC) 9/3/82
;

; C library -- open
;
;   file = open(string, mode)
;  
;   file == -1 means error

open:	.equ    5	; system call table (sysent.c) index

	.import cerror

_open::
	movqd	open,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
