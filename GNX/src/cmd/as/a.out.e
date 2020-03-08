
! @(#)a.out.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

(*		A.OUT.X for 16K			        *)
(* "+" indicates an addition to the Berkeley 4.1 format *)

(*
 * Header prepended to each a.out file.
 *)
const

OMAGIC =	0263;		(* old impure format *)
NMAGIC =	0264;		(* read-only text *)
ZMAGIC =	0267;		(* demand load format *)
XMAGIC =	0268;		(*+demand load format
				 *+locations 0-1023 unmapped *)

 R_SEG  = %3;			(* segment mask *)
 R_PROC = %4;			(* bit that specifies procedure descriptor *)
 R_TEXT = %1;			(* text segmebt *)
 R_IDATA= %2;			(* initialized data segment *)
 R_UDATA= %3;			(* uninitialized data segment bss *)
type

exec = record
 a_magic      : integer;	(* magic number *)
 a_text       : integer;	(* size of text segment *)
 a_data       : integer;	(* size of initialized data *)
 a_bss        : integer;	(* size of uninitialized data *)
 a_syms       : integer;	(* size of symbol table *)
 a_entry      : integer;	(* entry point address *)
 a_entry_mod  : integer;	(*+entry point - mod number *)
 a_trsize     : integer;	(* size of text relocation -
				 * current assembler sets to zero *)
 a_drsize     : integer;	(* size of data relocation -
				 * current assembler sets to zero *)
 a_mod        : integer;	(*+size of mod table within text *)
 a_link       : integer;	(*+size of link table within text *)
 a_strings    : integer;	(*+size of string table *)
 a_text_addr  : integer;	(* address of text section in addresrr space *)
 a_mod_addr   : integer;	(* address of mod table in address space *)
 a_dat_addr   : integer;	(* address of data section in address space *)
end;

relocation_caca = packed record
 r_address   : integer;		(* address which is relocated *)
 r_symbolnum : 0..%FFFFFF;	(* local symbol ordinal or addr mode *)
 r_length    : 0..3;		(* 0=byte, 1=word, 2=double word *)
 r_external  : 0..1;		(* does not include value of sym referenced???*)
 r_disp	     : 0..1;		
 r_unused    : 0..15;		
end;
