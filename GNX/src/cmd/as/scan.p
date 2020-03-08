{!L}
{!B}

module scan;

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

const SCCSId = Addr('@(#)scan.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'list_types.e'; ! DEPENDENCY
import 'scantoken.e'; ! DEPENDENCY
import 'bio.e'; ! DEPENDENCY
import 'init1.e'; ! DEPENDENCY

export 'scan.e'; ! DEPENDENCY
  
  const
    tab = chr(9);

  var 
    cursorpos : integer;


  export procedure reporterror {(errornum : integer)};
  
    var errrec : listrec;
        tempchar : char;
	xx,i : integer;
  

    begin
      if errornum >= (1000*100) then done_list := true else begin
          error_flag := true;
          source_errors := true;
      end;
      with errrec do
        begin
	  listrectype := err_rec;
          linenum := linenumber;
          errnum := errornum;
          errpos := 0;
          for i:=1 to charpos do 
		if line[i]=tab then errpos:=errpos+8-((errpos-1)mod 8)
		else errpos:=errpos+1;
          if tree_display then 
            writeln('error ',errnum:4,' reported from column ',errpos:2,
                    ' of line ',linenumber:4);
	  flush(output);
        end;
      xx := fwrite ( errrec,size(listrec),1,lstbuff1);
      if not pass2 then
	while (current_char <> eol) and (not eof_sbuffer) do
    	  begin
            tempchar := getlchar;
	  end;
    end;
  
  
export procedure insert_align_expr(n1s,n1e,n2s,n2e:integer);
  var 
    s1:packed array[1..7]of char;
    s2:packed array[1..3]of char;
    s3:packed array[1..15]of char;
    s4:packed array[1..9]of char;
    s5:packed array[1..2]of char;
    e1,e2:input_text_line; i,j:integer;
  begin
    !write(n1s,n1e,n2s,n2e); {XXXXX}
    !write('  "');for i:=n1s to n1e do write(line[i]); {XXXXX}
    !write('" "');for i:=n2s to n2e do write(line[i]);writeln('"'); {XXXXX}
    s1:='*+(((((';
    s2:=')+(';
    s3:='))(pc)-(* mod (';
    s4:='))) mod (';
    s5:='))';
    if (n1e-n1s+n2e-n2s+35)>size(input_text_line) then 
	reporterror(100*200+align_error)
    else
      begin j:=1;
        for i:=n1s to n1e do begin e1[j]:=line[i];j:=j+1 end;
        j:=1;
        if n2s>n2e then
	  begin e2[1]:='0';
	    n2s:=1;
	    n2e:=1 
	  end
	else
          for i:=n2s to n2e do begin e2[j]:=line[i];j:=j+1 end;
        for i:=1 to 7 do line[i]:=s1[i];
	j:=7;
	for i:=1 to (n1e-n1s+1) do begin line[j]:=e1[i];j:=j+1 end;
	for i:=1 to 3 do begin line[j]:=s2[i];j:=j+1 end;
	for i:=1 to (n2e-n2s+1) do begin line[j]:=e2[i];j:=j+1 end;
	for i:=1 to 15 do begin line[j]:=s3[i];j:=j+1 end;
	for i:=1 to (n1e-n1s+1) do begin line[j]:=e1[i];j:=j+1 end;
	for i:=1 to 9 do begin line[j]:=s4[i];j:=j+1 end;
	for i:=1 to (n1e-n1s+1) do begin line[j]:=e1[i];j:=j+1 end;
	for i:=1 to 2 do begin line[j]:=s5[i];j:=j+1 end;
	charpos:=1;line[j]:=' ';max_char_pos:=j;
	current_char:='*';
        !write('align: ');for i:=1 to j do write(line[i]);writeln; {XXXXX}
	flush(output);
      end
  end;

        
  
    export procedure read_line;
       var dn:boolean;

  begin 
    dn:=false;
    repeat
      if eof(sbuffer) then
        begin charpos:=1;cursorpos:=1;
          line[1]:=' ';max_char_pos:=1;
          current_char:=' ';eof_sbuffer:=true;
        end 
      else
        begin
	  max_char_pos := 128;
	  readln(sbuffer, line : max_char_pos);
	  charpos := 1; cursorpos := 1;
	  if max_char_pos > 0 then
	    current_char := line[1]
	  else current_char := eol;
        end;
      while((line[charpos]=' ')or(line[charpos]=chr(9))or(line[charpos]=chr(0)))
           and (charpos<=max_char_pos) do 
        charpos:=charpos+1;
      if charpos>max_char_pos then current_char:=eol 
      else current_char:=line[charpos];
      if eof_sbuffer then dn:=true
      else if (charpos<=max_char_pos) then
        if (line[charpos]<>';') then dn:=true;
      if not dn then linenumber:=linenumber+1
    until dn
  end;

    export procedure  processeoln;

      begin
        linenumber := linenumber + 1;
        if not eof_sbuffer then
	  read_line;
      end;

    export function getlchar {lchar : char};

      begin
	if charpos > max_char_pos then
	  lchar := eol
	else
	  begin
	    lchar := current_char;
	    charpos := charpos + 1;
	  end;
	if charpos > max_char_pos then
	  current_char := eol
	else current_char := line[charpos];
      end;
	  
  
  export function getschar {schar : char};
  
    begin
      (*schar := getlchar;*)
(*----------------------------------------------------------------------------*)
	if charpos > max_char_pos then
	  schar := eol
	else
	  begin
	    schar := current_char;
	    charpos := charpos + 1;
	  end;
	if charpos > max_char_pos then
	  current_char := eol
	else current_char := line[charpos];
(*----------------------------------------------------------------------------*)
      if not upperlower then
	if (schar >='A') and (schar <= 'Z')  then
          schar := chr(ord(schar)+32);
    end;
  
  export procedure skipblanks;
  
    var tchar : char;
  
    begin
      while (current_char<>eol) and
       ((current_char=' ')or(current_char=chr(9))or(current_char=chr(0))) do
        begin if charpos<=max_char_pos then charpos:=charpos+1;
          if charpos>max_char_pos then current_char:=eol 
  	  else current_char:=line[charpos]
	end
    end;

  begin 
  end.
