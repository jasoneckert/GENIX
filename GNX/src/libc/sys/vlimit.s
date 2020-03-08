;
; vlimit.S: version 1.2 of 1/26/83
; Mesa Unix C Library Source File
;
; @(#)vlimit.S	1.2 (NSC) 1/26/83
;

; C library -- vlimit

; oldvalue = vlimit(what, newvalue);
; if newvalue == -1 old value is returned and the limit is not changed

vlimit: .equ	66

	.import	cerror

_vlimit::
	addr	@vlimit,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
