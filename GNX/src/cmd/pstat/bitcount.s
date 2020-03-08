;
; bitcount.s: version 1.1 of 2/14/83
; 
;
; @(#)bitcount.s	1.1 (NSC) 2/14/83
;

; routine to count the number of bits in a structure which are set to 1
;
; val = bitcount(tableaddress, tablebitlength);
; returns number of bits set to 1

_bitcount::
	movqd	0,r0		;initialize count
loop:	cmpqd	0,12(sp)	;test length of table
	blt	test		;something left to do
	rxp	0		;nothing left, return
test:	addqd	-1,12(sp)	;decrement bit offset
	tbitd	12(sp),0(8(sp))	;see if bit set
	bfc	loop
	addqd	1,r0		;it was, increment count
	br	loop
