;
; access.S: version 1.2 of 9/23/82
; Mesa Unix C Library Source File
;
; @(#)access.S	1.2 (NSC) 9/23/82
;

; access(file, request)
;  test ability to access file in all indicated ways
;  1 - read
;  2 - write
;  4 - execute

access:	.equ	33

	.import cerror

_access::
	addr	@access,r0
	addr	8(sp),r1
	svc
	bfc 	done
	cxp 	cerror
done:	rxp	0
