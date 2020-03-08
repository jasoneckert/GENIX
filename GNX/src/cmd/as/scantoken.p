{!L}
module scantoken;

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

const SCCSId = Addr('@(#)scantoken.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'mnem_def.e'; ! DEPENDENCY
export 'scantoken.e'; ! DEPENDENCY
  
import 'scan.e'; ! DEPENDENCY
import 'prsymtable.e'; ! DEPENDENCY
import 'cmpc.e'; ! DEPENDENCY
import 'sdis.e'; ! DEPENDENCY
import 'disp.e'; ! DEPENDENCY
import 'float.e'; ! DEPENDENCY

  var

    spchar_set,firstletters,symletters : set of char; 
    nil_tmp : integer; nil_string : name;
    counts:integer;
    alpha_set:set of char;

  export procedure check_for_comma;

    begin
      if not((currtoken.tokentype=specchar) and (currtoken.spch=',')) then
        reporterror(100*165+invalidchar);
      end;

procedure constructsymdef( var temptoken : token);

  var temp : integer;
      tempchar:	char;

  begin
    with temptoken do
      begin
	tokentype := symbol;
        symlength := 0;
        if ((current_char >= 'a') and (current_char <= 'z')) or
           ((current_char >= 'A') and (current_char <= 'Z')) or
           (current_char = '_') or (current_char = '.') or (current_char = '%') 
	then
	  begin
	    sym := nil_string;
	    symlength := 1;
	    sym[1] := getschar;
	    while ((current_char >= 'a') and (current_char <= 'z')) or
                  ((current_char >= 'A') and (current_char <= 'Z')) or
                  ((current_char >= '0') and (current_char <= '9')) or
                  (current_char = '_') or (current_char = '.') or 
		  (current_char = '%') and (symlength < maxsymlength ) do
	    	begin
		  symlength := symlength + 1;
		  sym[symlength] := getschar;
		end;
	    if(symlength = maxsymlength) and (current_char in symletters) then
		while current_char in symletters do 
		  tempchar := getlchar;
	  end;
      end
   end;


export procedure scan_comma;

  var tchar : char;

  begin
    skipblanks;
    if current_char <> ',' then
      reporterror(100*167+invalidchar);
    tchar := getlchar;
  end;

procedure scanhex(var temptoken : token; count : integer);

  var hexchar	: char;
      newnum    : integer;
      counter   : integer;

  begin
    counter := 0;
    with temptoken.genref do
    begin
      if count <> 0 then
        begin
 	  numtype := floati;
	  imfloat.d_hi := 0;
	  imfloat.d_lo := 0;
	end
      else imint := 0;
      while ((current_char >= '0') and (current_char <= '9') or
	     (current_char >= 'a') and (current_char <= 'f') or
	     (current_char >= 'A') and (current_char <= 'F')) do
        begin
	  hexchar := getlchar;
	  if (hexchar >= '0') and (hexchar <= '9') then
	    newnum := ord(hexchar) - ord('0')
	  else if (hexchar >= 'A') and (hexchar <= 'Z') then
	    newnum := ord(hexchar) - (ord('A') - 10)
	  else 
	    newnum := ord(hexchar) - (ord('a') - 10);
	  if count = 16 then
	    begin
	      if counter < 8
	        then imfloat.d_hi := imfloat.d_hi*16 + newnum
	        else imfloat.d_lo := imfloat.d_lo*16 + newnum
	    end
	  else imint := imint*16 + newnum;
	  counter := counter + 1;
        end;
    end;
    if (count <> 0) and (counter <> count)
      then reporterror(226*100+float_syntax_error);
  end;

procedure scanbinary(var temptoken : token);
  var binchar	: char;
      newnum    : integer;
  begin
    with temptoken.genref do
    begin
      imint := 0;
      while (current_char = '0') or (current_char = '1') do
        begin
          binchar := getlchar;
          newnum := ord(binchar) - ord('0');
	  imint := imint*2 + newnum;
        end;
    end;
  end;

procedure scanoctal (var temptoken : token);

  var octchar : char;
      newnum  : integer;

  begin
    with temptoken.genref do
    begin
      imint := 0;
      while (current_char >= '0') and (current_char <= '7') do
        begin
          octchar := getlchar;
	  newnum := ord(octchar) - ord('0');
	  imint := imint*8 + newnum
        end
    end
  end;


procedure getnumber(var number_high,number_low:integer);

(* getnumber scans the unsigned integers upto 18 digits, and rejects if  *)
(* all but the 16 most significant digits if the integer has more than   *)
(* that many. it returns the integer as two numbers of 9 digits each.The *)
(* value of the integer is Number_high*100000000+number_low.             *)

  var newnum,m:integer;
      digit:char;
      nohi, nolo: integer;
  begin nohi:=0;nolo:=0;
    while (current_char >= '0')and(current_char <='9') do
      begin digit:=getlchar;
        newnum:=ord(digit)-ord('0');
        if nohi<100000000 then 
          begin
            m:= nolo div 100000000;
            nohi:=(nohi*10)+m;
            nolo:=(nolo mod 100000000)*10+newnum;
          end
      end;
     number_high:= nohi;
     number_low:= nolo;
  end;
     

export procedure scannumber(var temptoken : token);

  var num_hi,num_lo,i,pos1,pos2:integer;
      junk:char; tp_str: tp_string; err:boolean;

begin with temptoken.genref do
  begin pos1:=charpos;getnumber(num_hi,num_lo);
    if (current_char<>'.')and(current_char<>'e')and(current_char<>'E')  then
      begin numtype:=inti;
        if (num_hi>2)or((num_hi=2)and(num_lo>147483648)) then 
           reporterror(100*168+integer_too_large)
         else if (num_hi=2)and(num_lo=147483648) then imint:=minint
        else imint:=num_hi*1000000000+num_lo;
      end
    else
      begin numtype:=floati;
        junk:=getlchar;err:=false;
        if (current_char in['0'..'9']) then  getnumber(num_hi,num_lo);
        if (current_char='e')or(current_char='E') then
          begin junk:=getlchar;
            if (current_char='-')or(current_char='+')  then junk:=getlchar;
            if not(current_char in ['0'..'9']) then
              begin
                reporterror(100*169+exponent_missing);err:=true
              end
            else getnumber(num_hi,num_lo)
          end;
        if not err then
          begin getnumber(num_hi,num_lo);
            pos2:=charpos-1;
            if (pos2-pos1)>=max_float_length then 
                begin reporterror(100*199+float_too_long);err:=true end
          end;
          if not err then
            begin new(tp_str);
              for i:=pos1 to pos2 do 
                tp_str^[i-pos1+1]:=line[i];
              tp_str^[pos2-pos1+2]:=' ';
              asc_2_real(tp_str,true,imfloat,err);
              if err then reporterror(100*171+float_syntax_error)
            end
      end
  end
end;



procedure scanconstant(var temptoken : token);

  begin
    with temptoken do
    begin
      if (sym[1] = 'd') or (sym[1] = 'b') or (sym[1] = 'o') or 
	 (sym[1] = 'q') or (sym[1] = 'h') or (sym[1] = 'x') or
         (sym[1] = 'D') or (sym[1] = 'B') or (sym[1] = 'O') or 
	 (sym[1] = 'Q') or (sym[1] = 'H') or (sym[1] = 'X') or
	 (sym[1] = 'L') or (sym[1] = 'l')  then
	begin
          tokentype := gentoken;
	  with genref do
	    begin
              gtype := immediate;
              numtype := inti;
		(*sdi_gnum:=-1;*)
	    end;
          if (sym[1] = 'd')or(sym[1]='D') then
	    scannumber(temptoken)
          else if (sym[1] = 'b')or(sym[1] = 'B') then
	    scanbinary(temptoken)
          else if (sym[1]='o')or(sym[1]='q')or(sym[1]='O')or(sym[1]='Q') then
	    scanoctal(temptoken)
          else if (sym[1]='h')or(sym[1]='x')or(sym[1]='H')or(sym[1]='X') then
	    scanhex(temptoken,0)
	  else if (sym[1]='l')or(sym[1]='L') then
	    scanhex(temptoken,16);
	end
      else reporterror(100*172+invalidchar);
    end
  end;


  procedure rtokencheck( var temptoken : token);          

    var temprt       : packed array[1..maxrtlen] of char;
	tchr	     : char;
	temp,ctest   : integer;
	finished     : boolean;
        cmp_in       : integer;

    begin
      with temptoken do
      if (symlength <= maxrtlen) and (sym[1] <> '.') then
	begin
	  for temp := 1 to maxrtlen do begin
	    tchr := sym[temp];
	    if (tchr >= 'A') and (tchr <= 'Z') then
	      tchr := chr(ord(tchr) + (ord('a') - ord('A')));
	    temprt[temp] := tchr;
	  end;
	  temp := rtstart[ord(temprt[1])];
	  if temp <> -1 then
	    finished := false
	  else finished := true;
	  while not finished do
	    if (temp <= maxtokens) then
	      begin
		ctest := cmpc(maxrtlen,addr(temprt),
					addr(rtmnemonics[temp].rtchars));
	        if ctest > 0 then           {temprt > rtmnemonics[temp].rtchars}
	          temp := temp + 1
	        else
		  begin
		    finished := true;
	            if ctest = 0 then       {temprt = rtmnemonics[temp].rtchars}
	              with temptoken do
	              begin
	                tokentype := restoken;
	                rtokenindex := temp;
	              end;
		  end
	      end
	    else finished := true
	end 
    end;

function getnchar : char;

  var symch : char;

  begin
    if charpos > max_char_pos then
      symch := eol
    else
      begin
	symch := current_char;
	charpos := charpos + 1;
      end;
    if charpos > max_char_pos then
      current_char := eol
      else current_char := line[charpos];
    if not upperlower then
      if (symch >= 'A') and (symch <= 'Z') then 
	getnchar := chr(ord(symch)+32)
      else getnchar := symch
    else getnchar := symch;
  end;


export procedure nexttoken{(var temptoken : token)}; 

    var temprt       : packed array[1..maxrtlen] of char;
	ctest        : integer;
        temp         : integer;
        num_hi,num_lo,pos1,pos2:integer;
        tp_str       : tp_string; 
        err          : boolean;
        tchar        : char;
        delimiter    : char;
	count,shift  : integer;
	finished     : boolean;
        symch        : char;

  begin       {nexttoken}
    lastpos := charpos;
    skipblanks;
    with temptoken do
    begin
      (*constructsymdef(temptoken);*)
(*----------------------------------------------------------------------------*)
      if mod_name 
        then
	  begin
	    tokentype := symbol;
	    symlength := 0;
	    if (current_char <> eol) and (current_char <> ';')
	      then
		begin
		  sym := nil_string;
		  symlength := 1;
		  sym[1] := getschar;
		  while (current_char <> eol) and (current_char <> ' ')
		        and (current_char <> chr(9)) and (current_char <> ';') 
		        and (symlength < maxsymlength) do
		    begin
		      symlength := symlength + 1;
		      sym[symlength] := getnchar;
		    end;
		    if (symlength = maxsymlength) then
		      while (current_char <> eol) and (current_char <> ' ') and
			   (current_char <> ';') and (current_char <> chr(9)) do
			tchar := getlchar;
		end;
	  end
	  else
            begin
	      tokentype := symbol;
              symlength := 0;
              if ((current_char >= 'a') and (current_char <= 'z')) or
                 ((current_char >= 'A') and (current_char <= 'Z')) or
                 (current_char = '_') or (current_char = '.') or (current_char = '%') 
	      then
	        begin
	          sym := nil_string;
	          symlength := 1;
	          sym[1] := getschar;
	          while (current_char in symletters)
                        (*((current_char >= 'a') and (current_char <= 'z')) or
                        ((current_char >= 'A') and (current_char <= 'Z')) or
                        ((current_char >= '0') and (current_char <= '9')) or
                        (current_char = '_') or (current_char = '.') or 
		        (current_char = '%')*)
		       	        and (symlength < maxsymlength ) do
	    	      begin
		        symlength := symlength + 1;
		        sym[symlength] := getnchar;
		      end;
	          if(symlength = maxsymlength) and (current_char in symletters) then
		      while current_char in symletters do 
		        tchar := getlchar;
	        end;
            end;
(*----------------------------------------------------------------------------*)
      if(symlength=0) then
        if (current_char = '''') or (current_char = '"') then
	  (*scan_ascii(temptoken)*)
(*----------------------------------------------------------------------------*)
          with global_string^ do
          begin
            strglen := 0;
            finished := false;
            delimiter := getlchar;
            repeat
	      tchar := getlchar;
	      if (delimiter = tchar) and (current_char<>delimiter) then
	        finished := true  (* above checks for double delimiter *)
	      else
	        begin
	          strglen := strglen + 1;
	          strchars[strglen] := tchar;
	          if (delimiter=tchar) and (current_char=delimiter) then
	            tchar := getlchar;
	          if strglen = 80 then
	            begin
	              finished := true;
	              if current_char<>delimiter then
		        reporterror(100*166+string_too_long)
		      else
		        tchar := getlchar;
	            end
	        end;
            until finished;
            tokentype := gentoken;
	    with genref do
	      begin
	        gtype := stringg;
	        string_rec := global_string;
		(*sdi_gnum:=-1;*)
	      end;
	    new(global_string);
          end
(*----------------------------------------------------------------------------*)
        else if (current_char >= '0') and (current_char <= '9') then
	  with temptoken do
	  begin
	    tokentype := gentoken;
	    (*scannumber(temptoken) *)
(*---------------------------------------------------------------------------*)
	    with genref do
  	      begin 
    	        start_of_stack:=nil;(* insrtd by SUNIL *)
    	        gtype := immediate;
		(*sdi_gnum:=-1;*)
    	        pos1:=charpos;getnumber(num_hi,num_lo);
    	        if (current_char<>'.')and(current_char<>'e')and
		   (current_char<>'E')  then
      	          begin numtype:=inti;
        	    if (num_hi>2)or((num_hi=2)and(num_lo>147483648)) then
           	      reporterror(100*179+integer_too_large)
        	     else if(num_hi=2)and(num_lo=147483648)then 
				imint:=minint 
        	    else imint:=num_hi*1000000000+num_lo;
      	          end
    	        else
      	          begin numtype:=floati;
        	    tchar:=getlchar;err:=false;
        	    if(current_char >= '0') and (current_char<='9')
			then  getnumber(num_hi,num_lo);
        	    if (current_char='e')or(current_char='E') then
          	      begin tchar:=getlchar;
            	        if (current_char='-')or(current_char='+')  then 
			  tchar:=getlchar;
            	        if (current_char <'0')or(current_char>'9') then
              	          begin
                	    reporterror(100*197+exponent_missing);err:=true
              	          end
            	        else getnumber(num_hi,num_lo)
          	      end;
        	    if not err then
          	      begin getnumber(num_hi,num_lo);
            	        pos2:=charpos-1;
            	        if (pos2-pos1)>=max_float_length then 
                	  begin reporterror(100*170+float_too_long);err:=true 
			  end
          	      end;
          	    if not err then
            	      begin new(tp_str);
              	        for temp:=pos1 to pos2 do 
                	  tp_str^[temp-pos1+1]:=line[temp];
              	        tp_str^[pos2-pos1+2]:=' ';
              	        asc_2_real(tp_str,true,imfloat,err);
              	        if err then reporterror(100*198+float_syntax_error)
            	      end
      	          end
  	      end
(*---------------------------------------------------------------------------*)
	  end
        else
	  (*scanspchar(temptoken)*)
(*---------------------------------------------------------------------------*)
          begin
            tokentype := specchar;
            if current_char = eol then
	      begin
	        spch := eol;
	      end
            else
	      begin
                if current_char in spchar_set
	        then
	          spch := getlchar
                else
	          begin
	            reporterror(100*173+invalidchar);
	            spch := ';'
	          end;
	      end
          end
(*---------------------------------------------------------------------------*)
      else    
        if symlength = 1 then
	  begin
	    if current_char = '''' then
	      with temptoken do
	      begin
		tchar := getlchar;
	        scanconstant(temptoken);
	      end;
	  end
	else (*rtokencheck(temptoken);*)
(*----------------------------------------------------------------------------*)
	  begin
            if (symlength <= maxrtlen) and (sym[1] in alpha_set) then
	      begin
	        for temp := 1 to maxrtlen do begin
	          tchar := sym[temp];
	          if (tchar >= 'A') and (tchar <= 'Z') then
	            tchar := chr(ord(tchar) + (ord('a') - ord('A')));
	          temprt[temp] := tchar;
	        end;
	        temp := rtstart[ord(temprt[1])];
	        finished := (temp=-1);
	        while not finished do
	          if (temp <= maxtokens) then
	            begin
		      ctest := cmpc(maxrtlen,addr(temprt),
					      addr(rtmnemonics[temp].rtchars));
	              if ctest > 0 then    {temprt > rtmnemonics[temp].rtchars}
	                temp := temp + 1
	              else
		        begin
		          finished := true;
	                  if ctest = 0 then {temprt = rtmnemonics[temp].rtchars}
	                    with temptoken do
	                    begin
	                      tokentype := restoken;
	                      rtokenindex := temp;
	                    end;
		        end
	            end
	          else finished := true
	      end 
          end;
(*----------------------------------------------------------------------------*)
    end;
  end;

  begin 
    firstletters := ['A'..'Z','a'..'z','_','.','%'];
    symletters := firstletters + ['0'..'9'];
    new(global_string);
    counts:=0;
    for nil_tmp := 1 to 33 do
      nil_string[nil_tmp] := chr(0);
    spchar_set:= ['{','[',']',':','+','-','/','*',';',',','(',')','@','>'];
    alpha_set:=['A'..'Z','a'..'z']
  end.
