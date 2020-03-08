;
; mknod.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)mknod.S	1.2 (NSC) 9/23/82
;

; C library -- mknod

; error = mknod(string, mode, major.minor);

mknod:	.equ	14

	.import cerror

_mknod::
	addr 	@mknod,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
