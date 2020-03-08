;
; wait.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)wait.S	1.1 (NSC) 9/3/82
;

; C library - wait
;
;    pid = wait(0);
;      or,
;    pid = wait(&status);
;   
;    if error pid == -1, status indicates fate of process.

wait: 	.equ	7	;System call table (sysent.c) index

	.import cerror
                
_wait::
	movqd	wait,r0
	addr	8(sp),r1
	svc
	bfc	noerror  
	cxp     cerror
	movqd	-1,r1
noerror:
	cmpqd	0,8(sp)		; status desired?
	beq	nostatus	; no
	movd	r1,0(8(sp))	; store child's status
nostatus:
	rxp     0
