;
; getfsr.S: version 1.1 of 3/23/83
; 
;
; @(#)getfsr.S	1.1 (NSC) 3/23/83
;
;Read the floating point status register

_getfsr::
	sfsr	r0
	rxp	0
