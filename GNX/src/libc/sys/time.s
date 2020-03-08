;
; time.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)time.S	1.2 (NSC) 9/23/82
;

; C library -- time

; tloc = time(tloc);
; long *tloc;
;

time:	.equ	13

_time::
	addr	@time,r0
	addr	8(sp),r1
	svc
	cmpqd	0,8(sp)
	beq	nostore
	movd	r0,0(8(sp))
nostore:
	rxp	0

;
; ftime(tp)
; struct timeb *tp;
;

ftime:	.equ	35

_ftime::
	addr	@ftime,r0
	addr	8(sp),r1
	svc
	rxp	0

;
; stime - set time
;
; stime(tp)
; int *tp;
;

.import	cerror

stime:	.equ	25

_stime::
	addr	@stime,r0
	movd	8(sp),r1		; get time pointer
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
