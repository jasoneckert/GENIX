;
; chdir.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)chdir.S	1.2 (NSC) 9/23/82
;

; C library -- chdir -- set current working directory

; chdir(dirname)

chdir:	.equ	12  	; system call table (sysent.c) index

	.import cerror

_chdir::
	addr	@chdir,r0  
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
