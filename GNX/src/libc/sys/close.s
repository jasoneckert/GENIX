;
; close.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)close.S	1.2 (NSC) 9/23/82
;

; C library -- close

; error =  close(file);

close:	.equ	6	; system call table (sysent.c) index

	.import cerror

_close::
	movqd	close,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
