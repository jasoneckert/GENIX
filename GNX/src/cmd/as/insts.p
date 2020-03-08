
module insts;

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

const SCCSId = Addr('@(#)insts.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

! routines for first phase of instructions

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'mnem_def.e'; ! DEPENDENCY
import 'scantoken.e'; ! DEPENDENCY
import 'prsymtable.e'; ! DEPENDENCY
import 'scanr.e'; ! DEPENDENCY
import 'scan.e'; ! DEPENDENCY
import 'scanexp.e'; ! DEPENDENCY
import 'sdis.e'; ! DEPENDENCY
import 'genrout.e'; ! DEPENDENCY
import 'bio.e'; ! DEPENDENCY
import 'init1.e'; ! DEPENDENCY
import 'cmpc.e'; ! DEPENDENCY
import 'float.e'; ! DEPENDENCY
import 'disp.e'; ! DEPENDENCY
import 'dirs.e'; ! DEPENDENCY
{!L}

export 'insts.e'; ! DEPENDENCY

  var gen1,gen2 : genptr;
      cxp_d_bwd,nul_bwd, true_bwd, cxp_bwd, lxp_bwd : attribute;



  procedure get_new_gen(var gptr : genptr);

    begin
      if gen1 <> nil then
	begin
	  gptr := gen1; gen1 := nil;
	end
      else 
	begin
	  gptr := gen2; gen2 := nil;
	end;gptr^.start_of_stack:=nil
    end;

  procedure return_gen(var gptr : genptr);

    begin
      if gen1 = nil then
	gen1 := gptr
      else gen2 := gptr;
    end;

  function get_inst_index(var temptoken : token) i : integer;
     ! search for given instruction in the mnmemonics table
    var test,l,u : integer;
	tchr	 : char;
	temp     : integer;
	tempinst : packed array[1..maxinstlen] of char;
	found    : boolean;
        cmp_in   : integer;
    
    begin
      i := -1;
      if (temptoken.symlength <= maxinstlen) then
	begin
	  for temp := 1 to maxinstlen do
	    begin
	      tchr := temptoken.sym[temp];
	      if (tchr >= 'A') and (tchr <= 'Z') then
		tempinst[temp] := chr(ord(tchr) + (ord('a')-ord('A')))
	      else tempinst[temp] := tchr;
	    end;
	  l := opstart[ord(tempinst[1])];
	  if l <> -1 then
	    begin
	      temp := ord(tempinst[1]) + 1;
	      u := opstart[temp];
	      while u < 0 do
		begin
		  temp := temp + 1; u := opstart [temp];
		end;
	      found := false;
	      repeat
		i := (l + u) div 2;
		test := cmpc(maxinstlen,addr(tempinst),
					addr(mnemonics[i].instchars));
		if test < 0 then
		  u := i - 1
		else if test > 0 then
		  l := i + 1
		else found := true;
	      until found or (u < l);
	      if not found then 
		i := -1;
	    end
        end;
    end;

  procedure add_to_sdi_chain( exp     : stackptr;
			      gindex : irec;
			      op_size : integer;
			      op_num   : integer;
			      var op_bwd : attribute);
    var tmp_ptr : sdi_ptr;
    ! make an entry in sdi_chain, containing the linenumber, operand number,
    ! pointer to root of tree of expression etc.
    begin
      new_sdi_ptr(tmp_ptr);
      with tmp_ptr^ do
      begin
	loc_field := locationcounter;
        sdi_linnumber := linenumber;
	gen_rec := gindex;
	operand := exp;
	back_sdi := current_sdi;
	next_sdi := nil;
	opsize := op_size;
	locdir_number := total_loc_dirs;
	opnum := op_num;
	immed_bwd := op_bwd;
      end;
      if current_sdi = nil then
        sdi_start := tmp_ptr
      else
	begin
          current_sdi^.next_sdi := tmp_ptr;
          current_sdi^.sdi_chain := tmp_ptr;
	end;
      current_sdi := tmp_ptr;
    end;

  function check_for_sdi(var gref  : gen;
			 g_index   : irec;
			 var opsize : integer;
			 opnum      : integer;
			 var op_bwd : attribute) : boolean;

    var tmp_rec : stackptr;

    begin
      check_for_sdi := false;
      if gref.gtype = unresolved then
	begin
	  add_to_sdi_chain(gref.start_of_stack,g_index,opsize,opnum,op_bwd);
	  check_for_sdi := true;
	end
      else if sdi_flag then
	begin
	  tmp_rec := gref.start_of_stack; reset_estack;
	  add_to_sdi_chain(tmp_rec,g_index,opsize,opnum,op_bwd);
	  check_for_sdi := true;
	end;
    end;

  procedure perform_immed_check(instbwd : pchar {bwd};
				var gref : gen);
    begin
      with gref do
	if gtype = immediate then
	  begin
	    immatt.attribstate[0] := chr(ord(true));
	    case ord(instbwd[0])::bwd of
	 b : begin
	       immatt.attrib[0] := chr(ord(b));
	       if (imint < -128) or (imint > 255) then
	         reporterror(100*079+termsizeextreme);
	     end;
	 w : begin
	       immatt.attrib[0] := chr(ord(w));
	       if (imint < -32768) or (imint > 65535) then
	         reporterror(100*080+termsizeextreme);
	     end;
         d : immatt.attrib[0] := chr(ord(d));
	    end;
	  end;
    end;

  function get_op_size(var gref : gen;
		       var opbwd : attribute) : integer;

    var temp : integer;

    begin
      case gref.gtype of
	register	: get_op_size := 0;
	regrelative	: begin
			    if gref.rltype = pc then
			     gref.regreloffset:=gref.regreloffset -
						 locationcounter;
			    check_range(gref.regreloffatt,
					     gref.regreloffset);
			    get_op_size := get_size(gref.regreloffatt);
			  end;
	memoryrelative  : get_op_size := 
			    get_size(gref.memoffatt2)+get_size(gref.memoffatt1);
	tos		: get_op_size := 0;
	externalg	: begin 
			    temp := get_size(gref.extinxatt);
			    if opbwd.attrib[0] <> chr(ord(cx)) then
			      temp := temp + get_size(gref.extoffatt);
			    get_op_size := temp;
			  end;
	absolute	: begin
			    check_range(gref.absatt,gref.absnum);
			    get_op_size := get_size(gref.absatt);
			  end;
	immediate	: get_op_size := get_size(gref.immatt);
	unresolved	: get_op_size := 1;
      end;
    end;

  export procedure check_operand(var gref : gen;
			  var opsize : integer;
			  g_index : irec;
			  opnum	  : integer;
			  var op_bwd : attribute);
    ! get the size of operand. if operand is sdi, put it in the chain
    begin
      opsize := get_op_size(gref,op_bwd); 
      if check_for_sdi(gref,g_index,opsize,opnum,op_bwd) then
	total_sdi_count := total_sdi_count + 1;
      if gref.gen_scale.scalestate then
	begin
	  opsize := opsize + 1;
	end;
    end;
	
  procedure check_float_operand(var gref : gen;
			  var opsize : integer;
			  g_index : irec;
			  opnum	  : integer;
			  var op_bwd : attribute);

    begin
      opsize := 4;
      if check_for_sdi(gref,g_index,opsize,opnum,op_bwd) then
	total_sdi_count := total_sdi_count + 1;
      if gref.gen_scale.scalestate then
	begin
	  opsize := opsize + 1;
	end;
    end;

  procedure check_long_operand(var gref : gen;
			  var opsize : integer;
			  g_index : irec;
			  opnum	  : integer;
			  var op_bwd : attribute);

    begin
      opsize :=  8;
      if check_for_sdi(gref,g_index,opsize,opnum,op_bwd) then
	total_sdi_count := total_sdi_count + 1;
      if gref.gen_scale.scalestate then
	begin
	  opsize := opsize + 1;
	end;
    end;

  procedure check_long_reg(gref : gen; count : integer; inst_index : integer);
    var checkchar : char;
	evenreg   : integer;

    begin
      checkchar := mnemonics[inst_index].instchars[count];
      if ((checkchar = 'l') or (checkchar = 'L')) and (gref.gtype = register)
	then
          begin
       	    if gref.rtype = gregs
	      then evenreg := ord(gref.reg[0])
	      else evenreg := ord(gref.freg[0]);
	    if (evenreg mod 2) <> 0
	      then reporterror(225*100+even_register_expected);
	  end;
    end;

  procedure processfmt0(inst_index : integer; g_index : irec);
    var opsize : integer;

    begin
      with global_interm_rec do
	begin
          bcond := mnemonics[inst_index].f0cond;
	  bmode[0] := chr(ord(currentmode));
          scan_label_operand(blabel);
          check_operand(blabel,opsize,g_index,1,nul_bwd);
          increment_locationcounter(opsize+1)
	end;
    end;

  procedure processfmt1(inst_index : integer; g_index : irec);
    var opsize,index : integer;
        tempgen:gen;

    begin
      with global_interm_rec do
	begin
          f1op := mnemonics[inst_index].instf1;
          opsize := 0;
          case ord(f1op[0])::f1opcodes of
       BSR	: begin
		    scan_label_operand(f1operand);
      		    check_operand(f1operand,opsize,g_index,1,nul_bwd);
		  end;
       CXP	: begin
	 	    process_d_cxp:=ext_to_sbrelative;
		    scan_ext_operand(f1operand); 
		    if ext_to_sbrelative then
		      begin
			tempgen:=f1operand;
			process_d_cxp:=false;
			ftype:=format3;
			f3operand:=tempgen;
			f3op[0]:=chr(ord(CXPD));
			false_cxpd[0]:=chr(ord(true));
			f3x[0]:=chr(ord(d));
			true_bwd.attrib:=f3x;
			check_operand(f3operand,opsize,g_index,1,cxp_d_bwd);
			opsize:=opsize+1; (* diffrence between CXP and CXPD *)
		      end		  (* instruction sizes              *)
		    else
		      begin
		        if f1operand.gtype <> unresolved then
		          opsize := get_size(f1operand.extinxatt)
		        else opsize := 1;
		        if check_for_sdi(f1operand,g_index,
				     opsize,1,cxp_bwd) then
		        total_sdi_count := total_sdi_count + 1;
		      end;
		  end;
	   RET,RXP,RETT
	        : begin
		    scan_const_operand(f1operand);
      		    check_operand(f1operand,opsize,g_index,1,nul_bwd);
		  end;
	    SAVE,RESTORE,EXITo
	        : begin
		    scan_reg_list(f1regs);
		    opsize := 1;
		  end;
           ENTER: begin
		    scan_reg_list(f1regs);
		    skipblanks;
		    if (current_char=',') or (ord(f1regs[0]) <> 0)  then
		      scan_comma;
		    scan_const_operand(f1operand);
      		    check_operand(f1operand,opsize,g_index,1,nul_bwd);
		    opsize := opsize + 1;
		   end;
       NOP,WAIT,FLAG,SVC,BPT
                   : begin
		     end;
          end;
          increment_locationcounter(opsize+1)
	end;
    end;

  procedure processfmt2(inst_index : integer; g_index : irec);
    var opsize : integer;
	size2  : integer;

    begin
      with global_interm_rec do
	begin
          f2op := ord(mnemonics[inst_index].instf2[0])::f2opcodes;
          f2x := mnemonics[inst_index].f2bwd;
	  true_bwd.attrib := f2x;
          opsize := 0;
          case ord(f2op) of
	    (*ADDQx,CMPQx,MOVQx*)
	    0,1,2: begin
		    scan_short_operand(f2short);
		    check_for_comma;
		    scan_general_expr(f2operand);
		    !fixgen(f2operand);
		    perform_immed_check(f2x,f2operand);
      		    check_operand(f2operand,opsize,g_index,1,nul_bwd);
		  end;
      (*Scondx*)3:begin
      		    q1cond := mnemonics[inst_index].f2cond;
		    scan_general_expr(f2operand);
		    !fixgen(f2operand);
      		    check_operand(f2operand,opsize,g_index,1,nul_bwd);
		  end;
        (*ACBx*)4:begin
		    scan_short_operand(f2short);
		    check_for_comma;
		    scan_general_expr(f2operand);
		    !fixgen(f2operand);
      		    check_operand(f2operand,opsize,g_index,1,nul_bwd);
		    check_for_comma;
		    get_new_gen(f2label);
		    scan_label_operand(f2label^);
      		    check_operand(f2label^,size2,g_index,2,nul_bwd);
		    opsize := opsize + size2;
		  end;
    	    (*SPRx,,LPRx*)
	    5,6 : begin
		    scan_preg(preg);
		    scan_comma;
		    scan_general_expr(f2operand);
		    !fixgen(f2operand);
		    perform_immed_check(f2x,f2operand);
      		    check_operand(f2operand,opsize,g_index,1,true_bwd);
		  end;
          end;
          increment_locationcounter(opsize+2)
	end;
    end;

  procedure processfmt3(inst_index : integer; g_index : irec);
    var opsize : integer;
	temp   : integer;	(* slow down for debug purposes *)
    begin
      with global_interm_rec do
	begin
          f3op := mnemonics[inst_index].instf3;
          f3x := mnemonics[inst_index].f3bwd;
	  false_cxpd[0]:=chr(ord(false));
          true_bwd.attrib := f3x;
          scan_general_expr(f3operand);
	  !fixgen(f3operand);
          if (f3op[0] = chr(ord(ADJSPx))) or
	     (f3op[0] = chr(ord(BICPSRx))) or
	     (f3op[0] = chr(ord(BIsPSRx))) then
	    perform_immed_check(f3x,f3operand);
	  if f3op[0] = chr(ord(CASEx)) then (* for debug purposes *)
	    temp := 1;
          check_operand(f3operand,opsize,g_index,1,true_bwd);
          increment_locationcounter(opsize+2)
	end;
    end;
procedure dummy4;
   begin end;

  procedure processfmt4(inst_index : integer; g_index : irec);
    var opsize : integer;
	size2  : integer;
    begin
      with global_interm_rec do
	begin
          f4op := mnemonics[inst_index].instf4;
          f4x := mnemonics[inst_index].f4bwd;
          true_bwd.attrib := f4x;
	  if f4op[0] = chr(ord(LXPD)) then
	    begin !process_d_cxp:=ext_to_sbrelative;
	      scan_ext_operand(f4operand1);
	      process_d_cxp:=false
	    end
	  else scan_general_expr(f4operand1);
	  if linenumber>1975 then dispgen(5,linenumber,f4operand1);
	  !fixgen(f4operand1);
          if (f4op[0] <> chr(ord(ADDRo))) or (f4op[0] <> chr(ord(LXPD)))
	     or (f4op[0] <> chr(ord(ADDRx))) then
	    perform_immed_check(f4x,f4operand1);
	  if f4op[0] = chr(ord(LXPD)) then
	    check_operand(f4operand1,opsize,g_index,1,lxp_bwd)
	  else check_operand(f4operand1,opsize,g_index,1,true_bwd);
          check_for_comma;
          scan_general_expr(f4operand2);
	  !fixgen(f4operand2);
          if not((f4op[0] = chr(ord(CMPx))) or (f4op[0]=chr(ord(TBITx)))) then
	    begin
              if f4operand2.gtype = immediate then
	        reporterror(100*180+illegal_term);
	    end
	  else
	    perform_immed_check(f4x,f4operand2);
          check_operand(f4operand2,size2,g_index,2,true_bwd);
          increment_locationcounter(opsize+size2+2);
	end;
    end;

  procedure processfmt5(inst_index : integer; g_index : irec;sixth_char:char);
    var junkchar:char;
    begin
      with global_interm_rec do
	begin
          fop5 := ord(mnemonics[inst_index].instf5[0])::f5opcodes;
          if fop5 = SETCFG then
	    begin
	      scan_cmfi(stcfgoperand);
	    end
          else
	    begin
	      if (sixth_char<>chr(0))and(not BACK_COMPATIBLE) then
		reporterror(306*100+back_only);
              f5bwd := mnemonics[inst_index].f5bwd;
              f5bck := mnemonics[inst_index].f5bck;
              f5t := mnemonics[inst_index].f5t;
              f5uw := mnemonics[inst_index].f5uw;
              if (ord(f5bck[0])=ord(false))and(ord(f5uw[0])=ord(nouw)) then
                begin
                  skipblanks;
                      if (current_char='b')or(current_char='B') then
                    begin junkchar:=getschar;
                      f5bck[0]:=chr(ord(true));
                    end;
	 	  skipblanks;
		  if current_char=',' then
		    begin junkchar:=getschar; skipblanks
		    end;
                  if (current_char='u')or(current_char='U') then
                    begin junkchar:=getschar;
                       f5uw[0]:=chr(ord(untilmatch));
                    end
                  else
                        if (current_char='w')or(current_char='W') then
                      begin junkchar:=getschar;
                        f5uw[0]:=chr(ord(whilematch));
                      end;
                end
	    end;
          increment_locationcounter(3)
	end;
    end;

  procedure processfmt6(inst_index : integer; g_index : irec);
    var opsize : integer;
	size2  : integer;
    begin
      with global_interm_rec do
	begin
          f6op := mnemonics[inst_index].instf6;
          f6x := mnemonics[inst_index].f6bwd;
	  true_bwd.attrib := f6x;
          scan_general_expr(f6operand1);
	  !fixgen(f6operand1);
          perform_immed_check(f6x,f6operand1);
          check_operand(f6operand1,opsize,g_index,1,true_bwd);
	  if (f6op[0] = chr(ord(ROTx))) or (f6op[0] = chr(ord(LSHx))) or
	     (f6op[0] = chr(ord(ASHx))) then
	    if f6operand1.gtype = immediate then
	      opsize := 1;
          check_for_comma;
          scan_general_expr(f6operand2);
	  !fixgen(f6operand2);
          if f6operand2.gtype = immediate then
	    reporterror(100*181+illegal_term);
          check_operand(f6operand2,size2,g_index,2,nul_bwd);
          increment_locationcounter(opsize+size2+3)
	end;
    end;

  procedure processfmt7(inst_index : integer; g_index : irec);
    var opsize : integer;
	size2  : integer;
	size3  : integer;
    begin
      with global_interm_rec do
	begin
          fop7 := ord(mnemonics[inst_index].instf7[0])::f7opcodes;
          f7x := mnemonics[inst_index].f7bwd;
          true_bwd.attrib := f7x;
          scan_general_expr(f7operand1);
	  !fixgen(f7operand1);
          perform_immed_check(f7x,f7operand1);
          check_operand(f7operand1,opsize,g_index,1,true_bwd);
          check_for_comma;
          scan_general_expr(f7operand2);
	  !fixgen(f7operand2);
          check_operand(f7operand2,size2,g_index,2,true_bwd);
          size3 := 1;
          if (fop7 = MOVMx) or (fop7 = CMPMx) then
	    begin
              check_for_comma;
	      get_new_gen(f7constant);
              scan_const_operand(f7constant^);
	    end
          else if (fop7 = INSSx) or (fop7 =EXTSx) then
	    begin
              check_for_comma;
	      get_new_gen(f7constant1);
	      scan_short_operand(f7constant1^);
	      if f7constant1^.gtype = immediate then
	        if (f7constant1^.imint < 0) or
	           (f7constant1^.imint > 8) then
	          reporterror(100*081+termsizeextreme);
              check_for_comma;
	      get_new_gen(f7constant2);
	      scan_const_operand(f7constant2^);
	    end
          else
	    size3 := 0;
          increment_locationcounter(opsize+size2+size3+3)
	end;
    end;

  procedure processfmt8(inst_index : integer; g_index : irec);
    var opsize : integer;
	size2  : integer;
	size3  : integer;
    begin
      with global_interm_rec do
	begin
          fop8 := ord(mnemonics[inst_index].instf8[0])::f8opcodes;
          f8x := mnemonics[inst_index].f8bwd;
          true_bwd.attrib := f8x;
          if not((fop8 = FFSx) or (fop8 = MOVUSx) or
		 (fop8 = MOVSUx)) then
	    begin
	      f8reg[0] := chr(ord(scan_reg()));
              scan_comma;
	    end;
          scan_general_expr(f8operand1);
	  !fixgen(f8operand1);
          perform_immed_check(f8x,f8operand1);
          check_operand(f8operand1,opsize,g_index,1,true_bwd);
          check_for_comma;
          scan_general_expr(f8operand2);
	  !fixgen(f8operand2);
          perform_immed_check(f8x,f8operand2);
          check_operand(f8operand2,size2,g_index,2,true_bwd);
          if (fop8 = EXTs) or (fop8 = INSx) then
	    begin
              check_for_comma;
	      get_new_gen(f8disp);
              scan_const_operand(f8disp^);
              check_operand(f8disp^,size3,g_index,3,nul_bwd);
	    end
          else size3 := 0;
          increment_locationcounter(opsize+size2+size3+3)
	end;
    end;


  procedure processfmt9(inst_index : integer; g_index : irec);
    var opsize : integer;
	size2  : integer;
	checknum : integer;
    begin
      with global_interm_rec do
	begin
          fop9 := ord(mnemonics[inst_index].instf9[0])::f9opcodes;
          f9f := mnemonics[inst_index].f9fl;
          f9x := mnemonics[inst_index].f9bwd;
	  if fop9 = MOVLF
	    then checknum := 4
	    else if (fop9 = MOVxf) or (fop9 = MOVFL)
	      then checknum := 5
	      else checknum := 6;
          true_bwd.attrib :=  f9x;
          scan_general_expr(f9operand1);
	  !fixgen(f9operand1);
          if fop9 = MOVxf then 
          perform_immed_check(f9x,f9operand1);
          if (fop9 = MOVxf) or
             (fop9 = LFSR) or
             (fop9 = SFSR) or
             (f9operand1.gtype<>immediate) then
            begin
	      check_operand(f9operand1,opsize,g_index,1,true_bwd);
	      if (fop9 <> LFSR) and (fop9 <> SFSR) and
		 (fop9 <> MOVxf) and (fop9 <> MOVFL)
		then check_long_reg(f9operand1,checknum,inst_index)
	    end
          else if (f9operand1.numtype=inti) then
            reporterror(100*082+float_expected)
          else if (f9operand1.numtype=floati) then
	    if ord(f9f[0])=ord(f) then
              check_float_operand(f9operand1,opsize,g_index,1,true_bwd)
            else 
              check_long_operand(f9operand1,opsize,g_index,1,true_bwd);
          if (f9operand1.gtype=immediate) and
             (f9operand1.numtype=inti) then
            begin
	      if f9operand1.immatt.attrib[0] = chr(ord(b))
		then opsize := 1
	      else if f9operand1.immatt.attrib[0] = chr(ord(w))
		then opsize := 2
	      else opsize:=4;
            end;
          if (fop9=LFSR) or (fop9=SFSR) then
	    size2 := 0
          else 
	    begin
              check_for_comma;
              scan_general_expr(f9operand2);
	      !fixgen(f9operand2);
              check_operand(f9operand2,size2,g_index,2,true_bwd);
	      if (fop9 = MOVxf) or (fop9 = MOVFL)
		then check_long_reg(f9operand2,checknum,inst_index);
	    end;
          increment_locationcounter(opsize+size2+3);
	end;
    end;

  procedure processfmt10(inst_index : integer; g_index : irec);
    begin
      with global_interm_rec do
	begin
          fop10 := mnemonics[inst_index].instf10;
	end;
    end;

  procedure processfmt11(inst_index : integer; g_index : irec);
    var opsize : integer;
	size2  : integer;
	checknum : integer;
    begin
      with global_interm_rec do
	begin
          fop11 := mnemonics[inst_index].instf11;
          f11f := mnemonics[inst_index].f11fl;
	  if fop11[0] = chr(ord(FRACf))
	    then checknum := 5
	    else checknum := 4;
          scan_general_expr(f11operand1);
	  !fixgen(f11operand1);
          if (f11operand1.gtype=stringg) then
             reporterror(100*083+illegal_term);
          if (f11operand1.gtype=immediate) then
            if f11operand1.numtype=inti then reporterror(100*084+float_expected)
            else if f11f[0]=chr(ord(f)) then
              check_float_operand(f11operand1,opsize,g_index,1,nul_bwd)
            else check_long_operand(f11operand1,opsize,g_index,1,nul_bwd)
          else 
	    begin
	      check_operand(f11operand1,opsize,g_index,1,nul_bwd);
	      check_long_reg(f11operand1,checknum,inst_index)
	    end;;
          check_for_comma;
          scan_general_expr(f11operand2);
	  !fixgen(f11operand2);
          if(f11operand2.gtype=immediate)and
               (fop11[0]<>chr(ord(CMPf))) then reporterror(100*085+illegal_term)
          else if (f11operand2.gtype=immediate) and
               (f11operand2.numtype=inti) then reporterror(100*086+illegal_term)
          else if (f11operand2.gtype<>immediate)
                    then 
		      begin
			check_operand(f11operand2,size2,g_index,2,nul_bwd);
			check_long_reg(f11operand2,checknum,inst_index)
		      end
          else if f11operand2.numtype=floati
	    then if f11f[0]=chr(ord(f)) then
                   check_float_operand(f11operand2,size2,g_index,2,nul_bwd)
            else check_long_operand(f11operand2,size2,g_index,2,nul_bwd);
          increment_locationcounter(opsize+size2+3)
	end;
    end;

  procedure processfmt12(inst_index : integer; g_index : irec);
    var opsize : integer;
	size2  : integer;
    begin
      with global_interm_rec do
	begin
          fop12 := mnemonics[inst_index].instf12;
          f12f := mnemonics[inst_index].f12fl;
          f12reg[0] := chr(ord(scan_freg()));
          scan_comma;
          scan_general_expr(f12operand1);
	  !fixgen(f12operand1);
          check_operand(f12operand1,opsize,g_index,1,nul_bwd);
          check_for_comma;
          scan_general_expr(f12operand2);
	  !fixgen(f12operand2);
          if f12operand2.gtype = immediate then
	    reporterror(100*087+illegal_term);
          check_operand(f12operand2,size2,g_index,2,nul_bwd);
          increment_locationcounter(opsize+size2+3)
	end;
    end;
  procedure processfmt14(inst_index : integer; g_index : irec);
    var opsize : integer;
    begin
      with global_interm_rec do
	begin
          fop14 := mnemonics[inst_index].instf14;
          if (fop14[0] = chr(ord(LMR))) or (fop14[0] = chr(ord(SMR))) then
	    begin
              scan_mreg(f14mreg);
              scan_comma;
	    end;
          scan_general_expr(f14operand);
	  !fixgen(f14operand);
	  true_bwd.attrib[0] := chr(ord(d));
          if (f14operand.gtype = immediate) then
	    if ((fop14[0] = chr(ord(SMR))) or (fop14[0] = chr(ord(WRVAL)))) then
	      reporterror(100*088+illegal_term)
	    else perform_immed_check(true_bwd.attrib,f14operand);
          check_operand(f14operand,opsize,g_index,1,nul_bwd);
          increment_locationcounter(opsize+3)
	end;
    end;
  export procedure process_instruction(var temptoken : token);
    var inst_index : integer;
	g_index    : irec;
	currfrmt   : formats;
	mike_caca  : integer;
        sixth_char : char;
    begin
      sixth_char:=temptoken.sym[6];
      inst_index := get_inst_index(temptoken);
      if ((currentmode = progrel) or (currentmode = stbsrel)) 
	 and (inst_index <> -1) then
	  begin
            g_index := new_g_index;
	    global_interm_rec.irec_ptr := g_index;
            global_interm_rec.linenumber := linenumber;
            g_index^.location := locationcounter;
	    g_index^.locdir_number := total_loc_dirs;
            currfrmt := mnemonics[inst_index].instformat;
            global_interm_rec.ftype := currfrmt;
	    case currfrmt of
	      format0  : processfmt0(inst_index,g_index);
	      format1  : processfmt1(inst_index,g_index);
	      format2  : processfmt2(inst_index,g_index);
	      format3  : processfmt3(inst_index,g_index);
	      format4  : processfmt4(inst_index,g_index);
	      format5  : processfmt5(inst_index,g_index,sixth_char);
	      format6  : processfmt6(inst_index,g_index);
	      format7  : processfmt7(inst_index,g_index);
	      format8  : processfmt8(inst_index,g_index);
	      format9  : processfmt9(inst_index,g_index);
	      format10 : processfmt10(inst_index,g_index);
	      format11 : processfmt11(inst_index,g_index);
	      format12 : processfmt12(inst_index,g_index);
	      format14 : processfmt14(inst_index,g_index);
	    end;
            currfrmt:=global_interm_rec.ftype ;
            if vm_flag then begin
              g_intermrec := new_g_intermrec;
              g_intermrec^.interm_rec := global_interm_rec;
            end else 
              mike_caca :=
                fwrite(global_interm_rec,size(intermcode),1,ibuffer1);
	    if currfrmt = format2 then
	      begin
	        if global_interm_rec.f2op =(ord(ACBx)::f2opcodes) then
		  begin
                    if vm_flag then begin
                      gen_intermrec := new_g_gen;
                      gen_intermrec^.gen_rec := global_interm_rec.f2label^; 
                    end else 
                      mike_caca :=
                       fwrite(global_interm_rec.f2label^,size(gen),1,ibuffer1);
		    return_gen(global_interm_rec.f2label);
		  end
	      end
	    else if currfrmt = format7 then
	      begin
		if (global_interm_rec.fop7 = MOVMx) or
		   (global_interm_rec.fop7 = CMPMx) then
		  begin
                    if vm_flag then begin
                      gen_intermrec := new_g_gen;
                      gen_intermrec^.gen_rec := global_interm_rec.f7constant^; 
                    end else 
                      mike_caca :=
                       fwrite(global_interm_rec.f7constant^,size(gen),1,ibuffer1);
		    return_gen(global_interm_rec.f7constant);
		  end
		else if (global_interm_rec.fop7 = INSSx) or
			(global_interm_rec.fop7 = EXTSx) then
		  begin
                    if vm_flag then begin
                      gen_intermrec := new_g_gen;
                      gen_intermrec^.gen_rec := global_interm_rec.f7constant1^; 
                    end else 
                      mike_caca :=
                       fwrite(global_interm_rec.f7constant1^,size(gen),1,ibuffer1);
                    if vm_flag then begin
                      gen_intermrec := new_g_gen;
                      gen_intermrec^.gen_rec := global_interm_rec.f7constant2^; 
                    end else 
                      mike_caca :=
                       fwrite(global_interm_rec.f7constant2^,size(gen),1,ibuffer1);
		    return_gen(global_interm_rec.f7constant1); 
		    return_gen(global_interm_rec.f7constant2);
		  end
	      end
	    else if currfrmt = format8 then
	      begin
		if (global_interm_rec.fop8 = EXTs) or
		   (global_interm_rec.fop8 = INSx) then
		  begin
                    if vm_flag then begin
                      gen_intermrec := new_g_gen;
                      gen_intermrec^.gen_rec := global_interm_rec.f8disp^; 
                    end else 
                      mike_caca :=
                       fwrite(global_interm_rec.f8disp^,size(gen),1,ibuffer1);
		    return_gen(global_interm_rec.f8disp);
		  end
	      end;
	  end
      else reporterror(100*089+illegalinst);
    end;

  begin
    new(gen1); new(gen2);
    nul_bwd.attribstate[0] := chr(ord(false));
    cxp_bwd.attribstate[0] := chr(ord(false));
    cxp_bwd.attrib[0] := chr(ord(cx));
    lxp_bwd.attribstate[0] := chr(ord(false));
    lxp_bwd.attrib[0] := chr(ord(lx));
    true_bwd.attribstate[0] := chr(ord(true));
    cxp_d_bwd.attribstate[0]:=chr(ord(false));
    cxp_d_bwd.attrib[0]:=chr(ord(cx_d));
  end.

