;
; chown.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)chown.S	1.1 (NSC) 9/3/82
;

; C library -- chown
;
; error = chown(name, owner, group)

chown:	.equ	16
	
	.import cerror

_chown::
	addr	@chown,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
	rxp	0
noerror:
	movqd	0, r0
	rxp	0
