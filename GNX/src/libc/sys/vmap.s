;
; vmap.S: version 1.1 of 2/4/83
; 
;
; @(#)vmap.S	1.1 (NSC) 2/4/83
;

; Svc to perform various page map functions


vmap: .equ	69

	.import cerror

_vmap::
	addr	@vmap,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
	rxp	0
noerror:
	movqd	0,r0
	rxp	0
