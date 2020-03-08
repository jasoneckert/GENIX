;
; times.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)times.S	1.1 (NSC) 9/3/82
;

; C library -- times
; times(buffer)
; struct tms *buffer;

times:	.equ	43

_times::
	addr	@times,r0
	addr	8(sp),r1
	svc
	rxp	0
