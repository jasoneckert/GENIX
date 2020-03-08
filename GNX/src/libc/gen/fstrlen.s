;
; fstrlen.S: version 1.1 of 1/14/83
; 
;
; @(#)fstrlen.S	1.1 (NSC) 1/14/83
;

	.program

_strlen::
	movd	r4,tos		;preserve ac
	movqd	-1,r0		;infinite byte count
	movd	12(sp),r1	;address of string
	movqd	0,r4		;terminate on null
	skpsbu			;skip string looking for null
	movd	r1,r0		;get address of the null
	subd	12(sp),r0	;subtract original address to get length
	movd	tos,r4		;restore ac
	rxp	0		;return

	.endseg
