;
; stat.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)stat.S	1.2 (NSC) 9/23/82
;

; C library -- stat

; error = stat(string, statbuf);

; char statbuf[36]

stat:	.equ    18	; system call table (sysent.c) index

	.import cerror

_stat::
	addr	@stat,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
