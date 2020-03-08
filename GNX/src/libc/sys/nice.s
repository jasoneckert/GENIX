;
; nice.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)nice.S	1.2 (NSC) 9/23/82
;

; C library-- nice

; error = nice(hownice)

nice:	.equ	34

	.import cerror

_nice::
	addr	@nice,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
