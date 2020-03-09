! pio.p: version 1.1 of 6/2/83
! 
!
! @(#)pio.p	1.1 (NSC) 6/2/83
!
(****************************************************************************)
(*                                                                          *)
(* The following subroutines are stand-alone libraries which can be used    *)
(* for PASCAL program on DB16K board. It should be compilied in assembly    *)
(* and the name of each subroutine should be changed as follows :           *)
(*                                                                          *)
(*     wrote  -->  write                                                    *)
(*     road   -->  read                                                     *)
(*                                                                          *)
(*     After changing these names, it should be compilied in object codes.  *)
(* On the other hand, pio.S has all these names being changed, it can be    *)
(* used to get .O file directly.                                            *)
(*                                                                          *)
(****************************************************************************)

module all;
import procedure putchar(c:char);ccall forward;
import procedure getchar return : char;ccall forward;
type
     ptr = array [1..80] of char;
     f_status = (closed,openforinput,openforoutput);
     l_status = (none,ateoln,ateolf);
     trec = record
	    stream : integer;
	    item_size: integer;
	    buf: integer;
	    fs: f_status;
	    ls: l_status;
	    b: boolean;
	    f_flushed: boolean;
	    end;
var
     endch : char;
     export output : text;
     export input : text;

(****************************************************************************)
(* Write a string of characters without format.                             *)
(****************************************************************************)
export procedure  wrote$textstring2(var t1 : trec; ref t : any string);
var
   i : integer;
begin
     i := 1;
     while (t[i] <> chr(0))do 
     begin
	  putchar(t[i]);
	  i := i + 1;
     end;
end;

(****************************************************************************)
(* Write a string of characters with specified width.                       *)
(****************************************************************************)
export procedure wrote$textstring3(var t1 : trec; ref t : any string; j : integer);
var
   i, k : integer;
begin
     if j < 0 then wrote$textstring2(t1,
		   'error : Negative width specified for formatting.')	
     else begin
     k := 1;
     while (k <= j) and (t[k] <> chr(0)) do k := k + 1;
     for i := 1 to j-k+1 do putchar(' ');
     for i := 1 to k-1 do putchar(t[i]);
     end;
end;

(****************************************************************************)
(* Write one character without formats.                                     *)
(****************************************************************************)
export procedure wrote$textchar2(var t1 : trec; c : char);
begin
     putchar(c);
end;

(****************************************************************************)
(* Write one character with specified width.                                *)
(****************************************************************************)
export procedure wrote$textchar3(var t1 : trec; c : char; j : integer);
var
   ch : char;
   i : integer;
begin
     if j < 0 then wrote$textstring2(t1,
		   'error : Negative width specified for formatting.')
     else begin
     ch := ' ';
     for i := 1 to j-1 do putchar(ch);
     putchar(c);
     end;
end;

(****************************************************************************)
(* Write the End-of-Line character and have the output ready for the next   *)
(* new line.                                                                *)
(****************************************************************************)
export procedure wroteln$text(var t1 : trec);
var
   lf : char := chr(10);
begin
     putchar(lf);
end;

(****************************************************************************)
(* Write the integer numbers with specified width.                          *)
(****************************************************************************)
export procedure wrote$textinteger3(var t1 :trec; n :integer; l : integer);
type star =packed array [1..12] of char;
var
   i, j : integer;
   radix : integer := 10;
   s : star := '            ';
   posn : integer := abs(n);
begin
     if l < 0 then wrote$textstring2(t1,
		   'error : Negative width specified for formatting.')
     else begin
     if n = minint then begin 
			     s := '-2147483648 ';
			     i := 11;
                        end
     else begin
	       i := 0;
	       repeat
		     s[11-i] := chr(posn mod radix + ord('0'));
		     i := i + 1;
		     posn := posn div radix
               until posn = 0;
	       if n < 0 then begin
				  s[11-i] := '-';
				  i := i + 1;
                             end;
          end;
     s[12] := chr(0);
     if l > i then begin
			for j := 1 to l-i do putchar(' ');
                   end;
     for j := 12-i to 11 do putchar(s[j]);
     end;
end;

(****************************************************************************)
(* Write the integer numbers by default width 11.                           *)
(****************************************************************************)
export procedure wrote$textinteger2(var t1: trec; n: integer);
var
   k : integer;
begin
     k := 11;
     wrote$textinteger3(t1, n, k);
end;

(****************************************************************************)
(* Write a boolean variable without formats.                                *)
(****************************************************************************)
export procedure wrote$textboolean2(var t1: trec; b: boolean);
begin
     if b then wrote$textstring2(t1,'true')
     else wrote$textstring2(t1,'false');
end;

(****************************************************************************)
(* Write a boolean variable with specified formats.                         *)
(****************************************************************************)
export procedure wrote$textboolean3(var t1: trec; b: boolean; j: integer);
begin
     if j < 0 then wrote$textstring2(t1,
		   'error : Negative width specified for formatting.')
     else begin
     if b then wrote$textstring3(t1, 'true', j)
     else wrote$textstring3(t1, 'false', j);
     end;
end;

(****************************************************************************)
(* Read  one character.                                                     *)
(****************************************************************************)
export procedure road$textchar2(var t1:trec; var c: char);
begin
     c := getchar;
     endch := c;
end;

(****************************************************************************)
(* Read a string of character.                                              *)
(****************************************************************************)
export procedure road$textstring3(var t1: trec; var t: any string; var j: integer);
var
   bound : integer;
begin
     if j < 0 then wrote$textstring2(t1,
		   'error : Negative width specified for formatting.')
     else begin
     bound := j - 1;
     j := 1;
     t[j] := getchar;
     while (t[j] <> chr(0)) and (bound > 0) do
     begin
	  j := j + 1;
	  t[j] := getchar;
	  bound := bound - 1;
     end;
     endch := t[j];
     end;
end;

(****************************************************************************)
(* Read the input until the end-of-line mark found and have the input ready *)
(* at the begining of next line.                                            *)
(****************************************************************************)
export procedure roadln$text(var t1: trec);
begin
     while (endch <> chr(13)) do endch := getchar;
end;

(****************************************************************************)
(* Read the integers.                                                       *)
(****************************************************************************)
export procedure road$textinteger2(var t1: trec; var n: integer);
var
   c : char;
   flag, sign : boolean;
begin
     flag := false;
     sign := true;
     repeat
	   c := getchar;
     until ((c <> ' ') and (c <> '	')) or (c = chr(13));
     if (c <> chr(13)) then
     begin
	  if (c = '-') then
	     begin
	          sign := false;
	          c := getchar;
	     end
          else if (c = '+') then c := getchar;
	  n := ord(c) -ord('0');
	  c := getchar;
	  while ((c <> chr(13)) and  (c <= '9') and (c >= '0') and (not flag)) do
	  begin
	       if (n < maxint div 10) or ((n = maxint div 10)
				     and  (ord(c) <= maxint mod 10)) then
                  begin
		       n := n * 10 + (ord(c) - ord('0'));
		       c := getchar;
                  end
               else if (n = maxint div 10) and (ord('c') = maxint mod 10 + 1)
					   and ( not sign) then
                       begin
			    n := minint;
			    flag := true;
			    sign := true;
                       end
                    else begin
			      wrote$textstring2(t1,
				    'error : Too big integer for input.');
			      flag := true;
                         end;
          end;
     end;
     endch := c;
end;

begin
end.
