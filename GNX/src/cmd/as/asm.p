{!L}
{!B}
program asm16000(input,output);

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

const SCCSId = Addr('@(#)asm.p	3.31         9/20/83');
const CpyrId = Addr('@(#)Copyright (C) 1983 by National Semiconductor Corp.');

export 'glob_const.e';  ! DEPENDENCY
export 'formatsmod.e';  ! DEPENDENCY
export 'glob_types.e';  ! DEPENDENCY
export 'glob_vars.e';  ! DEPENDENCY
export 'list_types.e';  ! DEPENDENCY
import 'bio.e';  ! DEPENDENCY
import 'scantoken.e';  ! DEPENDENCY
import 'init1.e';  ! DEPENDENCY
import 'scan.e';  ! DEPENDENCY
import 'scanr.e';  ! DEPENDENCY
import 'dirs.e';  ! DEPENDENCY
import 'insts.e';  ! DEPENDENCY
import 'traverse.e';  ! DEPENDENCY
import 'sdis.e';  ! DEPENDENCY
import 'genrout.e';  ! DEPENDENCY
import 'codegen.e';  ! DEPENDENCY
import 'disp.e';  ! DEPENDENCY

type rel_rec_type = (pcrel_rec, sbrel_rec);
var colindex:integer;
    ltimeu,ltimes:integer;
    timeu,times:integer;
    rec_mode: rel_rec_type;
import routine ptime(var a,b:integer);forward;
import procedure disp_girec; forward;
import procedure disp_genirec; forward;
export routine dummy1;begin end;

  procedure scanstatement; ! first phase. scans a line

! First phase, scan a line. Build symbol table entry, each source line has
! a corresponding intermcode entry in the intermediate files (ibuffer1(pc) 
! or ibuffer3(sb)).
! There are irec chain for pc relative code and irec chain for sb relative
! code.

    var tyinpt:type_info_ptr;

    begin
      this_symbol:=nil;
      global_type_info:=nil;
      labelptr := nil;
      error_flag := false;
      nexttoken(currtoken);
      skipblanks;
      if current_char = ':' then
        begin
          scanlabel(currtoken,nil);
	  nexttoken(currtoken);
	end
      (*else if (current_char='brace') then
        begin 
          scan_type_info(tyinpt);
	  global_type_info:=tyinpt;
          skipblanks;
          if current_char = ':' then
            begin
              scanlabel(currtoken,tyinpt);
	      nexttoken(currtoken);
    	    end
        end*);
      with currtoken do
        if tokentype = symbol then
	  begin
            if sym[1] = '.' then
              processdirective(currtoken)
            else
	      process_instruction(currtoken);
	    freelabel:=nil;
	  end
        else if not((tokentype=specchar) and ((spch=eol) or (spch=';'))) then
	  reporterror(100*001+invalidopcode); 
      scancomment;	
      if (assembly_mode = inverted) and (labelptr <> nil) then
      labelptr^.symbol_definition.regreloffset := locationcounter;
      above_symbol:=this_symbol;
    end;

  procedure gen_read(var gen_val: gen; current_rec : rel_rec_type);
    begin
    if current_rec = pcrel_rec then begin
       gen_val := first_genirec^.gen_rec;
       first_genirec := first_genirec^.gen_next;
    end else begin
       gen_val := first_sbgenirec^.gen_rec;
       first_sbgenirec := first_sbgenirec^.gen_next;
    end;
   end;
  
  procedure grec_read(g_index : irec; current_rec : rel_rec_type);
    var temp : integer;

    begin
      if g_index <> nil then
        begin
          if current_rec = pcrel_rec then  begin
             global_interm_rec := first_girec^.interm_rec;
             first_girec := first_girec^.interm_next;
          end else begin
             global_interm_rec := first_sbgirec^.interm_rec;
             first_sbgirec := first_sbgirec^.interm_next;
          end;
	  with global_interm_rec do
	    if ftype = format2 then
	      begin
	        if f2op = ACBx then
	        gen_read(f2label^, current_rec);
	      end
	    else if ftype = format7 then
	      begin
	        if(fop7=MOVMx)or(fop7=CMPMx)then
		  begin
		   gen_read(f7constant^, current_rec);
		  end
	        else if(fop7=INSSx)or(fop7=EXTSx)then
		  begin
		    gen_read(f7constant1^, current_rec);
		    gen_read(f7constant2^, current_rec);
		   end
	      end
	    else if ftype = format8 then
	      begin
	        if(fop8 = EXTs)or(fop8 = INSx)then
		  begin
		    gen_read(f8disp^, current_rec);
		  end
	      end;
       end
     else global_interm_rec.linenumber := maxint;
   end;

 procedure fgrec_read(g_index : irec; var thisfile : cfile);
   	! read intermediate file
    var temp : integer;

    begin
      if g_index <> nil then
        begin
	  temp := fread(global_interm_rec,size(intermcode),1,thisfile);
	  with global_interm_rec do
	    if ftype = format2 then
	      begin
	        if f2op = ACBx then
		  temp := fread(f2label^,size(gen),1,thisfile)
	      end
	    else if ftype = format7 then
	      begin
	        if(fop7=MOVMx)or(fop7=CMPMx)then
		  !begin
		    temp := fread(f7constant^,size(gen),1,thisfile)
		  !end
	        else if(fop7=INSSx)or(fop7=EXTSx)then
		  begin
		    temp := fread(f7constant1^,size(gen),1,thisfile);
		    temp := fread(f7constant2^,size(gen),1,thisfile);
		   end
	      end
	    else if ftype = format8 then
	      begin
	        if(fop8 = EXTs)or(fop8 = INSx)then
		  !begin
		    temp := fread(f8disp^,size(gen),1,thisfile)
		  !end
	      end;
       end
     else global_interm_rec.linenumber := maxint;
