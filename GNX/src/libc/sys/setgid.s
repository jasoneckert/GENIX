;
; setgid.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)setgid.S	1.2 (NSC) 9/23/82
;

; C library -- setgid

; error = setgid(uid);

setgid: .equ	46

	.import cerror

_setgid::
	addr	@setgid,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
