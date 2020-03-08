;
; read.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)read.S	1.1 (NSC) 9/3/82
;

; C library -- read

; nread = read(file, buffer, count);
;
; nread ==0 means eof; nread == -1 means error

read:	.equ	3  	; system call table (sysent.c) index

	.import cerror

_read::
	movqd	read,r0  
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
