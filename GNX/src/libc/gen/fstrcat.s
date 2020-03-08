;
; fstrcat.S: version 1.1 of 1/14/83
; 
;
; @(#)fstrcat.S	1.1 (NSC) 1/14/83
;

	.program

_strcat::
	movd	r4,tos		;preserve ac
	movqd	-1,r0		;infinite byte count
	movd	12(sp),r1	;destination string
	movqd	0,r4		;terminate on null
	skpsbu			;skip to end
	movd	r1,r2		;set as destination string
	movd	16(sp),r1	;source string
	movsbu			;copy the string
	movqb	0,0(r2)		;append null to string
	movd	12(sp),r0	;return destination string
	movd	tos,r4		;restore ac
	rxp	0		;return

	.endseg
