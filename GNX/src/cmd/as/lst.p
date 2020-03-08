program lst_asm16(input,output);

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

const SCCSId = Addr('@(#)lst.p	3.31         9/20/83');
const CpyrId = Addr('@(#)Copyright (C) 1983 by National Semiconductor Corp.');

export 'glob_const.e'; ! DEPENDENCY
export 'formatsmod.e'; ! DEPENDENCY
export 'glob_types.e'; ! DEPENDENCY
export 'glob_vars.e'; ! DEPENDENCY
{!L}
export 'a.out.e'; ! DEPENDENCY
export 'stab.e'; ! DEPENDENCY
export 'list_types.e'; ! DEPENDENCY

import 'bio.e'; ! DEPENDENCY
  
label 324;

const tab = chr(9);

type  symchars = packed array[1..32] of char;
      code_types = (dr_type, pc_type, sb_type);
      listrecptr = ^listrec;
      string32=symchars;
      unar=^array[1..1000]of^array[1..1000]of char;

var   t1,t2,t3 : symchars;
      lstbuff1, lstbuff2, lstbuff3, obuffer : cfile;
      curr_list_rec : listrecptr;
      dr_list_rec, pc_list_rec, sb_list_rec : listrecptr;
      sourcename : symchars;
      objectname : symchars;
      page_num, max_char_pos, obj_length, obj_location : integer;
      src_line, object_code, list_location : str_ptr;
      title_line, subtitle_line: str_ptr;
      width_specification : integer;
      s_linenum, line_num : integer;    (* current line number on page *)
      first_time, long_list, dot_list, tmp_bool, extended_line : boolean;
      last_code_type : code_types;
      pc_block, sb_block : ^packed array[0..511] of char;
      pc_index, sb_index : integer;
      pc_loc, sb_loc : integer;
      header_block : exec;
      eof_s_buffer:boolean;
      nothing_copied:boolean;
      argptr:integer;
      errfound:boolean;

import var ac_:integer;
import var av_:unar;

import routine fopen_n(var name: string32;mode:char):cfile;forward;
import routine pexit1;forward;
  
  routine clear_string(a_string : str_ptr);
    var tmp : integer;
    begin
      with a_string^ do
        begin
          for tmp := 1 to strglen do
            strchars[tmp] := blank;
          strglen := 0;
        end;
    end;

  routine read_src_line;

    begin
      if eof(sbuffer) then
        begin src_line^.strchars[1]:=' ';
          src_line^.strchars[2]:=chr(0);
          src_line^.strglen:=1;
          eof_s_buffer:=true
        end
      else
        begin
          src_line^.strglen := 80;
          readln(sbuffer, src_line^.strchars : src_line^.strglen);
          src_line^.strchars[src_line^.strglen+1] := chr(0);
        end;
        s_linenum := s_linenum + 1;
        if long_list then 
          begin
            clear_string(object_code);
            clear_string(list_location);
          end;
      end;

  routine read_lst(lst_file : cfile;
                   list_rec_ptr : listrecptr);
    var lsize : integer;

    begin
      lsize := fread(list_rec_ptr^,size(listrec),1,lst_file);
      if lsize = 0 then
        list_rec_ptr^.linenum := maxint;
    end;

