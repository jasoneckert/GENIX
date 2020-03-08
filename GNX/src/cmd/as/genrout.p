module genrout;

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

const SCCSId = Addr('@(#)genrout.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'glob_const.e'; ! DEPENDENCY
import 'formatsmod.e'; ! DEPENDENCY
import 'glob_types.e'; ! DEPENDENCY
import 'glob_vars.e'; ! DEPENDENCY
import 'scantoken.e'; ! DEPENDENCY
import 'dirs.e'; ! DEPENDENCY
import 'sdis.e'; ! DEPENDENCY
import 'disp.e'; ! DEPENDENCY
export 'genrout.e'; ! DEPENDENCY

import function fnew(n:integer):integer; forward;

var top , bot , sizeb : integer;

export function newpc(sizepc:integer):integer;
  begin
	if bot - sizepc < top then
	begin
	  top := fnew(sizeb);
	  bot := top + sizeb;
        end;
	bot := bot - sizepc;
	newpc := bot;
  end;
export function newsb(sizesb:integer):integer;
  begin
	if bot - sizesb < top then
	begin
	  top := fnew(sizeb);
	  bot := top + sizeb;
	end;
	bot := bot - sizesb;
	newsb := bot;
   end;


  export procedure increment_locationcounter(temp:integer);

    begin
      case assembly_mode of
	standard : begin
      		     locationcounter := locationcounter + temp;
      		     if currentmode = progrel then
		       last_alloc_pc := locationcounter
      		     else if currentmode = stbsrel then
		       last_alloc_sb := locationcounter;
		   end;
	inverted : begin
      		     locationcounter := locationcounter - temp;
		   end;
	link_inverted : begin
      		     locationcounter := locationcounter - temp;
		   end;
      end;
    end;
  
  export function new_g_index : irec;
    
    begin
      if currentmode = stbsrel then
	begin
	  if first_sbirec = nil then
	    begin
	      new_irec(first_sbirec); last_sbirec := first_sbirec;
	    end
	  else
	    begin
	      new_irec(last_sbirec^.next_irec);
	      last_sbirec := last_sbirec^.next_irec;
	    end;
	  last_sbirec^.next_irec := nil;
	  new_g_index := last_sbirec;
	end
      else
	begin
	  if first_irec = nil then
	    begin
	      new_irec(first_irec); last_irec := first_irec;
	    end
	  else
	    begin
	      new_irec(last_irec^.next_irec);
	      last_irec := last_irec^.next_irec;
	    end;
	  last_irec^.next_irec := nil;
	  new_g_index := last_irec;
        end;
    end;

  export function new_g_gen : genirec;
    var temp_size: integer;
    begin
      if currentmode = stbsrel then
	begin
	  if first_sbgenirec = nil then
	    begin
              temp_size := size(gen_interm);
	      first_sbgenirec.address :=  newsb(temp_size); 
	      last_sbgenirec := first_sbgenirec;
	    end
	  else
	    begin
              temp_size := size(gen_interm);
	      last_sbgenirec^.gen_next.address :=  newsb(temp_size); 
	      last_sbgenirec := last_sbgenirec^.gen_next;
	    end;
	  last_sbgenirec^.gen_next := nil;
	  new_g_gen := last_sbgenirec;
	end
      else
	begin
	  if first_genirec = nil then
	    begin
              temp_size := size(gen_interm);
	      first_genirec.address :=  newpc(temp_size); 
              last_genirec := first_genirec;
	    end
	  else
	    begin
              temp_size := size(gen_interm);
	      last_genirec^.gen_next.address :=  newpc(temp_size); 
	      last_genirec := last_genirec^.gen_next;
	    end;
	  last_genirec^.gen_next := nil;
	  new_g_gen := last_genirec;
        end;
    end;
 
  export function new_g_intermrec : girec;
    var  temp_size: integer;
    begin
      if currentmode = stbsrel then
	begin
	  if first_sbgirec = nil then
	    begin
              temp_size := size(pcsb_interm);
	      first_sbgirec.address :=  newsb(temp_size); 
	      last_sbgirec := first_sbgirec;
	    end
	  else
	    begin
              temp_size := size(pcsb_interm);
	      last_sbgirec^.interm_next.address :=  newsb(temp_size); 
	      last_sbgirec := last_sbgirec^.interm_next;
	    end;
	  last_sbgirec^.interm_next := nil;
	  new_g_intermrec := last_sbgirec;
	end
      else
	begin
	  if first_girec = nil then
	    begin
              temp_size := size(pcsb_interm);
	      first_girec.address :=  newpc(temp_size); 
	      last_girec := first_girec;
	    end
	  else
	    begin
              temp_size := size(pcsb_interm);
	      last_girec^.interm_next.address :=  newpc(temp_size); 
	      last_girec := last_girec^.interm_next;
	    end;
	  last_girec^.interm_next := nil;
	  new_g_intermrec := last_girec;
        end;
    end;

  begin
    first_irec := nil; last_irec := nil; 
    first_sbirec := nil; last_sbirec := nil;
    first_genirec := nil; last_genirec := nil;
    first_sbgenirec := nil; last_sbgenirec := nil;
    first_girec := nil; last_girec := nil;
    first_sbgirec := nil; last_sbgirec := nil;
    last_alloc_sb := 0; last_alloc_pc := 0;
    sizeb := %2000;
    top := 0; bot := 0;
  end.

