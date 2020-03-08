;
; alarm.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)alarm.S	1.1 (NSC) 9/3/82
;

; C library - alarm

;   prev=alarm(secs);

alarm: 	.equ	27	;System call table (sysent.c) index

	.import cerror
                
_alarm::
	addr	@alarm,r0
	addr	8(sp),r1
	svc
	bfc	noerror  
	cxp     cerror
noerror:
	rxp     0
