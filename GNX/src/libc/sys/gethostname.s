;
; gethostname.s: version 1.3 of 5/11/83
; 
;
; @(#)gethostname.s	1.3 (NSC) 5/11/83
;
; C library -- gethostname
; This system call is from version 4.2.
; error = gethostname(name, &namelen);

gethostname:	.equ	71

	.import cerror

_gethostname::
	movd	12(sp),r2		;get address of maximum length
	movd	0(r2),12(sp)		;store length for system call
	addr	@gethostname,r0
	addr	8(sp),r1
	svc
	bfs 	error
	movd	8(sp),r1		;address of returned string
	addd	0(r2),r1		;find end of storage area
	movqb	0,-1(r1)		;guarantee null byte at end of area
	movd	8(sp),r1		;address of string again
loop:	cmpqb	0,0(r1)			;found end yet?
	beq	done			;yes
	addqd	1,r1			;nope, keep looking
	br	loop

done:	subd	8(sp),r1		;compute length of string
	movd	r1,0(r2)		;store back into length
	rxp	0			;return success

error:	cxp 	cerror
	rxp	0
