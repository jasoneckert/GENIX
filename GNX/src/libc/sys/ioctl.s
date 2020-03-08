;
; ioctl.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)ioctl.S	1.2 (NSC) 9/23/82
;

;  C library -- ioctl
; 
;  ioctl(fdes,command,arg)
;  struct * arg;
;
;  result == -1 if error
 
ioctl:	.equ	54

	.import	cerror
 
_ioctl::
	addr	@ioctl,r0
	addr	8(sp),r1
	svc
	bfc	done
	cxp	cerror
done:	rxp	0
