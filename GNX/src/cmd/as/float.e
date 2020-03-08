
! @(#)float.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

  function make_short(a:quad_wrd):integer;forward;

  PROCEDURE asc_2_real(pstring:        tp_string;
		     long_format:    boolean;
		     VAR real_form:  quad_wrd;
		     VAR err:        boolean); FORWARD;

  ROUTINE float_mod; FORWARD;
