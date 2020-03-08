;
; vtimes.S: version 1.2 of 1/26/83
; Mesa Unix C Library Source File
;
; @(#)vtimes.S	1.2 (NSC) 1/26/83
;

; C library -- times
; 	vtimes(par_vm, ch_vm)
;	struct vtimes *par_vm, *ch_vm;

vtimes:	.equ	64

	.import	cerror

_vtimes::
	addr	@vtimes,r0
	addr	8(sp),r1
	svc
	bfc 	noerror
	cxp 	cerror
noerror:
	rxp	0
