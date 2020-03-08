;
; vread.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)vread.S	1.1 (NSC) 9/3/82
;

; C library -- vread

; nread = vread(file, buffer, count);
;
; nread ==0 means eof; nread == -1 means error

vread:  .equ	64+3

	.import	cerror

_vread::
	addr	@vread,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
