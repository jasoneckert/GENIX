{!L}
MODULE float_mod;

(*
 * Copyright (C) 1982 by National Semiconductor Corporation
 *
 *	National Semiconductor Corporation
 *	2900 Semiconductor Drive
 *	Santa Clara, California 95051
 *
 *	All Rights Reserved
 *
 * This software is furnished under a license and may be used and copied only
 * in accordance with the terms of such license and with the inclusion of the
 * above copyright notice.  This software or any other copies thereof may not
 * be provided or otherwise made available to any other person.  No title to
 * or ownership of the software is hereby transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by National Semiconductor
 * Corporation.
 *
 * National Semiconductor Corporation assumes no responsibility for the use
 * or reliability of its software on equipment configurations that are not
 * supported by National Semiconductor Corporation.
 *)

const SCCSId = Addr('@(#)float.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; !DEPENDENCY
import 'formatsmod.e'; !DEPENDENCY
import 'glob_types.e'; !DEPENDENCY
import 'scan.e'; !DEPENDENCY

export 'float.e'; !DEPENDENCY

const
    max_10 = 307;
    min_10 = - 308;
    s_max_10 = 38;
    s_min_10 = - 38;
    bias = 1023;
    s_bias = 127; { short floating point bias}
    msb = 59;
    bit_59 = 27;
    bit_60 = 28;
    bit_63 = 31;
    norm_bit_off = 4;
    bit_31_mask = minint; { -2**31 }
    mant_mask = 1048575; { 2**20 -1 }
    imp_bit = 1048576; { H'100000 }
    expn_mask = 2146435072; { H'7FF00000 }
    s_mant_mask = 8388607; { 2**23 -1 short mantissa mask}
    s_expn_mask = 2139095040; { H'7F800000}
    short = 0;
    fl_long = 1;

  TYPE


    format = integer;
    base_10_range = min_10..max_10;
    base_2_range = doub_wrd; { 0..2047; }
    s_base_2_range = doub_wrd; { 0..255 }
    sign_type = (plus, minus);
    line_lngth = 1..max_float_length;
    fl_byte = - 128..127;

  VAR
    f_field, m_temp, temp, r_num, shf: quad_wrd;
    s_field: doub_wrd;
    pstr: tp_string;
    ch: char;
    i, chr_cnt: line_lngth;
    do_move: boolean;
    shf_cnt, offset, dig_val, dig_cnt, sticky, smooth, comp: integer;
    form: format;
    error: boolean;
    r_sign, e_sign: sign_type;
    b_10_expn: base_10_range;
    b_2_expn: base_2_range;
    s_b_2_expn: s_base_2_range;

export function make_short;
  var n,m:integer;
  begin 
    n:=0;
    m:=ext(a.d_hi,31, 1);ins(n,31, 1,m);
    m:=ext(a.d_hi, 0,20);ins(n, 3,20,m);
    m:=ext(a.d_lo,29, 3);ins(n, 0, 3,m);
    m:=ext(a.d_hi,20,11);
    if m<>0 then
      begin m:=m-896;
        if (m<1) or (m>254) then 
          begin m:=254;
            reporterror(100*077+float_too_big) 
          end
      end;
    ins(n,23,8,m);
    make_short:=n
  end;


  FUNCTION d_tbit(bit_field: doub_wrd;
		  bit_num: fl_byte): boolean;

    !test bit in word.
    BEGIN {D_TBIT}
      d_tbit := ((bit_field >> bit_num) & 1)=1;
    END {D_TBIT} ;


  FUNCTION d_add(add_val1, add_val2: doub_wrd;
		 VAR c_flag: boolean): doub_wrd;

    !unsigned add with carry.
    VAR
      sign1, sign2: boolean;
      result: doub_wrd;

    BEGIN {D_ADD}
      sign1 := add_val1 < 0;
      sign2 := add_val2 < 0;
      result := add_val1 + add_val2;
      c_flag := ((sign1 AND sign2) OR ((sign1 OR sign2) AND
		(result >= 0)));
      d_add := result;
    END {D_ADD} ;


  PROCEDURE d_shf(VAR bit_field: doub_wrd;
		  shf_val: fl_byte);

    !logical left shift - shf_val may be negative to get right shift.
    temp: integer;
    BEGIN {D_SHF}
      temp:=0;
      if shf_val >= 0 then begin
	! really is a left shift.
	ins(temp, shf_val, 32-shf_val, bit_field);
      end
      else begin
	!really a right shift.
	temp:=ext(bit_field, -shf_val, 32+shf_val);
      end;
      bit_field:=temp;
    END {D_SHF} ;


  PROCEDURE d_and(VAR bit_field: doub_wrd;
		  mask: doub_wrd);

    BEGIN {D_AND}
      bit_field := bit_field & mask;
    END {D_AND} ;


  PROCEDURE d_or(VAR bit_field: doub_wrd;
		 mask: doub_wrd);

    BEGIN {D_OR}
      bit_field := bit_field | mask;
    END {D_OR} ;


  FUNCTION d_unsdiv_10(dividend: doub_wrd): doub_wrd;

    BEGIN {D_UNSDIV_10}
      d_unsdiv_10 := ext(dividend,1,31) DIV 5;
    END {D_UNSDIV_10} ;


  FUNCTION d_unsmod_10(dividend: doub_wrd): doub_wrd;

    BEGIN {D_UNSMOD_10}
      d_unsmod_10 := dividend - d_unsdiv_10(dividend) * 10;
    END {D_UNSMOD_10} ;


  PROCEDURE q_div_10(VAR dividend: quad_wrd);

    CONST
      max_div_10 = 429496729; {2**32 div 10}
      max_mod_10 = 6; {2**32 mod 10}

    VAR
      d_temp1, d_temp2: doub_wrd;
      q_temp: quad_wrd;

    BEGIN {Q_DIV_10}
      q_temp.d_hi := d_unsdiv_10(dividend.d_hi);
      q_temp.d_lo := d_unsdiv_10(dividend.d_lo);
      d_temp1 := d_unsmod_10(dividend.d_hi);
      d_temp2 := d_temp1 * max_div_10;
      d_temp1 := d_unsdiv_10((max_mod_10 * d_temp1) + d_unsmod_10(
			    dividend.d_lo));
      q_temp.d_lo := q_temp.d_lo + d_temp1 + d_temp2;
      dividend := q_temp;
    END {Q_DIV_10} ;


  PROCEDURE q_add(VAR f_field, add_val: quad_wrd);

    VAR
      c_flag: boolean;

    BEGIN {Q_ADD}
      WITH f_field DO
	BEGIN {add}
	d_lo := d_add(d_lo, add_val.d_lo, c_flag);
	IF c_flag THEN
	  d_hi := d_hi + 1;
	d_hi := d_add(d_hi, add_val.d_hi, c_flag);
	END {add} ;
    END {Q_ADD} ;


  PROCEDURE q_sub(VAR f_field, sub_val: quad_wrd);

    VAR
      c_flag: boolean;

    BEGIN {Q_SUB}
      WITH f_field DO
	BEGIN {add}
	d_lo := d_add(d_lo, ~ sub_val.d_lo, c_flag);
	IF c_flag THEN
	  d_hi := d_hi - 1;
	d_hi := d_add(d_hi, ~ sub_val.d_hi, c_flag);
	END {add} ;
    END {Q_SUB} ;


  PROCEDURE q_shf(VAR f_field: quad_wrd;
		  shf_val: fl_byte);

    LABEL
      1;

    BEGIN {Q_SHF}
      IF shf_val = 0 THEN
	GOTO 1;
      shf.d_lo := f_field.d_lo;
      shf.d_hi := f_field.d_hi;
      d_shf(f_field.d_lo, shf_val);
      d_shf(f_field.d_hi, shf_val);
      IF shf_val < 0 THEN
	BEGIN
	shf_val := 32 + shf_val;
	d_shf(shf.d_hi, shf_val);
	d_or(f_field.d_lo, shf.d_hi);
	END
      ELSE
	BEGIN
	shf_val := shf_val - 32;
	d_shf(shf.d_lo, shf_val);
	d_or(f_field.d_hi, shf.d_lo);
	END;
    1: ;
    END; {Q_SHF}


  PROCEDURE q_mul_10;

    BEGIN {Q_MUL_10}
      temp.d_lo := f_field.d_lo;
      temp.d_hi := f_field.d_hi;
      q_shf(f_field, 3); { mul X 8 }
      q_shf(temp, 1); { mul X 2 }
      q_add(f_field, temp);
    END; {Q_MUL_10}


  PROCEDURE q_mod_10;

    BEGIN {Q_MOD_10}
      m_temp.d_lo := f_field.d_lo;
      m_temp.d_hi := f_field.d_hi;
      q_div_10(f_field);
      q_mul_10;
      q_sub(m_temp, f_field);
      dig_val := m_temp.d_lo;
    END; {Q_MOD_10}


  FUNCTION digit: integer;

    VAR
      x: integer;

    BEGIN {DIGIT}
      x := ord(ch) - ord('0');
      IF (x < 0) OR (x > 9) THEN
	error := true;
      digit := x;
    END; {DIGIT}


  PROCEDURE move_pntr;

    BEGIN {MOVE_PNTR}
      i := i + 1;
      ch := pstr^[i];
      IF NOT (ch IN ['+', '-', '0'..'9', '.', 'E', 'e']) THEN
	do_move := false;
    END; {MOVE_PNTR}


  PROCEDURE real_conv;

    VAR
      do_offset: boolean;
    too_long: boolean;
    BEGIN {REAL_CONV}
      too_long:=false;
      dig_cnt := 0;
      r_num.d_hi := 0;
      do_offset := false;
      REPEAT
	BEGIN
	IF ch <> '.' THEN
	  BEGIN
	  IF form = short THEN
	    BEGIN
	    IF f_field.d_lo > 9999999 THEN too_long:=true;
	    END
	  ELSE IF f_field.d_hi > 9999999 THEN too_long:=true;
	  IF not too_long THEN
	    BEGIN
	    q_mul_10;
	    r_num.d_lo := digit;
	    q_add(f_field, r_num);
	    IF do_offset = true THEN
	      offset := offset + 1;
	    dig_cnt := dig_cnt + 1;
	    END
	  ELSE IF NOT do_offset THEN b_10_expn := b_10_expn + 1;
	  END
	ELSE
	  BEGIN
	  do_offset := true;
	  END;
	move_pntr;
	END;
      UNTIL (do_move = false) OR (ch IN ['E', 'e']);
    END; {REAL_CONV}


    PROCEDURE adjust;

      VAR
	offset: integer;

      BEGIN {ADJUST}
	FOR offset := 1 TO smooth DO
	  BEGIN
	  q_mod_10;
	  sticky := dig_val * shf_cnt + sticky;
	  shf_cnt := shf_cnt * 10;
	  q_div_10(f_field);
	  b_10_expn := b_10_expn + 1;
	  END;
	temp.d_lo := 1;
	temp.d_hi := 0;
	IF sticky >= comp THEN
	  q_add(f_field, temp);
      END; {ADJUST}


  PROCEDURE  round_off;

    LABEL
      1, 2;

    VAR
      offset: integer;
    BEGIN {ROUND_OFF}
      shf_cnt := 1;
      sticky := 0;
      IF form = short THEN
	BEGIN
	FOR offset := 1 TO 8 DO
	  BEGIN
	  q_div_10(f_field);
	  b_10_expn := b_10_expn + 1;
	  END;
	IF b_10_expn <= - 10 THEN
	  BEGIN
	  smooth := 3;
	  comp := 500;
	  adjust;
	  GOTO 2;
	  END
	ELSE
	  BEGIN
	  IF b_10_expn >= 2 THEN
	    BEGIN
	    smooth := 3;
	    comp := 500;
	    IF b_10_expn >= 10 THEN
	      BEGIN
	      smooth := 4;
	      comp := 5000
	      END;
	    adjust;
	    GOTO 2;
	    END;
	  END;
	GOTO 1;
	END
      ELSE
	BEGIN
	IF b_10_expn <= - 18 THEN
	  BEGIN
	  smooth := 3;
	  comp := 500;
	  adjust;
	  GOTO 2;
	  END
	ELSE
	  BEGIN
	  IF b_10_expn >= 5 THEN
	    BEGIN
	    smooth := 3;
	    comp := 500;
	    IF b_10_expn >= 23 THEN
	      BEGIN
	      smooth := 4;
	      comp := 5000
	      END;
	    adjust;
	    GOTO 2;
	    END;
	  END;
        END;
 1: smooth := 2;
    comp := 9999;
    adjust;
    IF sticky <> 0 THEN
      BEGIN
      smooth := 1;
      comp := 5;
      adjust;
      END;
 2: END; {ROUND_OFF}


  PROCEDURE expn;
    loc_expn: integer;
    BEGIN {EXP}
      loc_expn:=0;
      WHILE do_move = true DO
	BEGIN
	loc_expn := loc_expn * 10 + digit;
	move_pntr;
	END;
      ! this allows the user to specify the expnonent by
      ! placing the decimal point.
      b_10_expn := b_10_expn + loc_expn;
      IF e_sign = minus THEN
       b_10_expn := - b_10_expn;
      IF form = short THEN
	BEGIN
	IF b_10_expn > s_max_10 THEN
	  error := true;
	IF b_10_expn < s_min_10 THEN
	  error := true;
	END
      ELSE
	BEGIN
	IF b_10_expn > max_10 THEN
	  error := true;
	IF b_10_expn < min_10 THEN
	  error := true;
	END;
    END {EXP} ;


  PROCEDURE sign_chk(VAR sign_val: sign_type);

    BEGIN {SIGN_CHK}
      IF ch = '-' THEN
	BEGIN
	sign_val := minus;
	move_pntr
	END
      ELSE IF ch = '+' THEN
	move_pntr;
    END {SIGN_CHK} ;


  PROCEDURE normalize;

    BEGIN {NORMALIZE}
      shf_cnt := 0;
      WHILE (f_field.d_hi & bit_31_mask) = 0 DO
	BEGIN
	q_shf(f_field, 1); { shift left }
	shf_cnt := shf_cnt + 1;
	END;
      q_shf(f_field, - norm_bit_off); { shift right }
      offset := norm_bit_off - shf_cnt;
    END; {NORMALIZE}


  export PROCEDURE asc_2_real(pstring:        tp_string;
                            long_format  :  boolean;
		            VAR real_form:  quad_wrd;
		            VAR err:        boolean);

    LABEL
      1, 2;

    VAR
      x: integer;

    BEGIN {ASC_2_REAL}
      IF long_format THEN
	form := fl_long
      ELSE
	form := short;
      pstr := pstring;
      b_10_expn := 0;
      b_2_expn := 0;
      r_sign := plus;
      e_sign := plus;
      error := false;
      offset := 0;
      f_field.d_lo := 0;
      f_field.d_hi := 0;
      real_form.d_lo := 0;
      real_form.d_hi := 0;
      s_field := 0;

      i := 1;
      ch := pstr^[i];
      do_move := true;
      sign_chk(r_sign);
      real_conv;

      IF do_move = false THEN
	GOTO 1;
      move_pntr;

      sign_chk(e_sign);
      expn;

    1:
      IF (f_field.d_lo=0)  and (f_field.d_hi = 0) THEN
	GOTO 2;
      b_10_expn := b_10_expn - offset;

{
		This section does the initial conversion of a 
		real number to a normalized fraction
}

      shf_cnt := 0;
      WHILE d_tbit(f_field.d_hi, bit_59) = false DO
	BEGIN
	q_shf(f_field, 1);
	shf_cnt := shf_cnt + 1;
	END;
      offset := msb - shf_cnt;

      b_2_expn := b_2_expn + offset;

{
		This converts the base 10 factor
		to the base 2 factor
}

      WHILE b_10_expn <> 0 DO
	BEGIN
	IF b_10_expn > 0 THEN
	  BEGIN
	  q_mul_10;
	  b_10_expn := b_10_expn - 1;
	  normalize;
	  b_2_expn := b_2_expn + offset;
	  END
	ELSE
	  BEGIN
	  q_div_10(f_field);
	  b_10_expn := b_10_expn + 1;
	  normalize;
	  b_2_expn := b_2_expn + offset;
	  END;
	END;

      offset := b_2_expn;
      b_2_expn := b_2_expn + bias;
      s_b_2_expn := offset + s_bias;
      s_field := f_field.d_hi;

      d_shf(b_2_expn, 20); { adjust the expnonent field }
      q_shf(f_field, - 7); { adjust the mantissa field }
      d_and(f_field.d_hi, mant_mask); { strip the implicit bit }
      d_or(f_field.d_hi, b_2_expn); { insert expnonent field }
      IF r_sign = minus THEN
	f_field.d_hi:= f_field.d_hi | bit_31_mask;

      d_shf(s_b_2_expn, 23); { adjust the short expnonent field }
      d_shf(s_field, - 4); { adjust the short mantissa field }
      d_and(s_field, s_mant_mask); { strip the implicit bit }
      d_or(s_field, s_b_2_expn); { insert short expnonent field }
      IF r_sign = minus THEN
	s_field:= s_field | bit_31_mask;
      IF form = fl_long THEN
	BEGIN
	real_form.d_lo := f_field.d_lo;
	real_form.d_hi := f_field.d_hi;
	END
      ELSE
	BEGIN
	real_form.d_lo := s_field;
	real_form.d_hi := 0;
	END;
    2: ;
      err := error;
    END; {ASC_2_REAL}

{***************************************************************************}

  BEGIN {module float}
  END {module float} .
