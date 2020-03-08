;
; atof.S: version 1.1 of 4/5/83
; 
;
; @(#)atof.S	1.1 (NSC) 4/5/83
;

;;
;;	atof: convert ascii to floating
;;
;;	C usage:
;;		double atof (s)
;;		char *s;
;;
;;	Register usage:
;;
;;	r0-r1:		value being developed
;;	r2:		first section: pointer to next char
;;			second section: binary exponent
;;	r3:		flags
;;	r4:		first section: the current character
;;			second section: scratch
;;	r5:		the decimal exponent
;;	r6-r7		scratch
;;

t1:	.equ	r7
t2:	.equ	r6
decexp:	.equ	r5
flags:	.equ	r3
binexp:	.equ	r2

;;
;;	Flag word bit definitions
;;
msign:	.equ	0	; mantissa has negative sign
esign:	.equ	1	; exponent has negative sign
decpt:	.equ	2	; decimal point encountered

;;
;; the value 2^31 (2147483648) in floating point
;; (so atof doesn't have to convert it)
;;

two31:	.double 00000000,x'41e00000

digit:	.byte	"90"	; range of valid digits
white:	.byte	13,8	; range of white space

	.import _ldexp
	.export _atof

text:	.equ	12(fp)	; the argument to atof

_atof:
	enter	[r3,r4,r5,r6,r7],0
;;
;;	Initialize...
;;
	movqd	0,flags		; flags
	movqd	0,decexp	; clear starting exponent
;;
;;	Skip leading white space
;;
	movd	text,r1		; address the first character
	movqd	-1,r0		; scan leading blanks off
 sk0:	
	movb	" ",r4		; skip blanks
	skpsbw
	movb	0(r1),r4
	addqd	1,r1
	checkb	t1,white,r4	; white space?
	bfc	sk0		; yes, continue scanning
;;				; no
;;	Check for a sign
;;
	movd	r1,r2		; put string pointer where it should be
	cmpb	"+",r4		; Plus sign?
	beq	cs1		;  ... yes
	cmpb	"-",r4		; Minus sign?
	bne	cs2		;  ... nope
	sbitb	msign,flags	; Indicate a negative mantissa
 cs1:	movzbd	0(r2),r4	; skip the char
	addqd	1,r2
 cs2:
;;
;;	Accumulate digits, keeping track of the exponent
;;
	movqd	0,r0		; Clear the
 	movqd	0,r1		; accumulator
 ad0:	checkb	r4,digit,r4	; Valid digit?
	bfs	ad4		; Nope...
;;
;;	We got a digit. Accumulate it.
;;
	cmpd	r1,214748364	; Will we overflow?
	bge	ad1		;  ... yes
;;
;;	Multiply (r0,r1) by 10
;;
	movd	r0,r6
	movd	r1,r7		; (r6,r7) = (r0,r1)
	addd	r0,r0
	addcd	r1,r1		; (r0,r1)*=2
	addd	r0,r0
	addcd	r1,r1		; ...*=4
	addd	r6,r0
	addcd	r7,r1		; ...*=5
	addd	r0,r0
	addcd	r1,r1		; (r0,r1)*=10
;;
;;	Add in the digit
;;
	addd	r4,r0		; add the digit
	addcd	0,r1		; possible carry into high half
	br	ad2		; join common code...
;;
;;	Here when digit won't fit into the accumulator
;;
 ad1:	addqd	1,decexp	; Ignore digit, bump exponent
;;
;;	If we have seen a decimal point, decrease exponent by 1
;;
 ad2:	tbitb	decpt,flags
	bfc	ad3
	addqd	-1,decexp
 ad3:
;;
;;	Fetch the next char, and continue.
;;
	movzbd	0(r2),r4
	addqd	1,r2
	br	ad0
;;
;;	Not a digit. Decimal point perhaps?
;;
 ad4:	addr	"0"(r4),r4	; restore char trashed by the check
	cmpb	r4,"."		; if it is not a decimal point, either it's
	bne	ad5		;   the end of the number or the start of
				;   the exponent.
	sbitb	decpt,flags	; if it is a decimal point, remember that
	bfc	ad3		;   we've seen one and keep gathering digits
				;   if it is the first one.
;;
;;	Check for an exponent
;;
 ad5:	movqd	0,r6		; Initialize exponent accumulator
	cmpb	r4,"E"		; Allow both upper case E
	beq	ex1		;   ... and ...
	cmpb	r4,"e"		; lower case e
	bne	ex7
;;
;;	Does the exponent have a sign?
;;
 ex1:	movzbd	0(r2),r4	; Next character...
	addqd	1,r2
	cmpb	r4,"+"		; Plus sign?
	beq	ex2		; ... yes ...
	cmpb	r4,"-"		; minus sign?
	bne	ex3		; ... no ...
	sbitb	esign,flags	; indicate exponent is negative
 ex2:	movzbd	0(r2),r4	; eat the sign char..
	addqd	1,r2
;;
;;	Accumulate exponent digits in r6
;;
 ex3:	checkb	r4,digit,r4	; Legal digit?
	bfs	ex4		; ... nope
	cmpd	r6,350		; exponent too large?
	bge	ex2		; ... yes
	addr	(r6)[r6:d],r6	; r6 *= 5;
	addr	(r4)[r6:w],r6	; r6 = r6*2+r4
	br	ex2		; continue...
 ex4:
;;
;;	Now get the final exponent and force it within a reasonable
;;	range so the scaling loops don't take forever for values
;;	that will ultimately cause overflow or underflow anyway.
;;	A tighter check on over/underflow will be done later by ldexp.
;;
	negd	r6,r6
	tbitb	esign,flags	; jump if negative exponent
	bfs	ex5
	negd	r6,r6
 ex5:	addd	r6,decexp	; Add given exponent to calculated exponent
	cmpd	decexp,-350	; Absurdly small?
	bgt	ex6		;   ... no
	movd	-350,decexp	;   ... yes, force reasonableness
 ex6:	cmpd	decexp,350	; Absurdly large?
	blt	ex7		;   ... no
	movd	350,decexp	;   ... yes, force reasonableness
 ex7:
;;
;;	The number has now been reduced to a mantissa and an exponent.
;;	The mantissa is a 63-bit positive binary integer in r0,r1, and
;;	the exponent is a signed power of 10 in r5. The msign bit in r3
;;	will be on if the mantissa should ultimately be considered negative.
;;
;;	We now have to convert it to a standard format floating point
;;	number. This is done by accumulating a binary exponent in r2, as
;;	we progressively bring r5 closer to zero.
;;
;;	Don't bother scaling if the mantissa is zero
;;
	movbl	0,f0		; setup zero return
	movd	r0,r2		; mantissa zero?
	ord	r1,r2
	cmpqd	0,r2
	beq	xit		; yes, exit with (f0,f1)=0.0

	movqd	0,binexp	; initialize binary exponent
	cmpqd	0,decexp	; which way to scale?
	bge	sd0		; scale down if decimal exponent <= 0
;;
;;	Scale up by "multiplying" mantissa by 10 as many times as
;;	necessary as follows:
;;
;;	Step 1: shift mantissa right as necessary to ensure that no
;;	overflow can occur when multiplying.
;;
 su0:	cmpd	r1,429496729	; Compare high part to (2^31)/5
	blt	su1		; skip this if safe
	movqd	1,t1		; shift (r0,r1) right 1 bit (the hard way)
	andd	r1,t1		; save the low bit of r1
	lshd	-1,r0		; shift lo part right 1
	lshd	-1,r1		; shift hi part right 1
	rotd	-1,t1		; make low bit of r1 be hi bit of t1
	ord	t1,r0		; put it into r0 (whew...)
	addqd	1,binexp	; increment binary exponent to compensate
	br	su0		; continue
;;
;;	Step 2: Multiply mantissa by 5, by appropriate shifting and
;;	double precision addition
;;
 su1:	movd	r0,r6
	movd	r1,r7
	addd	r0,r0
	addcd	r1,r1		; (r0,r1)*2
	addd	r0,r0
	addcd	r1,r1		; ... *4
	addd	r6,r0
	addcd	r7,r1		; ... *5
;;
;;	Step 3: Increment the binary exponent to take care of the final
;;	factor of 2, and continue if we need to...
;;
	addqd	1,binexp	; increment the exponent
	acbd	-1,decexp,su0	;   and go back for more
	br	cm0		; else build final value
;;
;;	Scale down. We must "divide" r0,r1 by 10 as many times
;;	as needed, as follows:
;;
;;	Step 0: At this point the condition codes reflect the state
;;	of the decimal exponent. If it's zero, we are done.
;;
 sd0:	beq	cm0		; go build the final number
;;
;;	Step 1: Shift mantissa left until the high-order bit (not
;;	counting the sign bit) is nonzero, so that the division will
;;	preserve as much precision as possible.
;;
	cmpqd	0,r1		; is the entire high-order half 0?
	bne	sd2		;   ... no, go shift one bit at a time
	movd	r0,r1		;   ... yes, shift left 30,
	lshd	-2,r1		; copy r0 to r1, and shift right 2
	lshd	30,r0		;     shift r0 left by 30,
	subd	30,binexp	;     decrement the exponent to compensate,
				;     and now it's known to be safe to shift
				;     left at least once more.
 sd1:	addd	r0,r0		; shift (r0,r1) left one, and
	addcd	r1,r1		;   decrement the exponent to compensate,
	addqd	-1,binexp
 sd2:	tbitb	30,r1		; continue shifting till high-order bit
	bfc	sd1		; is set.
;;
;;	Step 2: Divide the high-order part of (r0,r1) by 5 giving a
;;	quotient in r1 and a remainder in r7.
;;
 sd3:	movd	r1,r6		; Copy the high order part
	movqd	0,r7		;   zero extend to 64 bits
	deid	5,r6		; divide
	movd	r7,r1
	movd	r6,r7
;;
;;	Step 3: Divide the low-order part of (r0,r1) by 5 using the
;;	remainder from step 2 for rounding.
;;	Note that the result of this computation is unsigned,
;;	so we have to allow for the fact that an ordinary division
;;	by 5 could overflow. We make allowance by dividing by 10,
;;	multiplying the quotient by 2, and using the remainder to adjust
;;	the modified quotient.
;;
	movd	r0,r6		; dividend is low part of (r0,r1) plus
	addqd	2,r6		; 2 for rounding plus (2^32) * previous
	addcd	0,r7		; remainder
	deid	10,r6		; divide
	movd	r7,r0		; r0 <- quotient, r6 <- remainder
	addd	r0,r0		; make r0 result of dividing by 5
	cmpqd	5,r6		; if remainder is 5 or greater,
	bgt	sd4		;   increment the adjusted quotient
	addqd	1,r0
;;
;;	Step 4: Increment the decimal exponent, decrement the binary
;;	exponent (to make the division by 5 into a division by 10), and
;;	back for another iteration.
;;
 sd4:	addqd	-1,binexp	; binary exp--
	acbd	1,decexp,sd2	;  and continue till done
;;
;;	We now have the following:
;;
;;	r0:	lo-order half of a 64-bit integer
;;	r1:	hi-order half of the same 64-bit integer
;;	r2:	a binary exponent
;;
;;	The final result is the integer represented by (r0,r1)
;;	multiplied by 2 to the power contained in r2.
;;	(r0,r1) will be transformed into a floating-point value,
;;	the sign will be set appropriately, and ldexp will do
;;	the rest of the work.
;;
;;	Step 1: If the high-order bit (excluding the sign) of the
;;	high-order half (r1) is 1, then we have 63 bits of fraction,
;;	too many to convert easily. However, we also know we won't
;;	need them all, so we will just throw the low-order bit away,
;;	(and adjust the exponent appropriately)
;;
 cm0:	tbitb	30,r1
	bfc	cm1		; jump if no adjustment needed
	lshd	-1,r0		; lose the low order bit
	addqd	1,binexp	; increase the exponent to compensate
	br	cm1a		; now have two 31 bit quantities
;;
;;	Step 2: split the 62-bit number in (r0,r1) into two
;;	31-bit positive quantities
;;
 cm1:
	addd	r0,r0		; bring high order bit from r0
	addcd	r1,r1		;    into r1
	lshd	-1,r0		; right justify r0
;;
;;	Step 3: convert both halves to floating point
;;
 cm1a:
	movdl	r0,f6		; lo-order part to f6-f7
	movdl	r1,f0		; hi-order part to f0-f1
;;
;;	Step 4: multiply the high order part by 2^31 and combine
;;
	mull	two31,f0	; multiply by 2^31
	addl	f6,f0		; combine
;;
;;	Step 5: negate the floating value if appropriate
;;
	tbitb	msign,flags
	bfc	cm2
	negl	f0,f0
;;
;;	Step 6: call ldexp to complete the job
 cm2:
	movd	binexp,tos	; binary exponent
	movl	f0,tos		; the mantissa
	cxp	_ldexp		; go combine them
	adjspb	-12

 xit:	exit	[r3,r4,r5,r6,r7]
	rxp	0

;; Debugging labels -- uncomment for debugging

;;	.export sk0,cs2,ad4,ad5,ex1,ex3,ex4,ex7,cm0,xit
;;	.export su0,su1,sd0,sd1,sd2,sd3,sd4
