;
; setfsr.S: version 1.1 of 3/23/83
; 
;
; @(#)setfsr.S	1.1 (NSC) 3/23/83
;
;Set the floating point status register.  Returns the old value.

_setfsr::
	sfsr	r0			;read current value
	lfsr	8(sp)			;set new one
	rxp	0			;return old value
