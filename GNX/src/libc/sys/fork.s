;
; fork.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)fork.S	1.1 (NSC) 9/3/82
;

; C library - fork
;
;    pid = fork();
;
; From SVC parent returns:
;	r0 =  pid of child
;	r1 =  0
; and child returns:
;	r0 =  pid of parent
;	r1 = 1

fork: 	.equ	2	;System call table (sysent.c) index

	.import cerror

_fork::
	movqd	fork,r0
	addr	8(sp),r1
	svc
	bfc	noerror
	cxp     cerror
	rxp     0
noerror:
	cmpqd	0,r1		;are we the child?
	beq	done		;no
	movqd	0,r0		;yes, clear return
done:	rxp     0
