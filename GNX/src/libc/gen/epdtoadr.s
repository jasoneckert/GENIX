;
; epdtoadr.S: version 1.1 of 2/27/83
; 
;
; @(#)epdtoadr.S	1.1 (NSC) 2/27/83
;

;Routine to convert an external procedure descriptor to an absolute address.
;Call from C:
;	addr = pdtoadr(desc);

_epdtoadr::
	movzwd	10(sp),r0	;extract offset
	movzwd	8(sp),r1	;extract mod
	addd	8(r1),r0	;add program base to offset
	rxp	0		;return absolute address
