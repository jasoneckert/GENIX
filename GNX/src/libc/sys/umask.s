;
; umask.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)umask.S	1.1 (NSC) 9/3/82
;

;  C library -- umask
 
;  omask = umask(mode);
 
umask:	.equ	60

	.import cerror

_umask::
	addr	@umask,r0
	addr	8(sp),r1
	svc
	bfc	noerror
	cxp	cerror
noerror:
	rxp	0
