;
; bcopy.s: version 1.1 of 1/3/83
; Fast byte copy routine for the 16K
;
; @(#)bcopy.s	1.1 (NSC) 1/3/83
;

;Copy count bytes from from to to.
;	bcopy(from, to, count);

_bcopy::
	movd	8(sp),r1		;address of source string
	movd	12(sp),r2		;address of destination string
	movd	16(sp),r0		;number of bytes to move
	movsb				;move the bytes
	rxp	0			;done

