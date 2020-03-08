;
; pipe.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)pipe.S	1.1 (NSC) 9/3/82
;

; pipe -- C library

;	pipe(f)
;	int f[2];

pipe:	.equ	42

	.import cerror

_pipe::
	addr	@pipe,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
	rxp	0
noerror:
	movd	r0,0(8(sp))
	movd	r1,4(8(sp))
	movqd	0,r0
	rxp	0
