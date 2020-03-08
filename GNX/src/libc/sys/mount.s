;
; mount.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)mount.S	1.2 (NSC) 9/23/82
;

; C library -- mount

; error = mount(dev, file, flag)

mount:	.equ	21

	.import	cerror

_mount::
	addr	@mount,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
