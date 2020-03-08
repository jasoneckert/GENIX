{!L}
{!B}
module codegen;
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

const SCCSId = Addr('@(#)codegen.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e';  !DEPENDENCY
import 'formatsmod.e';  !DEPENDENCY
import 'glob_types.e';  !DEPENDENCY
import 'glob_vars.e';  !DEPENDENCY
import 'scanexp.e';  !DEPENDENCY
import 'scan.e';  !DEPENDENCY
import 'sdis.e';  !DEPENDENCY
import 'genrout.e';  !DEPENDENCY
import 'bio.e';  !DEPENDENCY
import 'scantoken.e';  !DEPENDENCY
import 'init1.e';  !DEPENDENCY
import 'list_types.e';  !DEPENDENCY
import 'dirs.e';  !DEPENDENCY
import 'stab.e';  !DEPENDENCY
import 'cmpc.e';  !DEPENDENCY
import 'float.e';  !DEPENDENCY
import 'disp.e';  !DEPENDENCY


export 'codegen.e';  !DEPENDENCY

    type tmptypes = (tmpchar,tmpint);
	 sym_ent_type = record
	   n_nm : integer;
	   n_ty : integer;
	   n_v  : integer;
	  end;
    var tmprec : record
	  case tmptypes of
	    tmpchar : (tchar : packed array[1..4] of char);
	    tmpint  : (tint  : integer);
	end;
        sevenfffffff : integer;
	field_byte,field_bit_count:integer;
	ex_i,ex_j:integer;
	bwdtab:packed array[1..4]of char;
        expansion_tab:packed array[2..8,0..8] of char;
				(* gives required size of attribute one, if
				   a pair of them are to be expanded to the
				   second index. first index maps pair *)


  import procedure flush_bytes;forward;
  import procedure gen_byte(byte_value : integer);forward;
  import procedure gen_word(value : integer);forward;
  import procedure gen_doub(value : integer);forward;
  import procedure gen_sym_ent(sym:sym_ent_type);forward;


  

  function check_sdi(var tmp_sdi : sdi_ptr;
			 g_index : irec;
			 op_num  : integer) : boolean;
    begin	! check sdi_chain for an entry for this instruction.
      check_sdi := false;
      if current_sdi <> nil then
        if (current_sdi^.gen_rec = g_index) and
	   (current_sdi^.opnum = op_num) then
	  begin
	    tmp_sdi := current_sdi;
	    check_sdi := true;
	    current_sdi := current_sdi^.sdi_chain; ! if found, step to the next entry
          end;
    end;

  import procedure gen_word_const(value : integer);forward;
  import procedure gen_doub_const(value : integer);forward;

  procedure check_packed_no(temp : integer);  {ming}
  var i, temp_st: integer;
    begin
      i := 0;
      repeat
          temp_st := temp;
          if ((temp_st & %0000000f) > 9) then reporterror(100*001+illegal_packed);
          temp := temp >> 4;
          i := i + 1;
      until (i = 8) or (error_flag = true);
     end;
       
  procedure gen_constant(temp_att : attribute;
			 temp : integer);
    
    begin
     if ord(temp_att.attrib[0]) = ord(b) then 
       gen_byte(temp)
     else if ord(temp_att.attrib[0]) = ord(w) then 
       gen_word_const(temp)
     else if ord(temp_att.attrib[0]) = ord(d) then
       begin
         gen_word_const(temp >> 16);
         gen_word_const(temp & %ffff);
       end;
    end;

 procedure gen_float_immediate(oper :gen;is_long:boolean);
    var fl_num:integer;
    
   begin 
     if is_long then
       begin gen_doub_const(oper.imfloat.d_hi);
         gen_doub_const(oper.imfloat.d_lo);
       end 
     else
       begin fl_num:=make_short(oper.imfloat);
         gen_doub_const(fl_num)
       end
    end;

  procedure gen_displacement(temp_att : attribute;
			     temp     : integer);
    
    begin
      if temp < 0 then
	begin
	  if ord(temp_att.attrib[0]) = ord(b) then
	    gen_byte(temp & %7f)
	  else if ord(temp_att.attrib[0]) = ord(w) then
	    gen_word_const(temp & %bfff)
	  else if ord(temp_att.attrib[0]) = ord(d) then begin
	          gen_doub_const(temp);
                 end;
	end
      else
	begin 
	  if ord(temp_att.attrib[0]) = ord(b) then
	    gen_byte(temp)
	  else if ord(temp_att.attrib[0]) = ord(w) then
	    gen_word_const(temp | %8000)
	  else if ord(temp_att.attrib[0]) = ord(d) then begin
	    gen_doub_const(temp | %c0000000);
          end
	end
    end;

  procedure gen_const_displacement(temp : integer);

    begin
      if temp < 0 then
	begin
	  if temp > -65 then   ! changed to -64 from -65
	    gen_byte(temp & %7f)
	  else if temp > -8193 then  !changed from -8193 to -8192
	    begin
	      gen_word_const(temp & %bfff);
	    end
	  else
	    begin
	      gen_doub_const(temp);
	    end;
	end
      else
	begin
	  if temp < 64 then
	    gen_byte(temp)
	  else if temp <= 8191 then
	    begin
	      gen_word_const(temp | %8000);
	    end
	  else (* we have a double *)
	    begin
	      gen_doub_const(temp | %c0000000);
	    end;
	end;
    end;

  procedure gen_regs(var gen_regs : pchar {regset});
    
    var byte : integer;
	shift: integer;
	regn : reglist;

    begin
      byte := 0;
      shift := 0;
      for regn := r0 to r7 do
	begin
	  if (ord(gen_regs[0]) & (%80 >> shift)) <> 0 then
	    byte := byte + (1 << shift);
	  shift := shift + 1;
	end;
      gen_byte(byte);
    end;

  function get_bwd(var temp_bwd : pchar {bwd}) : integer;

    begin
      if ord(temp_bwd[0]) = ord(b) then get_bwd := 0
      else if ord(temp_bwd[0]) = ord(w) then get_bwd := 1
      else if ord(temp_bwd[0]) = ord(d) then get_bwd := 3;
    end;

  function get_cond(var temp_cond : pchar {cond}) : integer;

    begin
      case ord(temp_cond[0])::cond of
        r  : get_cond := 14;
        fc : get_cond := 9;
        fs : get_cond := 8;
        cc : get_cond := 3;
        cs : get_cond := 2;
        ne : get_cond := 1;
        eq : get_cond := 0;
        lt : get_cond := 12;
        gt : get_cond := 6;
        le : get_cond := 7;
        ge : get_cond := 13;
        lo : get_cond := 10;
        hi : get_cond := 4;
        ls : get_cond := 5;
        hs : get_cond := 11;
      end;
    end;

  function get_addr_mode(refgen : gen) : integer;

    begin
      with refgen do
      case gtype of
        register       : case rtype of
		            gregs : case ord(reg[0])::reglist of
				      r0 : get_addr_mode := 0;
				      r1 : get_addr_mode := 1;
				      r2 : get_addr_mode := 2;
				      r3 : get_addr_mode := 3;
				      r4 : get_addr_mode := 4;
				      r5 : get_addr_mode := 5;
				      r6 : get_addr_mode := 6;
				      r7 : get_addr_mode := 7;
				    end;
		            fregs : case ord(freg[0])::freglist of
				      f0 : get_addr_mode := 0;
				      f1 : get_addr_mode := 1;
				      f2 : get_addr_mode := 2;
				      f3 : get_addr_mode := 3;
				      f4 : get_addr_mode := 4;
				      f5 : get_addr_mode := 5;
				      f6 : get_addr_mode := 6;
				      f7 : get_addr_mode := 7;
				    end;
			 end;
        regrelative    : case rltype of
		            rn : case ord(rnum[0])::reglist  of
				   r0 : get_addr_mode := 8;
				   r1 : get_addr_mode := 9;
				   r2 : get_addr_mode := 10;
				   r3 : get_addr_mode := 11;
				   r4 : get_addr_mode := 12;
				   r5 : get_addr_mode := 13;
				   r6 : get_addr_mode := 14;
				   r7 : get_addr_mode := 15;
				 end;
		            fp : get_addr_mode := 24;
			    sp : get_addr_mode := 25;
			    sb : get_addr_mode := 26;
			    pc,rpc : get_addr_mode := 27;
			 end;
        memoryrelative : case ord(memreg[0])::preglist of
			   fp : get_addr_mode := 16;
			   sp : get_addr_mode := 17;
			   sb : get_addr_mode := 18;
			 end;
        tos            :  get_addr_mode := 23;
        externalg      :  get_addr_mode := 22;
        absolute       :  get_addr_mode := 21;
        immediate      :  get_addr_mode := 20;
        unresolved     :  begin
			    reporterror(100*002+undefined_symbol);
			    get_addr_mode := 0;
			  end;
	mod_def	       :  get_addr_mode := %20;
      end;
    end;

  procedure expand_attrib(var a:attribute;opsize:integer);
    begin ! expand operand to prevent empty bytes. needed only if sdi phase
	  ! is prevented from shrinking before completion
      if a.attribstate[0]=chr(ord(true)) then
	begin if opsize<>(1<<ord(a.attrib[0]))
	  then reporterror(100*219+expand_error)
	end
      else if opsize<=4 then a.attrib[0]:=bwdtab[opsize]
      else reporterror(100*220+expand_error)
    end;
	
  procedure expand_pair(var a,b:attribute;opsize:integer);
    var exp1,index:integer;
    begin 
      if a.attribstate[0]=chr(ord(true)) then 
	expand_attrib(b,opsize-(1<<ord(a.attrib[0])))
      else if b.attribstate[0]=chr(ord(true)) then
	expand_attrib(a,opsize-(1<<ord(a.attrib[0])))
      else
	begin
	  index:= 3*ord(a.attrib[0])+ord(b.attrib[0]);
    	  if (index<0)or(index>8) then reporterror(100*222+expand_error)
	  else
	    begin
	      exp1:=ord(expansion_tab[opsize,index]);
	      if exp1>3 then 
		begin reporterror(100*223+expand_error);
		  writeln(opsize,index,exp1);
		  flush(output)
		end
	      else
		begin
	          if exp1<>(1<<ord(a.attrib[0])) then 
	            expand_attrib(a,exp1);
	          if (opsize-exp1)<>(1<<ord(b.attrib[0])) then 
 	            expand_attrib(b,opsize-exp1);
		end
 	    end
	end
    end;

  procedure resolve_sdi_expression(temp_sdi : sdi_ptr;
				   var temp_gen : gen);
    begin
      resolve_op_expression(temp_sdi^.operand,temp_gen);
      (*if temp_gen.gtype = regrelative then
	expand_attrib(temp_gen.regreloffatt,temp_sdi^.opsize)
      else if temp_gen.gtype = memoryrelative then
	expand_pair(temp_gen.memoffatt1,temp_gen.memoffatt2,temp_sdi^.opsize)
      else if temp_gen.gtype = externalg then
	expand_pair(temp_gen.extoffatt,temp_gen.extinxatt,temp_sdi^.opsize)
      else if temp_gen.gtype = absolute then
	expand_attrib(temp_gen.absatt,temp_sdi^.opsize)
      else *)if temp_gen.gtype = immediate then
	if temp_sdi^.immed_bwd.attribstate[0] = chr(ord(true)) then
	  temp_gen.immatt := temp_sdi^.immed_bwd;
    end;

  procedure resolve_dir_expression(g_index : irec; 
				   var value : displacement);
    var result : gen;

    begin
      if (value.disp_type = const_value) and (value.varying) then
	begin
          locationcounter := g_index^.location;
	  resolve_op_expression(value.disp_expr,result);
	  if result.gtype = immediate then
	    value.disp_value := result.imint
	  else reporterror(100*211+illegal_expression)
	end
    end;

  procedure resolve_stab_expression( var value : displacement);
    var result : gen;

    begin
      if (value.disp_type = const_value) and (value.varying) then
	begin
	  resolve_op_expression(value.disp_expr,result);
	  if result.gtype = immediate then
	    value.disp_value := result.imint
	  else reporterror(100*003+illegal_expression)
	end
    end;

  function get_reg(var treg : pchar {reglist}) : integer;

    begin
	get_reg := ord(treg[0])  
       (* assumption: ord's of element of reglist start from 0 and 
                      increment by one *)
    end;

  function get_mreg(var treg : pchar {mreglist}) : integer;

    begin
      case ord(treg[0])::mreglist of
	bpr0 : get_mreg := 0;
	bpr1 : get_mreg := 1;
	pf0  : get_mreg := 4;
	pf1  : get_mreg := 5;
	sca  : get_mreg := 8;
	bc   : get_mreg := 11;
	ptb0 : get_mreg := 12;
	ptb1 : get_mreg := 13;
	msr  : get_mreg := 10;
	eia  : get_mreg := 15;
      end;
    end;

  function get_freg(var treg : pchar {freglist}) : integer;

    begin
      get_freg:=ord(treg[0])
    end;

  function get_scale(scale_ref : scaled;
		     var reg_num : integer) : integer;

    var temp : integer;

    begin
      if scale_ref.scalestate then
	begin
	  if ord(scale_ref.satt[0]) =  ord(b) then 
	    get_scale := %1c
	  else if ord(scale_ref.satt[0]) =  ord(w) then 
	    get_scale := %1d
	  else if ord(scale_ref.satt[0]) =  ord(d) then 
	    get_scale := %1e
	  else if ord(scale_ref.satt[0]) =  ord(q) then 
	    get_scale := %1f;
	  reg_num := get_reg(scale_ref.sreg);
	end
      else get_scale := -1;
    end;

  procedure gen_operand(refgen : gen; g_index: irec);
    var  temp_offset: integer;
    begin
      with refgen do
      if gtype = register then 
	begin end
      else if gtype = regrelative then 
	begin
         if rltype <> rn then begin
              if rltype = pc then temp_offset := regreloffset + g_index^.location
                                else temp_offset := regreloffset; {ming}
              if ((temp_offset > 16777215) or (temp_offset < -16777215 )) then
                 reporterror(1000*100);  {output warning message} {ming}
            end;
	  if regreloffatt.attribstate[0] = chr(ord(true))  then
	     gen_displacement(regreloffatt,regreloffset)
	  else
	    gen_const_displacement(regreloffset);
	end
      else if gtype = memoryrelative then 
	begin
          if ((memoffset1 < -16777215) and (memoffset1 > 16777215))
            then reporterror(1000*100); {warnign message, ming}
          if ((memoffset2 < -16777215) and (memoffset2 > 16777215))
            then reporterror(1000*100); {warnign message, ming}
	  if memoffatt1.attribstate[0] = chr(ord(true)) then
	    gen_displacement(memoffatt1,memoffset1)
	  else gen_const_displacement(memoffset1);
	  if memoffatt2.attribstate[0] = chr(ord(true)) then
	    gen_displacement(memoffatt2,memoffset2)
	  else gen_const_displacement(memoffset2);
	end
      else if gtype = tos then  
	begin end
      else if gtype = externalg then  
	begin
          if (extindex > 4194304) or (extindex < -4194304 ) then
            reporterror(1000*100);  {output warning message} {ming}
          if (extoffset > 16777215) or (extoffset < -16777215 ) then
            reporterror(1000*100);  {output warning message} {ming}
	  if extinxatt.attribstate[0] = chr(ord(true)) then
	    gen_displacement(extinxatt,extindex)
	  else gen_const_displacement(extindex);
	  if extoffatt.attribstate[0] = chr(ord(true)) then
	    gen_displacement(extoffatt,extoffset)
	  else gen_const_displacement(extoffset);
	end
      else if gtype = absolute then  
	begin
          if (regreloffset > 16777215) or (regreloffset < -16777215 ) then
            reporterror(1000*100);  {output warning message} {ming}
	  if absatt.attribstate[0] = chr(ord(true)) then
	    gen_displacement(absatt,absnum)
	  else
	    gen_const_displacement(absnum);
	end
      else if gtype = immediate then  
	begin
	  if numtype = inti then 
	    gen_constant(immatt,imint)
	  else if numtype = floati then begin end;
	end
      else if gtype = unresolved then  
	begin end;
    end;
  
  export procedure gencode(g_index : irec);

    var temp : integer;brerr:integer;
	xx, init_byte_count, temp1, temp2, temp3, reg_num : integer;
	tmp_sdi : sdi_ptr;
	lrec : listrec;
	flp : field_rec_ptr;
        temp_offset: integer;
    
    procedure emitc(size,value:integer); ! used for .field only
      var s1:integer;
      begin 
	if (value<0) or ((size<32)and(value>ext(sevenfffffff,0,size))) then
	  reporterror(100*216+termsizeextreme)
	else
	  while size>0 do
	    begin
	      s1:=8-field_bit_count; if s1>size then s1:=size;
 	      ins(field_byte,field_bit_count,s1,ext(value,0,s1));
	      value:=value>>s1;
	      size:=size-s1;
	      field_bit_count:=field_bit_count+s1;
	      if field_bit_count>=8 then 
	        begin 
		  gen_byte(field_byte);
		  field_bit_count:=0;
                  field_byte:=0; {ming}
	        end
	    end;
      end;
    procedure flushc; ! used for .field only
      begin if field_bit_count>0 then
	gen_byte(field_byte&((1<<(field_bit_count+1))-1))
      end;

    begin (* gencode *)
      linenumber := global_interm_rec.linenumber;
      with global_interm_rec,g_index^ do
      begin
        if location < pc_byte_count then
	  begin
	    if ftype<>last_interm then
	        reporterror(100*004+overwrite)
	  end
	else 
	  begin
	    while location > pc_byte_count do
	      begin
		gen_byte(0);
	      end;
	  end;
        if ((location > 16777215) or (location < 0))
             then reporterror(1000*100); {ming}
        if not error_flag then begin
	  init_byte_count := pc_byte_count;
          case ftype of
            format0 	: begin 
			      if check_sdi(tmp_sdi,g_index,1) then
			        begin
				  locationcounter := location;
				  resolve_sdi_expression(tmp_sdi,blabel);
			      end;
			      brerr:=0;
			      if (bmode[0]=chr(ord(ab)))and
					(blabel.gtype=absolute)
				then begin end
			      else if (bmode[0]=chr(ord(extg)))and
					(blabel.gtype=externalg)
				then begin end
			      else if (blabel.gtype<>regrelative) 
				then brerr:=203 (* reporterror(100*203+?????) *)
			      else
			        case ord(bmode[0])::modetype of
				  progrel: if blabel.rltype<>pc then brerr:=204;
						(* reporterror(100*204+??? ) *)
				  stbsrel: if blabel.rltype<>sb then brerr:=205;
						(* reporterror(100*205+??? ) *)
				  frrel  : if blabel.rltype<>fp then brerr:=206;
						(* reporterror(100*206+??? ) *)
				  stckrel: if blabel.rltype<>sp then brerr:=207;
						(* reporterror(100*207+??? ) *)
			        end;
			    if brerr>0 then 
					reporterror(100*brerr+jump_out_of_mode);
			    gen_byte(get_cond(bcond)*16 + 10);
                            if blabel.rltype = pc then temp_offset:=location+blabel.regreloffset 
                                                  else temp_offset:=blabel.regreloffset; {ming}
                            if ((temp_offset> 16777215) 
                              or (temp_offset < -16777215 )) then
                            reporterror(1000*100);  {output warning message} {ming}
			    if blabel.regreloffatt.attribstate[0] = 
							     chr(ord(true)) then
			      gen_displacement(blabel.regreloffatt,
					       blabel.regreloffset)
			    else gen_const_displacement(blabel.regreloffset);
			  end;
            format1	: begin 
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			       if f1op[0] = chr(ord(CXP)) then
				 process_cxp := true;
			      resolve_sdi_expression(tmp_sdi,f1operand);
			       process_cxp := false;
			      end;
			    case ord(f1op[0])::f1opcodes of
		     	      BSR	: begin
					    gen_byte(2);
					    if(f1operand.gtype=regrelative) and
					     (f1operand.rltype= pc ) then begin
                                             temp_offset:=f1operand.regreloffset+ location; {ming}
                                             if ((temp_offset> 16777215) 
                                               or (temp_offset< -16777215 )) then
                                                 reporterror(1000*100);  {output warning message} {ming}
			    		      gen_const_displacement(
				                         f1operand.regreloffset)
                                              end
					   else reporterror(100*005+illegal_expression);
					  end;

		     	      CXP	: begin
					    gen_byte(34);
					    if(f1operand.gtype = externalg) then
                                             begin if (f1operand.regreloffset > 4194304)
                                                      or (f1operand.regreloffset < -4194304 ) then
                                                 reporterror(1000*100);  {output warning message} {ming}
			    		         gen_const_displacement(
				                             f1operand.extindex)
                                             end
					   else reporterror(100*006+illegal_expression);
					  end;
	             	      RET	: begin
					    gen_byte(18);
					    if(f1operand.gtype = immediate) then
                                             begin if ((f1operand.regreloffset > 16777215) 
                                                     or (f1operand.regreloffset < -16777215 )) then
                                                 reporterror(1000*100);  {output warning message} {ming}
			    		         gen_const_displacement(f1operand.imint);
                                             end
					   else reporterror(100*007+illegal_expression);
					  end;
			      RXP	: begin
					    gen_byte(50);
					    if(f1operand.gtype = immediate) then
                                             begin if (f1operand.regreloffset > 16777215) 
                                                       or (f1operand.regreloffset < -16777215 ) then
                                                 reporterror(1000*100);  {output warning message} {ming}
			    		      gen_const_displacement(
				                                f1operand.imint)
                                               end
					   else reporterror(100*008+illegal_expression);
					  end;
			      RETT	: begin
					    gen_byte(66);
					    if(f1operand.gtype = immediate) then
                                             begin if (f1operand.regreloffset > 16777215)
                                                    or (f1operand.regreloffset < -16777215 ) then
                                               reporterror(1000*100);  {output warning message} {ming}
			    		       gen_const_displacement(
				               			f1operand.imint)
                                             end
					   else reporterror(100*009+illegal_expression);
					  end;
	             	      SAVE	: begin
					    gen_byte(98); gen_regs(f1regs);
					  end;
			      RESTORE	: begin
					    gen_byte(114); 
					    gen_byte(ord(f1regs[0]));
					  end;
	             	      ENTER	: begin
					    gen_byte(130); gen_regs(f1regs);
					    if(f1operand.gtype = immediate) then
                                             begin if (f1operand.regreloffset > 16777215) 
                                                     or (f1operand.regreloffset < -16777215 ) then
                                               reporterror(1000*100);  {output warning message} {ming}
			    		       gen_const_displacement(
				               			f1operand.imint)
                                           end
					   else reporterror(100*010+illegal_expression);
					  end;
			      EXITo	: begin
					    gen_byte(146); 
					    gen_byte(ord(f1regs[0]));
					  end;
		     	      RETI	: begin
					    gen_byte(82);
					  end;
			      NOP	: begin
					    gen_byte(162);
					  end;
			      WAIT	: begin
					    gen_byte(178);
					  end;
		     	      FLAG	: begin
					    gen_byte(210);
					  end;
			      SVC	: begin
					    gen_byte(226);
					  end;
			      BPT	: begin
					    gen_byte(242);
					  end;
			      DIA	: begin
					    gen_byte(194);
					  end;
			    end;
			  end;
            format2	: begin 
			    temp1 := get_bwd(f2x);
			    if (f2op=ADDQx) or
			       (f2op=CMPQx) or
			       (f2op=MOVQx) or
			       (f2op=ACBx) then
			      begin
			        if f2short.gtype = unresolved then
			          begin
				    locationcounter := location;
			          resolve_op_expression(f2short.start_of_stack,
				    		        f2short);
      				  if f2short.gtype =immediate then
			              if (f2short.imint >= -8) and 
					 (f2short.imint <= 7) then
					begin end
				      else reporterror(100*011+illegal_expression)
				    else reporterror(100*012+illegal_term);
			          end;
			        if f2short.imint < 0 then
				  f2short.imint := 16 + f2short.imint;
			        temp1 := temp1 + f2short.imint *
					128; (* shift left 7 bits *)
			      end
			    else
			     if (f2op = SPRx) or
				(f2op = LPRx) then
			       case ord(preg[0])::preglist of
				 upsr	: begin end;
				 fp	: temp1 := temp1 + (08 * 128);
	    			 sp	: temp1 := temp1 + (09 * 128);
				 sb	: temp1 := temp1 + (10 * 128);
				 psr	: temp1 := temp1 + (13 * 128);
				 intbase: temp1 := temp1 + (14 * 128);
				 modreg	: temp1 := temp1 + (15 * 128);
			       end;
			    temp2 := get_scale( f2operand.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f2operand)
			      end;
			    if temp2 <> -1 then
			      begin
			        temp1 := temp1 + (temp2*2048);
			        temp2 := reg_num + get_addr_mode(f2operand)*8
			      end
			    else temp1 := temp1 +
			      get_addr_mode(f2operand) * 2048;
			      (* shifting over 10 bits 2**10 *)
			    case f2op of
	  		      ADDQx	: begin
					    temp1 := temp1 + (3 * 4);
					  end;
			      CMPQx	: begin
					    temp1 := temp1 + (7 * 4);
					  end;
			      MOVQx	: begin
					    temp1 := temp1 + (23 * 4);
					  end;
			      Scondx	: begin
					    temp1 := temp1 + (get_cond(
					     q1cond) * 128) +
					     (15 * 4);
					  end;
			      ACBx	: begin
					    temp1 := temp1 + (19 * 4);
			    		    if check_sdi(tmp_sdi,g_index,2) then
			      		      begin
					        locationcounter := location;
			                        resolve_sdi_expression(tmp_sdi,
							               f2label^)
			      		      end;
					    if(f2label^.gtype = 
					              regrelative) and
					     (f2label^.rltype= pc ) then
			    		      begin end
					   else reporterror(100*013+illegal_expression);
					  end;
			      SPRx	: begin
					    temp1 := temp1 + (11 * 4);
					  end;
			      LPRx	: begin
					    temp1 := temp1 + (27 * 4);
					  end;
			    end;
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
			    gen_operand(f2operand,g_index);
			    if f2op = ACBx then
			      gen_operand(f2label^,g_index);
			  end;
            format3	: begin 
			    temp1 := get_bwd(f3x);
			    temp2 := get_scale( f3operand.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
				if f3op[0]=chr(ord(CXPD)) then
				  process_d_cxp:= ord(false_cxpd[0])::boolean; 
			        resolve_sdi_expression(tmp_sdi,f3operand);
				process_d_cxp:=false;
			      end;
			    if temp2 <> -1 then
			      begin
			        temp1 := temp1 + (temp2 * 2048);
				temp2 := reg_num + get_addr_mode(f3operand)*8;
			      end
			    else temp1 := temp1 + (2048 *
			             get_addr_mode(f3operand));
			    case ord(f3op[0])::f3opcodes of
	  		      BICPSRx	: begin
					    temp1 := temp1 + (95 * 4);
					  end;
			      BISPSRx	: begin
					    temp1 := temp1 + (223 * 4);
					  end;
			      CASEx	: begin
					    temp1 := temp1 + (479 * 4);
					  end;
			      ADJSPx	: begin
					    temp1 := temp1 + (351 * 4);
					  end;
			      CXPD	: begin 
					    temp1 := temp1 | %7f; 
					  end;
			      JMP	: begin
					    temp1 := temp1 + (159 * 4);
					  end;
			      JSR	: begin
					    temp1 := temp1 + (415 * 4);
					  end;
			    end;
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
                            if f3op[0]=chr(ord(JMP)) then 
                              if f3operand.gtype=absolute then
                                 if f3operand.absnum < 0 then
                                     reporterror(1000*100); {warning message, ming}
			    gen_operand(f3operand,g_index);
			  end;
            format4	: begin 
			    temp1 := get_bwd(f4x);
			    temp2 := get_scale( f4operand1.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			        if f4op[0] = chr(ord(LXPD)) then
				  process_cxp := true;
			        resolve_sdi_expression(tmp_sdi,f4operand1);
			        process_cxp := false;
			      end;
			    if temp2 <> -1 then
			      begin
			        temp1 := temp1 + (temp2 * 2048);
				temp2 := reg_num + get_addr_mode(f4operand1)*8;
			      end
			    else temp1 := temp1 + (2048 *
			            get_addr_mode(f4operand1));
			    temp3 := get_scale( f4operand2.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,2) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f4operand2);
                                if not ((f4op[0] = chr(ord(CMPx))) or
				   (f4op[0] = chr(ord(TBITx)))) then
                                  if f4operand2.gtype = immediate then
	                            reporterror(100*014+illegal_term);
			      end;
			    if temp3 <> -1 then
			      begin
			        temp1 := temp1 + (temp3 * 64);
				temp3 := reg_num +
			             get_addr_mode(f4operand2) *8;
			      end
			    else temp1 := temp1 +(64*get_addr_mode(f4operand2));
			    case ord(f4op[0])::f4opcodes of
	  		      ADDCx	: begin
					    temp1 := temp1 + (4 * 4);
					  end;
			      ADDRo,LXPD,
			      ADDRx	: begin
					    temp1 := temp1 + (9 * 4);
					  end;
			      ADDx	: begin end;
			      ANDx	: begin
					    temp1 := temp1 + (10 * 4);
					  end;
			      BICx	: begin 
					    temp1 := temp1 + (2 * 4);
					  end;
			      CMPx	: begin
					    temp1 := temp1 + (1 * 4);
					  end;
			      MOVx	: begin
					    temp1 := temp1 + (5 * 4);
					  end;
			      ORx	: begin
					    temp1 := temp1 + (6 * 4);
					  end;
			      SUBCx	: begin
					    temp1 := temp1 + (12 * 4);
					  end;
			      SUBx	: begin
					    temp1 := temp1 + (8 * 4);
					  end;
			      TBITx	: begin
					    temp1 := temp1 + (13 * 4);
					  end;
			      XORx	: begin
					    temp1 := temp1 + (14 * 4);
					  end;
			    end;
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
			    if temp3 <> -1 then gen_byte(temp3);
                            if f4op[0]=chr(ord(LXPD)) then 
                            if f4operand1.extoffset <> 0 then 
                               reporterror(100*027+illegal_term); {ming}
                           ! if (f4op[0]=chr(ord(LXPD)) and 
                           !        (f4operand1.extoffset <> 0))
                           !    then reporterror(100*027+illegal_term); {ming}
			    gen_operand(f4operand1,g_index);
			    gen_operand(f4operand2,g_index);
			  end;
            format5	: begin 
			    gen_byte(14);
			    if (fop5 = MOVSz) or
			       (fop5 = CMPSz) or
			       (fop5 = SKPSz) then
			      begin
			        temp1 := get_bwd(f5bwd);
			        if f5t[0] = chr(ord(true)) then
				  temp1 := temp1 + 128;
			        if f5bck[0] = chr(ord(true)) then
				  temp1 := temp1 + 256;
			        case ord(f5uw[0])::uwfield of
				  untilmatch : temp1 := temp1 + 1536;
				  whilematch : temp1 := temp1 + 512;
				  nouw       : begin end;
			        end;
			        case fop5 of
				  MOVSz : begin end;
				  CMPSz : temp1 := temp1 + (1 * 4);
				  SKPSz : temp1 := temp1 + (3 * 4);
			        end;
			      end
			    else
			      begin
				temp1 := (2*4+3);
			        if (ord(1<<ord(cdsp)) & 
                                        ord(stcfgoperand[0])) <> 0 then
	    			  temp1 := temp1 | 1024;
			        if (ord(1<<ord(mmu)) & 
                                        ord(stcfgoperand[0])) <> 0 then
	    			  temp1 := temp1 | 512;
			        if (ord(1<<ord(fpu)) & 
                                        ord(stcfgoperand[0])) <> 0 then
	    			  temp1 := temp1 | 256;
			        if (ord(1<<ord(ic)) & 
                                        ord(stcfgoperand[0])) <> 0 then
	    			  temp1 := temp1 | 128;
			      end;
			    gen_word(temp1);
			  end;
            format6	: begin 
			    gen_byte(78); temp1 := get_bwd(f6x);
			    temp2 := get_scale( f6operand1.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f6operand1)
			      end;
			    if temp2 <> -1 then
			      begin
			        temp1 := temp1 + (temp2 * 2048);
				temp2 := reg_num + get_addr_mode(f6operand1)*8;
			      end
			    else temp1 :=temp1+(2048*get_addr_mode(f6operand1));
			    temp3 := get_scale( f6operand2.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,2) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f6operand2)
			      end;
			    if f6operand2.gtype = immediate then
			      reporterror(100*015+illegal_term);
			    if temp3 <> -1 then
			      begin
			        temp1 := temp1 + (temp3 * 64);
				temp3 := reg_num + get_addr_mode(f6operand2)*8;
			      end
			    else temp1 := temp1+(64* get_addr_mode(f6operand2));
			    case ord(f6op[0])::f6opcodes of
	  		      ROTx	: begin end;
			      ASHx      : begin
					    temp1 := temp1 + (1 * 4);
					  end;
			      CBITx	: begin
					    temp1 := temp1 + (2 * 4);
					  end;
			      LSHx	: begin
					    temp1 := temp1 + (5 * 4);
					  end;
			      SBITx	: begin 
					    temp1 := temp1 + (6 * 4);
					  end;
			      SBITIx	: begin
					    temp1 := temp1 + (7 * 4);
					  end;
			      NEGx	: begin
					    temp1 := temp1 + (8 * 4);
					  end;
			      NOTx	: begin
					    temp1 := temp1 + (9 * 4);
					  end;
			      SUBPx	: begin
					    temp1 := temp1 + (11 * 4);
					  end;
			      ABSx	: begin
					    temp1 := temp1 + (12 * 4);
					  end;
			      COMx	: begin
					    temp1 := temp1 + (13 * 4);
					  end;
			      IBITx	: begin
					    temp1 := temp1 + (14 * 4);
					  end;
			      ADDPx	: begin
					    temp1 := temp1 + (15 * 4);
					  end;
			      CBITIx	: begin
					    temp1 := temp1 + (3 * 4);
					  end;
			    end;
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
			    if temp3 <> -1 then gen_byte(temp3);
			    if(f6op[0]=chr(ord(ROTx)))or(f6op[0]=chr(ord(ASHx)))
			     or(f6op[0]=chr(ord(LSHx))) then
			      if f6operand1.gtype = immediate then
				gen_byte(f6operand1.imint)
			      else gen_operand(f6operand1,g_index)
			    else begin
                             if (f6op[0]=chr(ord(subpx))) or (f6op[0]=chr(ord(addpx))) then begin
                                      if f6operand1.gtype=immediate then {ming}
                                           check_packed_no(f6operand1.imint)
                                    end;
			          gen_operand(f6operand1,g_index);
                                 end;
			    gen_operand(f6operand2,g_index);
			  end;
           format7	: begin 
			    gen_byte(206);
			    temp1 := get_bwd(f7x);
			    temp2 := get_scale( f7operand1.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f7operand1)
			      end;
			    if temp2 <> -1 then
			      begin
			        temp1 := temp1 + (temp2 * 2048);
				temp2 := reg_num + get_addr_mode(f7operand1)*8;
			      end
			    else temp1 := temp1 + (2048 *
			            get_addr_mode(f7operand1));
			    temp3 := get_scale( f7operand2.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,2) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f7operand2)
			      end;
			    if f7operand2.gtype=immediate then
			      reporterror(100*016+illegal_term);
			    if temp3 <> -1 then
			      begin
			        temp1 := temp1 + (temp3 * 64);
				temp3 := reg_num +
			             get_addr_mode(f7operand2)*8;
			      end
			    else temp1 := temp1 + (64 *
			            get_addr_mode(f7operand2));
			    case fop7 of
	  		      MOVMx	: begin end;
			      CMPMx      : begin
					    temp1 := temp1 + (1 * 4);
					  end;
			      INSSx	: begin
					    temp1 := temp1 + (2 * 4);
					  end;
			      EXTSx	: begin
					    temp1 := temp1 + (3 * 4);
					  end;
			      MULx	: begin 
					    temp1 := temp1 + (8 * 4);
					  end;
			      MEIx	: begin
					    temp1 := temp1 + (9 * 4);
					  end;
			      DEIx	: begin
					    temp1 := temp1 + (11 * 4);
					  end;
			      DIVx	: begin
					    temp1 := temp1 + (12 * 4);
					  end;
			      REMx	: begin
					    temp1 := temp1 + (13 * 4);(f6op[0]=chr(ord(addpx))) then begin
                                      if f6operand1.gtype=immediate then {ming}
                                           check_packed_no(f6operand1.imint)
                                    end;
			          gen_operand(f6operand1,g_index);
                                 end;
			    gen_operand(f6operand2,g_index);
			  end;
           format7	: begin 
			    gen_(6 * 4);
					  end;
			    end;
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
			    if temp3 <> -1 then gen_byte(temp3);
			    gen_operand(f7operand1,g_index);
			    gen_operand(f7operand2,g_index);
			    if (fop7 = MOVMx) or
			       (fop7 = CMPMx) then
			      begin
			       if f7constant^.gtype=unresolved then
			          begin
				    locationcounter := location;
			            resolve_op_expression(
				                     f7constant^.start_of_stack,
				                          f7constant^);
      				    if (f7constant^.gtype <> immediate)then
	  			      reporterror(100*017+illegal_expression)
			          end;
				if false then
				  begin
				    if ((f7constant^.imint<=0) or 
				       (f7constant^.imint>16)) then
				      reporterror(100*018+illegal_expression)
				    else f7constant^.imint:=f7constant^.imint-1
				  end
				else
				  begin
				    if f7x[0]=chr(ord(b)) then
				      f7constant^.imint:=f7constant^.imint-1
				    else if f7x[0]=chr(ord(w)) then
				      f7constant^.imint:=(f7constant^.imint-1)*2
				    else
				     f7constant^.imint:=(f7constant^.imint-1)*4;
				    if ((f7constant^.imint<0) or 
				       (f7constant^.imint>=16)) then
				      reporterror(100*224+57)
				  end;
				gen_byte(f7constant^.imint);
			      end
			    else if (fop7 = INSSx) or
				    (fop7 = EXTSx) then
			      begin
			        if f7constant1^.gtype = unresolved then
			          begin
				    locationcounter := location;
			            resolve_op_expression(
						    f7constant1^.start_of_stack,
						    f7constant1^);
      				    if not(f7constant1^.gtype = immediate) and
			             not((f7constant1^.imint>=0) and
				         (f7constant1^.imint<8))then
	  			        reporterror(100*019+illegal_expression)
			          end;
				temp2 := f7constant^.imint * 32;
			        if f7constant2^.gtype = unresolved then
			          begin
				    locationcounter := location;
			            resolve_op_expression(
						    f7constant2^.start_of_stack,
						     f7constant2^);
      				    if not(f7constant2^.gtype = immediate) and
			             not((f7constant2^.imint>=0) and
					 (f7constant2^.imint<33))then
	  			        reporterror(100*020+illegal_expression)
			          end;
			        gen_byte(temp2+ (f7constant2^.imint-1));
			      end
			  end;
            format8	: begin 
			    case fop8 of
	  		      EXTs	: gen_byte(46);
			      INSx      : gen_byte(174);
			      CVTP	: gen_byte(110);
			      CHECKx	: gen_byte(238);
			      INDEXx	: gen_byte(46); 
			      FFSx	: gen_byte(110);
			      MOVSUx	: gen_byte(174);
			      MOVUSx	: gen_byte(174); 
			    end;
			    if fop8 = CVTP then
			      temp1 := 3
			    else temp1 := get_bwd(f8x);
			    if (fop8 = INDEXx) or
			       (fop8 = FFSx) or
			       (fop8 = MOVSUx) or
			       (fop8 = MOVUSx) then
			      temp1 := temp1 + 4;
			    if (fop8 = EXTs) or
			       (fop8 = INSx) or
			       (fop8 = CVTP) or
			       (fop8 = CHECKx) or
			       (fop8 = INDEXx) then
			      temp1 := temp1 + (get_reg(f8reg)*8)
			    else if fop8 = MOVUSx then
			      temp1 := temp1 + 24
			    else if fop8 = MOVSUx then
			      temp1 := temp1 + 8;
			    temp2 := get_scale(f8operand1.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f8operand1)
			      end;
			    if temp2 <> -1 then
			      begin
			        temp1 := temp1 + (temp2 * 2048);
				temp2 := reg_num + get_addr_mode(f8operand1)*8;
			      end
			    else temp1 := temp1 + (2048 *
			            get_addr_mode(f8operand1));
			    temp3 := get_scale( f8operand2.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,2) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f8operand2)
			      end;
			    if (f8operand2.gtype=immediate) and
			      (fop8 <> INDEXx) and (fop8 <> CHECKx) then
			      reporterror(100*021+illegal_term);
			    if temp3 <> -1 then
			      begin
			        temp1 := temp1 + (temp3 * 64);
				temp3 := reg_num + get_addr_mode(f8operand2)*8;
			      end   (* changed 7 to 8 above : sunil *)
			    else temp1 := temp1 + (64 *
			            get_addr_mode(f8operand2));
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
			    if temp3 <> -1 then gen_byte(temp3);
			    gen_operand(f8operand1,g_index);
			    gen_operand(f8operand2,g_index);
			    if (fop8 = EXTs) or 
			       (fop8 = INSx) then
			      begin
			    	if check_sdi(tmp_sdi,g_index,3) then
			      	  begin
				    locationcounter := location;
			            resolve_sdi_expression(tmp_sdi,f8disp^)
			      	  end;
			        if f8disp^.gtype<>immediate then
				  reporterror(100*022+illegal_expression);
			        gen_operand(f8disp^,g_index);
			      end;
			  end;
        format9	: begin 
			    gen_byte(62);
			    if (fop9 = MOVxf) or
			       (fop9 = TRUNCfx) or
			       (fop9 = ROUNDfx) or
			       (fop9 = FLOORfx) then
			      begin
				temp1 := get_bwd(f9x);
				if f9f[0] = chr(ord(f)) then
				  temp1 := temp1 + 4;
			      end
			    else
			      if fop9 = MOVFL then temp1 := 3
				else if fop9 = MOVLF then temp1 := 6
                                   else temp1 := 7;
                            if f11operand1.gtype=immediate then
			      temp2 := -1 
                            else
                              temp2:=get_scale(f9operand1.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f9operand1)
			      end;
			    if temp2 <> -1 then
			      begin
			        temp1 := temp1 + (temp2 * 2048);
				temp2 := reg_num + get_addr_mode(f9operand1)*8;
			      end
			    else if fop9 = SFSR 
                                  then temp1 := temp1 + (64 * 
                                                     get_addr_mode(f9operand1))
			          else temp1 := temp1 + (2048 *
			                             get_addr_mode(f9operand1));
			    temp3 := 0;
			    if not ((fop9 = SFSR) or
				    (fop9 = LFSR)) then
			      begin
			        temp3 :=get_scale(f9operand2.gen_scale,reg_num);
			        if check_sdi(tmp_sdi,g_index,2) then
			          begin
				    locationcounter := location;
			            resolve_sdi_expression(tmp_sdi,f9operand2)
			          end;
			        if f9operand2.gtype = immediate then
			          reporterror(100*023+illegal_term);
                                if (fop9=SFSR) and 
                                  (f9operand1.gtype=immediate) then
                                  reporterror(100*024+illegal_term);
			        if temp3 <> -1 then
			          begin
			            temp1 := temp1 + (temp3 * 64);
				    temp3 := reg_num +
				                    get_addr_mode(f9operand2)*8;
			          end
			        else temp1:= temp1+
					     (64*get_addr_mode(f9operand2));
			      end;
			    case fop9 of
	  		      MOVxf	: begin end;
			      TRUNCfx   : begin
					    temp1 := temp1 + (5 * 8);
					  end;
			      ROUNDfx	: begin
					    temp1 := temp1 + (4 * 8);
					  end;
			      FLOORfx	: begin
					    temp1 := temp1 + (7 * 8);
					  end;
			      LFSR	: begin 
					    temp1 := temp1 + (1 * 8);
					  end;
			      SFSR	: begin
					    temp1 := temp1 + (6 * 8);
					  end;
			      MOVFL	: begin
					    temp1 := temp1 + (3 * 8);
					  end;
			      MOVLF	: begin
					    temp1 := temp1 + (2 * 8);
					  end;
			    end;
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
                            if (f9operand1.gtype=immediate) and
                               (fop9<>MOVxf) and 
                               (fop9<>LFSR) and
                               (fop9<>SFSR) then
	                      gen_float_immediate(f9operand1,
                                   ((ord(f9f[0])=ord(l))or(fop9=MOVLF)))
                            else gen_operand(f9operand1,g_index);
			    if not ((fop9 = SFSR) or
				    (fop9 = LFSR)) then
			      begin
			        gen_operand(f9operand2,g_index);
			        if temp3 <> -1 then gen_byte(temp3);
			      end;
			  end;

            format10	: begin end;
            format11	: begin 
			    gen_byte(190);
			    if f11f[0]= chr(ord(f)) then
			      temp1 := 1
			    else temp1 := 0;
                            if f11operand1.gtype=immediate then
			      temp2 := -1 
                            else
                              temp2:=get_scale(f11operand1.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f11operand1)
			      end;
			    if temp2 <> -1 then
			      begin
			        temp1 := temp1 + (temp2 * 2048);
				temp2 := reg_num + get_addr_mode(f11operand1)*8;
			      end
			    else temp1 := temp1 + (2048 *
			        get_addr_mode(f11operand1));
			    temp3 := get_scale( f11operand2.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,2) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f11operand2)
			      end;
			    if (f11operand2.gtype=immediate) and
                                (fop11[0]<>chr(ord(CMPf)))  then
			      reporterror(100*025+illegal_term);
			    if temp3 <> -1 then
			      begin
			        temp1 := temp1 + (temp3 * 64);
				temp3 := reg_num + 
					           get_addr_mode(f11operand2)*8;
			      end
			    else temp1 := temp1 + (64 *
			           get_addr_mode(f11operand2));
			    case ord(fop11[0])::f11opcodes of
	  		      ADDf	: begin end;
			      MOVf      : begin
					    temp1 := temp1 + (1 * 4);
					  end;
			      CMPf	: begin
					    temp1 := temp1 + (2 * 4);
					  end;
			      ABSf	: begin
					    temp1 := temp1 + (13 * 4);
					  end;
			      SUBf	: begin 
					    temp1 := temp1 + (4 * 4);
					  end;
			      NEGf	: begin
					    temp1 := temp1 + (5 * 4);
					  end;
			      DIVf	: begin
					    temp1 := temp1 + (8 * 4);
					  end;
			      FRACf	: begin
					    reporterror(100*227+illegalinst);
					    temp1 := temp1 + (9 * 4);
					  end;
			      MULf	: begin
					    temp1 := temp1 + (12 * 4);
					  end;
			    end;
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
			    if temp3 <> -1 then gen_byte(temp3);
                            if f11operand1.gtype=immediate then
	(*!!!*)               gen_float_immediate(f11operand1,
                                                  (ord(f11f[0])=ord(l)))
                            else gen_operand(f11operand1,g_index);
                            if f11operand2.gtype=immediate then
	(*!!!*)               gen_float_immediate(f11operand2,
                                                  (ord(f11f[0])=ord(l)))
			    else gen_operand(f11operand2,g_index);
			  end;
            format12	: begin 
			    gen_byte(254);
			    if (f12f[0] = chr(ord(l))) and 
			       ((fop12[0] = chr(ord(POLYf))) or
			        (fop12[0] = chr(ord(DOTf)))) then
			      temp1 := 1
			    else temp1 := 0;
			    temp1 := temp1 + (get_freg(f12reg) * 8);
			    temp2 := get_scale( f12operand1.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f12operand1)
			      end;
			    if temp2 <> 0 then
			      begin
			        temp1 := temp1 + (temp2 * 2048);
				temp2 := reg_num + get_addr_mode(f12operand1)*8;
			      end
			    else temp1 := temp1 + (2048 *
			        get_addr_mode(f12operand1));
			    temp3 := get_scale( f12operand2.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,2) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f12operand2)
			      end;
			    if f12operand2.gtype=immediate then
			      reporterror(100*026+illegal_term);
			    if temp3 <> 0 then
			      begin
			        temp1 := temp1 + (temp3 * 64);
				temp3 := reg_num + get_addr_mode(f12operand2)*8;
			      end
			    else temp1 := temp1 + (64 *
			           get_addr_mode(f12operand2));
			    case ord(fop12[0])::f12opcodes of
	  		      POLYf	: begin end;
			      DOTf      : begin
					    temp1 := temp1 + (1 * 2);
					  end;
			      POLYFL	: begin
					    temp1 := temp1 + (2 * 2);
					  end;
			      DOTFL	: begin
					    temp1 := temp1 + (3 * 2);
					  end;
			    end;
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
			    if temp3 <> -1 then gen_byte(temp3);
			    gen_operand(f12operand1,g_index);
			    gen_operand(f12operand2,g_index);
			  end;
            format14	: begin 
			    gen_byte(30); temp1 := 3;
			    if (fop14[0] = chr(ord(SMR))) or 
			       (fop14[0] = chr(ord(LMR))) then
			      temp1 := temp1 + (get_mreg(f14mreg) * 128);
			    temp2 := get_scale( f14operand.gen_scale, reg_num);
			    if check_sdi(tmp_sdi,g_index,1) then
			      begin
				locationcounter := location;
			        resolve_sdi_expression(tmp_sdi,f14operand)
			      end;
			    if temp2 <> -1 then
			      begin
			        temp1 := temp1 + (temp2 * 2048);
				temp2 := reg_num + get_addr_mode(f14operand)*8;
			      end
			    else temp1 := temp1 + (2048 * 
						get_addr_mode(f14operand));
			    case ord(fop14[0])::f14opcodes of
	  		      RDVAL	: begin end;
			      WRVAL     : begin
					    temp1 := temp1 + (1 * 4);
					  end;
			      LMR	: begin
					    temp1 := temp1 + (2 * 4);
					  end;
			      SMR	: begin
					    temp1 := temp1 + (3 * 4);
					  end;
			    end;
			    gen_word(temp1);
			    if temp2 <> -1 then gen_byte(temp2);
			    gen_operand(f14operand,g_index);
			  end;
            byteconst 	: begin
			    resolve_dir_expression(g_index,bcr_count);
			    for temp := 1 to bcr_count.disp_value do
			      begin
			        resolve_dir_expression(g_index, bc_value);
				if bc_value.disp_type = const_value then
				  if (sign_b_flag[0]=chr(ord(true))) and
				     (bc_value.disp_value>127) then
                                  !begin {ming}
				    reporterror(100*174+termsizeextreme)
                                  !end
                                  !else if ((bc_value.disp_value>255) or 
                                  !        (bc_value.disp_value<-128)) then
                                  !  begin
                                  !  reporterror(100*175+termsizeextreme) 
                                  !  end
				  else  !begin
		                    gen_byte( bc_value.disp_value)
                                  !end
				else
				  for temp1 := 1 to bc_value.str_rec^.strglen do
                                  ! begin
				    gen_byte(ord(
			                    bc_value.str_rec^.strchars[temp1]));
                                  ! end
			      end;
			  end;
            wordconst 	: begin
			    resolve_dir_expression(g_index,wcr_count);
			    for temp := 1 to wcr_count.disp_value do
			      begin
			        resolve_dir_expression(g_index, wc_value);
				if wc_value.disp_type = const_value then
				  if (sign_d_flag[0]=chr(ord(true))) and
				     (dc_value.disp_value>32767) then
				    reporterror(100*223+termsizeextreme)
				  ! else
                                  ! if ((dc_value.disp_value>65535) or
                                  !     (dc_value.disp_value<-32768)) then
				  !    reporterror(100*224+termsizeextreme)
                                    else
				    gen_word(wc_value.disp_value)
				else
				  for temp1 := 1 to wc_value.str_rec^.strglen do
				    gen_byte(ord(
			   		wc_value.str_rec^.strchars[temp1]));
			      end;
			  end;
            doubconst 	: begin
			    resolve_dir_expression(g_index,dcr_count);
			    for temp := 1 to dcr_count.disp_value do
			      begin
			        resolve_dir_expression(g_index, dc_value);
				if dc_value.disp_type = const_value then
				  begin
		       	            gen_doub(dc_value.disp_value)
				  end
				else
				  for temp1 := 1 to dc_value.str_rec^.strglen do
				    gen_byte(ord(
			   		dc_value.str_rec^.strchars[temp1]));
			      end;
			  end;
           floatconst 	: begin
			    resolve_dir_expression(g_index,dcr_count);
			    for temp := 1 to dcr_count.disp_value do
			      begin
			        resolve_dir_expression(g_index, dc_value);
				if dc_value.disp_type = const_value then
				  begin
		       	            gen_doub(dc_value.short_float_value)
				  end
				else
				  for temp1 := 1 to dc_value.str_rec^.strglen do
				    gen_byte(ord(
			   		dc_value.str_rec^.strchars[temp1]));
			      end;
			  end;

            longconst 	: begin
			    resolve_dir_expression(g_index,dcr_count);
			    for temp := 1 to dcr_count.disp_value do
			      begin
			        resolve_dir_expression(g_index, dc_value);
				if dc_value.disp_type = const_value then
				  begin
		       	            gen_doub(dc_value.long_float_value.d_lo);
                                    gen_doub(dc_value.long_float_value.d_hi)
				  end
				else
				  for temp1 := 1 to dc_value.str_rec^.strglen do
				    gen_byte(ord(
			   		dc_value.str_rec^.strchars[temp1]));
			      end;
                          end;

	     fieldconst : begin
			    flp:=global_interm_rec.first_field_rec;
			      field_bit_count:=0;
			      field_byte:=0;
			      while flp<>nil do with flp^ do
				begin
				  resolve_dir_expression(g_index,field_value);
				  if field_value.disp_type=const_value then
					emitc(field_size,field_value.disp_value)
				  else
				    if field_size<field_value.str_rec^.strglen*8
				      then reporterror(100*213+termsizeextreme)
				    else
				      for temp1:= 1 to 
					  field_value.str_rec^.strglen do
				         emitc(8,ord(field_value.str_rec^.
							  strchars[temp1]));
				  flp:=flp^.next_field_rec;
				end;
			      flushc
			  end;
          end;
	  if(ftype <> last_interm)and big_list then
	    with lrec do 
	      begin
	        linenum := global_interm_rec.linenumber;
		listrectype := code_rec;
		lrec.location := g_index^.location; 
		num_bytes := pc_byte_count - init_byte_count;
		if currentmode = stbsrel then
		  begin
		    reltype := datarel;
		    xx := fwrite (lrec,size(listrec),1,lstbuff3);
		  end
		else
		  begin
		    reltype := coderel;
		    xx := fwrite (lrec,size(listrec),1,lstbuff2);
		  end;
	      end;
	    end;
	if (ftype = intermlistdir) and big_list then 
	  with lrec do
	    begin
	      linenum := global_interm_rec.linenumber;
	      listrectype := list_rec;
	      nextpos := nextcharpos;
	      dirtype := list_type;
	      xx := fwrite(lrec,size(listrec),1,lstbuff2);
	    end;
      end;
    end;

  routine get_import_rec(sptr : symptr) imp_ptr : importptr;
    var c_test,cmp_in : integer;
    begin
      (* there better be an import_rec for this common type? *)
      c_test := -1; imp_ptr := importstart;
      while ((c_test <> 0) and (imp_ptr <> nil)) do
	begin
	  c_test := cmpc(maxsymlength,addr(sptr^.symbolname^),
				      addr(imp_ptr^.importsptr^.symbolname^));
          (*cmp_in:=1;
          repeat
            if (sptr^.symbolname^[cmp_in]=chr(0))or
               (imp_ptr^.importsptr^.symbolname^[cmp_in]=chr(0))or
               (sptr^.symbolname^[cmp_in]<>imp_ptr^.importsptr^.symbolname^[cmp_in])or
	       (cmp_in>maxsymlength) then
              begin c_test:=ord(sptr^.symbolname^[cmp_in])-ord(imp_ptr^.importsptr^.symbolname^[cmp_in]);
                if c_test>0 then c_test:=1 
                else if c_test<0 then c_test:=-1;
                cmp_in:=0
              end
            else cmp_in:=cmp_in+1
          until cmp_in=0;*)
	  if c_test <> 0 then
	    imp_ptr := imp_ptr^.nextimportrec;
	end;
    end;

