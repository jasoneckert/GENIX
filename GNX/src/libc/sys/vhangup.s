;
; vhangup.S: version 1.2 of 1/26/83
; Mesa Unix C Library Source File
;
; @(#)vhangup.S	1.2 (NSC) 1/26/83
;

; vhangup -- revoke access to terminal

vhangup:.equ	65

	.import	cerror

_vhangup::
	addr	@vhangup,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
