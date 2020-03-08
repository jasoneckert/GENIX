
! @(#)insts.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	insts.x

  type

    mnstart_def= packed array[61..123] of integer;

  var
    opstart    : mnstart_def;

  procedure check_operand(var gref : gen;
			  var opsize : integer;
			  g_index : irec;
			  opnum	  : integer;
			  var op_bwd : attribute); forward;


  procedure process_instruction(var temptoken : token); forward;

  routine insts; forward;
    
