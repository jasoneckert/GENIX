{!L}
{!B}
module genbytes;
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

const SCCSId = Addr('@(#)genbytes.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.


import 'glob_const.e'; !DEPENDENCY
import 'formatsmod.e'; !DEPENDENCY
import 'glob_types.e'; !DEPENDENCY
import 'glob_vars.e'; !DEPENDENCY
import 'scanexp.e'; !DEPENDENCY
import 'scan.e'; !DEPENDENCY
import 'sdis.e'; !DEPENDENCY
import 'genrout.e'; !DEPENDENCY
import 'bio.e'; !DEPENDENCY
import 'scantoken.e'; !DEPENDENCY
import 'init1.e'; !DEPENDENCY
import 'list_types.e'; !DEPENDENCY
import 'dirs.e'; !DEPENDENCY
import 'stab.e'; !DEPENDENCY
import 'cmpc.e'; !DEPENDENCY
import 'float.e'; !DEPENDENCY
import 'disp.e'; !DEPENDENCY


import 'codegen.e'; !DEPENDENCY

    type tmptypes = (tmpchar,tmpint);
	 sym_ent_type = record
	   n_nm : integer;
	   n_ty : integer;
	   n_v  : integer;
	  end;


  export procedure flush_bytes;
    begin 
      if byte_count>0 then
	if fwrite(byte_buffer,byte_count,1,obuffer)<>1
	  then writeln('fwrite error in obuffer/15;errno=',errno);
      byte_count:=0;
    end;

  export procedure gen_byte(byte_value : integer);

    var temp : integer;

    begin
      pc_byte_count := pc_byte_count + 1;
      if byte_count=512 then flush_bytes;
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr(byte_value);
    end;

  export procedure gen_word(value : integer);
      
    var temp : integer;

    begin
      pc_byte_count := pc_byte_count + 2;
      if byte_count>510 then flush_bytes;
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr(value & %ff);
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr((value & %ff00)>>8);
    end;

  export procedure gen_doub(value : integer);
      
    var temp : integer;

    begin
      pc_byte_count := pc_byte_count + 4;
      if byte_count>508 then flush_bytes;
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr(value & %ff);
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr((value & %ff00)>>8);
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr((value & %ff0000)>>16);
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr((value & %ff000000)>>24);
    end;

export procedure gen_sym_ent(sym:sym_ent_type);
  begin 
    gen_doub(sym.n_nm);
    gen_doub(sym.n_ty);
    gen_doub(sym.n_v)
  end;

  export procedure gen_word_const(value : integer);

    begin
      pc_byte_count := pc_byte_count + 2;
      if byte_count>510 then flush_bytes;
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr((value & %ff00)>>8);
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr(value & %ff);
    end;

  export procedure gen_doub_const(value : integer);

    begin
      pc_byte_count := pc_byte_count + 4;
      if byte_count>508 then flush_bytes;
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr((value & %ff000000)>>24);
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr((value & %ff0000)>>16);
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr((value & %ff00)>>8);
      byte_count:=byte_count+1;
      byte_buffer[byte_count]:=chr(value & %ff);
    end;


  begin
  end.

