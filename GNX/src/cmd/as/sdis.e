
! @(#)sdis.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	sdis.x

  type

    sdi_ptr = ^sdi_entry;
    sdi_entry = record
      loc_field	: integer;
      operand	: stackptr;
      opsize	: integer;
      opnum	: integer;
      gen_rec	: irec;
      next_sdi	: sdi_ptr;
      back_sdi	: sdi_ptr;
      sdi_chain : sdi_ptr;
      locdir_number: integer;		(* current loc dir count *)
      immed_bwd : attribute;
      unique    :integer;
      sdi_linnumber:integer;
    end;

    l_ptr = ^l_entry;		(* location table, actually chain *)
    l_entry = record
      loc	: integer;
      growth	: integer;
      next_l	: l_ptr;
      locdir_number: integer;		(* current loc dir count *)
      unique    :integer;
    end;

    loc_ptr = ^loc_entry;
    loc_entry = record
      loc	: integer;
      loc_value	: stackptr;
      next_loc	: loc_ptr;
      locdir_number: integer;		(* current loc dir count *)
      unique    :integer;
    end;


  var
      
    resolve_loc_flag : boolean;
    !no_check_att_flag: boolean; (* sdi without check attribute flag  *)

    sdi_start        : sdi_ptr;		(* the beginning of all sdi's *)
    sdi_chain_start  : sdi_ptr;	(* chain of remaining sdi's only *)
    current_sdi      : sdi_ptr;
    loc_start, current_loc : loc_ptr;

  function get_size(var length_attribute : attribute) : integer; forward;

  procedure resolve_op_expression(stck : stackptr;
				  var new_operand : gen); forward;

  procedure resolve_sdis; forward;

  routine sdis; forward;
