;
; sync.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)sync.S	1.2 (NSC) 9/23/82
;

sync:	.equ	36

_sync::
	addr	@sync,r0
	svc
	rxp	0
