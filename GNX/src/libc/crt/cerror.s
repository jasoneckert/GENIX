;
; cerror.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)cerror.S	1.1 (NSC) 9/3/82
;

; C library - System call return sequence.
;
;   R0 -> errno  
;   -1 -> R0
                
cerror::
	movd	r0,_errno
	movqd	-1,r0
	rxp	0

	.static
_errno::.double	0
        .endseg
