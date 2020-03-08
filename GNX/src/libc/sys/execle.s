;
; execle.S: version 1.1 of 10/9/82
; 
;
; @(#)execle.S	1.1 (NSC) 10/9/82
;

; C library -- execle
;
;  execle(file, arg1, arg2, ... , 0, envp);

	exece:	.equ	59	;system call index

	.import cerror

_execle::
	addr	12(sp),r0	;point at first entry of argument list
loop:	cmpqd	0,0(r0)		;check for end of list
	addqd	4,r0		;advance to next entry of list
	bne	loop		;loop until done with argument list
	movd	0(r0),tos	;put environment pointer on stack
	addr	16(sp),tos	;move addr of args to stack
	movd	16(sp),tos	;copy addr of file string to stack
	addr	@exece,r0	;get svc index
	addr	0(sp),r1	;point at arguments
	svc			;execute the program
	cxp	cerror		;only get here on failure
	adjspb	-12		;remove arguments
	rxp	0		;return to user, error intact in r0
