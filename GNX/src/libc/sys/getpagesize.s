;
; getpagesize.S: version 1.1 of 5/11/83
; 
;
; @(#)getpagesize.S	1.1 (NSC) 5/11/83
;

; C library -- getpagesize

; value = getpagesize();
;

getpagesize:	.equ	72  	; system call table (sysent.c) index

	.import cerror

_getpagesize::
	addr	@getpagesize,r0  
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
