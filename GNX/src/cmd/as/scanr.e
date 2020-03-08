
! @(#)scanr.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	scanr.x

  var

  labelptr : symptr;

  procedure reset_estack; forward;

  procedure scan_const_expr(var result : factorrec); forward;

  procedure scan_const(var temp : displacement); forward;

  procedure scan_immed_expr(var temp : displacement); forward;

  procedure scanlabel(ltoken : token;t_inf:type_info_ptr); forward;

  procedure scan_type_info(var t_inf:type_info_ptr);forward;

  procedure scan_label_operand(var labelop : gen); forward;
    
  procedure scan_ext_operand(var refgen : gen); forward;
    
  procedure scan_reg_list(var gen_regs : pchar {regset}); forward;

  procedure scancomment; forward;

  procedure scan_short_operand(var refgen : gen); forward;

  procedure scan_general_expr(var operand : gen); forward;
    
  procedure scan_preg(var proc_reg : pchar {preglist}); forward;

  procedure scan_cmfi(var temp : pchar {cfmi_set}); forward;

  procedure scan_const_operand(var refgen : gen); forward;

  function scan_reg : reglist; forward;

  function scan_freg : freglist; forward;

  procedure scan_mreg(var memreg : pchar {mreglist}); forward;

  function scanglobsymbol : symptr; forward;

  procedure scan_equ_operand(var refgen : gen); forward;

  routine scanr; forward;

