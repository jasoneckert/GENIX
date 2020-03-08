;
; syscall.S: version 1.2 of 2/3/83
; Mesa Unix C Library Source File
;
; @(#)syscall.S	1.2 (NSC) 2/3/83
;

	.import cerror

_syscall::
	movd	8(sp),r0		; syscall number
	addr	12(sp),r1
	svc
	bfc	noerror   
	cxp	cerror
noerror:
	rxp	0
	
