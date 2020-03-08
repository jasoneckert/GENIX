;
; getpid.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)getpid.S	1.1 (NSC) 9/3/82
;

; C library -- read

; pid = getpid();

getpid:	.equ	20  	; system call table (sysent.c) index

	.import cerror

_getpid::
	addr	@getpid,r0  
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
