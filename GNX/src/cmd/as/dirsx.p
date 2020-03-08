{!L}
module dirsx;


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

const SCCSId = Addr('@(#)dirsx.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e';! DENPENDENCY
import 'glob_types.e';! DENPENDENCY
import 'glob_vars.e';! DENPENDENCY
import 'mnem_def.e';! DENPENDENCY
import 'scantoken.e';! DENPENDENCY
import 'scanr.e';! DENPENDENCY
import 'prsymtable.e';! DENPENDENCY
import 'dirs.e';! DENPENDENCY
import 'insts.e';! DENPENDENCY
import 'bio.e';! DENPENDENCY
import 'init1.e';! DENPENDENCY
import 'scan.e';! DENPENDENCY
import 'list_types.e';! DENPENDENCY
import 'sdis.e';! DENPENDENCY
import 'disp.e';! DENPENDENCY

export 'dirsx.e';! DENPENDENCY

  procedure get_string(var temptoken : token);
    begin
      nexttoken(temptoken);
      with temptoken do
	if tokentype = gentoken then
	  begin
	    with genref do
	      if gtype = stringg then
	        begin end
	      else reporterror(100*065+illegal_str_op)
	  end
	else reporterror(100*066+illegal_str_op);
    end;

  procedure output_listinterm(dirindex : integer);
    var
      g_index : irec;
      xx : integer;
      lrec : listrec;
      temp_mode : modetype;
    begin
	with lrec do
	  begin
	    linenum := linenumber;
	    listrectype := list_rec;
	    nextpos := charpos;
	    dirtype := ord(dirmnemonics[dirindex].dirlist[0])::listdirs;
	  end;
	xx := fwrite (lrec,size(listrec),1,lstbuff1);
    end;

  export procedure process_title{dirindex : integer}; 
    var temptoken : token;
    begin
      get_string(temptoken);
      if not error_flag then
	begin
	  output_listinterm(dirindex);
	end;
    end;

  export procedure process_subtitile{dirindex : integer}; 
    var temptoken : token;
    begin
      get_string(temptoken);
      if not error_flag then
	begin
	  output_listinterm(dirindex);
	end;
    end;

  export  procedure process_eject{dirindex : integer}; 
    begin
      output_listinterm(dirindex);
    end;

  export procedure process_nolist{dirindex : integer};
    begin
      output_listinterm(dirindex);do_list:=false
    end;

  export procedure process_list{dirindex : integer}; 
    begin
      simple_list := true;do_list:=true;done_list:=true;
      output_listinterm(dirindex);
    end;

  export procedure process_width{dirindex : integer};

    var tchar : char;
    begin
      output_listinterm(dirindex);
      while current_char <> eol do
	tchar := getlchar;
    end;

  export procedure addtosymroot(sptr : symptr);

    var cptr : symtreeptr;
	temptoken : token;

    begin
      newsymtree(cptr);
      with cptr^ do
      begin
	leftlink := symroot;
	rightlink := nil;
	sonlink := nil;
	syment := sptr;
	if symroot <> nil then
	  begin
	    papalink := symroot^.papalink;
	    symroot^.rightlink := cptr;
	  end
	else 
	  begin
	    syment := nil;
	    papalink := nil; symtable := nil;
	  end;
	startloc := locationcounter;
	endloc := 0;
      end;
      symroot := cptr;
    end;

  export procedure appendtoroot(sptr : symptr);

    var cptr : symtreeptr;

    begin
      if symroot^.sonlink <> nil then
	with symroot^ do 
	begin
	  symroot := sonlink;
	  while rightlink <> nil do
	    symroot := rightlink;
	  addtosymroot(sptr);
	end
      else
	begin
          newsymtree(cptr);
          with cptr^ do
          begin
	    leftlink := nil;
	    rightlink := nil;
	    syment := sptr;
	    papalink := symroot;
	    symroot^.sonlink := cptr;
	    sonlink := nil;
	    startloc := locationcounter;
	    endloc := 0
          end;
	  symroot := cptr;
	end;
      ! symroot := cptr;
    end;

  export procedure pop_root_level;

    begin
      with symroot^ do
	if papalink <> nil then
	  begin
	    endloc := locationcounter;
	    symroot := papalink
	  end
	else
	  reporterror(100*067+nesting)
    end;


  export procedure adjust_proc_labels(chain: symptr; offset:integer; neglen: integer);
    var temp: symptr;
      next: symptr;
      delta: integer;

    begin
      temp := chain;
      while temp <> nil do 
	  begin
	    next := temp^.nextsym;
	    with temp^.symbol_definition do
	      begin
	        if next <> nil then
		  delta := next^.symbol_definition.regreloffset - regreloffset
		else 
		  delta := neglen - regreloffset;
	        regreloffset := -neglen + delta + regreloffset + offset;
	      end;
	    temp := next;
          end;
    end;

  export procedure scan_storage_directives(var next_proc: procdirs; var chain: symptr);

    var dirindex : integer;
	finished : boolean;
	curr_chain: symptr;
        tyinpt   :type_info_ptr;

    begin
      finished := false;
      repeat
        labelptr := nil;
	global_type_info:=nil;
	this_symbol:=nil;
        error_flag := false;
        nexttoken(currtoken);
      if (current_char=' ')or(current_char=chr(9))  then skipblanks;
        if current_char = ':' then
        begin
          scanlabel(currtoken,nil);
	    if chain = nil then
	      begin
	        chain := labelptr;
		curr_chain := chain;
	      end
	    else
	      begin
		labelptr^.nextsym := nil;
		curr_chain^.nextsym := labelptr;
		curr_chain := labelptr;
	      end;
	  nexttoken(currtoken);
	end
      (*else if (current_char='brace') then
        begin 
          scan_type_info(tyinpt);
          if (current_char=' ')or(current_char=chr(9))  then skipblanks;
          if current_char = ':' then
            begin
              scanlabel(currtoken,tyinpt);
	    if chain = nil then
	      begin
	        chain := labelptr;
		curr_chain := chain;
	      end
	    else
	      begin
		labelptr^.nextsym := nil;
		curr_chain^.nextsym := labelptr;
		curr_chain := labelptr;
	      end;
	      nexttoken(currtoken);
    	    end
        end*);

        with currtoken do
          if tokentype = symbol then
            if sym[1] = '.' then
	      begin
		dirindex := getdirindex(currtoken);
		if dirindex <> -1 then
	          case dirmnemonics[dirindex].dirtype of
	            relocscope : reporterror(100*068+invaliddir);
	            proc       : case ord(dirmnemonics[dirindex].dirproc[0])::procdirs of
  			           PROCd	: reporterror(100*069+invaliddir);
			           RETURNS	: begin
						    next_proc := RETURNS;
						    finished := true;
						  end;
			           VARd		: begin
						    next_proc := VARd;
						    finished := true;
						  end;
			           BEGINd	: begin
						    next_proc := BEGINd;
						    finished := true;
						  end;
			           ENDPROC	: begin
						    next_proc := ENDPROC;
						    finished := true;
						  end;
			           MODULEd      : reporterror(100*070+invaliddir);
			         end;
	            storage    : processstordir(dirindex);
	            listing    : processlistdir(dirindex);
	          end
		else reporterror(100*071+invaliddir);
		freelabel:=nil;
	      end
            else
	      reporterror(100*072+illegalinst)
          else if not((tokentype=specchar)and((spch=eol) or (spch=';'))) then
	      reporterror(100*073+invalidopcode); 
        if not(finished and (next_proc = VARd)) then
	  scancomment;	
	if (assembly_mode = inverted) and (labelptr <> nil) then
	  labelptr^.symbol_definition.regreloffset := locationcounter;
	above_symbol:=this_symbol
      until finished or eof_sbuffer;
    end;

  export procedure scan_proc_insts(var next_proc : procdirs);

    var dirindex : integer;
        tyinpt   : type_info_ptr;

    begin
      next_proc := RETURNS;		! just something other than ENDPROC
      repeat
        labelptr := nil;
	global_type_info:=nil;
	this_symbol:=nil;
        error_flag := false;
        nexttoken(currtoken);
        if (current_char=' ')or(current_char=chr(9))  then skipblanks;
        if current_char = ':' then
          begin
            scanlabel(currtoken,nil);
	    nexttoken(currtoken);
	  end
        (*else if (current_char='brace') then
          begin 
            scan_type_info(tyinpt);
            if (current_char=' ')or(current_char=chr(9))  then skipblanks;
            if current_char = ':' then
              begin
                scanlabel(currtoken,tyinpt);
	        nexttoken(currtoken);
    	      end
          end*);
        with currtoken do
          if tokentype = symbol then
            if sym[1] = '.' then
	      begin
		dirindex := getdirindex(currtoken);
		if dirindex <> -1 then
	          case dirmnemonics[dirindex].dirtype of
	            relocscope : processrelocdir(dirindex);
	            proc       : case ord(dirmnemonics[dirindex].dirproc[0])::procdirs of
  			       	   PROCd,
			           RETURNS,
			           VARd,
			           BEGINd  : 
				     begin
					reporterror(100*182+nested_proc);
					(*processprocdir(dirindex,next_proc);
					! if its an ENDPROC then change it 
					! because it applies to the nested
					! level.
					if next_proc = ENDPROC then 
					  next_proc := RETURNS;*)
				     end;
			           ENDPROC : begin 
				       next_proc := ENDPROC;
				       end;
			           MODULEd : reporterror(100*074+invaliddir);
			         end;
	            storage    : processstordir(dirindex);
	            listing    : processlistdir(dirindex);
	          end
		else reporterror(100*075+invaliddir);
		freelabel:=nil;
	      end
            else
	      begin 
	        process_instruction(currtoken);
		freelabel:=nil
	      end
          else if not((tokentype=specchar)and((spch=eol) or (spch=';'))) then
	      reporterror(100*076+invalidopcode); 
        if next_proc <> ENDPROC then scancomment;	
	if (assembly_mode = inverted) and (labelptr <> nil) then
	  labelptr^.symbol_definition.regreloffset := locationcounter;
	above_symbol:=this_symbol;
      until (next_proc = ENDPROC) or eof_sbuffer;
    end;

  begin
  end.

