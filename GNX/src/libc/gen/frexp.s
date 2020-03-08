;
; frexp.S: version 1.3 of 3/28/83
; 
;
; @(#)frexp.S	1.3 (NSC) 3/28/83
;
;Routine to accept a double precision floating point number, return
;its mantissa as a double precision value x less than one, and store the
;integer scaling factor n such that value = x * 2^n.  Call from C:
;	value = frexp(number, &intval);


_frexp::
	movbl	0,f0			;get zero value ready
	cmpl	f0,8(sp)		;is the number zero?
	beq	retzer			;yes
	extsd	14(sp),r0,4,11		;extract the exponent field
	inssw	1022,14(sp),4,11	;replace exponent field with good value
	subd	1022,r0			;remove bias
	movd	r0,0(16(sp))		;store exponent
	movl	8(sp),f0		;return scaled mantissa
	rxp	0			;done

retzer:	movqd	0,0(16(sp))		;store exponent of zero
	rxp	0			;done
