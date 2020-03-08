;
; ptrace.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)ptrace.S	1.1 (NSC) 9/3/82
;

; ptrace -- C library

;	result = ptrace(req, pid, addr, data);

ptrace: .equ	26
	
	.import cerror
	.import _errno

_ptrace::
	addr	@ptrace,r0
	addr	8(sp),r1
	movqd	0,_errno
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
