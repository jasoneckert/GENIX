;
; iomove.s: version 2.2 of 10/10/83
; Fast byte move routine for C programs
;
; @(#)iomove.s	2.2 (NSC) 10/10/83
;

; Fast byte move routine for C programs
;
; iomove(toaddr, fromaddr, length) (Hope for word alignment)

	.export	_iomove

toaddr:		.equ	12(fp)
fromaddr:	.equ	16(fp)
length:		.equ	20(fp)

_iomove:
	enter	0
	movd	fromaddr, r1	; from here
	movd	toaddr, r2	; to here
	movd	length, r0	; count of bytes
	ashd	-2,r0		; count of doublewords to move
	movsd			; move the doublewords
	movqd	3,r0		; mask bytecount out
	andw	length,r0	; count of bytes to move
	movsb			; move the bytes
	exit
	rxp	0
