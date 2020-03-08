;
; mdate.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)mdate.S	1.2 (NSC) 9/23/82
;

; C library-- mdate

mdate:	.equ	30

	.import cerror

_mdate::
	addr	@mdate,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
