{!L}
module init1;
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

const SCCSId = Addr('@(#)init1.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

!initialization and end routines

import 'glob_const.e'; !DEPENDENCY
import 'formatsmod.e'; !DEPENDENCY
import 'glob_types.e'; !DEPENDENCY
import 'glob_vars.e'; !DEPENDENCY
import 'mnem_def.e'; !DEPENDENCY
import 'bio.e'; !DEPENDENCY
import 'scantoken.e'; !DEPENDENCY
import 'scanexp.e'; !DEPENDENCY
import 'scanr.e'; !DEPENDENCY
import 'dirs.e'; !DEPENDENCY
import 'insts.e'; !DEPENDENCY
import 'scan.e'; !DEPENDENCY
import 'dirsx.e'; !DEPENDENCY
import 'prsymtable.e'; !DEPENDENCY
import 'sdis.e'; !DEPENDENCY
import 'codegen.e'; !DEPENDENCY
import 'genrout.e'; !DEPENDENCY
import 'traverse.e'; !DEPENDENCY
import 'disp.e'; !DEPENDENCY

export 'init1.e'; !DEPENDENCY
export 'a.out.e'; !DEPENDENCY
export 'stab.e'; !DEPENDENCY

type mod_entry = record
      m_static : integer;
      m_link   : integer;
      m_prog   : integer;
      m_reser  : integer;
     end;

     link_entry = record
      l_offset : integer;
      l_mod    : integer;
    end;

    tmp_reloc_rec = record
     r_address : integer;
     r_value : integer;
    end;
  unar=^array[1..1000]of ^array[1..1000]of char;

var mod_posn, link_posn : integer;
    mod_rec : mod_entry;
    link_rec: link_entry;
    hdr_rec     : exec;

    optable    : cfile;
    t1,t3,t4,t5,t6
		: string32;
import var ac_:integer;
import var av_:unar;
export var sizeb: integer;  ! size for malloc
import routine fopen_n(var name: string32;mode :char):cfile;forward;

procedure init_glbl_vars;
  begin 
    symroot := nil; 
    symtable := nil;
    locationcounter := 0;
    currentlinkpage := 0;
    linenumber := 1;  
    source_errors := false;
    pass2 := false;
    total_sdi_count := 0;
    total_loc_dirs := 0;
    last_pc_symbol := nil;
    first_pc_symbol := nil;
    process_cxp := false;
    process_d_cxp:=false;  (* for ext_to_sbrelative only *)
    upperlower := true;
    vm_flag := false; {default using files as intermediate storage}
    simple_list := false;
    big_list := true;
    tree_display:=false;     
    line_trace:=false;
    printsymtable := false;
    objsymtable := false;
    ext_to_sbrelative := false;
    BACK_COMPATIBLE:=true;
    make_global := false;
    out_sym_count := 0;
    eof_sbuffer :=false;
    last_symbol :=nil;
    above_symbol :=nil;
    this_symbol:=nil;
    stabfilename :=nil;
    mod_name:=false;
    do_list:=false;
    done_list:=false;
    freelabel:=nil;
    equ_flag:=false;
  end;

procedure buildoptables;
  var
    tmp1,tmp2	: integer;
    tempchar  : char;
    opfile:string32;
    lastop:integer;

  begin
    ! make opstart and rtstart tables. needed for searching
    if (drmn_size<>size(direntry)*maxdirs) or
     (inmn_size<>size(instentry)*maxinsts) or
     (rtmn_size<>size(rtentry)*maxtokens) 
    then 
      begin writeln('optable sizes don''t match ',drmn_size,
                  size(direntry)*maxdirs,inmn_size,size(instentry)*maxinsts,
		  rtmn_size,size(rtentry)*maxtokens);
        flush(output);
        assert(false);
      end;
    for tmp1 := 61 to 122 do
      begin
	opstart[tmp1] := -1;
	rtstart[tmp1] := -1;
      end;
    tempchar := chr(ord('a')-1);
    for tmp1 := 1 to maxinsts do
      begin
	if mnemonics[tmp1].instchars[1] > tempchar then
	  begin
	    repeat
	      tempchar := chr(ord(tempchar)+1);
	    until tempchar = mnemonics[tmp1].instchars[1];
	    opstart[ord(tempchar)] := tmp1;
	  end;
      end;
    opstart[123] := maxinsts;
    if false then
      begin
        for tmp1 := 61 to 122 do 
          writeln(' "',chr(tmp1),'" opstart[',tmp1:3,']=',opstart[tmp1]:3);
	for tmp1:=61 to 122 do 
          writeln(' "',chr(tmp1),'" rtstart[',tmp1:3,']=',rtstart[tmp1]:3)
      end;
    tempchar := chr(ord('a')-1);
    for tmp1 := 1 to maxtokens do
      begin
	if rtmnemonics[tmp1].rtchars[1] > tempchar then
	  begin
	    repeat
	      tempchar := chr(ord(tempchar)+1);
	    until tempchar = rtmnemonics[tmp1].rtchars[1];
	    rtstart[ord(tempchar)] := tmp1;
	  end;
      end;
  end;
procedure assert1(a:boolean);
  begin assert(a) end;

procedure openfiles;
  var temp1,temp2     : integer;
      btemp1		: boolean;
  ! set flags according to options, get filenames, make temporary names,
  ! if not given . Open files
  begin
    for temp1 := 1 to 32 do begin
      sourcename[temp1] := chr(0);
      objfilename[temp1] := chr(0);
    end;
    if ac_ > 1 then begin
      temp1 := 2;   
      while av_^[temp1]^[1] = '-' do begin   !process cmd line options
	with chtemp1 = av_^[temp1]^[2] do begin
	  if chtemp1 = 'l' then	!make listing
	    begin big_list := true;do_list:=true;done_list:=true end
	  else if chtemp1 = 'y' then	!dump symtbl to listing
	    printsymtable := true
          else if chtemp1 = 't' then    (* by sunil, for tree display *)
            tree_display  :=true
          else if chtemp1 = 'v' then  {intermediate files in virtual memory}
            vm_flag := true
          else if chtemp1 = 'q' then
            ext_to_sbrelative:= true
          else if chtemp1 = 'r' then
            line_trace:= true   
	  else if chtemp1 = 'L' then	!write symbols to objfil
	    objsymtable := true
	  else if chtemp1 = 'u' then 	     !Upper case kept
	    upperlower := true
	  else if chtemp1 = 'i' then	     ! ignore case in source
	    upperlower := false
	  else if chtemp1 = 'N' then
	    BACK_COMPATIBLE:=false
	  else if chtemp1 = 'o' then begin   !named object file
	    temp1 := temp1 + 1;
	    temp2 := 1;
	    while av_^[temp1]^[temp2]<>chr(0) do begin
	      objfilename[temp2] := av_^[temp1]^[temp2];
	      temp2 := temp2 + 1;
	    end;
	  end;
	end;
	temp1 := temp1 + 1;
      end;
      temp2 := 1;
      with arg=av_^[temp1]^ do
	while arg[temp2]<>chr(0) do begin
	  sourcename[temp2] := arg[temp2];
	  temp2 := temp2 + 1;
	end; {while, with}

      if temp1>=ac_ then
        begin t1:='ttasm1XXXXXX                    ';
	  t1[13]:=chr(0);
	  mktemp(t1);
          writeln(first_list_char:1,second_list_char:1,third_list_char:1,t1);
	end
      else 
	begin temp1:=temp1+1;temp2:=1;
          with arg=av_^[temp1]^ do
	    while arg[temp2]<>chr(0) do begin
	      t1[temp2] := arg[temp2];
	      temp2 := temp2 + 1;
	    end; {while, with}
        end;
     if not vm_flag then begin
      if temp1>=ac_ then
        begin t3:='ttasm3XXXXXX                    ';
	  t3[13]:=chr(0);
	  mktemp(t3);
          writeln(first_list_char:1,second_list_char:1,third_list_char:1,t3);
	end
      else 
	begin temp1:=temp1+1;temp2:=1;
          with arg=av_^[temp1]^ do
	    while arg[temp2]<>chr(0) do begin
	      t3[temp2] := arg[temp2];
	      temp2 := temp2 + 1;
	    end; {while, with}
        end;
     end; { if not vm_flag }

      if temp1>=ac_ then
        begin t4:='ttlst1XXXXXX                    ';
	  t4[13]:=chr(0);
	  mktemp(t4);
          writeln(first_list_char:1,second_list_char:1,third_list_char:1,t4);
	end
      else 
	begin temp1:=temp1+1;temp2:=1;
          with arg=av_^[temp1]^ do
	    while arg[temp2]<>chr(0) do begin
	      t4[temp2] := arg[temp2];
	      temp2 := temp2 + 1;
	    end; {while, with}
        end;
      if temp1>=ac_ then
        begin t5:='ttlst3XXXXXX                    ';
	  t5[13]:=chr(0);
	  mktemp(t5);
          writeln(first_list_char:1,second_list_char:1,third_list_char:1,t5);
	end
      else 
	begin temp1:=temp1+1;temp2:=1;
          with arg=av_^[temp1]^ do
	    while arg[temp2]<>chr(0) do begin
	      t5[temp2] := arg[temp2];
	      temp2 := temp2 + 1;
	    end; {while, with}
        end;
      if temp1>=ac_ then
        begin t6:='ttlst2XXXXXX                    ';
	  t6[13]:=chr(0);
	  mktemp(t6);
          writeln(first_list_char:1,second_list_char:1,third_list_char:1,t6);
	end
      else 
	begin temp1:=temp1+1;temp2:=1;
          with arg=av_^[temp1]^ do
	    while arg[temp2]<>chr(0) do begin
	      t6[temp2] := arg[temp2];
	      temp2 := temp2 + 1;
	    end; {while, with}
        end;
    end
    else 
      begin
	writeln('error no source file specified');
	assert1(false);
       end;
    if objfilename[1]=chr(0) then begin
      objfilename := 'a16.out                         ';
      objfilename[8] := chr(0);	!just to make sure it's null-terminated
    end;
    reset(sbuffer,sourcename);
    if not vm_flag then begin
      ibuffer1 := fopen_n(t1,'w');
      ibuffer3 := fopen_n(t3,'w');
    end;
    lstbuff1 := fopen_n(t4, 'w');
    unlink(objfilename); obuffer := fopen_n(objfilename,'w');
    if obuffer <= 0 then begin
      writeln('error opening ',objfilename);
      assert1(false);
    end;
    if not vm_flag then begin
      if ibuffer1 <= 0 then begin
        writeln('error opening asmtemp1');
        assert1(false);
      end;
      if ibuffer3 <= 0 then begin
        writeln('error opening asmtemp3');
        assert1(false);
      end;
    end;

    if lstbuff1 <= 0 then begin
      writeln('error opening lsttemp1 - pass1');
      assert1(false);
    end;

    {create hdr block -- all 'nil's for now}
    with hdr_rec do begin
      a_magic := NMAGIC;
      a_text := 16;
      a_data := 0;
      a_bss := 0;
      a_syms := 0;
      a_entry_mod := 0;
      a_trsize := 0;
      a_drsize := 0;
      a_mod := 16;
      a_link := 0;
      a_strings := 0;
    end;

    temp1 := fwrite(hdr_rec,size(exec),1,obuffer);
  end;

procedure report_illegal_export(var sptr : symptr);
  begin
    writeln('export error');
    listsym(sptr);
  end;

export procedure output_export_list;

!Output all the exported symbols according to the export record chain

  var
      e : exportptr;
  begin
  {  if not objsymtable then added by Tim 
      begin
        e := exportstart;
        while e <> nil do
          with e^ do begin
	    if not BACK_COMPATIBLE then
	      if exportsptr^.symbol_definition.gtype=immediate then
		reporterror(302*100+back_only);
	    output_sym(exportsptr);
	    e := nextexportrec;
          end;
      end;  }
  end;

export routine trans_name_ptr(nameptr : integer) value : integer;
  var i, tmp : integer;
  begin
    i := 1; tmp := 0;
    if {objsymtable} true then   { added by Tim }
      begin
        while (nameptr > (symnames[i].base_addr.address+1024)) do begin
          i := i + 1; tmp := tmp + 1024;
        end;
        value := tmp + (nameptr - symnames[i].base_addr.address);
      end
    else
      begin 
        while (nameptr > (g_symnames[i].base_addr.address+1024)) do begin
          i := i + 1; tmp := tmp + 1024;
        end;
        value := tmp + (nameptr - g_symnames[i].base_addr.address);
      end
  end;

export procedure output_import_list;

!Output to the object file all the imported symbols, also output all
!the relocation records.

  var
      i : importptr;
     { below is how it should be declared 
      i_rec : relocation_caca;
     }
      i_rec : tmp_reloc_rec; 
      temp,curr_loc,lnk_tbl_loc : integer;
      temptoken : token; loc_proc : symptr; h : boolean;
  begin
  (* this routine outputs the relocation records,
     the module symbol entry, and the imported symbols *)
    if importstart <> nil then
      begin
        temp := 16; (* offset from core image to first link record *)
	i := importstart;
	while i <> nil do
	  with i^, i^.importsptr^ do
	    begin
	      i_rec.r_address := temp;
	     {i_rec.r_symbolnum := trans_name_ptr(symbolname.address);}
	      i_rec.r_value := trans_name_ptr(symbolname.address);
	      i_rec.r_value := i_rec.r_value | %2000000; (* length *)
	      if symbol_definition.ext_proc[0] <> chr(3) then
	        i_rec.r_value := i_rec.r_value | %4000000; (* external *)
	     {i_rec.r_external := 1;}
	      if fwrite(i_rec,size(tmp_reloc_rec),1,obuffer)<> 1 then
                writeln('fwrite error in obuffer/7; errno= ',errno); 
	      temp := temp + 4; (* increment by link entry size *)
	      i := nextimportrec;
	    end;
      end {with, while};
      if symroot^.syment = nil then
	begin
	  with temptoken do begin
	    tokentype := symbol;
	    symlength := get_namelength(sourcename);
	    sym := sourcename;
	  end;
	  make_global := true; 
	  symroot^.syment := processsymbol(temptoken); 
	  make_global := false; 
	  with symroot^.syment^ do
	    if symdefined[0] = chr(ord(false)) then
	      begin
	        symbol_definition.gtype := mod_def;
	        linedefined := 0;
		symdefined[0] := chr(ord(true));
	      end
	    else reporterror(100*078+duplicate_def);
	end;
      output_sym(symroot^.syment);
      i := importstart;
      while i <> nil do
	with i^ do
	  begin
	    output_sym(importsptr);
	    i := nextimportrec;
	  end;
      curr_loc := ftell(obuffer);
      i := importstart; h := false;
      lnk_tbl_loc := link_posn;
      if ext_to_sbrelative then lnk_tbl_loc:=lnk_tbl_loc+(import_count-1)*4;
      while i <> nil do
	with i^, i^.importsptr^, i^.importsptr^.symbol_definition do
	  begin
	    if ext_proc[0] = chr(3) then
	      begin
		search(symtable,symbolname^,h,loc_proc);
    		{listsym(loc_proc);}
		if (loc_proc^.symbol_definition.gtype =regrelative)
		 and (loc_proc^.symbol_definition.rltype = pc) then
		  begin
    		    if fseek(obuffer,lnk_tbl_loc+2,m_abs) < 0 then
       		      writeln('fseek error in a16.out?2; errno= ',errno);
		    if fwrite(
		           loc_proc^.symbol_definition.regreloffset,2,1,obuffer)
			       <> 1 then 
                      writeln('fwrite error in obuffer/8; errno= ',errno);
		  end;
	      end;
	    i := nextimportrec;
	    if ext_to_sbrelative then lnk_tbl_loc := lnk_tbl_loc - 4
	    else lnk_tbl_loc := lnk_tbl_loc + 4;
	  end;
      if fseek(obuffer,curr_loc,m_abs) < 0 then
        writeln('fseek error in a16.out; errno= ',errno);
  end;

export procedure init2;

! Close intermediate files, and open ibuffer1 ibuffer3 for reading.
! If -l is on, then open lstbuff2 and lstbuff3.
! Seek the obuffer to pass the heading.
 
  var temp : integer;
      tfactor : factorrec;
  begin
    pc_byte_count := 0;
    pass2 := true; 
    if not vm_flag then begin
       fclose(ibuffer1);
       fclose(ibuffer3);
    end;
    fclose(lstbuff1);

    if not vm_flag then begin
      ibuffer1 := fopen_n(t1,'r');
      ibuffer3 := fopen_n(t3,'r');
      if ibuffer1 <= 0 then writeln('error in asmtemp1');
      if ibuffer3 <= 0 then writeln('error in asmtemp3');
    end;

    if big_list then 
      begin
	lstbuff2 := fopen_n (t6,'w'); lstbuff3 := fopen_n(t5,'w');
	if lstbuff2 <= 0 then writeln('error opening lsttemp2 - pass2');
        if lstbuff3 <= 0 then writeln('error opening lsttemp3');
      end;

    while modeindex > 1 do
      pop_current_mode;
    labelptr := nil;
    construct_temp_symbol(tfactor);
    last_loc := tfactor.fsymbol;
   {
    remember the file position of mod so it can be 
    overwritten by ``clean_up'':
   }
    mod_posn := ftell(obuffer);
   { seek past mod entry size to start code output }
    if fseek(obuffer,16,m_rel) < 0 then
       writeln('fseek error in asm.code; errno= ',errno);
  end;

export procedure closelistfiles;
  begin
    if big_list then 
      begin
        fclose(lstbuff3);
      end;
  end;

export procedure closeonelist;
  begin
    fclose(lstbuff1);
  end;

export procedure list_t4;
  begin
  end;

export procedure remove_t5;
  begin
  end;

export procedure remove_tmps;
  var list_status,tc:integer;
  begin
    if done_list then list_status:=1 else list_status:=0;
      if not vm_flag then fclose(ibuffer1);
      ibuffer1:=fopen_n(t1,'w');
      tc:=fwrite(list_status,4,1,ibuffer1);
      fclose(ibuffer1);
  end;

export routine sb_init;
  var tmp : integer;
  begin
    pc_byte_count := 0;
    currentmode := stbsrel;
    link_posn := ftell(obuffer);
    if (link_posn mod 2) <> 0 then
      begin
	hdr_rec.a_text := hdr_rec.a_text + 1;
	link_posn := link_posn + 1;
      end;
   { calculate size of link table }
   hdr_rec.a_link := import_count*4;
     tmp := link_posn + hdr_rec.a_link;
   { seek past specified size to start data code output }
    if fseek(obuffer,tmp,m_abs) < 0 then
       writeln('fseek error in a16.out; errno= ',errno);
  end;

export routine output_strings;

!Output all the strings associated with symbols to be put into the object
!file.

  var i : integer;
  begin
    i := 1;
    if {objsymtable} true then  { added by Tim }
      while symnames[i].last_used <> 0 do
	begin
	  if symnames[i+1].last_used <> 0 then
	    begin
	      if fwrite(symnames[i].base_addr^,1024,1,obuffer) <> 1 then
                writeln('fwrite error in obuffer/9; errno= ',errno);
              hdr_rec.a_strings := 1024 + hdr_rec.a_strings;
	    end
	  else
	    begin
	      if fwrite(symnames[i].base_addr^,symnames[i].last_used,1,obuffer)
		  <> 1 then
                writeln('fwrite error in obuffer/10; errno= ',errno);
              hdr_rec.a_strings := symnames[i].last_used + hdr_rec.a_strings;
	    end;
	  i := i + 1;
	end
    else 
      while g_symnames[i].last_used <> 0 do
	begin
	  if g_symnames[i+1].last_used <> 0 then
	    begin
	      if fwrite(g_symnames[i].base_addr^,1024,1,obuffer) <> 1 then
                writeln('fwrite error in obuffer/11; errno= ',errno);
              hdr_rec.a_strings := 1024 + hdr_rec.a_strings;
	    end
	  else
	    begin
	      if fwrite(g_symnames[i].base_addr^,g_symnames[i].last_used,
		1,obuffer) <> 1 then
                writeln('fwrite error in obuffer/12; errno= ',errno);
              hdr_rec.a_strings := g_symnames[i].last_used + hdr_rec.a_strings;
	    end;
	  i := i + 1;
	end;
  end;

export routine clean_up;

!Fix up the header record and mod entry record outputted to the
!object file.

  var tmp : integer;
      dat_addr:integer;
  begin
    if fseek(obuffer,0,m_abs) < 0 then
       writeln('fseek error in asm.code; errno= ',errno);
    (* update mod table & link table with data *)
    with mod_rec do begin
      m_prog   := 16;
      if ext_to_sbrelative then
	begin
	  m_link := link_posn - size(exec);
	  tmp := m_link mod 1024;
	  if tmp <> 0 then m_link := m_link + (1024 - tmp);
	  m_static:= m_link + import_count*4;
	  dat_addr:=m_link;
	end
      else
	begin
          m_static := last_loc^.symbol_definition.regreloffset + m_prog;
          tmp := m_static mod 1024;
          if tmp <> 0 then m_static := m_static + (1024 - tmp);
          m_link   := link_posn - size(exec);
	  dat_addr:=m_static;
	end;
      m_reser  := 0;
    end;
    with hdr_rec do begin
      a_magic := NMAGIC;
      a_text := a_text + last_loc^.symbol_definition.regreloffset;
      if not ext_to_sbrelative then a_text := a_text + a_link;
      a_data := last_alloc_sb;
      if ext_to_sbrelative then a_data:=a_data + a_link;
      a_bss := 0;
      a_syms := out_sym_count * 3 * 4;
      a_entry_mod := 0;
      a_trsize := import_count * 2 * 4;
      a_drsize := 0;
      a_text_addr:=0;
      a_mod_addr:=0;
      a_dat_addr:=dat_addr;
    end;
    if fwrite(hdr_rec,size(exec),1,obuffer) <> 1 then
       writeln('fwrite error in obuffer/13; errno= ',errno);
    if fseek(obuffer,mod_posn,m_abs) < 0 then
       writeln('fseek error in asm.code?9; errno= ',errno);
    if fwrite(mod_rec,size(mod_entry),1,obuffer) <> 1 then
       writeln('fwrite error in obuffer/14; errno= ',errno);
    fclose(obuffer);
  end;

begin {module init1}
  init_glbl_vars;
  scantoken;
  scan;
  scanexp;
  scanr; insts; dirs; dirsx; prsymtable; sdis; codegen; display;
  bio;
  openfiles;
  genrout;  {allocate space for intermediate records}
  buildoptables;
  addtosymroot(nil); 
  read_line;
  {resolve_loc_flag := false;}
end.
