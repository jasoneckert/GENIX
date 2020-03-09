;
; sio.s: version 1.1 of 6/2/83
; 
;
; @(#)sio.s	1.1 (NSC) 6/2/83
;

;Demo Board sio
usart::		.equ	h'c00000

_getchar:: 
	tbitb	1,@usart+2
	bfc	_getchar
	movzbd	@usart+0,r0
	andb    127,r0
	rxp	0

_putchar::
	tbitb	0,@usart+2
	bfc	_putchar
	movb	8(sp),@usart+0
	movd	400,r0			;avoid overrunning the vax
vaxw:	acbd	-1,r0,vaxw
	rxp	0
