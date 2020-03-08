module scanexp;

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

const SCCSId = Addr('@(#)scanexp.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'mnem_def.e'; ! DEPENDENCY
import 'scantoken.e'; ! DEPENDENCY
import 'scanr.e'; ! DEPENDENCY
import 'prsymtable.e'; ! DEPENDENCY
import 'scan.e'; ! DEPENDENCY
import 'dirs.e'; ! DEPENDENCY
import 'sdis.e'; ! DEPENDENCY
import 'scmpc.e'; ! DEPENDENCY
import 'stab.e'; ! DEPENDENCY
import 'disp.e'; ! DEPENDENCY


export 'scanexp.e'; ! DEPENDENCY

  var symbol_undefined : boolean;
      depth_gauge : integer;

  function create_stack_rec : stackptr;

    var new_rec : stackptr;

    begin 
      new(new_rec); new_rec^.next_stack_rec := nil;
      create_stack_rec := new_rec;
    end;



  function incrementesp : stackptr;

    var new_rec : stackptr;

    begin
      if resetesp then
        begin
          if stack_start = nil then
            begin
              new_rec := create_stack_rec;
              stack_start := new_rec;
            end
          else
            begin
              new_rec := stack_start;
            end;
          resetesp := false;
        end
      else
        begin
          if current_stack_rec^.next_stack_rec = nil then
            begin
              new_rec := create_stack_rec;
              current_stack_rec^.next_stack_rec := new_rec;
            end
          else
            begin
              new_rec := current_stack_rec^.next_stack_rec;
            end;
        end;
      current_stack_rec := new_rec;
      incrementesp := new_rec;
    end;

  export function newexp  nexp : stackptr;
    begin
      nexp := incrementesp; nexp^.stackrectype := exprec;
    end;

  export function newterm ntrm : stackptr;
    begin
      ntrm := incrementesp; ntrm^.stackrectype := termrec;
    end;

export procedure fixgen(var g:gen);
  var
    index,offset:integer;
    att:attribute;
  begin
    if ext_to_sbrelative then
      with g do
        if gtype=externalg then
          begin offset:=extoffset;
            index:=extindex;
            att:=extoffatt;
	    gtype:=memoryrelative;
	    memoffset2:=offset;
	    memoffset1:=-4*(index+1);
	    memoffatt2:=att;
	    memoffatt1.attribstate[0]:=chr(ord(false));
	    check_range(memoffatt1,memoffset1);
	    memreg[0]:=chr(ord(sb));
 	  end
  end;

procedure fix1gen(var g:gen);
  var index:integer;
  begin 
    if g.gtype = externalg then with g do
      if extoffset<>0 then reporterror(100*222+wrong_cxp)
      else
        begin
          index:=extindex;
	  gtype:=regrelative;
	  rltype:=sb;
	  regreloffset:=-4*(index+1);
	  regreloffatt.attribstate[0]:=chr(ord(false));
	  check_range(regreloffatt,regreloffset)
	end
  end;

  export procedure produce_external_entry(var sptr : symptr);

    var new_sptr : symptr;
        imp_ptr : importptr;
        c_test,cmp_in : integer;

    begin
      c_test := sptr^.scope.domain & N_EXT;
      makepublic(sptr); 
      if c_test = 0 then
        sptr^.scope.domain := sptr^.scope.domain & ~N_EXT;
      (* is there currently an import for this pc type? *)
      c_test := -1; imp_ptr := importstart;
      while ((c_test <> 0) and (imp_ptr <> nil)) do
        begin
          c_test := scmpc(addr(sptr^.symbolname^),
                          addr(imp_ptr^.importsptr^.symbolname^));
          if c_test <> 0 then
            imp_ptr := imp_ptr^.nextimportrec;
        end;
      if c_test <> 0 then
        begin
          newsymentry(new_sptr);
          new_sptr^.symbolname := sptr^.symbolname;
          make_external(new_sptr); 
          if (sptr^.scope.domain & N_EXT) = 0 then
            new_sptr^.symbol_definition.ext_proc[0] := chr(3);
          sptr := new_sptr;
        end
      else sptr := imp_ptr^.importsptr;
    end;

  export procedure construct_temp_symbol(var tempfactor : factorrec);

    var s_ptr,pc_ptr,lpc_ptr : symptr;

    begin
          newsymentry(s_ptr);
          tempfactor.factortype := fsym;
          tempfactor.fsymbol := s_ptr;
          with s_ptr^ do
            begin
              with symbol_definition do
              begin
                symdefined[0] := chr(ord(true));
                gtype := regrelative;
                regreloffset := locationcounter; 
                regreloffatt.attribstate[0] := chr(ord(false));
                locdir_number := total_loc_dirs;
                nextsym := nil;
                if labelptr=nil then rltype := pc else rltype:=pc;
		if equ_flag then
		  begin
		    pc_ptr:=first_pc_symbol;
		    lpc_ptr:=nil;
		    if pc_ptr<>nil then
		      while pc_ptr<>labelptr do
		        begin lpc_ptr:=pc_ptr;pc_ptr:=pc_ptr^.nextsym
	 	        end;
		    if lpc_ptr<>nil then
		      begin 
		        lpc_ptr^.nextsym:=s_ptr;
		        s_ptr^.nextsym:=labelptr
		      end
		    else
		      begin 
		        first_pc_symbol:=s_ptr;
		        s_ptr^.nextsym:=labelptr
		      end;
 		  end
		else
		  begin
		    if last_pc_symbol<>nil then
		      last_pc_symbol^.nextsym:=s_ptr
		    else first_pc_symbol:=s_ptr;
		    last_pc_symbol:=s_ptr
		  end;
                sdi_count := total_sdi_count;
              end;
            end
    end;

  procedure perform_integer_arith(temp1,temp2 : integer;
                                  tempop : optokenlist;
                                  var result : integer);
    var pos2:boolean;
    function divfun(temp1,temp2:integer)result:integer;
  	begin
	  if temp2=0 then reporterror(300*100+illegal_expression) else
	     begin
              if ((temp1=minint) and (temp2 < 0)) then begin
                  result:=((temp1-temp2) div temp2) + 1 
              end else begin
	       if temp2<0 then
		  begin temp1:=-temp1; temp2:=-temp2
		  end;
	       if temp1<0 then result:=-((-temp1-1)div temp2)-1
	       else result:=temp1 div temp2
              end
	     end
	end;
    begin
      case tempop of
        addop :  result := temp1 + temp2;
        orop  :  result := temp1 | temp2;   
        xorop :  result := temp1 xor temp2;   
        andop :  result := temp1 & temp2;   
        mulop :  result := temp1 * temp2;
        subop :  result := temp1 - temp2;
        divop :  result := divfun(temp1,temp2);
        modop :  result := temp1-divfun(temp1,temp2)*temp2;
        shrop :  begin
                  if (temp2 <> 0) and (temp1 < 0) then begin {ming}
                    temp1 := (temp1 >> 1) & %7fffffff;    
                    temp2 := temp2 - 1;
                  end;
                  result := temp1 >> temp2;
                 end; 
        shlop :  result := temp1 << temp2; 
      end;
    end;

  procedure assign_values(var tempterm : gen;
                          iresult : integer;
                          var resultatt : attribute;
                          var result : factorrec);

    begin
      result.fgenref := tempterm;
      with result.fgenref do
        begin
          if tempterm.gtype = regrelative then
            begin
              regreloffset := iresult;
              regreloffatt := resultatt;
            end
          else if tempterm.gtype = memoryrelative then 
            begin
              memoffset2 := iresult;
              memoffatt2 := resultatt;
            end
          else if tempterm.gtype = externalg then
            begin  
              extoffset := iresult;
              extoffatt := resultatt;
            end
          else if tempterm.gtype = absolute then
            begin
              absnum := iresult;
              absatt := resultatt;
            end
          else if tempterm.gtype = immediate then
            begin
              imint := iresult;
              immatt := resultatt;
            end
          else if tempterm.gtype = stringg then
            begin
              gtype := immediate;
              imint := iresult;
              immatt := resultatt;
              numtype := inti;
            end;
        end;
    end;


  procedure construct_type1_result(var term1,term2 : gen;
                                   iresult : integer;
                                   var resultatt : attribute;
                                   var result : factorrec);
    begin
      result.factortype := fgen;
      if term1.gtype = absolute then
        begin
          if term2.gtype = immediate then
            assign_values(term1,iresult,resultatt,result)
          else
            assign_values(term2,iresult,resultatt,result);
        end
      else if term2.gtype = absolute then
        begin
          if term1.gtype = immediate then
            assign_values(term2,iresult,resultatt,result)
          else
            assign_values(term1,iresult,resultatt,result);
        end
      else if (term1.gtype = immediate) or
          (term1.gtype = stringg) then
          begin
            assign_values(term2,iresult,resultatt,result)
          end
      else
          begin
            assign_values(term1,iresult,resultatt,result);
          end
    end;
          
  export function convert_string(temp_string : str_ptr) : integer;

    var shift,count,temp : integer;

    begin
      with temp_string^ do
      begin
        temp := 0;
        if strglen < 5 then
          begin
            if strglen > 0 then
              begin
                shift := 1;
                for count := 1 to strglen do
                  begin
                    temp := shift*ord(strchars[count]) + temp;
                    if count < 4 then
                      shift := shift * 256;
                  end
              end
          end
        else reporterror(100*092+exceed_4_byte);
      end;
      convert_string := temp;
    end;

  procedure getconstant( var tempgen  : gen;            (* input *)
                         var temptype : numtypes;      (* outputs *)
                         var temp     : integer;
                         var tempatt  : attribute;
                         var errorflag: boolean);

    begin
      temptype := inti;
      errorflag := false;
      with tempgen do
      begin
        if gtype = register then
          begin
            reporterror(100*093+illegal_term);
            errorflag := true;
          end
        else if gtype = regrelative then
          begin
            if (rltype = rpc) and not(resolve_loc_flag) then
            temp := locationcounter
            else temp := regreloffset;
            tempatt := regreloffatt;
          end
        else if gtype = memoryrelative then
          begin
            temp := memoffset2;
            tempatt := memoffatt2;
          end
        else if gtype = tos then
          begin
            reporterror(100*094+illegal_term);
            errorflag := true;
          end
        else if gtype = externalg then
          begin
            temp := extoffset;
            tempatt := extoffatt;
          end
        else if gtype = absolute then 
          begin
            temp := absnum;
            tempatt := absatt;
          end
        else if gtype = immediate then
          begin
            tempatt := immatt;
            if numtype = inti then
              temp := imint
            else if numtype = floati then
              begin
                {temptype := floati;
                 rtemp := imfloat;}
              end;
          end
        else if gtype = stringg then
          begin
            tempatt.attribstate[0] := chr(ord(false));
            temp := convert_string(string_rec);
          end;
      end;
      if error_flag then 
        errorflag := true;
    end;

  procedure check_lengths(var att1, att2 : attribute;
                          var ratt : attribute);
    begin
      if att1.attribstate[0] = chr(ord(false)) then
        ratt := att2
      else if att2.attribstate[0] = chr(ord(false)) then
        ratt := att1
      else if att1.attrib[0] = att2.attrib[0] then
        ratt := att1
      else
        begin
          reporterror(100*095+unequallengths);
          if att2.attrib[0] > att1.attrib[0] then
            ratt := att2
          else ratt := att1;
        end;
    end;
procedure dummy3; begin end;

  export procedure check_range(var att : attribute; temp : integer);

    begin
      with att do
      begin
        if attribstate[0] = chr(ord(true)) then
          begin
            if ord(attrib[0]) = ord(b) then
              begin
                if (temp < -128) or (temp > 255) then
                    reporterror(100*096+termsizeextreme);
              end
            else if ord(attrib[0]) = ord(w) then
              begin
                if (temp < -8192) or (temp > 8191) then
                    reporterror(100*097+termsizeextreme);
              end
            else if ord(attrib[0]) = ord(d) then
              begin end
          end
        else
          begin
            if (temp>=-64) and (temp<=63) then 
              attrib[0] := chr(ord(b))
            else if (temp>=-8192) and (temp<=8191) then
              attrib[0] := chr(ord(w))
            else attrib[0] := chr(ord(d));
          end;
      end;
    end;
        
  procedure process_arithmetic( var term1,term2 : gen;
                                tempop : optokenlist;
                                var result  : factorrec);
    var
      temp1type,temp2type,resulttype : numtypes;
      temp1,temp2,iresult : integer;
      {rtemp1,rtemp2,rresult : real;}
      error1,error2 : boolean;
      temp1att,temp2att,resultatt : attribute;

    begin
      getconstant(term1,temp1type,temp1{,rtemp1},temp1att,error1);
      getconstant(term2,temp2type,temp2{,rtemp2},temp2att,error2);
      if not(error1 or error2) then
        if tempop in [divop,modop,shrop,shlop] then
          begin
            if not((term2.gtype = immediate) or 
                   (term2.gtype = stringg)) then
              begin
                reporterror(100*098+illegal_term);
                error2 := true;
              end
          end
        else 
          if not((term1.gtype = immediate) or
                 (term1.gtype = stringg) or
                 (term1.gtype = absolute)) then
            if not((term2.gtype = immediate) or
                   (term2.gtype = stringg) or
                   (term2.gtype = absolute)) then
              begin
                reporterror(100*099+illegal_term);
                error1 := true;
              end;
      if not(error1 or error2) then
        begin
          if (temp1type = floati) or (temp2type = floati) then
            begin
              if (temp1type = floati) and (temp2type = floati) then
                begin
                end
              else
                begin
                  reporterror(100*100+illegal_term);
                  error1 := true;
                end;
            end
          else
	    begin
              perform_integer_arith(temp1,temp2,tempop,iresult);
	      (*if (tempop=addop) then
		begin
		  if term1.sdi_gnum=-1 then result.sdi_fnum:=term2.sdi_gnum
		  else if term2.sdi_gnum=-1 then result.sdi_fnum:=term1.sdi_gnum
		  else result.sdi_fnum:=-2
		end
	      else result.sdi_fnum:=-2*)
            end;
          check_lengths(temp1att,temp2att,resultatt);
          if not (error1 or error2) then
            begin
              if temp1type = inti then
                check_range(resultatt,iresult)
              else if temp1type = floati then
                begin end; 
              construct_type1_result(term1,term2,iresult,resultatt,result);
            end;
        end;
    end;

  function apply_op(temp:integer; var new_att:attribute; un_op:optokenlist)
                    :integer;
    begin 
      if un_op=subop then
        begin if temp<>minint then temp:=-temp end
      else if un_op=comop then temp:=~temp
      else if un_op=notop then 
        begin if (abs(temp) mod 2)=0 then temp:=temp+1
              else temp:=temp-1
        end;
      !if not (no_check_att_flag or sdi_flag) then check_range(new_att,temp);  {ming}
      check_range(new_att,temp);  {ming}
      apply_op:=temp
    end;
          

(*  procedure perform_check_op(var tempgen : gen;
                             unary_op : optokenlist;
                             var termattribute : attribute;
                             var tatt : attribute;
                             var temp : integer);
    var new_att : attribute;
        new_temp: integer;

    begin
      new_att := termattribute;
      check_range(new_att,temp);
      if unary_op = subop then
        begin
          if temp <> minint then
            temp := -temp;     
          tatt := new_att;
        end
      else if unary_op = addop then
        begin 
          tatt := new_att;
        end        
      else if unary_op = comop then
        begin
          temp := ~temp;
          tatt := new_att;
        end
      else if unary_op = extop then
        with tempgen do
          begin
            gtype := externalg;
            new_temp := temp;
            extindex := new_temp;
            extinxatt := new_att;
            extoffset := 0;
            extoffatt.attrib[0] := chr(ord(b));
            extoffatt.attribstate[0] := chr(ord(false));
          end
      else if unary_op = absop then
        with tempgen do
          begin
            gtype := absolute;
            new_temp := temp;
            absnum := new_temp;
            absatt := new_att;
          end
      else if unary_op = notop then
       begin end;
    end;  *)

  procedure process_ops (var tempgen : gen;
                         unary_op : optokenlist;
                         var termattribute : attribute);
      
    var new_att:attribute;
     begin 
       with tempgen do
         begin
           if (gtype = stringg) and ((unary_op <> addop) or 
                (termattribute.attribstate[0] = chr(ord(true)))) then
             begin
               imint := convert_string(string_rec);
               gtype := immediate;
               numtype := inti;
             end;
           if (gtype=register)or(gtype=externalg) then
             begin if unary_op<>addop then reporterror(100*101+illegal_term) end;
           if (gtype=externalg) then begin new_att:=termattribute; {ming}
                                           extoffset:=apply_op(extoffset,new_att,unary_op);
                                           extoffatt:=new_att;
                                           new_att:=termattribute;
                                           extindex:=apply_op(extindex,new_att,unary_op);
                                           extinxatt:=new_att;
                                      end                       
           else if (gtype=regrelative) then
             begin if (unary_op=extop) (* or(unary_op=absop) *)  then
                              reporterror(100*102+illegal_term)
               {else if (not (rltype = pc)) then}    {ming}
               else   
                 begin new_att:=termattribute;
                   regreloffset:=apply_op(regreloffset,new_att,unary_op);
                   regreloffatt:=new_att
                 end
             end 
           else if (gtype=memoryrelative) then
             begin if (unary_op=extop)(*or(unary_op=absop)*) then
                              reporterror(100*103+illegal_term)
               else 
                 begin new_att:=termattribute;
                   memoffset2:=apply_op(memoffset2,new_att,unary_op);
                   memoffatt2:=new_att;
                 end
             end
           else if (gtype=absolute) then
             begin if (unary_op=extop) (* or (unary_op=absop) *) then
                              reporterror(100*104+illegal_term)
               else 
                 begin new_att:=termattribute;
                   absnum:=apply_op(absnum,new_att,unary_op);
                   absatt:=new_att
                 end
             end 
           else if (gtype=immediate) then
             begin
               if (numtype=floati)and(unary_op<>addop)and(unary_op<>subop) then
                 reporterror(100*105+float_not_allowed)
               else if unary_op=extop then
                 begin gtype:=externalg;
                   extindex:=imint;
                   new_att:=termattribute;
                   check_range(new_att,imint);
                   extinxatt:=new_att;
                   extoffset:=0;
                   extoffatt.attrib[0]:=chr(ord(b));
                   extoffatt.attribstate[0]:=chr(ord(false))
                 end
               else if unary_op=absop then
                 begin gtype:=absolute;
                   new_att:=termattribute;
                   check_range(new_att,imint);
                   absnum:=imint;
                   absatt:=new_att
                 end
               else if numtype=inti then
                 begin new_att:=termattribute;
                   imint:=apply_op(imint,new_att,unary_op);
                   immatt:=new_att
                 end
               else if unary_op=subop then
                 imfloat.d_hi:=minint xor imfloat.d_hi
             end
           else if (unary_op<>addop) then
                reporterror(100*106+illegal_term);
         end;
     end;

  procedure gettermfactor(t : termptr; var tempterm : factorrec); forward;

  procedure process_unary_ops(var tempterm : factorrec;
                              unary_op : optokenlist;
                              var termattribute : attribute);
    var tempfact:factorrec;
    begin (* process_unary_op *)
      with tempterm do
      begin
        if factortype = fgen then
          begin
            process_ops(fgenref,unary_op,termattribute)
          end
        else if factortype = fsym then
          begin
            if fsymbol^.symdefined[0] = chr(ord(true)) then
              begin
                if not error_flag then
                  begin
                    with fsymbol^.symbol_definition do
                      begin
                        if (gtype=regrelative)and(rltype=pc) and 
			    (process_d_cxp or process_cxp) 
			   and (depth_gauge = 0) then
                             produce_external_entry(fsymbol);
                      end;
                    with fsymbol^.symbol_definition do
                      begin
                        if (gtype = regrelative) and (rltype=pc) 
                          and (fsymbol^.sdi_count <> total_sdi_count) then
                          begin
                            sdi_flag := true;
                          end
                        else if ((gtype = externalg) and
                                 (process_d_cxp or process_cxp)) then
                          begin
                            ext_proc[0] := chr(1 | ord(ext_proc[0]))
                          end
                      end;
		    (*tempterm.sdi_fnum:=fsymbol^.sdi_count;*)
		    if fsymbol^.equ_sdi[0]=chr(ord(false)) then
		      begin
                        fgenref := fsymbol^.symbol_definition; 
                        factortype := fgen; 
                        process_ops(tempterm.fgenref,unary_op,termattribute);
		      end
		    else
		      begin
			sdi_flag:=true;
			gettermfactor(fsymbol^.symbol_definition.
				start_of_stack,tempfact);
			tempterm:=tempfact;
			process_ops(tempterm.fgenref,unary_op,termattribute)
		      end;
                  end
              end
            else 
              begin
                symbol_undefined := true;
		!if (depth_gauge=0) and process_d_cxp then 
		  !produce_external_entry(fsymbol);
		  !fsymbol^.to_be_externed[0]:=chr(ord(true));
		(*tempterm.sdi_fnum:=-2;*)
              end
          end
        else if unary_op <> addop then
          begin
            reporterror(100*107+illegal_term);
          end;
      end;
    end;

  procedure process_rel_terms(var term1, term2 : factorrec;
                              var result : factorrec);
    var regrec : rtentry;
        temp : integer;
        tempatt : attribute;

    begin
      if term1.factortype = fgen then
        begin
          if (term1.fgenref.gtype = immediate) or
             (term1.fgenref.gtype = stringg) or
             (term1.fgenref.gtype = absolute) then
            begin
              if (term1.fgenref.gtype = immediate) then
                begin
                  temp :=term1.fgenref.imint;
                  tempatt := term1.fgenref.immatt
                end
              else if (term1.fgenref.gtype = absolute) then
                begin
                  temp :=term1.fgenref.absnum;
                  tempatt := term1.fgenref.absatt
                end
              else if (term1.fgenref.gtype = stringg) then
                begin
                  tempatt.attribstate[0] := chr(ord(false));
                  temp := convert_string(term1.fgenref.string_rec);
                  check_range(tempatt,temp)
                end;
              result.factortype := fgen;
              with result.fgenref do
              if term2.factortype = fres then
                begin
                  gtype := regrelative;
                  regrec := rtmnemonics[term2.fresindex];
                  if regrec.rtokentype = regtokens then
                    begin
                      rltype := rn;
                      rnum[0] := regrec.rtreg[0];
                      regreloffset := temp;
                      regreloffatt := tempatt;
                    end
                  else if regrec.rtokentype = pregtokens then
                    begin
                      if ord(regrec.rtpreg[0]) in 
                                      [ord(sb),ord(sp),ord(fp),ord(pc)] then
                        begin
                          rltype := ord(regrec.rtpreg[0])::preglist;
                          regreloffset := temp;
                          regreloffatt := tempatt;
                        end
                      else reporterror(100*108+illegal_term)
                    end
                  else reporterror(100*109+illegal_term)
                end
              else if term2.fgenref.gtype = regrelative then
                begin
                  gtype := memoryrelative;
                  if term2.fgenref.rltype <> rn then
                    begin
                      memoffset1 := term2.fgenref.regreloffset;
                      memoffatt1 := term2.fgenref.regreloffatt;
                      memoffset2 := temp;
                      memoffatt2 := tempatt;
                      if term2.fgenref.rltype <> pc then
                        memreg[0] := chr(ord(term2.fgenref.rltype))
                      else reporterror(100*110+illegal_term);
                    end
                  else reporterror(100*111+illegal_term)
                end
              else if ((term2.factortype = fgen) and
                 (term2.fgenref.gtype = register)) then
                begin
                  gtype := regrelative;
                  if term2.fgenref.rtype = gregs then
                    begin
                      rltype := rn;
                      rnum[0] := term2.fgenref.reg[0];
                      regreloffset := temp;
                      regreloffatt := tempatt;
                    end
                  else reporterror(100*112+illegal_term)
                end
              else reporterror(100*113+illegal_term)
            end
          else reporterror(100*114+illegal_term)
        end
      else reporterror(100*115+illegal_term);
    end;
  
  procedure process_sub_terms(var term1,term2 : gen;
                              var result : factorrec);

  begin
    if ((term2.gtype=immediate) or
        (term2.gtype=stringg)) then
      process_arithmetic(term1,term2,subop,result)
    else if term1.gtype = term2.gtype then
      begin
        result.factortype := fgen;
        with result.fgenref do
        begin
          gtype := immediate;
          numtype := inti;
          if term1.gtype = register then
            reporterror(100*116+illegal_term)
          else if term1.gtype = regrelative then
            if (term1.rltype = term2.rltype) or
               ((term1.rltype=pc) and (term2.rltype=rpc)) or
               ((term1.rltype=rpc) and (term2.rltype=pc)) then
              begin
                if term1.rltype <> rn then
                  begin
                    if (term1.rltype = rpc) and not resolve_loc_flag then
                      imint := locationcounter
                    else imint := term1.regreloffset;
                    if (term2.rltype = rpc) and not resolve_loc_flag then
                      imint := imint - locationcounter
                    else imint := imint - term2.regreloffset;
                    check_lengths(term1.regreloffatt,term2.regreloffatt,immatt);
                    check_range(immatt,imint);
                  end
                else if term1.rnum = term2.rnum then
                  begin
                    imint := term1.regreloffset- term2.regreloffset;
                    check_lengths(term1.regreloffatt,term2.regreloffatt,immatt);
                  end
                else  reporterror(100*117+illegal_term);
              end
            else reporterror(100*118+illegal_term)
          else if term1.gtype = memoryrelative then
            begin
              if term1.memreg = term2.memreg then
                if term1.memoffset1 = term2.memoffset1 then
                  begin
                    imint := term1.memoffset2-term2.memoffset2;
                    check_lengths(term1.memoffatt2,term2.memoffatt2,immatt);
                  end
                else reporterror(100*119+illegal_term)
              else reporterror(100*120+illegal_term);
            end
          else if term1.gtype = tos then reporterror(100*121+illegal_term)
          else if term1.gtype = externalg then
            begin
              if term1.extindex = term2.extindex then
                begin
                  imint := term1.extoffset - term2.extoffset;
                  check_lengths(term1.extoffatt,term2.extoffatt,immatt);
                end
              else reporterror(100*122+illegal_term);
            end
          else if term1.gtype = absolute then
            begin
              imint := term1.absnum - term2.absnum;
              check_lengths(term1.absatt,term2.absatt,immatt);
            end;
        end
      end
    else reporterror(100*123+illegal_term);
  end;


  procedure getexpr(e : expptr; var result : factorrec);

    var term1, term2 : factorrec;
        tempop : optokenlist;

    begin
      if not error_flag then
(* *)   begin                                 (* *)
(* *)     if e^.stackrectype=exprec then      (* *)
            begin
              depth_gauge := depth_gauge + 1;
              gettermfactor(e^.erec.opd1,term1);
              if symbol_undefined then
                result := term1
              else
                begin
                  gettermfactor(e^.erec.opd2,term2);
                  if symbol_undefined then
                    result := term2
                  else
                    begin
                      tempop := e^.erec.op;
                      if ((term1.factortype=fres) or (term2.factortype=fres))
                        and (tempop <> relop) then
                        reporterror(100*124+illegal_term)
                      else
                        case tempop of
                          addop,orop,
                          xorop,andop,
                          mulop,divop,
                          modop,shrop,
                          shlop     : process_arithmetic( term1.fgenref,
                                                          term2.fgenref,
                                                          tempop,result);
                          subop     : begin
                                        process_sub_terms(term1.fgenref,
                                                        term2.fgenref,result);
					(*if term1.sdi_fnum=-1 
					   then result.sdi_fnum:=term2.sdi_fnum
					else if term2.sdi_fnum=-1
					   then result.sdi_fnum:=term1.sdi_fnum
					else if term1.sdi_fnum=term2.sdi_fnum 
					  then result.sdi_fnum:=-1
					else result.sdi_fnum:=-2;*)
                                      end;
                          relop     : begin
					process_rel_terms(term1,term2,result);
					(*result.sdi_fnum:=-2*)   (* improve *)
				      end;
                        end;
                    end
                end;
              depth_gauge := depth_gauge - 1;
             end
(* *)     else                                                        (* *)
(* *)       begin gettermfactor(e,result); end                        (* *)
(* *)   end                                                           (* *)
      end;

    procedure gettermfactor;{t : termptr; var tempterm : factorrec}
    
      begin
        symbol_undefined := false;
        if not error_flag then
         begin
          if t^.trec.termtype = subexp then
            begin getexpr(t^.trec.sub,tempterm) end
          else
            begin tempterm := t^.trec.fact end;
          if not error_flag then
              process_unary_ops(tempterm,t^.trec.unop,t^.trec.termlength);
        end;
      end;

  export procedure resolve_expression(t : termptr; var result :factorrec);

    var i1,i2:integer;
        a1,a2:attribute;

    begin
      sdi_flag := false; depth_gauge := 0;
      gettermfactor(t,result);
      if ext_to_sbrelative and (result.factortype=fgen) then
	if process_d_cxp then fix1gen(result.fgenref)
	else fixgen(result.fgenref);
      (*sdi_flag:=sdi_flag or (result.sdi_fnum<>-1)*)
    end;

  export procedure resolve_1_expression(t : termptr; var result :factorrec);

    var i1,i2:integer;
        a1,a2:attribute;

    begin
      sdi_flag := false; depth_gauge := 0;
      gettermfactor(t,result);
      (*sdi_flag:=sdi_flag or (result.sdi_fnum<>-1)*)
    end;


  procedure check_for_bwd( var temptoken : token;
                               t : termptr);
    var tempchar : char;

    begin
      with temptoken do
        if (tokentype=specchar) and (spch=':') then
          begin
            tempchar := getlchar;
            if (tempchar = 'b') or (tempchar = 'd') or (tempchar = 'w')
            or (tempchar = 'B') or (tempchar = 'D') or (tempchar = 'W') then
              begin
                t^.trec.termlength.attribstate[0] := chr(ord(true));
                if (tempchar = 'b') or (tempchar= 'B') then
                  t^.trec.termlength.attrib[0] := chr(ord(b))
                else if (tempchar = 'd') or (tempchar = 'D') then
                  t^.trec.termlength.attrib[0] := chr(ord(d))
                else t^.trec.termlength.attrib[0] := chr(ord(w));
              end
            else reporterror(100*125+invalidchar);
            nexttoken(temptoken);
          end
        else
          t^.trec.termlength.attribstate[0] := chr(ord(false));
    end;

  procedure process_factor( var temptoken : token;
                            var tempfactor : factorrec);
  
    begin
      with temptoken do
      begin
        if tokentype =restoken then
          begin
            if rtmnemonics[rtokenindex].rtokentype = optokens then
              reporterror(100*126+illegal_term)
            else
              begin
                if (rtmnemonics[rtokenindex].rtokentype =pregtokens) and
                    (rtmnemonics[rtokenindex].rtpreg[0] = chr(ord(tosk))) then
                      with tempfactor do begin
                        factortype := fgen;
		        (*sdi_fnum:=-1;*)
                        fgenref.gtype := tos;
                      end
                else with tempfactor do
                  begin
                    factortype := fres;
		    (*sdi_fnum:=-1;*)
                    fresindex := rtokenindex;
                  end
              end
          end
        else if tokentype = symbol then with tempfactor do
          begin
            factortype := fsym;
            fsymbol := processsymbol(temptoken);
	    (*sdi_fnum:=fsymbol^.sdi_count;*)
	    (*if fsymbol^.symdefined[0]=chr(ord(false)) then sdi_fnum:=-2;*)
            assignlineref(fsymbol);
          end
        else if tokentype = gentoken then
          with tempfactor do begin 
            factortype := fgen;
	    (*sdi_fnum:=-1;*)
            fgenref := genref;
          end
        else if spch = '*' then
          with tempfactor do begin
            factortype := fgen;
            with fgenref do
              begin
                gtype := regrelative;
                regreloffset := locationcounter;
                regreloffatt.attribstate[0] := chr(ord(false));
                if currentmode = progrel then
		  begin
                    construct_temp_symbol(tempfactor);
	            (*sdi_gnum:=fsymbol^.sdi_count;
	            sdi_fnum:=fsymbol^.sdi_count;*)
		  end
                else if currentmode = stbsrel then
                  rltype := sb
                else if currentmode = frrel then
                  rltype := fp
                else if currentmode = stckrel then
                  rltype := sp
                else if currentmode = ab then
                  begin
                    gtype := absolute;
                    absnum := locationcounter;
                    absatt.attribstate[0] := chr(ord(false));
                  end;
              end;
          end
        else tempfactor.factortype := fnill;
      end;
    end;

    
  function check_for_op( var temptoken : token;
                         var op : optokenlist) : boolean;
  
    begin
      check_for_op := false;
      with temptoken do 
      begin
        if tokentype = specchar then
          begin
            if (spch = '-') or (spch = '+') or (spch = '*') or 
               (spch = '/') or (spch = '@') then
              begin
                if spch = '-' then op := subop
                else if spch = '+' then op := addop
                else if spch = '*' then op := mulop
                else if spch = '/' then op := divop
                else if spch = '@' then op := absop;
                check_for_op := true;
              end;
          end
        else if tokentype = restoken then
          begin
            if rtmnemonics[rtokenindex].rtokentype = optokens then
              begin
                op := ord(rtmnemonics[rtokenindex].rtops[0])::optokenlist;
                check_for_op := true;
              end;
          end;
      end;
    end;

  function unary_op( var temptoken : token;
                     var op : optokenlist) : boolean;

    begin
      unary_op := false;
      if check_for_op(temptoken,op) then
        if op in [addop,subop,comop,extop,notop,absop] then
          unary_op := true
        else op := addop                (* identity *)
      else op := addop;
    end;

 function level_2_op( var temptoken : token;
                      var op : optokenlist) : boolean;

   begin
     level_2_op := false;
     if check_for_op(temptoken,op) then
       if op in [mulop,divop,modop,andop,shlop,shrop] then
         level_2_op := true;
   end;

   function level_3_op( var temptoken : token;
                        var op : optokenlist) : boolean;

     begin
       level_3_op := false;
       if check_for_op(temptoken,op) then
         if op in [addop,subop,orop,xorop] then
           level_3_op := true;
     end;


  procedure getfactor(var f : termptr);

    var unaryop : optokenlist;
        exp	: expptr;
        t       :termptr;

  begin
    if unary_op(currtoken,unaryop) then
      begin nexttoken(currtoken);
        getfactor(t);
        f:=newterm;
        f^.trec.unop:=unaryop;
        f^.trec.termtype:=subexp;
	f^.trec.termlength:=t^.trec.termlength;
        f^.trec.sub:=t;
      end
    else
      begin
	with currtoken do
	begin
          if (tokentype = specchar) and (spch = '(') then
            begin
	      f := simpleexp;
	      if not((tokentype = specchar) and (spch = ')')) then
	        reporterror(100*127+illegal_expression);
            end
          else 
            begin
	      f := newterm;
	      f^.trec.termtype := factor;
	      process_factor(currtoken,f^.trec.fact);
              f^.trec.unop:=addop
            end;
	  nexttoken(currtoken);
	  check_for_bwd(currtoken,f);
          while (tokentype = specchar) and (spch = '(') do
            begin
	      exp := newexp;
	      exp^.erec.opd1 := f;
	      exp^.erec.op := relop;
	      exp^.erec.opd2 := simpleexp;
	      f := newterm;
	      f^.trec.termtype := subexp;
	      f^.trec.unop := addop; f^.trec.termlength.attribstate[0] := 
								chr(ord(false));
	      f^.trec.sub := exp;
	      if not((tokentype = specchar) and (spch = ')')) then
	        reporterror(100*128+illegal_expression);
	      nexttoken(currtoken);
	    end;
        end;
      end;
    end;


  procedure getterm(var t : termptr);

    var exp : expptr;
        level2op : optokenlist;

    begin (* procedure getterm *)
      getfactor(t);
      while level_2_op(currtoken,level2op) do
        begin 
          nexttoken(currtoken);
          exp := newexp;
          exp^.erec.opd1 := t;
          exp^.erec.op := level2op;
          getfactor(exp^.erec.opd2);
          t := newterm;
          t^.trec.termtype := subexp;
          t^.trec.unop := addop; 
          t^.trec.termlength.attribstate[0]:=chr(ord(false));
          t^.trec.sub := exp;
       end;
    end;

export function simpleexp;

  var t   : termptr;
      exp : expptr;
      level3op : optokenlist;

  begin  (* function simpleexp *)
    nexttoken(currtoken);
    getterm(t);
    while level_3_op(currtoken,level3op) do
      begin 
        nexttoken(currtoken);
        exp := newexp;
        exp^.erec.opd1 := t;
        exp^.erec.op := level3op;
        getterm(exp^.erec.opd2);
        t := newterm;
        t^.trec.termtype := subexp;
        t^.trec.unop := addop; 
        t^.trec.termlength.attribstate[0] := chr(ord(false));           
        t^.trec.sub := exp;  
     end;
    simpleexp := t;
  end;

  begin
  end.

