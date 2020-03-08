;
; stime.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)stime.S	1.2 (NSC) 9/23/82
;

stime:	.equ	25

	.import cerror

_stime::
	addr	@stime,r0
	addr	0(8(sp)),r1	;point at actual time data
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
