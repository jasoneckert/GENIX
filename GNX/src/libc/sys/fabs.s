
;
; fabs.S: version 1.1 of 4/12/83
; 
;
; @(#)fabs.S	1.1 (NSC) 4/12/83
;

;;
;;	return the absolute value |x|
;;
;;	double fabs(x)
;;	double x;
;;

_fabs::
	absl	8(fp),f0
	rxp	0
