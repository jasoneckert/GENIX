{!i}
module dirs;

(*
 * Copyright (C) 1982 by National Semiconductor Corporation
 *
 *      National Semiconductor Corporation
 *      2900 Semiconductor Drive
 *      Santa Clara, California 95051
 *
 *      All Rights Reserved
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

const SCCSId = Addr('@(#)dirs.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

! routines for first phase of directives

import 'glob_const.e';  !DEPENDENCY
import 'formatsmod.e';  !DEPENDENCY
import 'glob_types.e';  !DEPENDENCY
import 'glob_vars.e';  !DEPENDENCY
import 'mnem_def.e';  !DEPENDENCY
import 'scantoken.e';  !DEPENDENCY
import 'scanr.e';  !DEPENDENCY
import 'scan.e';  !DEPENDENCY
import 'genrout.e';  !DEPENDENCY
import 'sdis.e';  !DEPENDENCY
import 'prsymtable.e';  !DEPENDENCY
import 'scanexp.e';  !DEPENDENCY
import 'codegen.e';  !DEPENDENCY
import 'bio.e';  !DEPENDENCY
import 'init1.e';  !DEPENDENCY
import 'dirsx.e';  !DEPENDENCY
import 'insts.e';  !DEPENDENCY
import 'cmpc.e';  !DEPENDENCY
import 'scmpc.e';  !DEPENDENCY
import 'stab.e';  !DEPENDENCY
import 'disp.e';  !DEPENDENCY
import 'float.e';  !DEPENDENCY

export 'dirs.e'; !DEPENDENCY

  var
    mode_stack: packed array[1..maxmodes] of mode_record;
    routines_flag : boolean; uni_num:integer;
    field_bit_count,field_siz:integer;
    field_last_rec:field_rec_ptr;
    exportp_flag:boolean;
function uniqueinteger:integer;
  begin uniqueinteger:=uni_num;
    uni_num:=uni_num+1
  end;

  export procedure makepublic(s : symptr); !put it in export chain
					   !if not already there

    var e : exportptr;
        c_test : integer;
        cmp_in : integer;

    begin
      if not make_global and ((s^.scope.domain & N_EXT) <> N_EXT) then 
      {  if not objsymtable then  added by Tim}
      {    new_name(s^.symbolname,s^.symbolname^,}(* assure name is in proper *)
      {             currsymname2,g_symnames);  }  (* string table *)
      c_test := -1; e := exportstart;
      while ((c_test <> 0) and (e <> nil)) do
        begin
          c_test := scmpc(addr(s^.symbolname^),
                          addr(e^.exportsptr^.symbolname^));
          if c_test <> 0 then
            e := e^.nextexportrec;
        end;
      if c_test <> 0 then
        begin
          new(e); export_count := export_count + 1;
          with e^, s^ do
          begin
            exportsptr := s;
            scope.domain := N_EXT|scope.domain;
            nextexportrec := nil;
          end;
          if exportstart = nil then
            exportstart := e
          else
            exportchain^.nextexportrec := e;
          exportchain := e;
        end;
    end;

  procedure push_current_mode;
    begin
      if modeindex <=  maxmodes then
        begin
          mode_stack[modeindex].old_loc := locationcounter;
          mode_stack[modeindex].old_link := currentlinkpage;
          mode_stack[modeindex].old_mode := currentmode;
          mode_stack[modeindex].old_asm := assembly_mode;
          modeindex := modeindex + 1;
        end
      else reporterror(100*028+nesting);
    end;

  export procedure pop_current_mode;
    begin
      if modeindex > 1 then
        begin
          modeindex := modeindex - 1;
          locationcounter := mode_stack[modeindex].old_loc;
          currentlinkpage := mode_stack[modeindex].old_link;
          currentmode := mode_stack[modeindex].old_mode;
          if currentmode = progrel then
            locationcounter := last_alloc_pc
          else if currentmode = stbsrel then
            locationcounter := last_alloc_sb;
          assembly_mode := mode_stack[modeindex].old_asm;
        end
      else reporterror(100*029+nesting);
    end;

  procedure add_to_loc_chain(sos : stackptr);

    var loc_ent : loc_ptr;

    begin
      new_loc_ptr(loc_ent);
      loc_ent^.loc := locationcounter;
      loc_ent^.loc_value := sos; reset_estack;
      total_loc_dirs := total_loc_dirs + 1;
      loc_ent^.locdir_number := total_loc_dirs;
      loc_ent^.next_loc := nil;
      if loc_start = nil then
        loc_start := loc_ent
      else current_loc^.next_loc := loc_ent;
      current_loc := loc_ent;
      total_sdi_count := total_sdi_count + 1;
    end;

  export function getdirindex(var temptoken : token) : integer;
		! search for the found symbol in the directives table.
    var ctest,temp: integer;
        tchr    : char;
        tempdir : packed array[1..10] of char;
        finished : boolean;
        cmp_in   :integer;

    begin
      getdirindex := -1;
      if temptoken.symlength <= maxdirlen then
        begin
          for temp:= 1 to maxdirlen do begin
            tchr := temptoken.sym[temp];
            if (tchr >= 'A') and (tchr <= 'Z') then
              tchr := chr(ord(tchr) + (ord('a') - ord('A')));
            tempdir[temp] := tchr;
          end;
          temp := 1; finished := false;
          repeat
            ctest := cmpc(maxdirlen,addr(dirmnemonics[temp].dirchars),
                                                        addr(tempdir));
            if ctest < 0 then
                temp := temp + 1
            else 
              begin
                finished := true;
                if ctest = 0 then
                  getdirindex := temp;
              end;
          until finished or (temp > maxdirs);
        end
    end;

  procedure process_dsect;
    begin
      push_current_mode;
      locationcounter := 0;
      currentmode := ab;
      assembly_mode := standard;
    end;
  
  procedure process_static;
    begin
      push_current_mode;
      locationcounter := last_alloc_sb;
      currentmode := stbsrel;
      assembly_mode := standard;
    end;

  procedure process_program;
    begin
      push_current_mode;
      locationcounter := last_alloc_pc;
      currentmode := progrel;
      assembly_mode := standard;
    end;


procedure do_loc(fref:factorrec);
  begin
    push_current_mode;
    if fref.factortype = fgen then
    with fref.fgenref do
    begin
      assembly_mode := standard;
      case gtype of
        register             : reporterror(100*030+illegal_expression);
        regrelative    : begin
                           if  rltype <> rn then
                             begin
                               if ((rltype = pc) or (rltype = rpc)) and
                                 (regreloffset < last_alloc_pc) then
                                   begin 
                                     reporterror(100*031+illegal_expression);
                                   end;
                               locationcounter := regreloffset;
                               case rltype of
                                 fp: begin
                                       currentmode := frrel;
                                       assembly_mode := inverted;
                                     end;
                                 sp: begin
                                       currentmode := stckrel;
                                       assembly_mode := inverted;
                                     end;
                                 sb: begin currentmode := stbsrel;
                                       last_alloc_sb := locationcounter; {ming}
                                     end;
                                 pc: begin
                                       currentmode := progrel;
                                       add_to_loc_chain(start_of_stack);
                                       last_alloc_pc := locationcounter; {ming}
                                     end;
                                 rpc: begin
                                        currentmode := progrel;
                                        if sdi_flag then
                                         add_to_loc_chain(start_of_stack);
                                      end;
                                end;
                             end;
                         end;
        memoryrelative : reporterror(100*032+illegal_expression);
        tos          : reporterror(100*033+illegal_expression);
        externalg      : begin
                           locationcounter := extoffset;
                           currentlinkpage := extindex;
                           currentmode := extg;
                         end;
        immediate      : begin
                         locationcounter := imint;
                           currentmode := ab;
                         end;
        absolute       : begin
                           locationcounter := absnum;
                           currentmode := ab;
                         end;
      end;
    end
    else reporterror(100*034+illegal_term);
    if error_flag then pop_current_mode;
  end;



  procedure process_loc;
    var fref : factorrec;
    begin
      scan_const_expr(fref);
      if not error_flag then do_loc(fref);
        if labelptr <> nil then
          reporterror(100*035+invalidlabel);
    end;


    procedure process_align;
      var n1,n2,n1s,n1e,n2s,n2e,temp:integer;fref:factorrec;
      begin
        n1s:=charpos;
        scan_const_expr(fref);
        if (currtoken.tokentype=specchar) then
          if ((currtoken.spch=';')or(currtoken.spch=',')) 
            then n1e:=charpos-2 
             else if (currtoken.spch=eol)
               then n1e:=charpos-1 
                else reporterror(100*201+align_error);
        if not error_flag then with f=fref do
          begin 
            if (f.fgenref.gtype<>immediate)or(f.fgenref.numtype<>inti) then
              reporterror(100*195+align_error)
            else n1:=f.fgenref.imint;
            if (currtoken.tokentype=specchar) and (currtoken.spch=',') then
              begin 
                n2s:=charpos;
                scan_const_expr(fref);
                if (currtoken.tokentype=specchar) then
                 if (currtoken.spch=';') 
                  then n2e:=charpos-2 
                   else if (currtoken.spch=eol)
                     then n2e:=charpos-1 
                      else reporterror(100*202+align_error);
                if not error_flag then 
                if (f.fgenref.gtype<>immediate)or(f.fgenref.numtype<>inti)
                  then reporterror(100*196+align_error)
                else n2:=f.fgenref.imint;
              end
            else
              begin n2s:=n2e+1;
                n2:=0
              end
          end;
         if n1 <0 then reporterror(100*204+align_error);
        if not error_flag then 
          if currentmode <>progrel then
            if assembly_mode=standard then
              begin temp:=(((n1+n2)-(locationcounter mod n1))mod n1);
                increment_locationcounter(temp)
              end
            else
                 reporterror(100*203+align_error)
             {* begin temp:=(((n1+n2)-(locationcounter mod n1))mod n1);
                increment_locationcounter(temp) *}
          else
            begin
		!insert the expressions in the input stream, and call the parser
              insert_align_expr(n1s,n1e,n2s,n2e);
              scan_const_expr(fref);
		!use the parsed expression to generate a .loc
              do_loc(fref);
            end;
      end;
                
  export procedure make_external(var sptr : symptr);
	!make a link table entry. redefine sptr^ as external, and insert a
	!pointer to it in the import chain

    var i : importptr;

    begin
      with sptr^.symbol_definition do
        begin
          import_count := import_count + 1;
          gtype:= externalg;
          sptr^.scope.parent := symroot;
          sptr^.linedefined := linenumber;
	  !writeln('make external at line ',linenumber);
          sptr^.symdefined[0] := chr(ord(true));
          new(i);
            with i^ do
              begin
                importsptr := sptr;
                nextimportrec := nil;
                extoffset := 0;
                extoffatt.attribstate[0] := chr(ord(false));
                extoffatt.attrib[0] := chr(ord(b));
                extinxatt.attribstate[0] := chr(ord(false));
                if routines_flag then
		  ext_proc[0] := chr(ord(1))
                else ext_proc[0] := chr(ord(0));
                if importstart = nil then
                  begin
                    importstart := i;
                    extindex := 0;
                    extinxatt.attrib[0] := chr(ord(b));
                  end
                else
                  begin
                    importchain^.nextimportrec := i;
                    extindex := 
                       importchain^.importsptr^.symbol_definition.extindex + 1;
                    if extindex < 64 then
                      extinxatt.attrib[0] := chr(ord(b))
                    else if extindex < 8192 then
                      extinxatt.attrib[0] := chr(ord(w))
                    else extinxatt.attrib[0] := chr(ord(d));
                  end;
                importchain := i;
              end
        end
    end;

  procedure processimportitem;

     var s : symptr;

     begin
        make_global := true;
        s := scanglobsymbol;
        if s <> nil then
          with s^ do
          if symdefined[0] = chr(ord(false)) then
            make_external(s)
          else reporterror(100*036+duplicate_def);
        make_global := false;
    end;

  procedure process_import;

    var tchar : char;

      begin
        processimportitem;
        skipblanks;
        while current_char = ',' do
          begin
            lastpos := charpos;
            tchar := getschar;
            processimportitem;
            skipblanks 
          end
      end;

  procedure process_common;

    var fref  : factorrec;

    begin
      processimportitem; scan_comma;
      if not error_flag then
        begin
          scan_const_expr(fref);
          if not error_flag then
            begin
              if fref.factortype = fgen then
                if fref.fgenref.gtype = immediate then
                  begin
                    importchain^.comm_size := fref.fgenref.imint;
                    importchain^.importsptr^.symbol_definition.ext_proc[0] :=
                      chr(2);
                  end
                else reporterror(100*037+illegal_expression)
              else reporterror(100*038+illegal_expression)
            end;
            while ((currtoken.tokentype=specchar) and (currtoken.spch=','))
                  and (not error_flag) do
              begin
                processimportitem; 
                scan_comma;
                if not error_flag then
                  begin
                    scan_const_expr(fref);
                    if not error_flag then
                      begin
                        if fref.factortype = fgen then
                          if fref.fgenref.gtype = immediate then
                            begin
                              importchain^.comm_size := fref.fgenref.imint;
                    importchain^.importsptr^.symbol_definition.ext_proc[0] :=
                                chr(2);
                            end
                          else reporterror(100*039+illegal_expression)
                        else reporterror(100*040+illegal_expression)
                      end;
                  end;
              end;
        end;
    end;

  procedure process_pimport;

    begin
      routines_flag := true;
      process_import;
      routines_flag := false;
    end;

  procedure process_routines;

    begin if BACK_COMPATIBLE then
        begin
          routines_flag := true;
          process_import;
          routines_flag := false;
        end
      else reporterror(301*100+back_only)
    end;

  procedure prcss_export_item;

    var s : symptr;

    begin
      make_global := true;
      s := scanglobsymbol;
      if s <> nil then
        begin
	  if exportp_flag then s^.scope.domain:=s^.scope.domain|N_PROC;
          makepublic(s);
          assignlineref(s);
        end;
      make_global := false;
    end;
  
  procedure process_pexport;

    var tchar : char;

    begin
      exportp_flag:=true;
      prcss_export_item;
      skipblanks;
      while current_char = ',' do
        begin
          lastpos := charpos;
          tchar := getschar;
          prcss_export_item;
          skipblanks 
        end;
      exportp_flag:=false;
    end;

  procedure process_export;

    var tchar : char;

    begin
      prcss_export_item;
      skipblanks;
      while current_char = ',' do
        begin
          lastpos := charpos;
          tchar := getschar;
          prcss_export_item;
          skipblanks 
        end
    end;

    procedure adjust_location_counter(var temp       : displacement;
                                      multiplier : integer);
      var exp : expptr;
          modifier, opsize,i : integer;
          tempop : optokenlist;
          fctype : factortypes;
          tmp, temp_location, temp2_location : integer;
 
      begin
        if temp.varying then
          begin
            (* displayterm(temp.disp_expr);*)
            tempop := mulop;
            fctype := fgen;
            temp_location := locationcounter;
            increment_locationcounter(temp.disp_value * multiplier);
            for i := 1 to 2 do
              begin
                (* displayterm(temp.disp_expr);*)
                exp := newexp;
                exp^.erec.opd1 := temp.disp_expr;
                exp^.erec.op := tempop;
                exp^.erec.opd2 := newterm;
                exp^.erec.opd2^.trec.termtype := factor;
                exp^.erec.opd2^.trec.unop := addop;
                exp^.erec.opd2^.trec.termlength.attribstate[0]:=chr(ord(false));
                if fctype = fgen then
                  begin
                    exp^.erec.opd2^.trec.fact.factortype := fgen;
                    exp^.erec.opd2^.trec.fact.fgenref.gtype := immediate;
                    exp^.erec.opd2^.trec.fact.fgenref.immatt.attribstate[0] :=
                                                                chr(ord(false));
                    exp^.erec.opd2^.trec.fact.fgenref.imint := multiplier;
                  end
                else
                 (* adding a temp symbol rather than using 
                    location counter thus allowing for variable 
                    growth *)
                  begin
                    temp2_location := locationcounter;
                    locationcounter := temp_location;
                    construct_temp_symbol(exp^.erec.opd2^.trec.fact);
                    locationcounter := temp2_location;
                  end;
                temp.disp_expr := newterm;
                temp.disp_expr^.trec.termtype := subexp;
                temp.disp_expr^.trec.termlength.attribstate[0]:=chr(ord(false));
                temp.disp_expr^.trec.unop := addop;
                temp.disp_expr^.trec.sub := exp;
                tempop := addop;
                fctype := fsym;
              end;
            add_to_loc_chain(temp.disp_expr);
            locationcounter := temp_location;
          end
      end;

    procedure scan_repeat_count(var temp : displacement;var found:boolean);

      var tchar : char;

      begin
        temp.disp_type := const_value;
        temp.varying := false;
        temp.disp_value := 1;
        found:=false;
        skipblanks;
        if current_char = '[' then
          begin
            found:=true;
            lastpos := charpos;
            tchar := getschar;
            scan_immed_expr(temp);
            if not((currtoken.tokentype=specchar) and (currtoken.spch=']')) then
              reporterror(100*041+invalidchar);
		! should be prevented from accepting span dependent expressions
            (*if sdi_flag then reporterror(100*208+sdi_repeat_count)*)
          end;
      end;

    procedure round_off_string(temp_str : str_ptr;
                               mod_amt : integer);

      var addon, index : integer;

      begin
        with temp_str^ do
        begin
          addon := strglen mod mod_amt;
          if addon <> 0 then
            begin
              addon := mod_amt - addon;
              for index := 1 to addon do
                strchars[strglen+index] := chr(0);
              strglen := strglen + addon;
            end;
        end;
      end;
    
    export procedure storage_init(var g_index : irec);

      begin
        g_index := new_g_index;
        with g_index^ do 
          begin
            global_interm_rec.linenumber := linenumber;
            location := locationcounter;
            locdir_number := total_loc_dirs;
          end;
      end;

    procedure process_byte(sign_flag:boolean);

      var g_index : irec;
          psize,wsize : integer;
          value, temp, temp2 : displacement;
          save_label  : symptr;
          found:boolean;

      begin
        skipblanks;
        save_label := labelptr;
        repeat
          scan_repeat_count(temp,found); temp2 := temp;
          if not error_flag then
            with global_interm_rec do
              begin
                scan_const(value);
	 	sign_b_flag[0]:=chr(ord(sign_flag));
                if not error_flag then
                  begin
                    if value.disp_type = const_value then
                      begin
                        if abs(value.disp_value) > 255 then
                          reporterror(100*042+termsizeextreme);
                        adjust_location_counter(temp2,1);
                        psize := temp.disp_value;
                      end
                    else
                      begin
                        adjust_location_counter(temp2,value.str_rec^.strglen);
                        psize := temp.disp_value * value.str_rec^.strglen;
                      end;
                    labelptr := nil;
                  end;
                if not error_flag then
                  if (currentmode = progrel) or (currentmode = stbsrel) then
                    begin
                      storage_init(g_index);
                      ftype := byteconst;
                      bcr_count := temp;
                      bc_value := value;
                      if temp.varying then
                        g_index^.locdir_number := g_index^.locdir_number - 1;
                      increment_locationcounter(psize);
                      if vm_flag then begin
                        g_intermrec := new_g_intermrec;
                        g_intermrec^.interm_rec := global_interm_rec;
                      end else begin
                        if currentmode =stbsrel then
                        wsize:=fwrite(global_interm_rec,
                                size(type global_interm_rec),1,ibuffer3)
                        else 
                        wsize:=fwrite(global_interm_rec,
                                size(intermcode),1,ibuffer1);
                       end;
                     end
                  else reporterror(100*043+illegal_mode);
              end
        until not((currtoken.tokentype=specchar) and (currtoken.spch=','))
              or error_flag;
        labelptr := save_label;
      end;

    procedure process_word(sign_flag:boolean);
      var temp,temp2, value : displacement;
          g_index : irec;
          psize,wsize : integer;
          save_label  : symptr;
          found : boolean;
      begin
        skipblanks;
        save_label := labelptr;
        repeat
          scan_repeat_count(temp,found); temp2 := temp;
          if not error_flag then
            with global_interm_rec do
              begin
                scan_const(value);
	 	sign_w_flag[0]:=chr(ord(sign_flag));
                if not error_flag then
                  begin
                    if value.disp_type = const_value then
                      begin
                        if abs(value.disp_value) > 65535 then
                          reporterror(100*044+termsizeextreme);           
                        adjust_location_counter(temp2,2);
                        psize := temp.disp_value *2;
                      end
                    else
                      begin
                        round_off_string(value.str_rec,2);
                        adjust_location_counter(temp2,value.str_rec^.strglen);
                        psize := value.str_rec^.strglen * temp.disp_value;
                      end;
                    labelptr := nil;
                  end;
                if not error_flag then
                  if (currentmode = progrel) or (currentmode = stbsrel) then
                    begin
                      storage_init(g_index);
                      ftype := wordconst;
                      wcr_count := temp;
                      wc_value := value;
                      if temp.varying then
                        g_index^.locdir_number := g_index^.locdir_number - 1;
                      increment_locationcounter(psize);
                      if vm_flag then begin
                        g_intermrec := new_g_intermrec;
                        g_intermrec^.interm_rec := global_interm_rec;
                      end else begin
                        if currentmode =stbsrel then
                        wsize:=fwrite(global_interm_rec,
                                size(type global_interm_rec),1,ibuffer3)
                        else 
                        wsize:=fwrite(global_interm_rec,
                                size(intermcode),1,ibuffer1);
                       end;
                    end
                  else reporterror(100*045+illegal_mode);
              end
        until not((currtoken.tokentype=specchar) and (currtoken.spch=','))
            or error_flag;
        labelptr := save_label;
      end;

    procedure process_double(sign_flag:boolean);

      var temp, value, temp2 : displacement;
          g_index : irec;
          psize,wsize : integer;
          save_label  : symptr;
          found : boolean;

      begin
        skipblanks;
        save_label := labelptr;
        repeat
          scan_repeat_count(temp,found); temp2 := temp;
          if not error_flag then
            with global_interm_rec do
              begin
                scan_const(value);
	 	sign_d_flag[0]:=chr(ord(sign_flag));
                if not error_flag then
                  begin
                    if value.disp_type = const_value then
                      begin       
                        adjust_location_counter(temp2,4);
                        psize := temp.disp_value * 4;
                      end
                    else
                      begin
                        round_off_string(value.str_rec,4);
                        adjust_location_counter(temp2,value.str_rec^.strglen);
                        psize := value.str_rec^.strglen * temp.disp_value;
                      end;
                    labelptr := nil;
                  end;
                if not error_flag then
                  if (currentmode = progrel) or (currentmode = stbsrel) then
                    begin
                      storage_init(g_index);
                      ftype := doubconst;
                      dcr_count := temp;
                      dc_value := value;
                      if temp.varying then
                        g_index^.locdir_number := g_index^.locdir_number - 1;
                      increment_locationcounter(psize);
                      if vm_flag then begin
                        g_intermrec := new_g_intermrec;
                        g_intermrec^.interm_rec := global_interm_rec;
                      end else begin
                        if currentmode =stbsrel then
                        wsize:=fwrite(global_interm_rec,
                                size(type global_interm_rec),1,ibuffer3)
                        else 
                        wsize:=fwrite(global_interm_rec,
                                size(intermcode),1,ibuffer1);
                       end;
                     end
                  else reporterror(100*046+illegal_mode);
              end
        until not((currtoken.tokentype=specchar) and (currtoken.spch=','))
            or error_flag;
        labelptr := save_label;
      end;


    procedure process_float;

      var temp, value, temp2 : displacement;
          g_index : irec;
          psize,wsize : integer;
          save_label  : symptr;
          found : boolean;

      begin
        skipblanks;
        save_label := labelptr;
        repeat
          scan_repeat_count(temp,found); temp2 := temp;
          if not error_flag then
            with global_interm_rec do
              begin
                scan_const(value);
                if not error_flag then
                  begin
                    if value.disp_type = const_value then
                      begin       
                        if value.disp_const_type=inti then 
                          reporterror(100*047+float_expected)
                        else 
                          begin value.disp_float_format:=short_float;
                            value.short_float_value:=
                                 make_short(value.long_float_value);
                          end;
                        adjust_location_counter(temp2,4);
                        psize := temp.disp_value * 4;
                      end
                    else
                      begin
                        round_off_string(value.str_rec,4);
                        adjust_location_counter(temp2,value.str_rec^.strglen);
                        psize := value.str_rec^.strglen * temp.disp_value;
                      end;
                    labelptr := nil;
                  end;
                if not error_flag then
                  if (currentmode = progrel) or (currentmode = stbsrel) then
                    begin
                      storage_init(g_index);
                      ftype := floatconst;
                      dcr_count := temp;
                      dc_value := value;
                      if temp.varying then
                        g_index^.locdir_number := g_index^.locdir_number - 1;
                      increment_locationcounter(psize);
                      if vm_flag then begin
                        g_intermrec := new_g_intermrec;
                        g_intermrec^.interm_rec := global_interm_rec;
                      end else begin
                        if currentmode =stbsrel then
                        wsize:=fwrite(global_interm_rec,
                                size(type global_interm_rec),1,ibuffer3)
                        else 
                        wsize:=fwrite(global_interm_rec,
                                size(intermcode),1,ibuffer1);
                       end;
                    end
                  else reporterror(100*048+illegal_mode);
              end
        until not((currtoken.tokentype=specchar) and (currtoken.spch=','))
            or error_flag;
        labelptr := save_label;
      end;

    procedure process_long;

      var temp, value, temp2 : displacement;
          g_index : irec;
          psize,wsize : integer;
          save_label  : symptr;
          found : boolean;

      begin
        skipblanks;
        save_label := labelptr;
        repeat
          scan_repeat_count(temp,found); temp2 := temp;
          if not error_flag then
            with global_interm_rec do
              begin
                scan_const(value);
                if not error_flag then
                  begin
                    if value.disp_type = const_value then
                      begin       
                        if value.disp_const_type=inti then 
                          reporterror(100*049+float_expected)
                        else 
                        adjust_location_counter(temp2,8);
                        psize := temp.disp_value * 8;
                      end
                    else
                      begin
                        round_off_string(value.str_rec,4);
                        adjust_location_counter(temp2,value.str_rec^.strglen);
                        psize := value.str_rec^.strglen * temp.disp_value;
                      end;
                    labelptr := nil;
                  end;
                if not error_flag then
                  if (currentmode = progrel) or (currentmode = stbsrel) then
                    begin
                      storage_init(g_index);
                      ftype := longconst;
                      dcr_count := temp;
                      dc_value := value;
                      if temp.varying then
                        g_index^.locdir_number := g_index^.locdir_number - 1;
                      increment_locationcounter(psize);
                      if vm_flag then begin
                        g_intermrec := new_g_intermrec;
                        g_intermrec^.interm_rec := global_interm_rec;
                      end else begin
                        if currentmode =stbsrel then
                        wsize:=fwrite(global_interm_rec,
                                size(type global_interm_rec),1,ibuffer3)
                        else 
                        wsize:=fwrite(global_interm_rec,
                                size(intermcode),1,ibuffer1);
                       end;
                    end
                  else reporterror(100*050+illegal_mode);
              end
        until not((currtoken.tokentype=specchar) and (currtoken.spch=','))
            or error_flag;
        labelptr := save_label;
      end;

    procedure process_blkb;
      var temp : displacement;
      begin
        scan_immed_expr(temp);
        if not error_flag then
          begin
            adjust_location_counter(temp,1);
            increment_locationcounter(temp.disp_value);
          end;
      end;
    procedure process_blkw;
      var temp : displacement;
      begin
        scan_immed_expr(temp);
        if not error_flag then
          begin
            adjust_location_counter(temp,2);
            increment_locationcounter(temp.disp_value*2);
          end;
      end;
    procedure process_blkd;
      var temp : displacement;
      begin
        scan_immed_expr(temp);
        if not error_flag then
          begin
            adjust_location_counter(temp,4);
            increment_locationcounter(temp.disp_value*4);
          end;
      end;
    procedure process_blkq;
      var temp : displacement;
      begin
        scan_immed_expr(temp);
        if not error_flag then
          begin
            adjust_location_counter(temp,8);
            increment_locationcounter(temp.disp_value*8);
          end;
      end;
    procedure process_blkf;
      var temp : displacement;
      begin
        scan_immed_expr(temp);
        if not error_flag then
          begin
            adjust_location_counter(temp,4);
            increment_locationcounter(temp.disp_value*4);
          end
      end;
    procedure process_blkl;
      var temp : displacement;
      begin
        scan_immed_expr(temp);
        if not error_flag then
          begin
            adjust_location_counter(temp,8);
            increment_locationcounter(temp.disp_value*8);
          end;
      end;


    export procedure process_field;
    
      var temp,value : displacement;
        s1: integer;
        found : boolean;
        savelabel : symptr;

      procedure emit(siz:integer;value:displacement);
        var f: field_rec_ptr;
        begin new(f);
          if field_last_rec=nil then global_interm_rec.first_field_rec:=f
          else field_last_rec^.next_field_rec:=f;
          f^.field_value:=value;
          f^.field_size:=siz;
          field_last_rec:=f;
          field_bit_count:=field_bit_count+siz;
          if field_siz<=0 then reporterror(100*209+need_a_positive);
        end;
      
      procedure flush;
        var g_index:irec;
        begin 
          field_siz:=1+(field_bit_count-1)div 8;
          if (currentmode=progrel) or (currentmode=stbsrel) 
            then 
              begin
                      storage_init(g_index);
                      increment_locationcounter(field_siz);
                      if vm_flag then begin
                        g_intermrec := new_g_intermrec;
                        g_intermrec^.interm_rec := global_interm_rec;
                      end else begin
                        if currentmode =stbsrel then
                        field_siz:=fwrite(global_interm_rec,
                                size(type global_interm_rec),1,ibuffer3)
                        else 
                        field_siz:=fwrite(global_interm_rec,
                                size(intermcode),1,ibuffer1);
                       end;
              end
          else reporterror(100*210+illegal_mode)
        end;      

      begin
        field_bit_count:=0;
        global_interm_rec.ftype:=fieldconst;
        field_last_rec:=nil;
        skipblanks;
        savelabel:=labelptr;
        repeat
          scan_repeat_count(temp,found);
          if not found then 
            begin
              reporterror(100*192+length_missing);
              error_flag:=true
            end;
          if not error_flag then
            begin 
              scan_const(value);
              if not error_flag then
                begin field_siz:=temp.disp_value;
                  labelptr:=nil;
                  emit(field_siz,value)
                end
            end
        until error_flag or
            not((currtoken.tokentype=specchar)and(currtoken.spch=','));
        if not error_flag then flush;
      end;

    procedure process_equ;
      var pc_ptr : symptr;
      begin
        if labelptr=nil then labelptr:=freelabel;
        if labelptr = nil then
          reporterror(100*051+invalidlabel)
        else
          begin
            equ_flag:=true;
            scan_equ_operand(labelptr^.symbol_definition);
            equ_flag:=false;
            labelptr^.equ_sdi[0]:=chr(ord(sdi_flag));
            if currentmode = progrel then     
		!must have been put in the pc chain. remove from there
              begin
                this_symbol:=nil;
                pc_ptr := first_pc_symbol;
                if pc_ptr = labelptr then
                  begin
                    first_pc_symbol := nil;
                    last_pc_symbol := nil
                  end
                else 
                  begin
                    while pc_ptr^.nextsym <> labelptr do
                      pc_ptr := pc_ptr^.nextsym;
                    pc_ptr^.nextsym := nil;
                    last_pc_symbol := pc_ptr;
                  end
              end;
            labelptr := nil;
          end;
      end;

  export procedure processrelocdir(dirindex : integer);
    begin
      case ord(dirmnemonics[dirindex].dirrelo[0])::relocdirs of 
        IMPORTd  : process_import;
        EXPORTd  : process_export;
        IMPORTPd : process_pimport;
        EXPORTPd : process_pexport;
        ROUTINESd: process_routines;
        COMMd    : process_common;
        LOC      : process_loc;
        DSECT    : process_dsect;
        STATIC   : process_static;
        ENDSEG   : pop_current_mode;
        PROGRAMd : process_program;
      end;
    end;

  export procedure processprocdir(dirindex : integer; var next_proc: procdirs);
        ! recursive descent procedure for .proc ... .endproc
    var proc_label: symptr;
        param_chainptr, chainptr: symptr;
        param_start: integer;
        param_size: integer;
        extra_size: integer;
        tmp_extra_size: integer;
        tempgen: gen;
        var_regs : pchar;
        g_index    : irec;
        mike_caca  : integer;
        vars_size : integer;
        nul_bwd : attribute;

    begin
      if labelptr=nil then labelptr:=freelabel;
      nul_bwd.attribstate[0] := chr(ord(false));
      next_proc := ord(dirmnemonics[dirindex].dirproc[0])::procdirs;
      if dirmnemonics[dirindex].dirproc[0] = chr(ord(PROCd)) then
        begin
          if labelptr <> nil then
            begin
              proc_label := labelptr;
              with proc_label^.scope do
                begin
                  if (domain & N_EXT) = N_EXT
                    then param_start := 12
                  else
                    param_start := 8;
                  domain := domain | N_PROC;
                end;
              push_current_mode;
              appendtoroot(labelptr);
              currentmode := frrel;
              assembly_mode := link_inverted;
              locationcounter := 0;
              param_chainptr := nil;
              scan_storage_directives(next_proc, param_chainptr);
              if (next_proc = RETURNS) or (next_proc = VARd) then
                begin
                  param_size := locationcounter;
                  extra_size := param_size;
                  if next_proc = RETURNS then
                    begin
                      locationcounter := 0;
                      chainptr := nil;
                      scan_storage_directives(next_proc, chainptr);
                      if locationcounter < param_size then 
                        begin
                          param_size := locationcounter;
                          reporterror(100*052+returns_too_long);
                        end;
                      extra_size := - (param_size - locationcounter);
                      adjust_proc_labels(chainptr, param_start + extra_size,
                                 locationcounter);
                    end;
                  adjust_proc_labels(param_chainptr, param_start, param_size);
                  if next_proc = VARd then
                    begin
                      locationcounter := 0;
                      assembly_mode := inverted;
                      scan_reg_list(var_regs);
                      scancomment;
                      chainptr := nil;
                      scan_storage_directives(next_proc, chainptr);
                      if next_proc = BEGINd then
                        begin
                          currentmode := progrel;
                          assembly_mode := standard;
                          vars_size := -locationcounter;
                          g_index := new_g_index;
                          g_index^.location := last_alloc_pc;
                          g_index^.locdir_number := total_loc_dirs;
                          global_interm_rec.linenumber := linenumber-1;
                          with global_interm_rec do
                            begin
                              sdi_flag := false;
                              irec_ptr := g_index;
                              ftype := format1;
                              f1op[0] := chr(ord(ENTER));
                              f1operand.gtype := immediate;
                              f1operand.immatt.attribstate[0]:= chr(ord(false));
                              f1operand.gen_scale.scalestate:=false;
                              check_range (f1operand.immatt, vars_size);
                              f1operand.numtype := inti;
                              f1operand.imint := vars_size;
                              f1regs[0]:= var_regs[0];
                              check_operand(f1operand,mike_caca,
                                            g_index,1,nul_bwd);
                            end;
                          locationcounter := last_alloc_pc;
                          increment_locationcounter(mike_caca+2);
                          if vm_flag then begin
                            g_intermrec := new_g_intermrec;
                            g_intermrec^.interm_rec := global_interm_rec;
                          end else 
                            mike_caca:=fwrite(global_interm_rec,
                                    size(intermcode),1,ibuffer1);
                          scan_proc_insts(next_proc);
                          if next_proc <> ENDPROC then
                            begin
                              reporterror(100*053+missing_end_proc);
                            end
                          else 
                            begin
                              g_index := new_g_index;
                              g_index^.location := last_alloc_pc;
                              g_index^.locdir_number := total_loc_dirs;
                              global_interm_rec.linenumber := linenumber-1;
                              with global_interm_rec do
                                begin
                                  sdi_flag := false;
                                  irec_ptr := g_index;
                                  ftype := format1;
                                  f1op[0] := chr(ord(EXITo));
                                  f1regs[0]:= var_regs[0];
                                end;
                              increment_locationcounter(2);
                              if vm_flag then begin
                                g_intermrec := new_g_intermrec;
                                g_intermrec^.interm_rec := global_interm_rec;
                              end else 
                                mike_caca:=fwrite(global_interm_rec,
                                      size(intermcode),1,ibuffer1);
                          
                              g_index := new_g_index;
                              g_index^.location := last_alloc_pc;
                              g_index^.locdir_number := total_loc_dirs;
                              global_interm_rec.linenumber := linenumber-1;
                              with global_interm_rec do
                                begin
                                  sdi_flag := false;
                                  irec_ptr := g_index;
                                  ftype := format1;
                                  if param_start = 8 then 
                                    f1op[0] := chr(ord(RET))
                                  else
                                    f1op[0] := chr(ord(RXP));
                                  f1operand.gtype := immediate;
                                  f1operand.immatt.attribstate[0]:= 
                                                chr(ord(false));
                                  skipblanks;
                                  if (current_char=';')or(current_char=eol)
                                    then tmp_extra_size:=extra_size
                                    else
                                      begin 
                                      scan_general_expr(tempgen);
                                      if tempgen.gtype<>immediate
                                        then reporterror(100*054+illegal_term)
                                        else if tempgen.numtype=floati 
                                        then reporterror(100*055+illegal_term) 
                                        else tmp_extra_size:=tempgen.imint;
                                      end;
                                  check_range (f1operand.immatt,tmp_extra_size);
                                  f1operand.numtype := inti;
                                  f1operand.imint :=tmp_extra_size;
                                  f1operand.gen_scale.scalestate:=false;
                                  check_operand(f1operand,mike_caca,
                                              g_index,1,nul_bwd);
                                end;
                              locationcounter := last_alloc_pc;
                              increment_locationcounter(mike_caca+1);
                              if vm_flag then begin
                                g_intermrec := new_g_intermrec;
                                g_intermrec^.interm_rec := global_interm_rec;
                              end else 
                                mike_caca:=fwrite(global_interm_rec,
                                        size(intermcode),1,ibuffer1);
                              end;
                        end
                      else
                        reporterror(100*056+missing_begin);
                    end
                  else
                    reporterror(100*057+missing_var);
                end
              else reporterror(100*058+missing_var);
              pop_root_level;
              pop_current_mode
            end
          else reporterror(100*059+label_required);
        end
      else if dirmnemonics[dirindex].dirproc[0] = chr(ord(MODULEd)) then
        begin
          make_global := true;
	  mod_name := true;
          proc_label := scanglobsymbol;
          make_global := false;
	  mod_name := false;
          if proc_label <> nil then
            with proc_label^ do
            if symdefined[0] = chr(ord(false)) then
              begin
                symbol_definition.gtype := mod_def;
                linedefined := linenumber;
                with symroot^ do
                begin
                  if syment = nil then
                    begin
                      symdefined[0] := chr(ord(true));
                      syment := proc_label;
                    end
                  else reporterror(100*060+duplicate_def);
                end
              end
            else reporterror(100*061+duplicate_def);
        end
      else
        reporterror(100*062+invaliddir);
    end;

 procedure get_stab_rec(var h:stab_rec_ptr);
   begin h:=nil;
     if above_symbol=nil then reporterror(100*193+dangling_stab)
     else
       begin new(h);
         h^.next_stab_rec:=above_symbol^.first_stab;
         above_symbol^.first_stab:=h
       end;
     this_symbol:=above_symbol
   end;

 procedure process_sstabs;
   var h:stab_rec_ptr;
       tchar: char;
   begin 
     nexttoken(currtoken);
     if currtoken.tokentype<>symbol then reporterror(100*212+illegal_term)
     else 
       begin
	 currtoken.sym[currtoken.symlength+1]:=chr(0);
         get_stab_rec(h);
	 h^.whichstab:=STABSd;
         newsymentry(h^.symbol_ptr);
         with hs=h^.symbol_ptr^ do
           begin
              new_name(hs.symbolname,currtoken.sym,curr_symname,symnames);
              hs.linedefined:=linenumber;
              skipblanks;
	      !if current_char='{' then scan_type_info(hs.type_of_symbol) 
	      !else hs.type_of_symbol:=nil;
              skipblanks;
              if current_char=',' then
                begin
		  tchar:=getlchar;
                  equ_flag:=true;
                  scan_equ_operand(hs.symbol_definition);
                  equ_flag:=false;
                  hs.symdefined[0]:=chr(ord(true));
                end
              else hs.symdefined[0]:=chr(ord(false))
           end
       end
   end;

 procedure process_stab;
   var h:stab_rec_ptr;
       t_inf:type_info_ptr;
        tchar:char;
   begin 
     get_stab_rec(h);
     if h<> nil then with h^ do
       begin
         h^.whichstab:=STABd;
         skipblanks;
         if global_type_info=nil then reporterror(100*194+missing_type_info)
         else
           begin
	     type_info_doub:=global_type_info^.t_doub_word;
             skipblanks;
             if current_char=',' then
               begin tchar:=getlchar;
                 scan_const(stabnum2);
                 if not error_flag then
                   if(currtoken.tokentype=specchar)and(currtoken.spch=',')
                   then
                     begin
                       is_1_present:=true;
                       stabnum1:=stabnum2;
                       scan_const(stabnum2)
                     end
                   else is_1_present:=false
               end
           end
       end
   end;          

 procedure process_source;
   var s:stab_rec_ptr;  t:token;  sp:symptr;
       tchar:char;un,i:integer;
   begin
     skipblanks;
     if (current_char>='0') and (current_char<='9') then
       begin if currentmode<>progrel then reporterror(100*190+source_mode)
         else
           begin
             new(s);
             s^.whichstab:=SOURCEd;
             s^.ispcrel:=true;
             t.tokentype:=symbol;
             for i:=2 to 33 do t.sym[i]:=chr(0);
             t.sym[1]:=' ';
             un:=uniqueinteger;
             for i:=11 downto 2 do
               begin t.sym[i]:=chr(ord('0')+(un mod 10));
                 un:=un div 10
               end;
             t.symlength:=11;
             sp:=processsymbol(t);
             with sp^ do
               begin scope.parent:=symroot;
                 symdefined[0]:=chr(ord(true));
                 linedefined:=linenumber;
                 first_stab:=s;
                 with symbol_definition do
                   begin
                     gtype:=regrelative;
                     regreloffset:=locationcounter;
                     nextsym:=nil;
                     rltype:=pc;
                     if last_pc_symbol<>nil then 
                        last_pc_symbol^.nextsym:=sp
                     else first_pc_symbol:=sp;
                     last_pc_symbol:=sp;
                     sdi_count:=total_sdi_count;
                     locdir_number:=total_loc_dirs;
                   end
               end
           end
       end
     else
       begin nexttoken(t);
         if t.tokentype<>symbol then reporterror(100*183+illegal_term)
           else 
             begin sp:=findsym(symtable,t.sym);
               if sp=nil then reporterror(100*184+undefined_symbol)
                 else
                   begin new(s);s^.next_stab_rec:=sp^.first_stab;
                     s^.whichstab:=SOURCEd;
                     sp^.first_stab:=s;
                     s^.ispcrel:=false;
                     skipblanks;
                     if current_char<>',' then 
                                    reporterror(100*191+illegal_term)
                     else tchar:=getschar;
                   end
             end
       end;
     if (current_char<='0') or (current_char>='9') then 
                    reporterror(100*185+illegal_term)
     else
       begin t.tokentype:=gentoken;
         scannumber(t);
         if t.genref.numtype=floati then reporterror(100*186+illegal_term);
         s^.sourceitem:=t.genref.imint;
         skipblanks;
         if (current_char<>'l') and (current_char<>'c') and
            (current_char<>'L') and (current_char<>'C') then
                    reporterror(100*187+illegal_term)
         else
           begin 
             s^.islineorchar:=(current_char='L')or(current_char='l');
             tchar:=getschar;
             skipblanks;
             if (current_char=',') then
               begin tchar:=getschar;
                 nexttoken(t);
                 if t.tokentype<>symbol then 
                                    reporterror(100*188+illegal_term)
                 else 
                    new_name(s^.whichfile,t.sym,curr_symname,symnames)
               end
             else 
               if stabfilename=nil then
                 reporterror(100*189+missing_filename)
               else s^.whichfile:=stabfilename;
             stabfilename:=s^.whichfile
           end
       end      
   end;       






  export  procedure processstordir(dirindex : integer);

    begin
      if ((ord(dirmnemonics[dirindex].dirstor[0])::stordirs) in 
                                       [BYTE,WORD,DOUBLE,FLOATd,LONG]) and 
         not((currentmode = progrel) or (currentmode = stbsrel)) then
           reporterror(100*063+invaliddir)
      else
        case ord(dirmnemonics[dirindex].dirstor[0])::stordirs of 
          BYTE          : process_byte(false);
          WORD          : process_word(false);
          DOUBLE        : process_double(false);
          SBYTE         : process_byte(true);
          SWORD         : process_word(true);
          SDOUBLE       : process_double(true);
          FLOATd        : process_float;
          LONG          : process_long;
          BLKB          : process_blkb;
          BLKW          : process_blkw;
          BLKD          : process_blkd;
          BLKQ          : process_blkq;
          BLKF          : process_blkf;
          BLKL          : process_blkl;
          ALIGN         : process_align;
          FIELD         : process_field;
          EQU           : process_equ;
        end;
    end;
  export procedure processlistdir(dirindex : integer);
    begin
      case ord(dirmnemonics[dirindex].dirlist[0])::listdirs of 
        TITLE:          process_title(dirindex); 
        SUBTITLE:       process_subtitile(dirindex); 
        EJECT:          process_eject(dirindex); 
        NOLIST:         process_nolist(dirindex); 
        LIST:           process_list(dirindex); 
        WIDTH:          process_width(dirindex); 
      end;
    end;

procedure processstabdir(dirindex:integer);
  var tchar:char;
  begin 
    if {objsymtable} true then  { Tim }
      case ord(dirmnemonics[dirindex].dirstab[0])::stabdirs of
        STABd:          process_stab;
        STABSd:         process_sstabs;
        SOURCEd:        process_source;
      end
    else 
      while current_char<>eol do tchar:=getlchar
  end;

  export procedure processdirective(var temptoken : token);

    var dirindex : integer;
        next_proc: procdirs;

    begin
      dirindex := getdirindex(temptoken);
      if dirindex = -1 then
        reporterror(100*064+invaliddir)
      else
        begin
          case dirmnemonics[dirindex].dirtype of
            relocscope : processrelocdir(dirindex);
            proc       : processprocdir(dirindex, next_proc);
            storage    : processstordir(dirindex);
            stabs      : processstabdir(dirindex);
            lexscope   : writeln('lexical scoping not implemented');
            listing    : processlistdir(dirindex);
          end;
        end;
    end;

  begin 
    uni_num:=0;
    assembly_mode := standard;
    currentmode := progrel;
    exportchain := nil; exportstart := nil;
    importchain := nil; importstart := nil;
    modeindex := 1; export_count := 0; import_count := 0;
    routines_flag := false;
    exportp_flag:=false;
  end.

