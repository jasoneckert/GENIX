;
; move.s: version 1.2 of 11/10/82
; Mesa Unix libPW Source
;
; @(#)move.s	1.2 (NSC) 11/10/82
;

;;
;;	Assembly language block move routine.  No assumptions made
;;	about data alignment or length, so we have to use the movsb
;;	string instruction, instead of the faster movsd string
;;	instruction.
;;
;;	called by:	move (src, dst, length);
;;
_move::
	enter	[r2],0
	movd	20(fp),r0	; length
	movd	12(fp),r1	; src
	movd	16(fp),r2	; dst
	movsb
	exit	[r2]
	rxp	0
