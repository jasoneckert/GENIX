;
; chroot.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)chroot.S	1.1 (NSC) 9/3/82
;

;  C library -- chroot
 
;  error = chroot(string);
 
chroot:	.equ	61
 
	.import cerror

_chroot::
	addr	@chroot,r0
	addr	8(sp),r1
	svc
	bfc	noerror
	cxp	cerror
noerror:
	rxp	0
