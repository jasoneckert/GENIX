;
; fstat.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)fstat.S	1.2 (NSC) 9/23/82
;

; C library -- fstat
;
; error = fstat(file, statbuf);
;
; char statbuf[34]

fstat:	.equ	28

	.import	cerror

_fstat::
	addr	@fstat,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
