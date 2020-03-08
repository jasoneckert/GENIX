{!B}
module traverse;

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

const SCCSId = Addr('@(#)traverse.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'scantoken.e'; ! DEPENDENCY
import 'prsymtable.e'; ! DEPENDENCY
import 'sdis.e'; ! DEPENDENCY
import 'disp.e'; ! DEPENDENCY
import 'scanexp.e'; ! DEPENDENCY
import 'scan.e'; ! DEPENDENCY

export 'traverse.e'; ! DEPENDENCY

 

  export procedure listsym(sptr : symptr);
    
    var temp,spcount : integer;
	r : symrefptr;firstline:boolean;
	fct : factorrec;

    begin
      with sptr^ do 
      begin
	if equ_sdi[0]=chr(ord(true)) then
	  begin resolve_1_expression(symbol_definition.start_of_stack,fct);
	    if fct.factortype<>fgen then reporterror(100*215+undefined_symbol)
	    else symbol_definition:=fct.fgenref;
          end;
	spcount := largest_sym_size + 1;
	temp := 1;
	while (temp <= largest_sym_size) and (symbolname^[temp]<> nilchar) do
	  begin
	    write(symbolname^[temp]);
	    spcount := spcount - 1;
	    temp := temp + 1;
	  end;
	write(blank : spcount);
	if symdefined[0] = chr(ord(true)) then
	  begin
	  {  write('type: ');write_hex(scope.domain);  added by Tim }
	    write('  at:',linedefined:6,'    value = ');
            writegen(symbol_definition);writeln
	  end
	else writeln('***** undefined *****');
	r := references;
        firstline:=true;
	while r <> nil do
	  with r^ do
	  begin
	    if firstline then
              begin firstline:=false;write(blank:(largest_sym_size+1),'from:')
              end
            else
              write(blank : (largest_sym_size+4));
	    for temp := 1 to linerefinx do
	      write(lineref[temp] : 6);
	    writeln;
	    r := nextrefrec;
	  end;
      end;
    end;

  export procedure traversesymtable(sptr : symptr);

    begin
      if sptr<>nil then
	begin
	  traversesymtable(sptr^.leftptr);
	  if sptr^.symbolname^[1]<>'/' then listsym(sptr);
	  traversesymtable(sptr^.rightptr);
	end
    end;

  procedure traverseroot(rptr : symtreeptr);

    begin
      if rptr <> nil then
	begin
	  listsym(rptr^.syment);
	  if rptr^.sonlink <> nil then
	    traverseroot(rptr^.sonlink);
	  traversesymtable(symtable);
	  traverseroot(rptr^.rightlink);
	end
    end;

  begin end.
