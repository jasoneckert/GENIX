;
; acct.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)acct.S	1.2 (NSC) 9/23/82
;

; C library -- acct
;	acct(file)
;	char	*file
;

acct:	.equ	51

	.import	cerror

_acct::
	addr	@acct,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
