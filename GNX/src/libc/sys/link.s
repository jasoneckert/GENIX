;
; link.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)link.S	1.2 (NSC) 9/23/82
;

; C library -- link

; link(oldfile, newfile);
;

link:	.equ	9  	; system call table (sysent.c) index

	.import cerror

_link::	addr	@link,r0  
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
