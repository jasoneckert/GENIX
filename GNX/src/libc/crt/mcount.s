;
; mcount.s: version 1.2 of 9/21/83
; count subroutine called during profiling
;
; @(#)mcount.s	1.2 (NSC) 9/21/83
;


.comm	countbase,4

mcount::
	movd	0(r0),r1
	cmpqd	0,r1
	beq	init
incr:
	addqd	1,0(r1)
return:
	rxp	0
init:
	movd	countbase,r1
	cmpqd	0,r1
	beq	return
	addr	8(r1),countbase
	movd	0(sp),0(r1)
	addqd	4,r1
	movd	r1,0(r0)
	br 	incr
