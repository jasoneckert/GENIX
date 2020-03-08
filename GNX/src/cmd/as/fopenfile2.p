module fopenf;

const SCCSId = Addr('@(#)fopenfile2.p	3.31         9/20/83');
!@(#)Copyright (C) 1983 by National Semiconductor Corp.

import 'bio.e'; ! DEPENDENCY 
type string32=packed array[1..32]of char;
     string2 =packed array[1..2]of char;
var md:string2;
export routine fopen_n(var names: string32;mode :char):cfile;
  begin md[1]:=mode;md[2]:=' ';
   fopen_n:=fopen(names,md)
  end;
begin
end.

