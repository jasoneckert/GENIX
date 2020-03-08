;
; write.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)write.S	1.1 (NSC) 9/3/82
;

; C library -- write

; nwritten = write(file, buffer, count);
;
; nwritten == -1 means error

write:	.equ	4	; system call table (sysent.c) index

	.import cerror

_write::
	movqd	write,r0
	addr	8(sp),r1
	svc		
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
