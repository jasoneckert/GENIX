;
; vwrite.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)vwrite.S	1.1 (NSC) 9/3/82
;

; C library -- vwrite

; nwritten = vwrite(file, buffer, count);
;
; nwritten == -1 means error

vwrite: .equ	64+4

	.import cerror

_vwrite::
	addr	@vwrite,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
