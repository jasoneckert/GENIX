;
; wait3.S: version 1.2 of 12/16/82
; 
;
; @(#)wait3.S	1.2 (NSC) 12/16/82
;

; C library -- wait3

; pid = wait3(&status, flags, &vmstat);
;
; pid == -1 if error
; status indicates fate of process, if given
; flags may indicate process is not to hang or
; that untraced stopped children are to be reported.
; vmstat optionally returns detailed resource usage information
;

wait3:	.equ	7		;same as wait.  High order bit of
				;status is set to 1 to denote a wait3 call

	.import	cerror

_wait3::
	movqd	wait3,r0
	movd	16(sp),tos	;move vmstat
	movd	16(sp),tos	; and flags
	movd	16(sp),tos	; and status
	movqd	1,r1
	rotd	-1,r1		;generate a high order bit
	ord	r1,0(sp)	;set high order bit of status
	addr	0(sp),r1
	svc
	adjspb	-12		;remove copied args from stack
	bfc	noerror
	cxp	cerror
	rxp	0
noerror:
	cmpqd	0,8(sp)		;status desired?
	beq	nostatus	;no
	movd	r1,0(8(sp))	;store child's status
nostatus:
	rxp	0
