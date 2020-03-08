
! @(#)glob_const.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	global_const.x

  const

  blank = ' ';
  nilchar = chr(0);
  eol = chr(10);
  maxsymlength = 32;
  maxlinelength= 80;
  maxinstlen = 8;
  maxdirlen = 10;
  maxrtlen = 8;
  maxdirs = 50;
  maxinsts = 379;
  maxtokens = 44;
  maxmodes = 16;
  maxblocks = 50;
  max_float_length=80;

  (* error codes *)
  success       = 0;
  duplicate_def = 1;
  invalidlabel  = 2;
  illegalinst   = 3;
  symtoolong	= 4;
  integerexpected = 5;
  invalidchar 	= 6;
  asmbugdupsym  = 7;
  multiplydefined = 8;
  invaliddir = 9;
  illegal_term = 10;
  illegal_expression = 11;
  termsizeextreme = 12;
  unequallengths = 13;
  nesting = 14;
  no_more_memory = 15;
  overwrite = 16;
  illegal_mode = 17;
  invalidopcode = 18;
  bug_sdi = 19;
  bug_sdi_comp = 20;
  bug_sdi_loc = 21;
  undefined_symbol = 22;
  illegal_str_op = 23;
  offset_too_large = 24;
  missing_end_proc = 25;
  missing_begin = 26;
  missing_var = 27;
  label_required = 28;
  integer_too_large = 29;
  returns_too_long = 30;
  nomoresymbols = 31;
  float_too_long = 32;
  exponent_missing = 33;
  float_syntax_error = 34;
  float_too_large = 35;
  float_expected = 36;
  even_register_expected = 37;
  float_expression_found = 38;
  unresolved_float_const = 39;
  float_not_allowed = 40;
  mode_not_progrel = 41;
  invalid_external = 42; 
  string_too_long = 43;
  type_info_error = 44;
  nested_proc = 45;
  float_too_big = 46;
  source_mode =47;
  missing_filename = 48;
  align_error = 49;
  jump_out_of_mode = 50;
  sdi_repeat_count = 51;
  need_a_positive = 52;
  length_missing = 53;
  dangling_stab = 54;
  missing_type_info = 55;
  expand_error = 56;
  wrong_cxp = 57;
  back_only=58;
  illegal_packed=59;
  exceed_4_byte=60;

  max_error_code=60;

  first_list_char=chr(9);
  second_list_char='$';
  third_list_char='%';

  asm_title = ' NSC 16032 MESA ASSEMBLER Version 3.31    9/19/83  # ';
  lines_per_page = 60;

routine global_consts; forward;

