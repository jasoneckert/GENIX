
! @(#)stab.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

(*
 * Format of a symbol table entry (STE) (stab.x)
 *)
N_AMODE = %1f;    (* mask for addressing mode *)
N_EXT   = %10;    (* external bit, or'ed in *)
N_MORE  = %20;    (* more bit, or'ed in *)
N_ETYPE = %07;    (* mask for entry type bits *)

(*
 * addressing modes:
 *)
A_R0  = %0;	(* register 0 - r0 *)
A_R0R = %8;	(* register 0 relative - disp(r0) *)
A_R1R = %9;	(* register 1 relative - disp(r1) *)
A_R2R = %a;	(* register 2 relative - disp(r2) *)
A_R3R = %b;	(* register 3 relative - disp(r3) *)
A_R4R = %c;	(* register 4 relative - disp(r4) *)
A_R5R = %d;	(* register 5 relative - disp(r5) *)
A_R6R = %e;	(* register 6 relative - disp(r6) *)
A_R7R = %f;	(* register 7 relative - disp(r7) *)
A_FPR = %10;	(* frame memory relative - disp2(disp1(FP)) *)
A_SPR = %11;	(* stack memory relative - disp2(disp1(SP)) *)
A_SBR = %12;	(* static memory relative - disp2(disp1(SB)) *)
A_IMM = %14;	(* immediate - value *)
A_ABS = %15;	(* absolute - @disp - not relocated *)
A_EX  = %16;    (* external - EXTERNAL(disp1)+disp2 *)
A_TOS = %17;	(* top of stack - TOS *)
A_FP  = %18;	(* frame memory - disp(FP) *)
A_SP  = %19;	(* stack memory - disp(SP) *)
A_SB  = %1a;    (* static memory - disp(SB) *)
A_PC  = %1b;    (* program memory - disp(PC) 
			 * PC at beginning of module *)

(*
 * There are several kinds of info that ld(1) needs to be aware of
 * that might be in an STE:
 * NM - n_namestr points to a string.
 * TN - n_typestr points to the name of a type.
 * AM - n_addrmode specifies NS16032 addressing mode
 *      and n_value contains displacement(s) for addressing mode.
 *
 * A main symbol table entry (MSTE) defines a symbol (always has name)
 * Possible values of n_etype field:
 *)
N_MAT = 0;	(* (MSTE, NM, AM), uses n_atty *)
N_MTN = 1;	(* (MSTE, NM, AM, TN), symbol is of type specified *)
N_MPTN =2;	(* (MSTE, NM, AM, TN), symbol is ptr to type specd *)
N_METN =3;	(* (MSTE, NM, AM, TN), symbol uses type specd *)
N_SLINE=4;	(* (NM, AM), source line number instead of TN *)
N_SCHAR=5;	(* (NM, AM), source character number instead of TN *)
N_MAUX =6;	(* (MSTE, NM), auxiliary info - uses N_aux *)
N_AUX  =7;	(* auxiliary info - uses N_aux *)
		(* aux STEs use N_at if n_typeinfo is on *)

(*
 * masks for accessing atomic type fields from n_type
 *)
N_BEGIN   =%20;     (* begin lexical scope *)
N_PROC    =%100;    (* procedure entry point *)
		    (* three fields specify the high level
			      * language type of a symbol *)
N_NARGS   =%fe00;   (* number of doublewords of args *)
N_HLLMOD1 =%3000;   (* first type modifier *)
N_HLLMOD2 =%c000;   (* second type modifier *)
N_HLLPTYPE=%f0000;  (* primitive type of symbol *)
N_BFIELD  =%ff00000;(* bit field specification *)

(*
 * type modifier values (n_hllmod1 and n_hllmod2 fields)
 *)
M_NONE  =0;	(* no modifier *)
M_PTR   =1;	(* ptr to *)
M_FUN   =2;	(* function returning *)
M_ARRAY =3;	(* array of *)

(*
 * high level language primitive types (n_hllptype field)
 *)
T_UNDEF =0;	(* undefined or named type *)
T_INT   =1;	(* signed integer *)
T_CHAR  =2;	(* character variable *)
T_UINT  =3;	(* unsigned integer *)
T_SETE  =4;	(* set element *)
T_SET   =5;	(* set - defines a scope *)
T_FLOAT =6;	(* 32-bit float *)
T_DFLOAT=7;	(* 64-bit float *)
T_STRUCT=8;	(* structure - defines a scope *)
T_UNION =9;	(* union - defines a scope *)
T_ENUM =10;	(* enumeration - defines a scope *)
T_ENUME=11;	(* enumeration element *)
T_BOOL =12;	(* boolean *)
T_MORE =13;	(* more type info is needed *)

(*
 * masks for accessing auxiliary STE fields from n_type
 *)
N_TYPEINFO=%80;    (* use atomic type fields, not
			     * the auxiliary STE fields.
			     * n_auxtype is not valid *)
N_INIT    =%40;    (* data segment is initialized *)
N_ROM	  =%100;   (* text segment is in rom *)
N_AUXTYPE =%100;   (* type of N_AUX or N_MAUX STE *)

(*
 * values of n_auxtype field that concern ld(1)
 *)
N_MODTYPE=0;       (* module MSTE *)
N_TXTINFO=1;       (* text segment location and length *)
N_DATINFO=2;       (* data segment location and length *)

N_COMMON =4;       (* last MSTE is external for common
			    * n_value gives size in bytes *)
N_LDISP  =5;       (* uses n_ldisp1 and n_ldisp2 *)
N_LIFE   =6;       (* lifetime of MSTE *)
N_ARBND  =7;       (* array bounds *)

(*
 * masks and macro to recognize a module MSTE
 *)
N_MODMASK =%ff87;
N_MOD     =%6;

type

nlist_types = (N_name_type,N_ldisp1_type);

N_name_rec = record
  N_addrmode : 0..%1f;		(* addressing mode *)
  N_unused   : 0..1;
  N_namestr  : 0..%3ffffff;	(* symbol name *)
end;

N_nm_rec = record
 case nlist_types of
  N_name_type   : (N_name   : N_name_rec);
  N_ldisp1_type : (N_ldisp1 : integer);	  (* only for N_AUX *)
end;		  		(* first word in STE *)

n_type_types = (N_tf_type, N_at_type,N_aux_type);

N_tf_rec = packed record
 N_etype  : 0..7;  		(* type of STE (always) *)
 N_last   : 0..1;  		(* last STE of lexical scope *)
 N_ext    : 0..1;  		(* global symbol *)
 N_more	  : 0..1;		(* next STE is subsidiary *)
 N_typestr: 0..%3ffffff;  	(* type name *)
end;

N_at_rec = packed record
 N_etype   :0..7;  		(* see N_tf *)
 N_last    :0..1;  		(* last STE of lexical scope *)
 N_ext     :0..1;  		(* global symbol *)
 N_more	   :0..1;		(* next STE is subsidiary *)
 N_begin   :0..1;	  	(* begin a lexical scope *)
 N_typeinfo:0..1;
 N_proc    :0..1;	  	(* procedure entry point *)
 N_nargs   :0..%7f;	  	(* number of doublewords of args
				 * popped by function if a type
				 * modifier is M_FUN or N_proc is on *)
 N_hllmod1 :0..3;	  	(* first type modifier *)
 N_hllmod2 :0..3;	  	(* second type modifier *)
 N_hllptype:0..%f;  	        (* primitive type *)
 N_bfield  :0..%ff;	  	(* bit field specification *)
end;				(* atomic type declaration *)

N_aux_rec = packed record
 N_etype   :0..7;  		(* see N_tf *)
 N_last    :0..1;  		(* last STE of lexical scope *)
 N_ext     :0..1;  		(* global symbol *)
 N_more	   :0..1;		(* next STE is subsidiary *)
 N_initialized:0..1;	  	(* is data segment initialized? *)
 N_typeinfo:0..1;		(* use N_at format, N_aux *)
 N_rom	   :0..1;		(* is text in ROM? *)
 N_auxtype    :0..%7f;	  	(* specify purpose of aux STE *)
 unused2      :0..%ffff;
end; 			  	(* auxiliary type information *)

N_type_rec = record
 case n_type_types of
  N_tf_type : (N_tf : N_tf_rec); 
  N_at_type : (N_at : N_at_rec);
  N_aux_type: (N_aux: N_aux_rec);
end;

N_vf_rec = record 		(* 2 disps for some addr modes *)
 N_disp1 : -32768..32767;
 N_disp2 : -32768..32767;
end;

n_v_types = (N_value_type,N_vf_type);
N_v_rec = record
 case n_v_types of
  N_value_type : (N_value : integer);
  N_vf_type    : (N_vf : N_vf_rec);
end;

nlist = packed record		(* The STE *)
 N_nm : N_nm_rec;
 N_ty : N_type_rec;
 N_v  : N_v_rec;
end;
