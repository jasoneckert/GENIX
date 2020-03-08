;
; pause.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)pause.S	1.1 (NSC) 9/3/82
;

; C library - pause
;
;   pause();

pause: 	.equ	29	;System call table (sysent.c) index

	.import cerror
                
_pause::
	addr	@pause,r0
	addr	8(sp),r1
	svc
	bfc	noerror  
	cxp     cerror
noerror:
	rxp     0
