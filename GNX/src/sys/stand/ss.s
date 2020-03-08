
;
; ss.s: version 1.2 of 10/11/83
; 
;
; @(#)ss.s	1.2 (NSC) 10/11/83
;
; Startup and assembly language support for stand-alone programs

topmem:	.equ	x'40000		; we will reset our stack to 256k

	.static
savesp:	.blkd
	.endseg

; program starts here (jump to here from zero)
	.program

realstart::
	enter	0
	sprd	sp,savesp	;save stack pointer
	lprd	sp,topmem	; because of stupid bootsrt0...

; other initialization
	.import %initfun, _main 
	cxp	%initfun
	cxp	_main

	; fall into exit below...

	.export	_exit
_exit:
	lprd	sp,savesp	;restore stack pointer
	exit
	rxp    0		;return to boot program

	.endseg
