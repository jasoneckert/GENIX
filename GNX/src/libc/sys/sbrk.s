;
; sbrk.S: version 1.3 of 4/21/83
; Mesa Unix C Library Source File
;
; @(#)sbrk.S	1.3 (NSC) 4/21/83
;

;old = sbrk(increment);
;
;sbrk gets increment more core, and returns a pointer
;	to the beginning of the new core area
;

break:	.equ	17			;system table index

	.import	cerror
	.import	_end			;program break location

_sbrk::	cmpqd	0,brkaddr
	bne	gotbrk
	addr	_end,brkaddr		;set initial break value
gotbrk:	addd	brkaddr,8(sp)
	addr	@break,r0
	addr	8(sp),r1
	svc
	bfs	error
	movd	brkaddr,r0		;return old break
	movd	8(sp),brkaddr		;remember new one
	rxp	0

; brk(value)
; as described in man2.
; returns 0 for ok, -1 for error.

_brk::	addr	@break,r0
	addr	8(sp),r1
	svc
	bfs	error
	movd	8(sp),brkaddr		;remember new break
	rxp	0

error:	cxp	cerror
	rxp	0

	.static
brkaddr:.double	0			;current break location
	.endseg
