;
; kill.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)kill.S	1.2 (NSC) 9/23/82
;

; C library - kill
;
;   status = kill(pid,sig);

kill: 	.equ	37	;System call table (sysent.c) index

	.import cerror
                
_kill::
	addr	@kill,r0
	addr	8(sp),r1
	svc
	bfc	done  
	cxp     cerror
done:	rxp	0
