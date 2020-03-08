;
; fstrcmp.S: version 1.1 of 1/14/83
; 
;
; @(#)fstrcmp.S	1.1 (NSC) 1/14/83
;

	.program

_strcmp::
	movd	r4,tos		;preserve ac
	movqd	-1,r0		;infinite byte count
	movd	12(sp),r1	;first string
	movd	16(sp),r2	;second string
	movqd	0,r4		;terminate on null
	cmpsbu			;compare the strings
	cmpb	0(r1),0(r2)	;recompare final bytes (needed if "until" stop)
	movqd	-1,r0		;assume first string smaller
	blt	sfin		;if so, return -1
	sned	r0		;otherwise return 0 if equal or 1 if not
sfin:	movd	tos,r4		;restore ac
	rxp	0		;return

	.endseg
