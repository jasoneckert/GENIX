module  display;

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

const SCCSId = Addr('@(#)disp.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'mnem_def.e'; ! DEPENDENCY
import 'scantoken.e'; ! DEPENDENCY
import 'bio.e'; ! DEPENDENCY
import 'sdis.e'; ! DEPENDENCY
import 'genrout.e'; ! DEPENDENCY


   
export 'disp.e'; ! DEPENDENCY

const listsize = 1000;

type time_rec=record
		u_time,s_time:integer
	      end;

var i,uniquenumber :integer;
    symentrylist:array[1..listsize]of symptr;symentrycount:integer;
    symtreelist :array[1..listsize]of symtreeptr;symtreecount:integer;
    sdi_ptr_list:array[1..listsize]of sdi_ptr;sdi_ptr_count:integer;
    l_ptr_list  :array[1..listsize]of l_ptr; l_ptr_count:integer;
    loc_ptr_list:array[1..listsize]of loc_ptr;loc_ptr_count:integer;
    irec_list:   array[0..listsize]of irec;irec_count:integer;

export function uniquei:integer;
  begin uniquei:=uniquenumber;uniquenumber:=uniquenumber+1;
  end;

export procedure newsymentry(var s:symptr);
  begin new(s);s^.unique1:=uniquei; 
    s^.equ_sdi[0]:=chr(ord(false));
    s^.first_stab:=nil;
    s^.to_be_externed[0]:=chr(ord(false));
    s^.type_of_symbol:=nil;
    s^.leftptr:=nil;
    s^.rightptr:=nil;
    s^.nextsym:=nil;
    s^.symbolname:=nil;
    s^.references:=nil;
    s^.scope.domain:=0;
    s^.proc_name[0]:=chr(ord(false));
    if tree_display then symentrycount:=symentrycount mod listsize+1;
    symentrylist[symentrycount]:=s
  end;


export procedure newsymtree(var s:symtreeptr);
  begin new(s);s^.unique2:=uniquei;
    if tree_display then symtreecount:=symtreecount mod listsize +1;
    symtreelist[symtreecount]:=s
  end;

export procedure new_l_ptr(var l:l_ptr);
  begin new(l);l^.unique:=uniquei;
    if tree_display then l_ptr_count:=l_ptr_count mod listsize +1;
    l_ptr_list[l_ptr_count]:=l
  end;

export procedure new_sdi_ptr(var s:sdi_ptr);
  begin new(s);s^.unique:=uniquei;
    if tree_display then sdi_ptr_count:=sdi_ptr_count mod listsize +1;
    sdi_ptr_list[sdi_ptr_count]:=s
  end;

export procedure new_loc_ptr(var lc:loc_ptr);
  begin new(lc);lc^.unique:=uniquei;
    if tree_display then loc_ptr_count:=loc_ptr_count mod listsize +1;
    loc_ptr_list[loc_ptr_count]:=lc
  end;

export procedure disp_girec;
begin
  g_intermrec:= first_girec;
  while g_intermrec <> nil do begin
      write('girec: ');
      writeln(g_intermrec^.interm_rec.linenumber);
      g_intermrec := g_intermrec^.interm_next;
  end;
end;
export procedure disp_genirec;
begin
  g_intermrec:= first_girec;
  while gen_intermrec <> nil do begin
      write('genirec: ');
      writegen(gen_intermrec^.gen_rec);
      gen_intermrec := gen_intermrec^.gen_next;
  end;
end;
export procedure new_irec(var ir:irec);
  begin new(ir);ir^.unique:=uniquei;
    if tree_display then irec_count:=irec_count mod listsize +1;
    irec_list[irec_count]:=ir
  end;
procedure write_l_ptr(l:l_ptr);forward;
procedure write_sdi_ptr(s:sdi_ptr);forward;
procedure write_loc_ptr(l:loc_ptr);forward;
procedure write_irec(ir:irec);forward;
procedure writeattrib(a:attribute);forward;
procedure writestack(s:stackptr);forward;
procedure writeop(e:optokenlist);forward;
procedure writeexpr(e:expression);forward;
procedure writetrm(t:trm);forward;
procedure writefact(f:factorrec);forward;
procedure writesym(s:symptr);forward;
procedure writegen(g: gen); forward;	

export procedure writegen( g:gen);
  procedure write_attrib(a:attribute);
    begin {if ord(a.attribstate[0])=ord(true) then
      case ord(a.attrib[0]) of
        0:write(':b');1:write(':w');3:write(':d')
      end; added by Tim }
!!
!!{ming 3/31}
    if ord(a.attribstate[0])=ord(true) then
     begin write(':');
      case ord(a.attrib[0]) of
        0:write(':b');1:write(':w');3:write(':d')
      end
     end else if tree_display then
      begin write('1');
         case ord(a.attrib[0]) of
           0:write(':b');1:write(':w');3:write(':d')
         end
      end;
!!
    end;
  begin 
    case ord(g.gtype) of 
       0: case g.rtype of 
             gregs : begin write('r');write(ord(g.reg[0]):1) end;
             fregs : begin write('f');write(ord(g.freg[0]):1) end
          end;
       1: begin 
             write(g.regreloffset:1);write_attrib(g.regreloffatt);write('(');
             if g.rltype=rn then write('r',ord(g.rnum[0]):1)
             else
                case ord(g.rltype) of
                  0:write('sp');  1:write('sb');  2:write('intbase');
                  3:write('psr'); 4:write('fp');  5:write('us');
                  6:write('upsr');7:write('pc');  8:write('tos');
                 10:write('rpc');11:write('modreg')
                end;
             write(')')
          end;
       2: begin 
            write(g.memoffset2:1);write_attrib(g.memoffatt2);
            write('(',g.memoffset1:1);write_attrib(g.memoffatt1);write('(');
             if ord(g.memreg[0])=9 then write('rx')
             else
                case ord(g.memreg[0]) of
                  0:write('sp');  1:write('sb');  2:write('intbase');
                  3:write('psr'); 4:write('fp');  5:write('us');
                  6:write('upsr');7:write('pc');  8:write('tos');
                 10:write('rpc');11:write('modreg')
                end;
             write('))')
          end;
       3: write('tos');
       4: begin 
            write('ext(',g.extindex:1);write_attrib(g.extinxatt);
            write(')+',g.extoffset:1);write_attrib(g.extoffatt);
          end;
       5: begin 
            write('@',g.absnum:1);write_attrib(g.absatt)
          end;
       6: begin 
            if g.numtype=inti then 
              begin write(g.imint:0);write_attrib(g.immatt) end
            else 
              begin write('float(');write_hex(g.imfloat.d_hi);write(',');
                write_hex(g.imfloat.d_lo);write(')')
              end;
          end;
       7: begin write('unresolved');writeattrib(g.unratt)end;
       8: begin write('string') end;
       9: write ('mod_def')
    end; (* case *)
    if g.gen_scale.scalestate then
      begin write('[r',ord(g.gen_scale.sreg[0]):1);
        case ord(g.gen_scale.satt[0]) of
          0:write(':b');1:write(':w');2:write(':d')
        end; 
        write(']')
      end;
   end;

export procedure displine(n,m:integer);
  begin if line_trace then writeln(n:3,'  -----  ',m:5) end;

procedure skip(c:char;n:integer;var m:integer);
  var i:integer;
  begin write(c:1,m:2);for i:=1 to n do write(' ')
  end;
procedure write_l_ptr(l:l_ptr);
  begin if l=nil then write('*l*') 
    else with l^ do 
      begin write(unique:4,':',loc:4,',',growth:4,',');
        if next_l=nil then write(',   *') else write(next_l^.unique:4);
        write(',',locdir_number:4,',')
      end
  end;

export procedure disp_l_ptr_list(n,m:integer);
  var i,j:integer;
    begin if tree_display then
      begin skip('l',n,m);writeln('---------- l_ptr display ',l_ptr_count:1);
        for i:=1 to l_ptr_count do 
          begin skip('l',n+5,m);write_l_ptr(l_ptr_list[i]);
            writeln
          end
      end
    end;
procedure write_sdi_ptr(s:sdi_ptr);
   begin if s=nil then write('*s*') else 
     with s^ do
       begin write(unique:4,':',sdi_linnumber:4,':',loc_field:4,',',opsize:4,',',opnum:4);
         if gen_rec=nil then write(',   *') else write(',',gen_rec^.unique:4);
         if next_sdi=nil then write(',   *')else write(',',next_sdi^.unique:4);
         if back_sdi=nil then write(',   *')else write(',',back_sdi^.unique:4);
         if sdi_chain=nil then write(',   *')else write(',',sdi_chain^.unique:4);
         write(',',locdir_number:4)
        end
   end;

export procedure disp_sdi_ptr_list(n,m:integer);
  var i:integer;
    begin if tree_display then
      begin skip('s',n,m);writeln('---------- sdi_ptr disp ',sdi_ptr_count:1);
       for i:= 1 to sdi_ptr_count do
        begin skip('s',n+5,m);write_sdi_ptr(sdi_ptr_list[i]);
          writeln
        end
      end
    end;

procedure write_loc_ptr(l:loc_ptr);
  begin if l=nil then write('*l*') else
    begin write(l^.unique:4,':',l^.loc:4);
      if l^.next_loc=nil then write(',   *')else write(',',l^.next_loc^.unique:4);
      write(',',l^.locdir_number:4)
    end
  end;
export procedure disp_loc_ptr_list(n,m:integer);
  var i:integer;
    begin if tree_display then
      begin skip('p',n,m);writeln('---------- loc_ptr disp ',loc_ptr_count:1);
        for i:=1 to loc_ptr_count do
         begin skip('p',n+5,m);write_loc_ptr(loc_ptr_list[i]);
           writeln
         end
      end
    end;
procedure write_irec(r:irec);
  begin if r=nil then write('*i*') else
    begin write(r^.unique:2,':');
     if r^.next_irec=nil then write(',*')else write(',',r^.next_irec^.unique:2);
     write(',',r^.location:2,',',r^.locdir_number:2)
    end
  end;

export procedure disp_irec_list(n,m:integer);
  var i:integer;
    begin if tree_display then 
     begin skip('i',n,m);writeln('---------- irec disp ',irec_count:5);
      skip('i',n+5,m);
      for i:=1 to irec_count do with  r=irec_list[i]^ do
       begin write_irec(irec_list[i]);
         if (i mod 4)=0 then begin writeln;skip('i',n+5,m) end else write('|')
       end;
      writeln
     end
    end;

 
export procedure write_hex(num:integer);
  var i:integer;buf:packed array[1..8]of char;
      chars:packed array[1..16] of char;
  begin write('h''');chars:='0123456789abcdef';
    for i:=8 downto 1 do 
      begin buf[i]:=chars[(num & 15) +1];num :=num>>4 end;
    for i:=1 to 8 do write(buf[i])
  end;
 
export procedure dispgen(n:integer; m:integer;var g:gen);
 begin if tree_display then
  begin  skip('g',n,m);write('dispgen -- '); writegen(g);writeln
  end
 end;

procedure dispsymref(n:integer;m:integer;s:symrefptr);
  var i,j:integer;
  begin if tree_display then
    begin
      while s<>nil do
        begin skip('=',n,m);
          with s^ do
            for i:=1 to linerefinx do write(lineref[i]:3,' ');
          writeln;
          s:=s^.nextrefrec
        end
    end
  end;
export procedure dispsymentree(n:integer;m:integer;s:symptr);
  var i:integer;fin:boolean;
  begin if tree_display then if s<>nil then
    begin skip('*',n,m);fin:=false; 
      for i:=1 to 33 do if not fin then
        if ord(s^.symbolname^[i])<32 then fin:=true
        else write(s^.symbolname^[i]);
      writeln;
      skip('*',n,m);write('line ',s^.linedefined:3,' sdi_c ',s^.sdi_count,
           ' ld_num ',s^.locdir_number);
      if ord(s^.symdefined[0])=ord(false) then write(' un') else write(' ');
      writeln('defined');
      dispsymref(n,m,s^.references);
      dispsymentree(n+2,m,s^.leftptr);
      dispsymentree(n+2,m,s^.rightptr)
    end
  end;
      
export procedure dispsymentrylist(n,m:integer);
  var i,j:integer;fin:boolean;
  begin if tree_display then
    for j:=1 to symentrycount do with symentrylist[j]^ do
      begin skip('<',n,m);fin:=false;write('"');
        for i:=1 to 33 do  if not fin then
          if ord(symbolname^[i])<32 then  fin:=true
          else write(symbolname^[i]:1);
        writeln('"  tag:',unique1:2);
        skip('<',n,m);write('line ',linedefined:3,' sdi_c ',
        sdi_count,' ld_number ',locdir_number);
        if ord(symdefined[0])=ord(false) then writeln(' undef')
        else 
	  begin write(' v=');writegen(symbol_definition);writeln;
	    skip('<',n,m);writestack(symbol_definition.start_of_stack);writeln
          end;
        skip('<',n,m);
        write(' left ');
        if leftptr=nil then write('***') else write(leftptr^.unique1:3);
        write(' right ');
        if rightptr=nil then write('***') else write(rightptr^.unique1:3);
        write(' next ');
        if nextsym=nil then write('***') else write(nextsym^.unique1:3);
        write(' scope ');
       if scope.parent=nil then write('***')else write(scope.parent^.unique2:3);
        writeln;
      end
  end;
    
export procedure dispsymtreelist(n,m:integer);
  var i,j:integer;
  begin if tree_display then
    for j:=1 to symtreecount do with symtreelist[j]^ do
      begin skip('>',n,m);write('symtree tag ',unique2:3,' startloc ',startloc:3
           ,' endloc ',endloc:3,' symptr ');
        if syment=nil then writeln('***') else writeln(syment^.unique1:3);
        skip('>',n,m);write(' left right papa son = ');
        if leftlink=nil then write('***')else write(leftlink^.unique2:3);
        if rightlink=nil then write('***') else write(rightlink^.unique2:3);
        if papalink=nil then write('***') else write(papalink^.unique2:3);
        if sonlink=nil then writeln('***') else writeln(sonlink^.unique2:3)
      end
  end;

procedure writeattrib(a:attribute);
  begin if ord(a.attribstate[0])=ord(false) then write('|') else write(':');
    if ord(a.attrib[0])=0 then write('b') else
    if ord(a.attrib[0])=1 then write('w') else
    if ord(a.attrib[0])=2 then write('d') else
    write('<',ord(a.attrib[0]):0,'>')
 end;

export procedure dispattrib(n:integer; m:integer;var a:attribute);
 begin if tree_display then
  begin skip('a',n,m);
    writeattrib(a);writeln 
  end
 end;
 
export procedure dispstack(n:integer; m:integer;var s:stackptr);
 begin if tree_display then
  begin skip('s',n,m);writestack(s);writeln end
 end;

procedure writestack;
  begin write('s(');
    if s=nil then write('***') else
    if s^.stackrectype=exprec then writeexpr(s^.erec) else writetrm(s^.trec);
    write(')')
  end;


export procedure dispscale(n:integer; m:integer;var s:scaled);
 begin if tree_display then
  begin skip('c',n,m);write('scaled ');
    if s.scalestate then 
       writeln('yes "',ord(s.sreg[0]):1,'"',ord(s.satt[0]):1,'"')
    else writeln('no')
  end
 end;

export procedure dispexpr(n:integer; m:integer;var e:expression);
 begin if tree_display then begin skip('e',n,m);writeexpr(e);writeln end;
 end;

procedure writeop;
  begin
    case e of
     comop:write(',');notop:write('~');extop:write('^');modop:write('%');
     andop:write('&');shlop:write('<');shrop:write('>'); orop:write('|');
     xorop:write('!');addop:write('+');subop:write('-');mulop:write('*');
     divop:write('/');relop:write('#');absop:write('@');
     otherwise write('?',ord(e):1,'?');
   end;
 end;
procedure writeexpr;
  begin write('e(');writestack(e.opd1);writeop(e.op);
    writestack(e.opd2);write(')')
  end;
  

export procedure disptrm(n:integer; m:integer;var t:trm);
 begin if tree_display then
  begin skip('t',n,m);writetrm(t);
    writeln
  end
 end;
procedure writetrm;
 begin write('t(');
   writeop(t.unop);
   if t.termtype=factor then writefact(t.fact) else writestack(t.sub);
   writeattrib(t.termlength);write(')')
 end;

export procedure dispfact(n:integer; m:integer;var f:factorrec);
 begin if tree_display then 
  begin skip('f',n,m);writefact(f);writeln
  end
 end;

procedure writefact;
  var i:integer;
  begin write('f(');
    if f.factortype=fsym then writesym(f.fsymbol) else
    if f.factortype=fgen then
      begin write('gen(');writegen(f.fgenref);write(')') end
    else if f.factortype=fres then
      begin 
        for i:=1 to maxrtlen do if rtmnemonics[f.fresindex].rtchars[i]>' ' then
             write(rtmnemonics[f.fresindex].rtchars[i])
      end
    else write('***');
   write('{',f.sdi_fnum:0,'})')
 end;
procedure writesym;
  begin write('sym(');
    if s=nil then write('***') else
    write(s^.unique1:1);
    write(')')
  end;

export procedure disptoken(n:integer;m:integer;var t:token);
 var i:integer;
 begin if tree_display then
  begin skip('k',n,m);writeln('token type = ',ord(t.tokentype):1);
    case t.tokentype of 
     symbol:begin skip('k',n+2,m);write('symbol="'); 
              for i:=1 to t.symlength do write(t.sym[i]);writeln('"')
            end;
     specchar:begin skip('k',n+2,m);writeln('specchar="',t.spch,'"') end;
     gentoken:dispgen(n+3,m,t.genref);
     restoken:begin skip('k',n+2,m);writeln('restoken index=',t.rtokenindex)end
    end
  end
 end;

procedure writedisplacement(d:displacement);
  begin
    write('d(');
    case d.disp_type of
      const_value: write(d.disp_value:0);
      string_value: 
	begin write('"');
	  for i:=1 to d.str_rec^.strglen do write(d.str_rec^.strchars[i]);
	  write('"')
	end
    end;
    write(')')
  end;

procedure write_stab_rec(s:stab_rec_ptr);
  begin write('Stab_rec ');
    case s^.whichstab of
      STABSd:begin 
		writesym(s^.symbol_ptr);
	     end;
      STABd :begin write('STAB ');write_hex(s^.type_info_doub);
		if s^.is_1_present then 
		  begin writedisplacement(s^.stabnum1);write(','); end;
		writedisplacement(s^.stabnum2);
	     end;
      SOURCEd:begin
		write('SOURCE ',s^.sourceitem:0,'|');
		if s^.islineorchar then write('l') else write('c');
		if s^.ispcrel then write(' pcrel ') else write(' symol ')
	      end;
    end;
  end;

export procedure disp_stab_rec(n,m:integer;s:stab_rec_ptr);
  begin if tree_display then
    begin skip('S',n,m);write_stab_rec(s);writeln end;
  end;
        
export procedure disp_stab_chain(n,m:integer;s:stab_rec_ptr);
  begin 
    repeat
      if s<> nil then
        begin disp_stab_rec(n,m,s);s:=s^.next_stab_rec end;
    until s= nil
  end;

  begin 
    uniquenumber:=0;
    symtreecount:=0;
    sdi_ptr_count:=0;
    l_ptr_count:=0;
    loc_ptr_count:=0;
    irec_count:=0;
    symentrycount:=0;
  end.
