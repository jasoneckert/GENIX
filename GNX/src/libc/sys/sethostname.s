;
; sethostname.S: version 1.1 of 5/10/83
; 
;
; @(#)sethostname.S	1.1 (NSC) 5/10/83
;
; C library -- sethostname
; This system call is from version 4.2.
; error = sethostname(name, namelen);

sethostname:	.equ	70

	.import cerror

_sethostname::
	addr	@sethostname,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
