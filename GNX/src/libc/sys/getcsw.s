;
; getcsw.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)getcsw.S	1.1 (NSC) 9/3/82
;

; C library - getcsw

; csw = getcsw();

getcsw: .equ 	38

_getcsw::
	addr	@getcsw,r0
	addr	8(sp),r1
	svc
	rxp	0
