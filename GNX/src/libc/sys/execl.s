;
; execl.S: version 1.2 of 10/9/82
; Mesa Unix C Library Source File
;
; @(#)execl.S	1.2 (NSC) 10/9/82
;

; C library -- execl
;
;  execl(file, arg1, arg2, ... , 0);

	.import cerror
	.import _execv

_execl::
	addr	12(sp),tos	;move addr of args to stack
	movd	12(sp),tos	;copy addr of file string to stack
	cxp	_execv		;let execv do the hard stuff
	adjspb	-8		;failed, remove arguments
	rxp	0		;return to user, error intact in r0
