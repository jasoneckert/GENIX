;
; ldexp.S: version 1.4 of 3/28/83
; 
;
; @(#)ldexp.S	1.4 (NSC) 3/28/83
;
;Routine to return the double precision floating point number equal
;to value * 2^exp, where value is a double precision number and exp
;is an integer.  Thus this is the inverse of the frexp function.  C call:
;	result = ldexp(value, exp);
;Underflow or overflow is handled by returning zero or the largest possible
;number, and setting errno to ERANGE.  Otherwise errno is unchanged, so you
;must clear it before calling frexp if you want to check it afterwards.


ERANGE:	.equ	34			;error code for overflow

	.import	_errno			;error location

_ldexp::
	movbl	0,f0			;get zero ready
	cmpl	f0,8(sp)		;is value zero?
	beq	retzer			;yes, return that
	extsd	14(sp),r0,4,11		;extract current exponent field
	addd	16(sp),r0		;add desired scaling factor
	cmpqd	0,r0			;underflow?
	bge	under			;yes
	cmpd	2047,r0			;overflow?
	ble	over			;yes
	inssd	r0,14(sp),4,11		;set new exponent field
	movl	8(sp),f0		;get value
retzer:	rxp	0			;done


over:	movl	huge,f0			;get positive large number
	cmpqb	0,15(sp)		;was original number negative?
	ble	under			;nope
	negl	f0,f0			;yes, create negative large number
under:	addr	@ERANGE,_errno		;set the error code
	rxp	0			;done

huge:	.double	h'ffffffff		;largest floating point number
	.double	h'7fefffff
