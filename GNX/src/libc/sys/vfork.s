; @(#)vfork.S	1.1	(National)	9/4/82
; C library - vfork
;
;    pid = vfork();
;
; From SVC parent returns:
;	r0 =  pid of child
;	r1 =  0
; and child returns:
;	r0 =  pid of parent
;	r1 = 1
;
;This code is rather obscure, since if the child process returns from the
;procedure which executed the vfork svc, it can then trash the return pc
;of the parent stack (since they share the same memory).  Therefore, we
;have to adjust things so that the svc is executed in the same stack frame
;as our caller.  Notice that we are running with the wrong mod and static
;base register values for most of this routine.

vfork: 	.equ	62	;System call table (sysent.c) index

	.import cerror

_vfork::
	movd	tos,r2		;get return pc
	addr	here,tos	;replace it with our own address
	rxp	0		;"return" back here with caller's mod register

here:	addr	@vfork,r0	;set up for svc
	svc			;do the vfork
	bfc	noerror		;branch if successful
	cxp     cerror		;set error if not
	jump	r2		;return (mod register was already restored)
noerror:
	cmpqd	0,r1		;are we the child?
	beq	done		;no
	movqd	0,r0		;yes, clear return
done:	jump	r2		;return to caller (mod was already restored)
