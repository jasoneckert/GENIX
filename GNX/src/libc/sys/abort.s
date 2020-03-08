;
; abort.S: version 1.3 of 2/3/83
; Mesa Unix C Library Source File
;
; @(#)abort.S	1.3 (NSC) 2/3/83
;

;# C library -- abort

_abort::
	enter	0	;give more informative stack trace
	bispsrb	h'020	;set flag bit
	flag		;cause a flag trap
	movqd	0,r0	;zero return value
	rxp	0	;return