procedure reverse_stab_chain(s:symptr);
  var h,h1,hn:stab_rec_ptr;
  begin 
    disp_stab_chain(5,5,s^.first_stab);
    h:=s^.first_stab;
    hn:=nil;
    while h<>nil do
      begin h1:=h^.next_stab_rec;
	h^.next_stab_rec:=hn;
	disp_stab_rec(8,8,h);
	hn:=h;
	h:=h1
      end;
    s^.first_stab:=hn;
    disp_stab_chain(6,6,hn)
  end;


procedure output_stab_chain(i:symptr);
   var sym_ent:sym_ent_type;amode:integer;sp,sp1:stab_rec_ptr;
     first_flag:boolean;
   begin
     reverse_stab_chain(i); !stabs were inserted in the chain in reverse order
     first_flag:=true;
     sp:=i^.first_stab;
     sp1:=sp;
     while sp1<> nil do with stp=sp1^ do
       begin 
         if stp.whichstab=STABd then 
	   begin
	     sym_ent.n_ty:=stp.type_info_doub;
	     if stp.is_1_present then
	       begin resolve_stab_expression(stp.stabnum1);
		 if (stp.stabnum1.disp_type=const_value) and
		    (stp.stabnum1.disp_const_type=inti) then
		   sym_ent.n_nm:=stp.stabnum1.disp_value
		 else reporterror(100*217+illegal_term)
	       end
	     else sym_ent.n_nm:=0;
	     resolve_stab_expression(stp.stabnum2);
	     if (stp.stabnum2.disp_type=const_value) and
	        (stp.stabnum2.disp_const_type=inti) then
	       sym_ent.n_v:=stp.stabnum2.disp_value
	     else reporterror(100*218+illegal_term);
	     if fwrite(sym_ent,size(sym_ent_type),1,obuffer)<> 1 then
	       writeln('fwrite error in obuffer/1');
	     out_sym_count:=out_sym_count+1;
 	   end
	 else if stp.whichstab=STABSd then 
	   begin
	     if first_flag then 
	       begin
                 while sp<>nil do
                   begin
                     if sp^.whichstab=SOURCEd then with se=sym_ent do
                       begin
	                 if sp^.islineorchar then se.n_ty:=N_SLINE
	                 else se.n_ty:=N_SCHAR;
	                 se.n_ty:=se.n_ty|(sp^.sourceitem<<6);
	                 if sp^.ispcrel then
	                   begin amode:=27;
	                     se.n_v:=i^.symbol_definition.regreloffset
	                   end
	                 else
	                  begin amode:=20;
	                    se.n_v:=0;
	                  end;
	                 se.n_nm:=amode|
				(trans_name_ptr(sp^.whichfile.address)<<6);
	                 out_sym_count:=out_sym_count+1;
  	                 !gen_sym_ent(sym_ent);
	                 if fwrite(sym_ent,size(sym_ent_type),1,obuffer)<>1 then
				writeln('fwrite error in obuffer/2')
                       end;
	             sp:=sp^.next_stab_rec;
                   end;
	         first_flag:=false
	       end;
	     output_sym(stp.symbol_ptr);
	   end;
         sp1:=sp1^.next_stab_rec
       end;
     if first_flag then 
       while sp<>nil do
         begin
           if sp^.whichstab=SOURCEd then with se=sym_ent do
             begin
	       if sp^.islineorchar then se.n_ty:=N_SLINE
	       else se.n_ty:=N_SCHAR;
	       se.n_ty:=se.n_ty|(sp^.sourceitem<<6);
	       if sp^.ispcrel then
	         begin amode:=27;
	           se.n_v:=i^.symbol_definition.regreloffset
	         end
	       else
	        begin amode:=20;
	          se.n_v:=0;
	        end;
	       se.n_nm:=amode|(trans_name_ptr(sp^.whichfile.address)<<6);
	       out_sym_count:=out_sym_count+1;
  	       !gen_sym_ent(sym_ent);
	       if fwrite(sym_ent,size(sym_ent_type),1,obuffer)<> 1 then
	           writeln('fwrite error in obuffer/3')
             end;
	   sp:=sp^.next_stab_rec;
         end;
     end;
  
  export routine output_sym(i : symptr);

  var sym_ent : sym_ent_type;
      sym_ent2, sym_ent3 : sym_ent_type;
      s2, s3 : boolean;
      tmp : integer;
      imp_ptr : importptr;
      fct : factorrec;

 begin
  sym_ent.n_ty := i^.scope.domain;  
  if i^.type_of_symbol<>nil then
    if i^.type_of_symbol^.t_name_ptr<>nil then
      sym_ent.n_ty :=
	sym_ent.n_ty|(trans_name_ptr(i^.type_of_symbol^.t_name_ptr.address)<<6);
  if (not(objsymtable)) and (i^.symbolname^[1] = 'L') and
     (i^.symbol_definition.gtype = regrelative) and
     (sym_ent.n_ty = 0) then
   begin end
   else begin
   if i^.symbolname^[1]<> ' ' then
    begin 
     with sym_ent do 
     begin
       s2 := false; s3 := false;
       if i^.equ_sdi[0]=chr(ord(true)) then
	 begin resolve_1_expression(i^.symbol_definition.start_of_stack,fct);
	   if fct.factortype<>fgen then reporterror(100*214+undefined_symbol)
	   else i^.symbol_definition:=fct.fgenref;
	   i^.equ_sdi[0]:=chr(ord(false))
         end;
       tmp := get_addr_mode(i^.symbol_definition);
       sym_ent.n_nm := (trans_name_ptr(i^.symbolname.address)<<6) | 
							(tmp & N_AMODE);
       i^.scope.domain:=n_ty; (* this puts the value back *)
       if ((tmp >= a_r0) and (tmp <= 7)) or (tmp = a_tos) then
         begin
           n_v := 0;
         end
       else if ((tmp >= a_r0r) and (tmp <= a_r7r)) or 
	       ((tmp >= a_imm) and (tmp <= a_abs)) or
	       ((tmp >= a_fp) and (tmp <= a_pc)) then
         begin with i^.symbol_definition do 
	   begin
             case gtype of
               regrelative : n_v := regreloffset;
               absolute    : n_v := absnum;
               immediate   : n_v := imint;
             end;
	    end;
         end
       else with i^.symbol_definition do
         if gtype = memoryrelative then 
           begin
             if (memoffset1 > -32768) and (memoffset1 <= 32767) and
		(memoffset2 > -32768) and (memoffset2 <= 32767) then
	       n_v := ((memoffset1 & %ffff) << 16) | (memoffset2 & %ffff)
             else begin 
	       n_ty := n_ty | N_MORE;
	       n_v := %80008000; s2 := true;
	       sym_ent2.n_nm := memoffset1;
	       sym_ent2.n_v := memoffset2;
	       sym_ent2.n_ty := N_AUX | (N_LDISP << 9);
	     end
           end
         else if gtype = mod_def then
           begin
	     sym_ent.n_ty := N_MAUX | N_MORE;
	     if ext_to_sbrelative then sym_ent.n_ty:=sym_ent.n_ty|(1<<16);
	     if last_alloc_sb <> 0 then
	       sym_ent.n_ty :=  sym_ent.n_ty | N_INIT; 
	     sym_ent.n_v := 0;
	     s2 := true; s3 := true;
	     sym_ent2.n_nm := last_loc^.symbol_definition.regreloffset;
	     sym_ent2.n_v := import_count * 4;
	     sym_ent2.n_ty := (N_AUX | N_MORE) | (N_TXTINFO << 9);
	     sym_ent3.n_nm := 0;
	     sym_ent3.n_v := last_alloc_sb;
	     sym_ent3.n_ty := N_AUX| (N_DATINFO << 9);
           end
         else if gtype = externalg then
           begin
	     if ext_proc[0] <> chr(3) then
	       n_ty := n_ty | N_EXT;
	     if ext_proc[0] = chr(1) then
	       n_ty := n_ty | N_PROC;
	     if ext_to_sbrelative then extindex:=import_count-1-extindex;
             if (extindex > -32768) and (extindex <= 32767) and
		(extoffset > -32768) and (extoffset <= 32767) then
		n_v := ((extoffset & %ffff) << 16) | (extindex & %ffff)
       	     else begin 
	       n_v := %80008000; s2 := true;
	       sym_ent2.n_nm := extindex;
	       sym_ent2.n_v := extoffset;
	       sym_ent2.n_ty := N_AUX | (N_LDISP << 9);
	       sym_ent.n_ty := sym_ent.n_ty | N_MORE; 
	     end;
	     if ext_to_sbrelative then extindex:=import_count-1-extindex;
	     if ext_proc[0] = chr(2) then
	       begin
	         s3 := true; sym_ent3.n_nm := n_nm;
	         sym_ent3.n_ty := N_AUX | (N_COMMON << 9);
		 sym_ent3.n_nm := 0; { added by Tim }
		 if s2 then
	           sym_ent2.n_ty := sym_ent2.n_ty | N_MORE
	         else sym_ent.n_ty := sym_ent.n_ty | N_MORE; 
	         imp_ptr := get_import_rec(i);
	         if imp_ptr <> nil then
	           sym_ent3.n_v := imp_ptr^.comm_size
	         else reporterror(100*027+invalid_external);
		 
	       end;
	   end;
     end;
     !gen_sym_ent(sym_ent);
     if fwrite(sym_ent,size(sym_ent_type),1,obuffer) <> 1 then
       writeln('fwrite error in obuffer/4; errno= ',errno);
     out_sym_count := out_sym_count + 1;
     if s2 then
       begin
         out_sym_count := out_sym_count + 1;
         !gen_sym_ent(sym_ent);
         if fwrite(sym_ent2,size(sym_ent_type),1,obuffer) <> 1 then
           writeln('fwrite error in obuffer/5; errno= ',errno);
       end;
     if s3 then
       begin
         out_sym_count := out_sym_count + 1;
         !gen_sym_ent(sym_ent);
         if fwrite(sym_ent3,size(sym_ent_type),1,obuffer) <> 1 then
             writeln('fwrite error in obuffer/6; errno= ',errno);
       end;
   end;
   output_stab_chain(i);
  end;
