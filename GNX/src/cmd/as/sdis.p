{!L}
module sdis;
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

const SCCSId = Addr('@(#)sdis.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'scantoken.e'; ! DEPENDENCY
import 'scanr.e'; ! DEPENDENCY
import 'scan.e'; ! DEPENDENCY
import 'scanexp.e'; ! DEPENDENCY
import 'genrout.e'; ! DEPENDENCY
import 'traverse.e'; ! DEPENDENCY

export 'sdis.e'; ! DEPENDENCY

import 'disp.e'; ! DEPENDENCY

  const max_shrink_passes = 5000000;
  var
    current_l_ptr, l_chain_start, last_l_ptr : l_ptr; 
    pc_offset, accumulative_increase : integer;  
    sdi_change, reset_cumulative_field, start_of_a_sdi_pass : boolean;
    pass_number:integer; shrink_allowed:boolean;

  export function get_size(var length_attribute : attribute) : integer;

    begin
      if ord(length_attribute.attrib[0]) = ord(b) then 
	get_size := 1
      else if ord(length_attribute.attrib[0]) = ord(w) then 
	get_size := 2
      else get_size := 4;
    end;
procedure doall(tag:integer);
 begin writeln('<<<<<< dump all (',tag:3,') >>>>>>');
  write('sd_st=');
  if sdi_start=nil then write('**') else write(sdi_start^.unique:2);
  write(' s_c_s=');
  if sdi_chain_start=nil then write('**') else write(sdi_chain_start^.unique:2);
  write(' cr_sd=');
  if current_sdi=nil then write('**') else write(current_sdi^.unique:2);
  write(' lo_st=');
  if loc_start=nil then write('**') else write(loc_start^.unique:2);
  write(' cr_lo=');
  if current_loc=nil then write('**') else write(current_loc^.unique:2);
  write(' c_l_p=');
  if current_l_ptr=nil then write('**') else write(current_l_ptr^.unique:2);
  write(' l_c_s=');
  if l_chain_start=nil then write('**') else write(l_chain_start^.unique:2);
  write(' l_l_p=');
  if last_l_ptr=nil then write('**') else write(last_l_ptr^.unique:2);
  writeln;
  write('pc_of=',pc_offset:3,' ac_in=',accumulative_increase:2,' sd_ch=');
  if sdi_change then write('T') else write('F');
  write(' r_c_f=');
  if reset_cumulative_field then write('T') else write('F');
  write(' s_o_a_s_p=');
  if start_of_a_sdi_pass then write('T') else write('F');
  writeln;
  disp_l_ptr_list(0,tag);
  disp_sdi_ptr_list(0,tag);
  disp_loc_ptr_list(0,tag);
  disp_irec_list(0,tag);
 end;

  function compare_operands(cur_sdi : sdi_ptr;
			    var new_operand : gen) : integer;
    var new_size : integer;

    begin
      if new_operand.gtype = register then
	new_size := 0 
      else if new_operand.gtype = regrelative then
	new_size := get_size(new_operand.regreloffatt)
      else if new_operand.gtype = memoryrelative then
	new_size := get_size(new_operand.memoffatt2) 
                   +get_size(new_operand.memoffatt1)
      else if new_operand.gtype = tos then
	new_size := 0
      else if new_operand.gtype = externalg	then
	begin
	  new_size := get_size(new_operand.extinxatt);
	  if not process_cxp then	
	    new_size := new_size + get_size(new_operand.extoffatt);
	end
      else if new_operand.gtype = absolute then
	new_size := get_size(new_operand.absatt)
      else if new_operand.gtype = immediate	then
	begin
	  if cur_sdi^.immed_bwd.attribstate[0]=chr(ord(true)) then
	      new_size := get_size(cur_sdi^.immed_bwd)
	  else new_size := get_size(new_operand.immatt)
	end
      else if new_operand.gtype = unresolved then
        new_size:=1;
      if (new_size <> cur_sdi^.opsize)
         and (shrink_allowed or(new_size>cur_sdi^.opsize))  then  
	begin 
	  compare_operands := new_size - cur_sdi^.opsize;
	  cur_sdi^.opsize := new_size;
	end
      else compare_operands := 0; 
    end;

  function resolveloc_expr(stck : stackptr) : integer;

    var fresult : factorrec;

    begin
      resolve_expression(stck,fresult);
      if not error_flag then
	if (fresult.factortype = fgen) and
	   (fresult.fgenref.gtype = regrelative) and
	   ((fresult.fgenref.rltype = pc) or(fresult.fgenref.rltype = rpc)) then
	     resolveloc_expr := fresult.fgenref.regreloffset
	else reporterror(100*175+bug_sdi_loc);
    end;

  procedure report_undefined_symbol(result : factorrec);
    var tmp:integer;
    begin
      reporterror(100*176+undefined_symbol);
      write(' Undefined symbol :');
      tmp:=1;
      with a=result.fsymbol^ do
        while a.symbolname^[tmp]<>nilchar do
          begin write(a.symbolname^[tmp]);tmp:=tmp+1
          end;
      writeln
    end;

  export procedure resolve_op_expression(stck : stackptr;
				  var new_operand : gen);
    
    var result : factorrec;

    begin
     ! no_check_att_flag := true ; {ming}
      resolve_expression(stck,result);
     ! no_check_att_flag := false ; {ming}
      if not error_flag then
	begin
	  if result.factortype = fsym then
	    begin
	      report_undefined_symbol(result);
	      new_operand.gtype:=unresolved
	    end
	  else if result.factortype = fgen then
	    begin
	      new_operand := result.fgenref;
	      with np=new_operand do
	        if (np.gtype = regrelative) and (np.rltype = pc) then
	          begin
	            np.regreloffset := np.regreloffset - locationcounter;
	            check_range(np.regreloffatt,np.regreloffset);
	         end
	    end
	  else if result.factortype = fres then
	    reporterror(100*177+bug_sdi)
	  else if result.factortype = fnill then
	    reporterror(100*178+bug_sdi);
	end
      else 
	begin
	  error_flag := false;
	end;
    end;
 
  procedure add_to_l_chain(loc_field : integer;
			   change_in_size : integer;
			   loc_num   : integer);
    var tmp_l : l_ptr;

    begin
      (*doall(1)*);
      if l_chain_start = nil then
	begin
	  new_l_ptr(l_chain_start);
	  current_l_ptr := l_chain_start;
	  current_l_ptr^.next_l := nil;
	end
      else if current_l_ptr = nil then	(* start chain over again, reset *)
	current_l_ptr := l_chain_start;
      with clp=current_l_ptr^ do
	begin
	  clp.loc := loc_field;
	  clp.locdir_number := loc_num;
	  accumulative_increase := accumulative_increase+change_in_size;
	  clp.growth := accumulative_increase;
	end;
      if current_l_ptr^.next_l <> nil then
	current_l_ptr := current_l_ptr^.next_l
      else
	begin
	  new_l_ptr(tmp_l);
	  tmp_l^.next_l := nil;
	  current_l_ptr^.next_l := tmp_l;
	  current_l_ptr := tmp_l;
	end;
      (*doall(2)*)
    end;
	  

  function get_loc_adjustment : integer;

    begin
      get_loc_adjustment := 0;
      if reset_cumulative_field then
	accumulative_increase := 0
      else
	if current_l_ptr <> nil then
	  get_loc_adjustment := accumulative_increase;
    end;

  procedure check_loc_directives(loc_field : integer;
				 sdi_loc_num : integer);

    var temp : integer;
	ok   : boolean;
(* This routine adjusts accumulative_increase because the source contained
   either a loc directive affecting the pc of a storage directive whose
   ultimate length was dependent on the value of one or more pc labels.
   sdi_loc_num refers to a loc type directive which affects the sdi, each
   time a loc type directive was encounter the locdir_number got incremented
   loc_field is the location of the current sdi*)

    begin
      ok := true;
      if loc_start <> nil then			(* any nasty directives *)
	begin
          if start_of_a_sdi_pass then		(* start of a sdi pass *)
	    with ls=loc_start^ do
	    begin
	      resolve_loc_flag := true;
	      pc_offset := resolveloc_expr(ls.loc_value);  
	      resolve_loc_flag := false;
	      (* pc offset is the offset the loc directive thought it should
		 be at	*)
	      
	      start_of_a_sdi_pass := false;
	      current_loc := loc_start;
	    end
          else
	  if current_loc <> nil then
	    while (current_loc^.locdir_number <= sdi_loc_num) and ok do
	      begin	(* is the current sdi within the scope of the current
			   directive *)
	         
	        accumulative_increase := 0;   (* reset accumulative increase *)
					      (* has the location of the 
						 directive changed ? *)
		(* determine any change in directives location *)
	        temp := pc_offset - current_loc^.loc; 
	        if temp <> 0 then
		  begin
		    sdi_change := true;
		  end;
		(* in either case we stop affect how much inst's & sym's get
		   moved *)
	        add_to_l_chain(current_loc^.loc-1,temp,current_loc^.locdir_number);
	        current_loc^.loc := pc_offset;
	        current_loc := current_loc^.next_loc;
	        if current_loc <> nil then
	          begin
	            resolve_loc_flag := true;
	            pc_offset := resolveloc_expr(current_loc^.loc_value);
	            resolve_loc_flag := false;
	          end
	       else 
	         begin 
	           pc_offset := maxint;  ok := false;
	         end
	      end
	end;
  end;

  procedure process_sdis;

    var
	cur_sdi : sdi_ptr;
	new_operand : gen;
	change_in_operand_size : integer;
	last_g_index : irec;
	temp_loc_field : integer;

    begin
      cur_sdi := sdi_chain_start;
      last_g_index := nil;
      while cur_sdi <> nil do
	with cs=cur_sdi^ do
	begin
          (*doall(3)*);
	  locationcounter := cs.loc_field;
	  linenumber:=cs.sdi_linnumber;
	  check_loc_directives(cs.loc_field,cs.locdir_number);
          (*doall(4)*);
	  temp_loc_field := cs.loc_field;
	  if last_g_index <> cs.gen_rec then
	      cs.loc_field := cs.loc_field + accumulative_increase
	  else cs.loc_field := cs.back_sdi^.loc_field;
	  last_g_index := cs.gen_rec;
	  locationcounter := cs.loc_field; 
	  if (cs.immed_bwd.attrib[0] = chr(ord(cx))) or
	     (cs.immed_bwd.attrib[0] = chr(ord(lx))) then
	    process_cxp := true
	  else if (cs.immed_bwd.attrib[0] = chr(ord(cx_d))) then
	    process_d_cxp:=true;
	  resolve_op_expression(cs.operand,new_operand);
	  process_d_cxp:=false;
	  if cs.immed_bwd.attrib[0] <> chr(ord(cx)) then
	    process_cxp := false;
	  change_in_operand_size := compare_operands(cur_sdi,new_operand);
	  process_cxp := false;
	  if change_in_operand_size <> 0 then  
	    begin
	      add_to_l_chain(temp_loc_field,
			     change_in_operand_size,
			     cs.locdir_number);
	      sdi_change := true;
	    end;
	  cur_sdi := cs.next_sdi;
	end;
	if current_loc <> nil then
	  begin
	    check_loc_directives(maxint-1,maxint-1);
	  end;
    end;

    procedure get_l_chain_data(var loc_field : integer;
			       var adjustment : integer;
			       var loc_num : integer);
      
      begin
	if (current_l_ptr <> nil) and (current_l_ptr <> last_l_ptr) then
	    begin
	      loc_field := current_l_ptr^.loc;
	      adjustment := current_l_ptr^.growth;
	      loc_num := current_l_ptr^.locdir_number;
	      current_l_ptr := current_l_ptr^.next_l;
	    end
	else
	  begin
	    loc_field := maxint;
	    adjustment := 0;
	    loc_num := maxint;
	  end;
      end;
	    
    procedure process_symbol_locs;
      
      var start_loc, start_adjustment, sl_cnt : integer;
	  next_loc, next_adjustment, nl_cnt   : integer;
	  g_index : irec;
	  curr_sym : symptr;
	  finished : boolean;

      begin
	finished := false;
	get_l_chain_data(start_loc,start_adjustment,sl_cnt);
	get_l_chain_data(next_loc,next_adjustment,nl_cnt);
	curr_sym := first_pc_symbol;
	if (start_loc = maxint) or (curr_sym = nil) then
	  finished := true;
	while not finished do
	  with csd=curr_sym^.symbol_definition, cs=curr_sym^ do
	  begin
	    if ((csd.regreloffset <= start_loc) and (cs.locdir_number<= sl_cnt))
		or (sl_cnt > cs.locdir_number) then
	      if cs.nextsym <> nil then
		curr_sym := cs.nextsym
	      else finished := true
	    else if (start_loc<csd.regreloffset) and
                     (csd.regreloffset<=next_loc) then
	      begin
		csd.regreloffset := csd.regreloffset + start_adjustment;
		check_range(csd.regreloffatt,csd.regreloffset);
		if cs.nextsym <> nil then
		  curr_sym := cs.nextsym
		else finished := true
	      end
	    else if (start_loc<csd.regreloffset) and
		      (cs.locdir_number< nl_cnt) then
	      begin
		csd.regreloffset := csd.regreloffset + start_adjustment;
		check_range(csd.regreloffatt,csd.regreloffset);
		if cs.nextsym <> nil then
		  curr_sym := cs.nextsym
		else finished := true
	      end
	    else 
	      begin
		start_loc := next_loc;
		start_adjustment := next_adjustment;
		sl_cnt := nl_cnt;
		get_l_chain_data(next_loc,next_adjustment,nl_cnt);
	      end;
	  end;
	current_l_ptr := l_chain_start;
	(* now we take care of the intermediate code records *)
	get_l_chain_data(start_loc,start_adjustment,sl_cnt);
	get_l_chain_data(next_loc,next_adjustment,nl_cnt);
	if start_loc <> maxint then
	  g_index := first_irec;
	  repeat
	    begin
	      if (g_index^.location <= start_loc) and
		 (g_index^.locdir_number <= sl_cnt) then
	        begin
		  g_index := g_index^.next_irec;
                end
	      else if (start_loc<g_index^.location) and
		      (g_index^.location<=next_loc) then
	        begin
		  g_index^.location := g_index^.location
					       + start_adjustment;
		  g_index := g_index^.next_irec;
	        end
	      else if (start_loc<g_index^.location) and
		      (g_index^.locdir_number< nl_cnt) then
	        begin
		  g_index^.location := g_index^.location
					       + start_adjustment;
		  g_index := g_index^.next_irec;
	        end
	      else 
	        begin
		  start_loc := next_loc;
		  start_adjustment := next_adjustment;
		  sl_cnt := nl_cnt;
		  get_l_chain_data(next_loc,next_adjustment,nl_cnt);
	        end;
	    end;
	  until g_index = nil;
      end;

      export procedure resolve_sdis;

    ! Using the symbol table and the chains generated by the first phase,
    ! at the end of this pass the location of all the instructions is fixed.

       begin
	 l_chain_start := nil;
	 sdi_chain_start := sdi_start;
	 current_l_ptr := nil;
	 repeat
	   (*doall(0)*);
	   pass_number:=pass_number+1;
	   shrink_allowed:=true;!pass_number<=max_shrink_passes);
	   accumulative_increase := 0;
	   sdi_change := false;
	   start_of_a_sdi_pass := true;
	   process_sdis;
	   last_l_ptr := current_l_ptr;
	   current_l_ptr := l_chain_start;
	   if sdi_change then
	     begin
  	       process_symbol_locs;
	       last_l_ptr := current_l_ptr;
	       current_l_ptr := l_chain_start;
	     end;
	 until not sdi_change;
       end;

    begin
      ! no_check_att_flag := false; {ming}
      sdi_start := nil;
      loc_start := nil;
      resolve_loc_flag := false;
      l_chain_start := nil;
      pc_offset := 0;
      pass_number:=0;
    end.

