;
; setuid.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)setuid.S	1.2 (NSC) 9/23/82
;

; C library -- setuid

; error = setuid(uid);

setuid:	.equ	23

	.import cerror

_setuid::
	addr	@setuid,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