end;


  export routine output_sym_ents(i : symptr);
    begin
      if i <> nil then
	begin
	  output_sym_ents(i^.leftptr);
          if(i^.symbol_definition.gtype = externalg)or 
	    (i^.symbol_definition.gtype = mod_def)then
	    begin end
	  else 
	      output_sym(i);
	  output_sym_ents(i^.rightptr);
	end;
    end;

  begin
    pc_byte_count := 0;
    byte_count:=0;
    sevenfffffff:=%7fffffff;

    for ex_i:=0 to 8 do for ex_j:=2 to 8 do expansion_tab[ex_j,ex_i]:=chr(4);
    expansion_tab[2,0]:=chr(0); expansion_tab[3,0]:=chr(1); 
    expansion_tab[4,0]:=chr(1); expansion_tab[5,0]:=chr(3);
    expansion_tab[6,0]:=chr(1); expansion_tab[8,0]:=chr(3);
    expansion_tab[3,1]:=chr(0); expansion_tab[4,1]:=chr(1);
    expansion_tab[5,1]:=chr(0); expansion_tab[6,1]:=chr(3);
    expansion_tab[8,1]:=chr(3); expansion_tab[5,2]:=chr(0);
    expansion_tab[6,2]:=chr(1); expansion_tab[8,2]:=chr(3);
    expansion_tab[3,3]:=chr(0); expansion_tab[4,3]:=chr(0);
    expansion_tab[5,3]:=chr(2); expansion_tab[6,3]:=chr(0);
    expansion_tab[8,3]:=chr(2); expansion_tab[4,4]:=chr(0);
    expansion_tab[6,4]:=chr(2); expansion_tab[8,4]:=chr(2);
    expansion_tab[6,5]:=chr(0); expansion_tab[8,5]:=chr(2);
    expansion_tab[5,6]:=chr(0); expansion_tab[6,6]:=chr(0);
    expansion_tab[8,6]:=chr(0); expansion_tab[6,7]:=chr(0);
    expansion_tab[8,7]:=chr(0); expansion_tab[8,8]:=chr(0);
  end.

