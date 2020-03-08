;
; getuid.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)getuid.S	1.1 (NSC) 9/3/82
;

; C library -- getuid
;
; uid = getuid();
;  returns real user id in r0
; uid = geteuid();
;  returns effective uid in r1

getuid: .equ	24

_getuid::
	addr	@getuid,r0
	addr	8(sp),r1
	svc
	rxp	0

_geteuid::
	addr	@getuid,r0
	addr	8(sp),r1
	svc
	movd	r1,r0
	rxp	0
