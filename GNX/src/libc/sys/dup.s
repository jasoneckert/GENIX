;
; dup.S: version 1.2 of 9/6/82
; Mesa Unix C Library Source File
;
; @(#)dup.S	1.2 (NSC) 9/6/82
;

; C library - dup,dup2
;
;   fildes1 = dup(fildes2)
;
;   fildes1 = dup2(fildes2,fildes3)

dup: 	.equ	41	;System call table (sysent.c) index

	.import cerror
                
_dup2::	orb	h'40,8(sp)		;set 0100 bit to indicate dup2 call
;	br	_dup			;go do the svc

_dup::	addr	@dup,r0
	addr	8(sp),r1
	svc
	bfc	noerror  
	cxp     cerror
noerror:
	rxp     0
