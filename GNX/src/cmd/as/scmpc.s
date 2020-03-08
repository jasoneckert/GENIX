
        .byte '@(#)scmpc.s	3.31    9/20/83',0
	;@(#) Copyright (c) 1983 by National Semiconductor Corp.

; skip characters of two strings until 1st string ends with h'0
; or two strings have the different character.
; return: (s1 > s2, r0=1;)  (s1 = s2, r0 = 0;)  (s1 < s2, r0 = -1;)

s1:	.equ 	12(sp)	
s2:	.equ	 8(sp)  

_scmpc::
	movqd	-1,r0		; infinite count
	movd	s1,r1		; string 1
	movd	s2,r2		; string 2
	movd	r4,tos		; save this register
	movqd	0,r4		; terminate on null
	cmpsbu			; do the compare
	cmpb	0(r1),0(r2)	; test last char again
	movqd	-1,r0		; assume less...
	blt	exit_scmpc
	sned	r0		; sets r0 to 0 if equal, 1 if not equal
exit_scmpc:
	movd	tos,r4		; restore the register
	rxp	8

