;
; getgid.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)getgid.S	1.1 (NSC) 9/3/82
;

; C library -- getgid

; gid = getgid();
; returns gid in r0
; gid = getegid();
; returns effective gid in r1

getgid:	.equ	47

_getgid::
	addr	@getgid,r0
	addr	8(sp),r1
	svc
	rxp	0

_getegid::
	addr	@getgid,r0
	addr	8(sp),r1
	svc
	movd	r1,r0
	rxp	0
