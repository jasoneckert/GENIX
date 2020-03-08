;
; modf.S: version 1.1 of 3/25/83
; 
;
; @(#)modf.S	1.1 (NSC) 3/25/83
;
;Routine to split a double precision floating point number into its
;integral and fractional parts (both returned as double precision numbers).
;	fracpart = modf(number, &intpart);
;where fracpart is the returned fractional part, and intpart is a pointer to 
;the place to store the integral part.

maxint:	.long	9007199254740991.0	;largest exact integer (2^53 - 1)
minint:	.long	-9007199254740991.0	;smallest exact integer
maxcnv:	.long	2147483647.0		;largest doubleword integer (2^31 - 1)
mincnv:	.long	-2147483648.0		;smallest doubleword integer
scale:	.long	4194304.0		;scaling value (2^22)


_modf::
	movl	8(sp),f0		;get number
	cmpl	f0,maxcnv		;verify value within integer range
	bgt	over1			;nope
	cmpl	f0,mincnv		;another check
	blt	over1			;nope
	truncld	f0,r0			;convert float to integer
	movdl	r0,f2			;convert back to float
	subl	f2,f0			;subtract to get fractional part
	movl	f2,0(16(sp))		;store integer part
	rxp	0			;return fractional part

;Here if the number is too large for its integer value to fit into a
;doubleword.  Try to get the result by scaling it down.

over1:	cmpl	f0,maxint		;verify value within representable range
	bgt	over2			;nope
	cmpl	f0,minint		;another check
	blt	over2			;nope
	divl	scale,f0		;scale down the number
	truncld	f0,r0			;convert float to integer
	movdl	r0,f2			;convert back to float
	subl	f2,f0			;subtract to get fractional part
	mull	scale,f0		;scale back up the fractional part
	mull	scale,f2		;scale back up the integral part
	truncld	f0,r0			;convert fractional part to integer 
	movdl	r0,f4			;then back into float
	subl	f4,f0			;subtract to get fractional part
	addl	f2,f4			;add integral parts together
	movl	f4,0(16(sp))		;store integral part
	rxp	0			;return fractional part

;Here if the number is too large to be represented exactly as an integer
;in floating point.  Return the number itself as the integer part, and zero
;as the fractional part.

over2:	movbl	0,f0			;return zero as fractional part
	movl	f0,0(16(sp))		;and original number as integral part
	rxp	0			;done
