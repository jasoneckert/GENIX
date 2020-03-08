;
; udiv.s: version 1.2 of 9/3/82
; Mesa Unix C Library Source
;
; @(#)udiv.s	1.2 (NSC) 9/3/82
;

;udiv(dividend,divisor)

udiv::	movd	8(sp),r0
	movqd	0,r1
	deid	12(sp),r0
	movd	r1,r0
	rxp	0

;urem(dividend,divisor)

urem::	movd	8(sp),r0
	movqd	0,r1
	deid	12(sp),r0
	rxp	0