routine read_lst_file;

  begin
    if long_list then 
      begin
        if last_code_type = pc_type then
          read_lst(lstbuff2,pc_list_rec)
        else if last_code_type = sb_type then
          read_lst(lstbuff3,sb_list_rec)
        else read_lst(lstbuff1,dr_list_rec);
        if (pc_list_rec^.linenum < sb_list_rec^.linenum) and
           (pc_list_rec^.linenum < dr_list_rec^.linenum) then 
          begin
            curr_list_rec := pc_list_rec;
            last_code_type := pc_type;
          end
        else if (sb_list_rec^.linenum < pc_list_rec^.linenum) and
           (sb_list_rec^.linenum < dr_list_rec^.linenum) then 
          begin
            curr_list_rec := sb_list_rec;
            last_code_type := sb_type;
          end
        else 
          begin
            curr_list_rec := dr_list_rec;
            last_code_type := dr_type;
          end
      end
    else 
           read_lst(lstbuff1,curr_list_rec);
  end;

  routine get_pc_blk;
    begin
      pc_index := pc_index+512;
      pc_loc := pc_loc + 512;
      if fseek(obuffer,pc_index,m_abs) < 0 then
        begin
          writeln('Error: Fseeking to pc segment ',errno);
          assert(false);
        end;
      if fread(pc_block^,512,1,obuffer) < 0 then
        writeln('Error reading pc segment');
    end;

  routine get_sb_blk;
    begin
      sb_index := sb_index+512;
      sb_loc := sb_loc + 512;
      if fseek(obuffer,sb_index,m_abs) < 0 then
        begin
          writeln('Error: Fseeking to sb segment ',errno);
          assert(false);
        end;
      if fread(sb_block^,512,1,obuffer) < 0 then
        writeln('Error reading sb segment');
    end;

  routine lst_file_start;
    
    begin
      lstbuff1 := fopen_n(t1,'r'); 
      if lstbuff1 <= 0 then 
        begin
          writeln('Cannot open file l1: ', t1);
          flush(output);
          exit lst_file_start;
        end;
      if long_list then 
        begin
          lstbuff2 := fopen_n(t2,'r');
          if lstbuff2 <= 0 then 
            begin
              writeln('Cannot open file l2: ', t2);
              lstbuff2:=-1;
              flush(output);
              exit lst_file_start;
            end;
          lstbuff3 := fopen_n(t3,'r');
          if lstbuff3 <= 0 then 
            begin
              writeln('Cannot open file l3: ', t3);
              lstbuff3:=-1;
              flush(output);
              exit lst_file_start;
            end;
          read_lst(lstbuff1,dr_list_rec);
          read_lst(lstbuff2,pc_list_rec);
          last_code_type := sb_type;
          obuffer := fopen_n(objectname,'r');
          if obuffer <= 0 then 
            begin
              writeln('Cannot open file of: ', objectname);
              obuffer:=-1;
              flush(output);
              exit lst_file_start;
            end;
          if fread(header_block,size(exec),1,obuffer) <> 1 then
            begin
              writeln('Error: reading object header block');
              assert(false);
            end;
          with header_block do 
            begin
              pc_index := a_mod + size(type header_block) -512;
              sb_index := a_text + size(type header_block) -512;
              pc_loc := -512; sb_loc := -512;
            end;
          new(pc_block); new(sb_block);
          get_pc_blk; get_sb_blk;
        end;
    end;

  routine convert_to_hex(bin_val : integer; o_string : str_ptr);
    var tmp : integer;
    begin
      with o_string^ do 
        begin
          tmp := (bin_val >> 4) & %F;
          if tmp > 9 then
            strchars[strglen+1] := chr(ord('A') + tmp - 10)
          else strchars[strglen+1] := chr(ord('0') + tmp);
          tmp := bin_val & %F;
          if tmp > 9 then
            strchars[strglen+2] := chr(ord('A') + tmp - 10)
          else strchars[strglen+2] := chr(ord('0') + tmp);
          strglen := strglen + 2;
        end;
    end;

  routine format_location;
    var i : integer;
    begin
      list_location^.strglen := 0;
      with curr_list_rec^ do 
        begin
          for i := 2 downto 0 do
            begin
              convert_to_hex(((location >> (i*8)) & %FF),list_location);
            end;
        end;
    end;

  routine get_pc_byte(location : integer; o_string : str_ptr);
    begin
      while (location >= (pc_loc + 512)) do get_pc_blk;
      convert_to_hex(ord(pc_block^[location-pc_loc]),o_string);
    end;

  routine get_sb_byte(location : integer; o_string : str_ptr);
    begin
      while (location >= (sb_loc + 512)) do get_sb_blk;
      convert_to_hex(ord(sb_block^[location-sb_loc]),o_string);
    end;

  routine insert_code;
    tmp : integer;
    begin
      clear_string(object_code); format_location;
      with curr_list_rec^ do 
        begin
          if (num_bytes*2) > 80 then num_bytes := 40;
          if curr_list_rec = pc_list_rec then
            begin
              for tmp := 0 to (num_bytes-1) do
                get_pc_byte(location+tmp,object_code);
            end
          else
            begin
              for tmp := 0 to (num_bytes-1) do
                get_sb_byte(location+tmp,object_code);
            end;
          object_code^.strglen := num_bytes*2;
        end;
    end;

  routine get_number(var the_num : integer);

    var tmp : integer;
    begin
      tmp := 1; the_num := 0; 
      with src_line^ do 
        begin
          while ((strchars[tmp] <= '0') or (strchars[tmp] > '9')) do
            begin
              tmp := tmp + 1;
            end;
          while ((strchars[tmp] >= '0') and (strchars[tmp] <= '9')) do
            begin
              the_num := (the_num *10) + ord(strchars[tmp])-ord('0');
              tmp := tmp + 1;
            end;
          if (the_num <= 80) or (the_num > 132) then
          the_num := 132;
        end;
    end;

  routine get_string(a_string : str_ptr);

    var tmp,tmp2 : integer;
        delimiter,tchar : char;
        finished : boolean;

    begin
      tmp := 1; tmp2 := 0; finished := false;
      with src_line^ do 
        begin
          while not((strchars[tmp] = '''') or (strchars[tmp] = '"')) do
            tmp := tmp + 1;
          delimiter := strchars[tmp]; 
          repeat
            tmp := tmp + 1;
            tchar := strchars[tmp];
            if (delimiter = tchar) and (strchars[tmp+1]<>delimiter) then
              finished := true  (* above checks for double delimiter *)
            else
              begin
                tmp2 := tmp2 + 1;
                a_string^.strchars[tmp2] := tchar;
                if (delimiter=tchar) then
                  begin
                    tmp := tmp + 1;
                  end;
                if tmp2 = 80 then
                  begin
                    finished := true;
                  end
              end;
          until finished;
        end;
      a_string^.strglen := tmp2;
    end;

routine displaywarn(error: integer);
 
  begin
    write('address exceeding memory range');
  end;
 
routine displayerror(error: integer);

  begin
    errfound:=true; write((error div 100):3,':');
    if (error mod 100)>max_error_code then write('**** error number ',error)
    else
      case (error mod 100) of
        1: write('duplicate definition');
        2: write('invalid label');
        3: write('illegal instruction');
        4: write('symbol too long');
        5: write('integer expected');
        6: write('invalid character');
        7: write('assembler bug: duplicate symbol');
        8: write('multiply defined');
        9: write('invalid directive');
        10: write('illegal term');
        11: write('illegal expression');
        12: write('term size extreme');
        13: write('unequal lengths');
        14: write('nesting too deep');
        15: write('no more memory');
        16: write('attempted overwrite');
        17: write('illegal mode');
        18: write('invalid op code');
        19: write('assembler bug: sdi resolve op expression');
        20: write('assembler bug: sdi new undefined symbol');
        21: write('assembler bug: sdi resolve loc expression');
        22: write('undefined symbol');
        23: write('missing string parameter or invalid string constant');
        24: write('offset too large');
        25: write('missing .endproc');
        26: write('missing .begin');
        27: write('missing .var');
        28: write('label required');
        29: write('integer too large or too small');
        30: write('returned values longer than parameters');
        31: write('no more symbols available');
        32: write('float constant too long');
        33: write('exponent missing in float constant');
        34: write('syntax error in real number or number too large');
        35: write('float number too large');
        36: write('real number expected');
        37: write('only even numbered registers allowed here');
        38: write('float expressions not implemented');
        39: write('unresolved long-floats not implemented');
        40: write('float not allowed');
        41: write('mode not progrel');
        42: write('invalid external');
        43: write('string longer than 80 characters or without right quote');
        44: write('type info syntax error');
        45: write('nesting of procs not allowed');
        46: write('float too big for short representation');
        47: write('.source allowed only in progrel mode');
        48: write('no filename specified with .source');
        49: write('error in align expression');
        50: write('attempt to jump out of current mode');
        51: write('repeat count must not be span dependent');
        52: write('only positive numbers allowed in .field');
        53: write('bit field length missing ');
        54: write('dangling stab directive');
        55: write('type info needed for this directive');
	56: write('error while expanding operands');
	57: begin writeln('size of third operand is too big.');
		 writeln('perhaps you are using old encoding.');
		 write('use -E option and check the binary of this instruction')	
	    end;
        58: write('in backward compatible mode only. do not use -N switch');
        59: write('invalid packed decimal number');
        60: write('integer converted from string exceeding 4 byte range');
      end;
  end;

  routine process_page_eject;

    var temp,i : integer;

    begin
      if dot_list then
        begin
          write(chr(ord(12))); writeln; line_num := 3;
          write(blank:5);
          i:=1;
          while (asm_title[i]<>'#') do
            begin write(asm_title[i]);
              i:=i+1;
            end;
          while i<65 do
            begin write(' ');
              i:=i+1
            end;
          writeln('PAGE:',page_num:2);
          if title_line <> nil then
            begin
              writeln(blank:8,title_line^.strchars:title_line^.strglen);
              line_num := line_num + 1;
            end;
          if subtitle_line <> nil then
            begin
              writeln(blank:8,subtitle_line^.strchars:subtitle_line^.strglen);
              line_num := line_num + 1;
            end;
          writeln;
          line_num := 5; page_num := page_num + 1;
        end;
    end;

  routine list_line;
  
    var temp,temp2, cwidth, swidth,nwidth : integer;
  
    begin
      if dot_list then
        begin
          if line_num >= lines_per_page then
            process_page_eject;
          if not extended_line then
            begin
              write(blank,s_linenum:6); 
            end
          else write(' ':7);
          if long_list then 
            begin
              write(blank:2,list_location^.strchars:6,blank:2,
                    object_code^.strchars:8,blank);
              cwidth := 25;
            end
          else 
            begin
              write(blank:2); cwidth := 9;
            end;
          swidth := cwidth;
          if not extended_line then
            begin
            for temp := 1 to src_line^.strglen do 
              begin
                if cwidth > width_specification then 
                  begin
                    writeln; 
                    if ((src_line^.strglen-temp)-width_specification) < 0 then
                      write('&',
                        blank:((swidth-1)+(width_specification-
                                           (src_line^.strglen-temp))))
                    else write('&',blank:swidth-1); 
                    cwidth := swidth+1;
                  end; (* if begin *)
                if src_line^.strchars[temp] = tab then
                  begin
                    nwidth :=  8 -((cwidth-1) mod 8);
                    cwidth := cwidth + nwidth;
                    write(blank:nwidth);
                  end
                else 
                  begin
                    cwidth := cwidth + 1;
                    write(src_line^.strchars[temp]);
                  end;
              end;
            end;
          writeln; line_num := line_num + 1;
          if (object_code^.strglen > 8)  and long_list then 
            begin
              clear_string(src_line); temp := 9; 
              write(blank:17);
              while (temp <= object_code^.strglen) do 
                begin 
                  if (temp+7) <= object_code^.strglen then
                    begin
                      for temp2 := temp to (temp + 7) do
                        write(object_code^.strchars[temp2]); 
                      temp := temp + 8;
                      writeln; line_num := line_num+1;
                      if line_num >= lines_per_page then
                        process_page_eject;
                      if temp < object_code^.strglen then
                        write(blank:17);
                    end
                  else
                    begin
                      write(object_code^.strchars[temp]); 
                      temp := temp + 1; 
                    end
                end; (* while begin *)
              if (object_code^.strglen mod 8) <> 0 then
                begin
                  writeln; line_num := line_num + 1;
                end;
            end (* if begin *)
        end;
    end;

  routine process_list_dir;

    begin
      case curr_list_rec^.dirtype of
        TITLE    : begin
                     if title_line = nil then
                       new(title_line);
                     get_string(title_line);
                   end;
        SUBTITLE : begin
                     if subtitle_line = nil then
                       new(subtitle_line);
                     get_string(subtitle_line);
                   end;
        EJECT    : begin
                     list_line; process_page_eject;
                     read_src_line;
                   end;
        NOLIST   : dot_list := false;
        LIST     : dot_list := true;
        WIDTH    : begin
                     get_number(width_specification);
                   end;
      end;
    end;

  routine list_error;
    const
      marker = '^';
    begin
     if (curr_list_rec^.listrectype = err_rec) then begin
      write(blank : 8);
      if curr_list_rec^.errpos > 70 then
        curr_list_rec^.errpos := 40;
      write(marker : curr_list_rec^.errpos); 
      if curr_list_rec^.errnum >= 1000*100 then begin {ming}
        write('warning:');
        displaywarn(curr_list_rec^.errnum);
      end else begin
        write('ERROR:');
        displayerror(curr_list_rec^.errnum);
      end;
      writeln; line_num := line_num + 1;
     end;
    end;

procedure read_file_name(var nm:symchars);
  var
    i:integer;
  begin 
    if argptr>ac_ then
      begin writeln('more arguments needed by lst');
        exit read_file_name
      end
    else
      begin
        i:=0;
        repeat
          i:=i+1;
          nm[i]:=av_^[argptr]^[i]
        until (i>32)or(nm[i]=chr(0));
        argptr:=argptr+1
      end
  end;


function file_exists(f:string32):boolean;
  begin file_exists:=fopen_n(f,'r')>0 end;

begin
  (* initialization *)
  width_specification := 132;
  argptr:=2;
  errfound:=false;
  title_line := nil;
  subtitle_line := nil;
  new(src_line);
  src_line^.strglen := 80;
  clear_string(src_line);
  dot_list := false;
  if (av_^[2]^[1]='-')and(av_^[2]^[2]='l') then
    begin dot_list:=true;
      argptr:=3
    end;
  line_num := 61;
  page_num := 1;
  extended_line := false;
  new(dr_list_rec);
  pc_block := nil;
  sb_block := nil;
  eof_s_buffer:=false;
  nothing_copied:=true;

  (* get source file name, list file name, and object file name *)
  read_file_name(sourcename);
  read_file_name(objectname);
  read_file_name(t1);
  if file_exists(t1) then begin
    t3 := t1; 
    long_list := true;
    new(list_location);
    new(object_code);
    list_location^.strglen := 80;
    clear_string(list_location);
    object_code^.strglen := 80;
    clear_string(object_code);
    new(pc_list_rec);
    read_file_name(t2);
    new(sb_list_rec);
    read_file_name(t1);
  end 
  else 
  begin
    read_file_name(t1);
    read_file_name(t1);
    long_list := false;
    max_char_pos := 32; 
    curr_list_rec := dr_list_rec;
  end;
  reset(sbuffer,sourcename);
  lst_file_start;
  if lstbuff1 < 0 then goto 324; (* go to very end *)
  read_lst_file;
  s_linenum := 0;
  read_src_line;
{  if curr_list_rec^.linenum <> maxint then  }{ take this out so listing will }
    while not eof_s_buffer do 		      { be made, even if no statements }
      begin
        if curr_list_rec^.linenum > s_linenum then 
          begin
            list_line; 
            read_src_line; 
          end 
        else 
          begin with curr_list_rec^ do
            if listrectype = code_rec then 
              begin
                insert_code; 
                list_line; 
                read_lst_file;
                while(curr_list_rec^.linenum = s_linenum) do 
                  begin
                    insert_code;  
                    extended_line := true; 
                    list_line; 
                    read_lst_file;
                  end; 
                extended_line := false; 
                read_src_line; 
              end 
            else if listrectype = list_rec then 
              begin
                process_list_dir; 
                read_lst_file;
              end 
            else if listrectype = err_rec then 
              begin
                tmp_bool := dot_list; 
                dot_list := true;
                list_line; 
                dot_list := tmp_bool;
                while(curr_list_rec^.linenum = s_linenum) do 
                  begin
                    list_error; 
                    read_lst_file;
                  end; 
                read_src_line;
              end 
          end; 
      end; 
  if curr_list_rec^.linenum <> maxint then
    while curr_list_rec^.linenum <> maxint do 
      begin
        with curr_list_rec^ do
          if listrectype = code_rec then 
            begin
              if s_linenum <> curr_list_rec^.linenum then
                writeln('Error in lst file: extra code_rec');
              insert_code; 
              list_line; 
              read_lst_file;
              read_lst_file; 
            end 
          else if listrectype = list_rec then 
            begin
              if s_linenum <> curr_list_rec^.linenum then
                writeln('Error in lst file: extra list_rec'); read_lst_file; 
            end 
          else if listrectype = err_rec then 
            begin
              list_error; 
              read_lst_file;
            end 
      end;
324:
  if errfound then pexit1;
end.
