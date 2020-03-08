;
; makdir.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)makdir.S	1.2 (NSC) 9/23/82
;

; C library -- makdir

; error = makdir(string);

mknod:	.equ	14

	.import	cerror

_makdir::
	addr	@mknod,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
