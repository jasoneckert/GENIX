;
; unlink.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)unlink.S	1.2 (NSC) 9/23/82
;

; C library -- unlink

; error =  unlink(filename);

unlink:	.equ	10	; system call table (sysent.c) index

	.import cerror

_unlink::
	addr	@unlink,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
