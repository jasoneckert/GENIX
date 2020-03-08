;
; umount.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)umount.S	1.2 (NSC) 9/23/82
;

; C library -- umount
;	umount(special)
;	char  *special
;

umount: .equ	22

	.import cerror

_umount::
	addr	@umount,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
