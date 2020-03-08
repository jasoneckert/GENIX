{!L}
module prsymtable;	{process symbols & symbol table}

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

const SCCSId = Addr('@(#)prsymtable.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'scantoken.e'; ! DEPENDENCY
import 'scan.e'; ! DEPENDENCY
import 'traverse.e'; ! DEPENDENCY
import 'scmpc.e'; ! DEPENDENCY
import 'stab.e'; ! DEPENDENCY
import 'sdis.e'; ! DEPENDENCY
import 'disp.e'; ! DEPENDENCY


export 'prsymtable.e'; ! DEPENDENCY

var new_name_flag : boolean;


export function get_namelength(var s_name : name) name_length : bytez;
  var temp : integer;
  begin
    temp := 1;
    while (temp <= 32) and (s_name[temp] <> nilchar) do
      temp := temp + 1;
    name_length := temp - 1;
  end;

export procedure new_name (var nam : nameptr; var str : name;
		    var name_index : integer; 
		    var name_table : names_table);

  i : integer;
  np : nameptr;

begin
  new_name_flag := true;
  if name_index <= maxblocks then
    begin
      if get_namelength (str) >= (1024 - name_table[name_index].last_used) then
	begin
	  name_index := name_index + 1;
	  new (name_table[name_index].base_addr);
	end;
      i := 0;
      with name_table[name_index] do
	begin
	  nam.address := addr (base_addr^[last_used + 1]);
          repeat
	    i := i + 1;
	    nam^[i] := str[i];
	  until (nam^[i] = nilchar) or (i = 33);
          last_used := last_used + i;
	end;
    end
  else
    reporterror(100*090+nomoresymbols);
end;


      
export function findsym(sptr : symptr;
		 var name1 : name) : symptr;

var finished : boolean;
    cmp : integer;
    cmp_in : integer;

begin
  finished := false;
  repeat
    if sptr = nil then
      finished := true
    else
      begin
	with sptr^ do
	  begin
	    cmp := scmpc(addr(name1),symbolname.address);
	    if cmp < 0 then 	{key < symbolname}
	      sptr := leftptr
	    else if cmp > 0 then	{key > symbolname}
	      sptr := rightptr
	    else finished := true;
	  end
      end;
  until finished;
  findsym := sptr;
end;


export function process_typename(t_sym:name):nameptr;
  var t_sptr:symptr;tt_sym:nameptr;
  begin
    t_sptr:=findsym(symtable,t_sym);
    if t_sptr=nil then new_name(tt_sym,t_sym,curr_symname,symnames)
    else tt_sym:=t_sptr^.symbolname;
    process_typename:=tt_sym
  end;

export procedure search( var sptr : symptr;
		  var name1 : name;
		  var h : boolean;
		  var newsym : symptr);
var
  p1,p2 : symptr;
  cmp,cmp_in : integer;

begin
  if sptr = nil then
    begin
      newsymentry(sptr); h := true; newsym := sptr;
      with sptr^ do
	begin
	  if name1[1]=' ' then
	    new_name(symbolname, name1,currsymnamed,d_symnames)
	  else if {objsymtable} true or (not make_global) then { Tim }
	    new_name (symbolname, name1,
		      curr_symname,symnames)
	  else new_name(symbolname,name1,
			currsymname2,g_symnames);
	  leftptr := nil; rightptr := nil; balance := 0;
	  references := nil; symdefined[0] := chr(ord(false));
	  scope.domain := N_MAT; symbol_definition.start_of_stack:=nil;
	end;
    end
  else 
    begin
      cmp := scmpc(addr(name1),sptr^.symbolname.address);
      if cmp < 0 then {less than}
	begin
	  search(sptr^.leftptr,name1,h,newsym);
	  if h then
	    if sptr^.balance = 1 then
	      begin
		sptr^.balance := 0; h := false;
	      end
	    else if sptr^.balance = 0 then
	      sptr^.balance := -1
	    else 
	      begin	{rebalance}
		p1 := sptr^.leftptr;
		if p1^.balance = -1 then
		  begin {single LL rotation}
		    sptr^.leftptr := p1^.rightptr; p1^.rightptr := sptr;
		    sptr^.balance := 0; sptr := p1;
		  end
		else
		  begin {double lr rotation}
		    p2 := p1^.rightptr;
		    p1^.rightptr := p2^.leftptr; p2^.leftptr := p1;
		    sptr^.leftptr := p2^.rightptr; p2^.rightptr := sptr;
		    if p2^.balance = -1 then
		      sptr^.balance := 1
		    else sptr^.balance := 0;
		    if p2^.balance = 1 then
		      p1^.balance := -1
		    else p1^.balance := 0;
		    sptr := p2;
		  end;
		sptr^.balance := 0; h := false;
	      end
	end
      else if cmp > 0 then {greater}
	begin
	  search(sptr^.rightptr,name1,h,newsym);
	  if h then
	    if sptr^.balance = -1 then
	      begin
		sptr^.balance := 0; h := false;
	      end
	    else if sptr^.balance = 0 then
	      sptr^.balance := 1
	    else 
	      begin	{rebalance}
		p1 := sptr^.rightptr;
		if p1^.balance = 1 then
		  begin {single RR rotation}
		    sptr^.rightptr := p1^.leftptr; p1^.leftptr := sptr;
		    sptr^.balance := 0; sptr := p1;
		  end
		else
		  begin {double rl rotation}
		    p2 := p1^.leftptr;
		    p1^.leftptr := p2^.rightptr; p2^.rightptr := p1;
		    sptr^.rightptr := p2^.leftptr; p2^.leftptr := sptr;
		    if p2^.balance = 1 then
		      sptr^.balance := -1
		    else sptr^.balance := 0;
		    if p2^.balance = -1 then
		      p1^.balance := 1
		    else p1^.balance := 0;
		    sptr := p2;
		  end;
		sptr^.balance := 0; h := false;
	      end
	end
      else 
	begin
	  h := false;
	  newsym := sptr;
	end
    end
end;

export function processsymbol{(curtoken : token) sptr : symptr};

var
  name1 : name;
  temp1 : integer;
  h : boolean;

begin
  new_name_flag := false;
  if curtoken.tokentype = symbol then
    begin
      name1[1] := nilchar;
      h := false; 
      if curtoken.symlength > largest_sym_size then
        largest_sym_size := curtoken.symlength;
      temp1 := 1;
      while (curtoken.sym[temp1] <> nilchar) and (temp1 < 33) do
        begin
          name1[temp1] := curtoken.sym[temp1];
          temp1 := temp1 + 1;
        end;
      name1[temp1] := nilchar;
      search(symtable,name1,h,sptr);
      if not new_name_flag and make_global then
{ 	if not objsymtable then      added by Tim 
	  new_name(sptr^.symbolname,sptr^.symbolname^,
		   currsymname2,g_symnames); }(* in proper string table *)
    end
  else
    begin
      reporterror(100*091+invalidchar);
      sptr := nil
    end;
end;

export procedure assignlineref(s: symptr);

var r : symrefptr;

begin
  if printsymtable then
    with s^ do
    begin
      if references = nil then
        begin
	  new(references);
	  r := references;
	  r^.linerefinx := 0; r^.nextrefrec := nil;
        end
      else
        begin
	  r := references;
	  while r^.nextrefrec <> nil do
	    r := r^.nextrefrec;
        end;
      with r^ do
      begin
        linerefinx := linerefinx + 1;
        lineref[linerefinx] := linenumber;
        if linerefinx = 10 then
	  begin
	    new(nextrefrec);
	    nextrefrec^.linerefinx := 0;
	    nextrefrec^.nextrefrec := nil;
	  end;
      end;
    end;
end;

var
  i: integer;

begin
  for i := 1 to maxblocks do 
    begin
      symnames[i].last_used := 0;
      d_symnames[i].last_used := 0;
      g_symnames[i].last_used := 0;
    end;
  new (symnames[1].base_addr);
  new (d_symnames[1].base_addr);
  new (g_symnames[1].base_addr);
  curr_symname := 1;
  currsymnamed := 1;
  currsymname2 := 1;
  largest_sym_size := 0;
end.
