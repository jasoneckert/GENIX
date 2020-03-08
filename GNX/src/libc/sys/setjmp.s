;
; setjmp.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)setjmp.S	1.1 (NSC) 9/3/82
;

; C library -- setjmp, longjmp
;
; 	longjmp(a,v)
;  will generate a "return(v)" 
;  from the last call to
; 	setjmp(a)
;  by restoring r3-r7, fp, sp,
;  upsr, mod, and pc from 'a'
;  and doing a return.

_setjmp::
	movd    8(sp),r0 
	movd    r3,0(r0)
	movd    r4,4(r0)
	movd    r5,8(r0)
	movd    r6,12(r0)
	movd    r7,16(r0)
	sprd    fp,20(r0)      ; FP
	sprd    sp,24(r0)      ; user SP
	sprw    upsr,28(r0)    ; user PSR
	movw    4(sp),30(r0)   ; MOD
        movd    0(sp),32(r0)   ; PC
        movqd   0,r0
	rxp     0

_longjmp::
	movd    12(sp),r0      ; return(v)
	movd    8(sp),r1
	movd    0(r1),r3
	movd    4(r1),r4
	movd    8(r1),r5
	movd    12(r1),r6
	movd    16(r1),r7
	lprd    fp,20(r1)      ; FP
	lprd    sp,24(r1)      ; user SP
	lprw    upsr,28(r1)    ; user PSR
	movw    30(r1),4(sp)   ; MOD
        movd    32(r1),0(sp)   ; PC
	cmpqd   0,r0
	bne     L1 
	movqd   1,r0
L1:     rxp     0
