	(* NS16032 Assembler module scanr.e *)
module scanr; {scan routines}

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

const SCCSId = Addr('@(#)scanr.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'mnem_def.e'; ! DEPENDENCY
import 'scantoken.e'; ! DEPENDENCY
import 'prsymtable.e'; ! DEPENDENCY
import 'scan.e'; ! DEPENDENCY
import 'scanexp.e'; ! DEPENDENCY
import 'dirs.e'; ! DEPENDENCY
import 'sdis.e'; ! DEPENDENCY
import 'disp.e'; ! DEPENDENCY
export 'scanr.e'; ! DEPENDENCY

type alpha_word = packed array[1..8] of char;
var scan_errfound:boolean;



  export procedure reset_estack;

    begin
      stack_start := current_stack_rec^.next_stack_rec;
    end;
  
  export function scanglobsymbol : symptr;

    var curtoken : token;
	t_inf:type_info_ptr;
	s:symptr;

    begin
      nexttoken(curtoken);
      skipblanks;
      if current_char='{' then
	begin
	  scan_type_info(t_inf);
	  s:=processsymbol(curtoken);
	  new_name(t_inf^.t_name_ptr,t_inf^.t_name_ptr^,currsymname2,g_symnames);
	  s^.scope.domain:=s^.scope.domain|t_inf^.t_doub_word;
	  ins(s^.scope.domain,4,1,1);
	  scanglobsymbol:=s
	end
      else
        scanglobsymbol := processsymbol(curtoken);
    end;

  export procedure scan_const_expr(var result : factorrec);

    var t : termptr;

    begin
      resetesp := true;
      sdi_flag := false;
      result.factortype := fnill;
      t := simpleexp;
      if not error_flag then
        begin
          resolve_expression(t,result);
          if not error_flag then
            if result.factortype <> fgen then
              begin
                reporterror(100*129+illegal_expression);
              end
            else result.fgenref.start_of_stack := t;
        end
    end;

  export procedure scan_const(var temp : displacement);
    
    var t      : termptr;
        result : factorrec;

    begin
      temp.varying := false;
      temp.disp_value := 1;
      resetesp := true;
      sdi_flag := false;
      t := simpleexp;
      if not error_flag then
        begin
          resolve_expression(t,result);
          if not error_flag then
            begin
              if result.factortype = fsym then
                begin
                  if currentmode = progrel then
                    begin
                      temp.disp_type := const_value;
                      temp.varying := true;
                      temp.disp_value := 0;
                      temp.disp_expr := t;
                      reset_estack;
                    end
                  else 
                    begin
                      reporterror(100*130+mode_not_progrel);
                      reporterror(100*131+illegal_expression);
                    end
                end
              else if (result.factortype = fgen) and 
                      (result.fgenref.gtype = immediate) then
                begin 
                  temp.disp_type := const_value;
                  if sdi_flag then
                    begin
                      if result.fgenref.numtype=inti then
                        begin
                          temp.varying := true;
                          temp.disp_const_type:=inti;
                          temp.disp_value := result.fgenref.imint;
                          temp.disp_expr := t;
                          reset_estack;
                        end
                      else
                        begin temp.varying:=true;
                          temp.disp_const_type:=floati;
                          temp.disp_float_format:=long_float;
                          temp.long_float_value:=result.fgenref.imfloat;
                          temp.disp_expr := t;
                          reset_estack;
                        end
                    end
                  else
                    begin
                      if result.fgenref.numtype=inti then
                        begin
                          temp.disp_const_type:=inti;
                          temp.disp_value := result.fgenref.imint
                        end
                      else
                        begin temp.disp_const_type:=floati;
                          temp.disp_float_format:=long_float;
                          temp.long_float_value:=result.fgenref.imfloat
                        end
                    end
                end
              else if (result.factortype = fgen) and 
                      (result.fgenref.gtype = stringg) then
                begin
                  temp.disp_type := string_value;
                  temp.str_rec := result.fgenref.string_rec
                end
              else if (result.factortype = fgen) and 
                      (result.fgenref.gtype = absolute) then
                begin
                  temp.disp_type := const_value;
                  temp.disp_const_type:=inti;
                  temp.disp_value := result.fgenref.absnum;
                end
              else 
                begin
                  reporterror(100*132+illegal_expression);
                end;
            end;
        end;
    end;

  export procedure scan_immed_expr(var temp : displacement);

    begin
    begin
      skipblanks;
      if (current_char = eol) or (current_char=';') then
        begin
          temp.varying := false;
          temp.disp_value := 1;
        end
      else
        begin
          scan_const(temp);
          if temp.disp_type = string_value then
            begin
              temp.disp_value := convert_string(temp.str_rec);
              temp.disp_type := const_value;
              temp.varying := false;
            end
          else if temp.disp_value < 1 then
            begin
              reporterror(100*133+illegal_expression);
            end;
        end;
    end;
    end;

    export function scan_reg : reglist;

      var sptr  : symptr; temp:reglist;

      begin
        scan_reg := r0;
        with currtoken do
        begin
          nexttoken(currtoken);
          if tokentype = symbol then
            begin
              sptr := processsymbol(currtoken);
              with sptr^ do
              begin
                if symdefined[0] = chr(ord(true)) then
                  begin
                    assignlineref(sptr);
                    if (symbol_definition.gtype = register) and 
                      (symbol_definition.rtype = gregs) then
                      scan_reg := ord(symbol_definition.reg[0])::reglist
                    else reporterror(100*134+illegal_term);
                  end
                else reporterror(100*135+illegal_term)
              end
            end
          else if tokentype = restoken then
            begin
              if rtmnemonics[rtokenindex].rtokentype = regtokens then
                scan_reg := ord(rtmnemonics[rtokenindex].rtreg[0])::reglist
              else reporterror(100*136+illegal_term);
            end
          else reporterror(100*137+illegal_term);
        end
      end;

    export function scan_freg : freglist;

      var sptr  : symptr;

      begin
        scan_freg := f0;
        with currtoken do
        begin
          nexttoken(currtoken);
          if tokentype = symbol then
            begin
              sptr := processsymbol(currtoken);
              with sptr^ do
              begin
                if symdefined[0] = chr(ord(true))  then
                  begin
                    assignlineref(sptr);
                    if (symbol_definition.gtype = register) and 
                      (symbol_definition.rtype = fregs) then
                      scan_freg := ord(symbol_definition.freg[0])::freglist
                    else reporterror(100*138+illegal_term);
                  end
                else reporterror(100*139+illegal_term)
              end
            end
          else if tokentype = restoken then
            begin
              if rtmnemonics[rtokenindex].rtokentype = fregtokens then
                scan_freg := ord(rtmnemonics[rtokenindex].rtfreg[0])::freglist
              else reporterror(100*140+illegal_term);
            end
          else reporterror(100*141+illegal_term);
        end
      end;

  procedure scan_scale(var scale_ref : scaled);

    var sptr : symptr;
        tempchar : char;

    begin
      scale_ref.sreg[0] := chr(ord(scan_reg()));
      with currtoken do
        begin
          nexttoken(currtoken);
          if (tokentype=specchar) and (spch=':') then
            begin
              tempchar := getlchar;
              if ((tempchar = 'b')or(tempchar = 'B')) then
                    scale_ref.satt[0] := chr(ord(b))
              else if ((tempchar = 'd')or(tempchar = 'D')) then
                    scale_ref.satt[0] := chr(ord(d))
              else if ((tempchar = 'w')or(tempchar = 'W')) then
                    scale_ref.satt[0] := chr(ord(w))
              else if ((tempchar = 'q')or(tempchar = 'Q')) then
                    scale_ref.satt[0] := chr(ord(q))
              else reporterror(100*142+invalidchar);
              nexttoken(currtoken);
            end
          else reporterror(100*143+invalidchar);
          if not error_flag then
           scale_ref.scalestate := true;
        end;
    end;

  procedure check_for_scale(var refgen : gen);

    begin
      if (currtoken.tokentype = specchar) and (currtoken.spch = '[') then
        begin
          with refgen do
          if gtype = immediate then
            reporterror(100*144+illegal_term)
          else scan_scale(gen_scale);
          if not((currtoken.tokentype=specchar)and(currtoken.spch=']')) then
            reporterror(100*145+invalidchar)
          else nexttoken(currtoken);
        end
      else with refgen do
        gen_scale.scalestate := false;
    end;

  export procedure scan_general_expr(var operand : gen);
    
    var t:stackptr;
        regrec : rtentry;
        result : factorrec;onenumber:integer;

    begin
      resetesp := true;onenumber:=1;
      t := simpleexp;
      sdi_flag := false;
      if not error_flag then
        begin  
          resolve_expression(t,result);
          if not error_flag then
            if result.factortype = fsym then
              with operand do
              begin
                gtype := unresolved;
                start_of_stack := t;
                reset_estack;
              end
            else if result.factortype = fres then
              begin
                regrec := rtmnemonics[result.fresindex];
                if ((regrec.rtokentype = regtokens) or
                    (regrec.rtokentype = fregtokens)) then
                  with operand do
                  begin
                    gtype := register;
                    if regrec.rtokentype = regtokens then
                      begin
                        rtype := gregs;
                        reg := regrec.rtreg;
                      end
                    else
                      begin
                        rtype := fregs;
                        freg := regrec.rtfreg;
                      end
                  end
                else reporterror(100*146+illegal_term) 
              end
            else 
              begin
                if result.factortype <> fnill then
                  begin
                    operand := result.fgenref;
                    if operand.gtype = stringg then
                      begin
                        operand.imint := convert_string(operand.string_rec);
                        operand.numtype := inti;
                        operand.immatt.attribstate[0] := chr(ord(false));
                        if (operand.imint > -129) or (operand.imint < 256) then
                          operand.immatt.attrib[0] := chr(ord(b))
                        else if (operand.imint>-8192) 
                          or (operand.imint<65536) then
                          operand.immatt.attrib[0] := chr(ord(w))
                        else operand.immatt.attrib[0] := chr(ord(d));
                        operand.gtype := immediate;
                      end;
                    operand.start_of_stack := t;
                  end
              end;
          check_for_scale(operand);
        end;
    end;

  export procedure scan_equ_operand(var refgen : gen);

    begin
      scan_general_expr(refgen);
      with refgen do begin {ming}
         if gtype =regrelative then 
            if ((rltype <> rn) and (rltype <> rpc))  then
                if ((regreloffset < -16777215) or (regreloffset > 16777215)) 
                    then reporterror(1000*100);
      end; {with} {ming}
      if sdi_flag then reset_estack;
      if refgen.gtype = unresolved then
        reporterror(100*147+illegal_term)
      
    end;

  export procedure scan_const_operand(var refgen : gen);

    begin
      refgen.gtype := register;
      scan_general_expr(refgen);
      if refgen.gtype = stringg then
            begin
              refgen.imint := convert_string(refgen.string_rec);
              refgen.numtype := inti;
              refgen.immatt.attribstate[0] := chr(ord(false));
              refgen.gtype := immediate;
            end
      else if not((refgen.gtype=immediate) or 
                (refgen.gtype=unresolved)) then
        reporterror(100*148+illegal_term);
    end;
    
  export procedure scan_ext_operand(var refgen : gen);

    begin
      process_cxp := true;
      scan_general_expr(refgen);
      process_cxp := false;
      if not((ext_to_sbrelative and ((refgen.gtype=unresolved)or
				     (  process_d_cxp and 
				        (refgen.gtype=regrelative) and
				        (refgen.rltype=sb)  ) or
				     (  (not process_d_cxp) and
					(refgen.gtype=memoryrelative) and
					(refgen.memreg[0]=chr(ord(sb))) ) ) ) or
             ((not ext_to_sbrelative) and ((refgen.gtype=unresolved) or
					   (refgen.gtype=externalg))))  then   
        reporterror(100*149+illegal_term);
    end;

  export procedure scan_short_operand(var refgen : gen);

    begin
      scan_general_expr(refgen);
      if refgen.gtype = immediate then 
        begin
          if sdi_flag then
            begin
              reset_estack;
              refgen.gtype := unresolved
            end
          else if not((refgen.imint > -9) and (refgen.imint < 8)) then
            reporterror(100*150+illegal_expression)
          end
      else if not(refgen.gtype = unresolved) then
        reporterror(100*151+illegal_term);
    end;

    export procedure scan_label_operand(var labelop : gen);
    
    var t :termptr;
        exp : expptr;
        result : factorrec;

    begin
      sdi_flag := false;
      resetesp := true;
      t := simpleexp;
      if not error_flag then
        begin
          resolve_expression(t,result);
          result.fgenref.gen_scale.scalestate := false;
          if not error_flag then
            if result.factortype = fgen then
              begin
                if (result.fgenref.gtype = regrelative) (*and
                 ((result.fgenref.rltype=pc)or(result.fgenref.rltype=rpc))*)then
                    labelop := result.fgenref
                else reporterror(100*152+illegal_expression);
                labelop.start_of_stack := t;
              end
            else if result.factortype = fsym then
              begin
                (* construct gen for unresolved
                   create new estack *)
                labelop.gtype := unresolved;
                labelop.gen_scale.scalestate := false;
                labelop.start_of_stack := t;
                reset_estack;
              end
            else reporterror(100*153+illegal_expression);
        end;
    end;

    export procedure scan_preg(var proc_reg : pchar {preglist});

      var temptoken : token;

      begin
        nexttoken(temptoken);
        with temptoken do
        if tokentype = restoken then 
          begin
            if rtmnemonics[rtokenindex].rtokentype = pregtokens then
              if (ord(rtmnemonics[rtokenindex].rtpreg[0])::preglist) 
                                in [sp,sb,modreg,intbase,psr,fp,upsr] then
                proc_reg := rtmnemonics[rtokenindex].rtpreg
              else reporterror(100*154+illegal_term)
            else if rtmnemonics[rtokenindex].rtokentype = optokens then
              if rtmnemonics[rtokenindex].rtops[0] = chr(ord(modop)) then
                proc_reg[0] := chr(ord(modreg))
              else reporterror(100*155+illegal_term);
          end
        else reporterror(100*156+illegal_term)
      end;

    export procedure scan_mreg(var memreg : pchar {mreglist});

      var temptoken : token;

      begin
        nexttoken(temptoken);
        memreg[0] := chr(ord(bpr0));
        with temptoken do
        if tokentype = restoken then
          if rtmnemonics[rtokenindex].rtokentype = mregtokens then
            memreg := rtmnemonics[rtokenindex].rtmreg
          else reporterror(100*157+illegal_term)
        else reporterror(100*158+illegal_term)
      end;

    export procedure scan_cmfi(var temp : pchar {cfmi_set});

      var tchar : char;

      begin
        skipblanks;
        temp[0] := chr(0);
	if current_char='[' then
	  begin tchar:=getlchar; skipblanks
	  end
	else if not BACK_COMPATIBLE then reporterror(304*100+back_only);
        repeat
          skipblanks;
          if not((current_char = ']') or (current_char = ';') or 
                 (current_char = chr(10))) then
            begin
              tchar := getlchar;
              if (tchar = 'c')or(tchar='C') then
                temp[0]:= chr(ord(temp[0]) | {[cdsp]} (1 << ord(cdsp)))
              else if (tchar = 'm')or(tchar='M') then
                temp[0] := chr(ord(temp[0]) | {[mmu]} (1 << ord(mmu)))
              else if (tchar = 'f')or(tchar='F') then
                temp[0] := chr(ord(temp[0]) | {[fpu]} (1 << ord(fpu)))
              else if (tchar = 'i')or(tchar='I') then
                temp[0] := chr(ord(temp[0]) | {[ic]} (1 << ord(ic)))
              else reporterror(100*159+illegal_term);
            end;
          nexttoken(currtoken);
        until not((currtoken.tokentype=specchar) and (currtoken.spch=','));
	if (currtoken.tokentype=specchar)and(currtoken.spch=']') then 
	  nexttoken(currtoken)
	else if not BACK_COMPATIBLE then reporterror(305*100+back_only)
      end;
    
    export procedure scan_reg_list(var gen_regs : pchar {regset});

      var tchar : char;
          temp  : integer; {kludge}

      begin
        skipblanks;
        {gen_regs := [];}
        temp := 0;
        if current_char = '[' then
          begin
            lastpos := charpos;
            tchar := getlchar;
  	    skipblanks;
	    if current_char<>']' then
              begin
		repeat
                  temp := temp | (%80 >> ord(scan_reg()));
                  nexttoken(currtoken);
                until not((currtoken.tokentype=specchar)and(currtoken.spch=','));
                if not((currtoken.tokentype=specchar) and (currtoken.spch=']')) 
		then reporterror(100*160+invalidchar);
	      end
	    else  tchar:=getlchar;
          end
	else 
	  begin
	     if not BACK_COMPATIBLE then reporterror(303*100+back_only);
	  end;
        gen_regs[0] := chr(temp);
      end;



export function getword:alpha_word;
  var i:integer;
    dn:boolean;
    dd,jc:char;
    gw:alpha_word;
  function lc(c:char):char;
    begin 
      if (c>='A') and (c<='Z') then c:=chr(ord(c)+32);
      lc:=c
    end;
  begin
    if (current_char=' ') or (current_char=chr(9)) then skipblanks;
    dd:=lc(current_char);
    if (dd<'a')or(dd>'z') then
      begin getword:='********';gw:='********';
        exit getword
      end;
    for i:=1 to 8 do gw[i]:=' ';
    i:=1;dn:=false;
    repeat
      gw[i]:=lc(current_char);jc:=getlchar;
      i:=i+1;dd:=lc(current_char);
      if (i>8) or (dd<'a') or (dd>'z') then dn:=true;
    until dn;
    getword:=gw;
  end; (* getword *)

function get_pos_num:integer;
  var nn:integer;c:char;dn:boolean;
  begin if (current_char=' ')or(current_char=chr(9)) then skipblanks;
    dn:=false;nn:=0;
    if not((current_char<='9')and(current_char>='0')) then nn:=-1;
    while not dn do
     begin
      if (current_char<='9') and (current_char>='0') then
        begin nn:=10*nn+ord(current_char)-ord('0');
          c:=getlchar; if nn>128 then dn:=true
        end
      else 
        dn:=true;
     end;
    if nn>127 then nn:=-1;
    get_pos_num:=nn;
  end;
   

export procedure scan_type_info(var t_inf:type_info_ptr);
  sti,wn,wn2,modf_1,modf_2,num1,num2:integer;ttoken:token;
  wd:alpha_word; tchar:char;

  procedure founderror(n:integer);
    begin if not scan_errfound then reporterror(100*161+type_info_error);
      writeln('type info error ',n);
      scan_errfound:=true;
    end;
  begin
    tchar:=getlchar; scan_errfound:=false;
    wd:=getword;new(t_inf);t_inf^.t_name_ptr:=nil;
    if (wd='tn      ')or(wd='ptn     ')or(wd='etn     ') then
      begin
        if (wd='tn      ') then sti:=1
        else if (wd='ptn     ') then sti:=2
        else if (wd='etn     ') then sti:=3;
        wd:=getword;
        if (wd='ex      ') then begin ins(sti,4,1,1);wd:=getword end
        else if (wd='end     ') then begin ins(sti,3,1,1);wd:=getword end
        else if(wd='more    ') then begin ins(sti,5,1,1);wd:=getword end;
        if (wd='end     ') then begin ins(sti,3,1,1);wd:=getword end
        else if (wd='ex      ') then begin ins(sti,4,1,1);wd:=getword end
        else if(wd='more    ') then begin ins(sti,5,1,1);wd:=getword end;
        if (wd='********')and(current_char=',') then
          begin
	    tchar:=getlchar;
	    nexttoken(ttoken);
	    if ttoken.tokentype<>symbol then founderror(0)
            else
	      t_inf^.t_name_ptr:=process_typename(ttoken.sym)
	  end
        else founderror(1) 
      end
    else if (wd='aux     ')or(wd='maux    ') then
      begin 
        if      (wd='aux     ') then begin sti:=7;wd:=getword end
        else if (wd='maux    ') then begin sti:=6;wd:=getword end;
        if      (wd='txt     ') then ins(sti,9,7,1)
        else if (wd='dat     ') then ins(sti,9,7,2)
        else if (wd='com     ') then ins(sti,9,7,3)
        else if (wd='arbnd   ') then ins(sti,9,7,7)
        else if (wd='ldisp   ') then ins(sti,9,7,5)
        else if (wd='module  ') then ins(sti,9,7,0)
        else if (wd='life    ') then ins(sti,9,7,6)
        else founderror(2);
        if not scan_errfound then 
          begin wd:=getword;
             repeat
               if wd='ex      ' then
                 begin ins(sti,4,1,1);wd:=getword end
               else if wd='end     ' then
                 begin ins(sti,3,1,1);wd:=getword end
               else if wd='more    ' then
                 begin ins(sti,5,1,1);wd:=getword end
               else if wd='init    ' then
                 begin ins(sti,6,1,1);wd:=getword end
               else if wd='rom     ' then 
                 begin ins(sti,8,1,1);wd:=getword end
               else if wd<>'********' then
                 begin founderror(4);wd:='********' end
             until wd='********';
          end
      end
    else
      begin 
        sti:=0;modf_1:=0;modf_2:=0;
        if wd='taux    ' then
          begin sti:=7;wd:=getword
          end;
        if (wd='ptrto   ')or(wd='funret  ')or(wd='arrayof ') then
          begin 
            if wd='ptrto   ' then modf_1:=1 else
            if wd='funret  ' then modf_1:=2 else
            if wd='arrayof ' then modf_1:=3;
            wd:=getword
          end;
        if (wd='ptrto   ')or(wd='funret  ')or(wd='arrayof ') then
          begin 
            if wd='ptrto   ' then modf_2:=1 else
            if wd='funret  ' then modf_2:=2 else
            if wd='arrayof ' then modf_2:=3;
            wd:=getword
          end;
        if wd='undef   ' then begin end
        else if wd='int     ' then ins(sti,20,4,1)
        else if wd='char    ' then ins(sti,20,4,2)
        else if wd='uint    ' then ins(sti,20,4,3)
        else if wd='sete    ' then ins(sti,20,4,4)
        else if wd='set     ' then ins(sti,20,4,5)
        else if wd='float   ' then ins(sti,20,4,6)
        else if wd='dfloat  ' then ins(sti,20,4,7)
        else if wd='struct  ' then ins(sti,20,4,8)
        else if wd='union   ' then ins(sti,20,4,9)
        else if wd='enum    ' then ins(sti,20,4,10)
        else if wd='enume   ' then ins(sti,20,4,11)
        else if wd='bool    ' then ins(sti,20,4,12)
        else if wd='more    ' then ins(sti,20,4,13)
        else if wd<>'********' then founderror(5);
        if not scan_errfound then
          begin ins(sti,16,2,modf_2);
            ins(sti,18,2,modf_1);
            if (current_char=' ')or(current_char=chr(9)) then skipblanks;
            if current_char<>':' then 
	      begin ins(sti,24,8,32) ;
 		wd:=getword
	      end
	    else 
              begin 
		tchar:=getlchar;
                wd:=getword;
                if wd='b       ' then
                  begin ins(sti,24,8,8);wd:=getword end
                else if wd='w       ' then
                  begin ins(sti,24,8,16);wd:=getword end
                else if wd='d       ' then
                  begin ins(sti,24,8,32);wd:=getword end
                else if wd<>'********' then 
                  founderror (7)
                else
                  begin 
                    wn:=get_pos_num;
                    if (wn<0)or(wn>31) then founderror(8)
                    else
                      begin
                        if (current_char=' ')or(current_char=chr(9)) then
                                             skipblanks;
                        if current_char<>',' then founderror(9)
                        else
                          begin tchar:=getlchar;wn2:=get_pos_num;
                            if (wn2<0)or(wn2>7) then founderror(10)
                            else
                              begin wn:=wn+32*wn2;
                                ins(sti,24,8,wn);
                                wd:=getword;
                              end
                          end
                      end
                  end
              end;        
            repeat 
              if wd='ex      ' then 
                begin ins(sti,4,1,1);wd:=getword end
              else if wd='end     ' then 
                begin ins(sti,3,1,1);wd:=getword end
              else if wd='more    ' then
                begin ins(sti,5,1,1);wd:=getword end
              else if wd='begin   ' then
                begin ins(sti,6,1,1);wd:=getword end
              else if wd='f       ' then 
                begin ins(sti,8,1,1);
		  skipblanks;
                   wn:=get_pos_num;wd:=getword;
                   if wn>=0 then
                     ins(sti,9,7,wn)
                   else
                     founderror(11);
                end
              else if wd<>'********' then
                begin founderror(12);
                   wd:='********';
                end
            until wd='********';
          end
      end;
    t_inf^.t_doub_word:=sti;
    if (current_char=' ')or(current_char=chr(9)) then skipblanks;
    if (current_char='}') then tchar:=getlchar else founderror(13);
  end;



  export procedure scanlabel(ltoken : token;t_inf:type_info_ptr);

    var
      lptr      : symptr;
      tchar     : char;

    begin
      tchar := getlchar; {consume ':'}
      lptr := processsymbol(ltoken);
      if lptr <> nil then
        with lptr^ do
        if (symdefined[0] = chr(ord(true))) and
	   (not((symbol_definition.gtype=externalg) and
	   (symbol_definition.ext_proc[0] = chr(ord(1))))) then
          begin
            reporterror(100*162+duplicate_def)
          end
        else
          with lptr^ do
          begin
            scope.parent := symroot;
            if t_inf<>nil then scope.domain:=scope.domain|t_inf^.t_doub_word;
	    type_of_symbol:=t_inf;
            symdefined[0] := chr(ord(true));
            linedefined := linenumber;
            with symbol_definition do
              begin
                gtype := regrelative;
                regreloffatt.attribstate[0] := chr(ord(false));
                regreloffset := locationcounter;
                check_range(regreloffatt,regreloffset);
                nextsym := nil;
                case currentmode of
                  progrel : begin
                              rltype := pc;
                              if last_pc_symbol <> nil then
                                last_pc_symbol^.nextsym := lptr
                              else first_pc_symbol := lptr;
                              last_pc_symbol := lptr;
                              sdi_count := total_sdi_count;
			      (*sdi_gnum:=sdi_count;*)
                              locdir_number := total_loc_dirs;
                            end;
                  stbsrel : rltype := sb;
                  frrel   : rltype := fp;
                  stckrel : rltype := sp;
                  ab      : begin
                              gtype := absolute;
                              absnum := locationcounter;
                              absatt.attribstate[0] := chr(ord(false));
                              check_range(absatt,absnum);
                            end;
                  extg    : begin
                              gtype := externalg;   (* WARNING attrib's uninit*)
                              extoffset := locationcounter;
                              extindex := currentlinkpage;
                            end;
                end;
              end;
            labelptr := lptr;
	    freelabel:=lptr;
	    this_symbol:=lptr;
            if current_char = ':' then
              begin
                makepublic(lptr);
                if (t_inf<>nil) then ins(t_inf^.t_doub_word,4,1,1);
		if t_inf<>nil then new_name(t_inf^.t_name_ptr,
				t_inf^.t_name_ptr^,currsymname2,g_symnames);
		if lptr^.to_be_externed[0]=chr(ord(true)) then
                	produce_external_entry (lptr);
                lastpos := charpos;
                tchar := getlchar
              end
            else if current_char = '-' then
              begin
                makepublic (lptr);
                if (t_inf<>nil) then ins(t_inf^.t_doub_word,4,1,1);
		if t_inf<>nil then new_name(t_inf^.t_name_ptr,
			t_inf^.t_name_ptr^,currsymname2,g_symnames);
                produce_external_entry (lptr);
                lastpos := charpos;
                tchar := getlchar;
              end
	    else 
	      if lptr^.to_be_externed[0]=chr(ord(true)) then
		begin 
		  produce_external_entry(lptr);
		end;
          end;
    end;

  export procedure scancomment;



    begin
      with currtoken do
          begin
            if not((tokentype = specchar) and (spch = ';')) then
              nexttoken(currtoken);
            if tokentype = specchar then
              if spch = ';' then
              else
                begin
                  if spch <> eol then
                      reporterror(100*163+invalidchar);
                end
            else
                reporterror(100*164+invalidchar);
          end;
      processeoln
    end;

  begin 
  end.