end;

 procedure phase2;

! Second phase, code generation, output the pc relative code and
! the sb relative code to obuffer(object file) according to the 
! information got from the intermcodes in intermediate files
! , the sdi pass and related tables.

   var g_index : irec;

    begin
      current_sdi := sdi_start;
      g_index := first_irec;
      while (g_index <> nil) do ! generate text 
	begin
	  if vm_flag then grec_read(g_index,pcrel_rec)
                       else fgrec_read(g_index,ibuffer1);
	  gencode(g_index);
	  g_index := g_index^.next_irec;
	end;
      flush_bytes;
      sb_init;
      g_index := first_sbirec;
      while (g_index <> nil) do  ! generate static base area
	begin
	  if vm_flag then grec_read(g_index,sbrel_rec)
                       else fgrec_read(g_index,ibuffer3);
	  gencode(g_index);
	  g_index := g_index^.next_irec;
	end;
      flush_bytes;
    end;


  procedure end1; ! write two last intermcodes on intermediate files

var 
  g_index: irec;
  wsize: integer;
  temp_global: intermcode;
begin
  with global_interm_rec do begin
    currentmode := stbsrel;
    g_index := new_g_index;
    g_index^.location := last_alloc_sb;
    g_index^.locdir_number := total_loc_dirs;
    global_interm_rec.linenumber := linenumber-1;
    irec_ptr := g_index;
    ftype := last_interm;
    if vm_flag then begin g_intermrec := new_g_intermrec;
                        g_intermrec^.interm_rec := global_interm_rec;
                    end
               else wsize := fwrite(global_interm_rec,size(intermcode),1,ibuffer3);
    currentmode := progrel;
    g_index := new_g_index;
    g_index^.location := last_alloc_pc;
    g_index^.locdir_number := total_loc_dirs;
    global_interm_rec.linenumber := linenumber-1;
    irec_ptr := g_index;
    ftype := last_interm;
    if vm_flag then begin g_intermrec := new_g_intermrec;
                        g_intermrec^.interm_rec := global_interm_rec;
                    end
               else wsize := fwrite(global_interm_rec,size(intermcode),1,ibuffer1);
  end; { do }
end;

import routine pexit2;forward;
import routine pexit1;forward;

begin    { MAIN ROUTINE }
  init1;
  ptime(ltimeu,ltimes);
  repeat   ! phase 1
    scanstatement;
  until eof_sbuffer;
  scanstatement;
  ptime(timeu,times);
  !writeln('Time in P1 :',timeu-ltimeu,times-ltimes);
  ltimes:=times;ltimeu:=timeu;
  if not source_errors then 
    begin
      end1;
      init2;
      !ptime(ltimeu,ltimes);
      !writeln('import count = ',import_count:0);
      !dispsymentrylist(3,3);
      !disp_sdi_ptr_list(3,3);
      !disp_irec_list(3,3);
      resolve_sdis; ! sdi phase
      !dispsymentrylist(4,4);
      !disp_sdi_ptr_list(4,4);
      !disp_irec_list(4,4);
      !ptime(timeu,times);
      !writeln('Time in SD :',timeu-ltimeu,times-ltimes);
      ltimes:=times;ltimeu:=timeu;
      if not source_errors then
        begin
	  phase2;
	  output_import_list;
	  output_export_list;
	  output_sym_ents(symtable);
	  output_strings;
	  closelistfiles;
          !ptime(timeu,times);
          !writeln('Time in P2 :',timeu-ltimeu,times-ltimes);
        end
      else
	begin
	  remove_t5;
	end;
    end
  else list_t4;
  closeonelist;
  remove_tmps;
  clean_up;	!! fix up module header record in output file
  if printsymtable then
    begin
      writeln(chr(12));	!form feed
      write(blank:5);
      colindex:=1;
      while (asm_title[colindex]<>'#') do
        begin write(asm_title[colindex]);
          colindex:=colindex+1;
        end;
      while colindex<55 do
        begin write(' ');
          colindex:=colindex+1
        end;
      writeln('Cross reference');
      writeln;
      traversesymtable(symtable);
    end;
  if (source_errors or error_flag) then pexit1;
  if false then writeln(sccsid,cpyrid)

  ! determine exit status here and set accordingly
  ! four types of exit are:
  !	- no errors and no listing to generate (source_errors, big_list and
  !	  simple_list all false)
  !     - errors but no listing (source_errors true and the rest false)
  !     - simple listing to be generated (simple_list true, big_list false,
  !       source_errors either way)
  !	- -l listing - big_list true and don't care about the rest
end.

