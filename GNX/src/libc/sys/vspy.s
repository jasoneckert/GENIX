;
; vspy.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)vspy.S	1.2 (NSC) 9/23/82
;

; C library - vspy
; Enable spying on physical addresses.  Requires super-user privileges.
;	vspy(useraddr, physaddr, writeflag);
; useraddr/	user virtual address to map to
; physaddr/	desired physical address to be mapped, or -1 to unmap
; writeflag/	nonzero if mapping allows writing

vspy: 	.equ	38	;System call table (sysent.c) index

               .import	cerror 

_vspy::	addr	@vspy,r0
	addr	8(sp),r1
	svc
	bfc	done  
	cxp     cerror
done:	rxp	0
