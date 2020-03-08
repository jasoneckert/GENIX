;
; abs.S: version 1.2 of 9/3/82
; Mesa Unix C Library Source
;
; @(#)abs.S	1.2 (NSC) 9/3/82
;

;;
;;	int abs (x)
;;		int x;
;;	{	if (x < 0)
;;			return (-x);
;;		return (x);
;;	}
;;
_abs::
	absd	8(sp),r0
	rxp	0
