;
; chmod.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)chmod.S	1.2 (NSC) 9/23/82
;

; C library -- chmod

; error = chmod(string, mode);


chmod:	.equ	15  	; system call table (sysent.c) index

	.import cerror

_chmod::
	addr	@chmod,r0  
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
