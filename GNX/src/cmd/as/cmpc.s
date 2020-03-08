
        .byte '@(#)cmpc.s	3.31    9/20/83',0
	;@(#) Copyright (c) 1983 by National Semiconductor Corp.

length:	.equ	16(sp)
s1:	.equ 	12(sp)
s2:	.equ	 8(sp)

_cmpc::
	movd	length,r0
	movd	s1,r1
	movd	s2,r2
	cmpsb
	movqd	-1,r0
	blt	exit_cmpc
	sned	r0
exit_cmpc:
	rxp	12

