
! @(#)disp.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

! disp.x

procedure displine(n,m:integer);forward;
procedure write_hex(num:integer);forward;
procedure dispgen(n:integer; m:integer;var g:gen);forward;
procedure dispattrib(n:integer; m:integer;var a:attribute);forward; 
procedure dispstack(n:integer; m:integer;var s:stackptr);forward; 
procedure dispscale(n:integer; m:integer;var s:scaled);forward; 
procedure dispexpr(n:integer; m:integer;var e:expression);forward; 
procedure dispsymref(n:integer;m:integer;s:symrefptr);forward;
procedure dispsymentree(n:integer;m:integer;s:symptr);forward;
procedure disptrm(n:integer; m:integer;var t:trm);forward; 
procedure dispfact(n:integer; m:integer;var f:factorrec);forward; 
procedure disptoken(n:integer;m:integer;var t:token);forward;
function uniquei:integer;forward;
procedure newsymentry(var s:symptr);forward;
procedure newsymtree(var s:symtreeptr);forward;
procedure dispsymentrylist(n,m:integer);forward;
procedure dispsymtreelist(n,m:integer);forward;
procedure new_l_ptr(var l:l_ptr);forward;
procedure new_sdi_ptr(var s:sdi_ptr);forward;
procedure new_loc_ptr(var lc:loc_ptr);forward;
procedure new_irec(var ir:irec);forward;
procedure disp_l_ptr_list(n,m:integer);forward;
procedure disp_sdi_ptr_list(n,m:integer);forward;
procedure disp_loc_ptr_list(n,m:integer);forward;
procedure disp_irec_list(n,m:integer);forward;
procedure writegen(g:gen);forward;
procedure disp_stab_rec(n,m:integer;s:stab_rec_ptr);forward;
procedure disp_stab_chain(n,m:integer;s:stab_rec_ptr);forward;
routine display;forward;
