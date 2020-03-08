/*
 * @(#)stab.h	1.5	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Complete, untranslated format of a symbol table entry:
 *
 * Note:
 *	Complete symbol table definitions reside in <stab.h> and simplified
 *	definitions in <nlist.h>.  These sets of definitions are not fully
 *	compatible with each other; symbol table information as defined in
 *	<nlist.h> does not correspond directly to a.out file contents, but
 *	instead is obtained by translating such information with nlist(3).
 *	The <nlist.h> definitions do not provide access to all symbol table
 *	information, but are more convenient than the <stab.h> definitions
 *	for what they do cover.
 *
 *	<Stab.h> and <nlist.h> are mutually incompatible;
 *	at most one may legally be included in a compilation.
 *
 * All terminal fields named N_xxx can be accessed simply by n_xxx.
 */
struct nlist {
 union {			/* First doubleword:	*/
   unsigned long N_name;
   struct nmdesc {
     unsigned long
	N_addrmode :  5,	/* addressing mode	*/
	N_unused   :  1,
	N_namestr  : 26;	/* symbol name		*/
   } N_nf;
   long N_ldisp1;		/* only for N_AUX	*/
 } N_nm;
#define n_name		N_nm.N_name
#define n_addrmode	N_nm.N_nf.N_addrmode
#define n_namestr	N_nm.N_nf.N_namestr
#define n_ldisp1	N_nm.N_ldisp1


 union {			/* Second doubleword:		*/
   unsigned long N_type;	/* allows word access to n_type	*/
   struct {
     unsigned long
	N_etype   :  3,	/* type of STE (always)		*/
	N_last    :  1,	/* last STE of lexical scope	*/
	N_ext     :  1,	/* global symbol		*/
	N_more    :  1,	/* next STE is subsidiary	*/
	N_typestr : 26;	/* type name			*/
   } N_tf;		/* basic fields in the n_type word	*/
   struct {
     unsigned long
	N_etype    : 3,	/* see N_tf			*/
	N_last     : 1,
	N_ext      : 1,
	N_more     : 1,
	N_begin    : 1,	/* begin a lexical scope	*/
	N_typeinfo : 1,	/* see N_aux			*/
	N_proc     : 1,	/* procedure entry point	*/
	N_nargs    : 7,	/* number of doublewords of args
			 * popped by function if a type
			 * modifier is M_FUN or N_proc
			 * is on			*/
	N_hllmod1  : 2,	/* first type modifier		*/
	N_hllmod2  : 2,	/* second type modifier		*/
	N_hllptype : 4,	/* primitive type		*/
	N_bfield   : 8;	/* bit field specification	*/
   } N_at;		/* atomic type declaration	*/
   struct {
     unsigned long 
	N_etype       :  3,	/* see N_tf				*/
	N_last        :  1,
	N_ext         :  1,
	N_more        :  1,
	N_initialized :  1,	/* is data segment initialized?		*/
	N_typeinfo    :  1,	/* use N_at format, not N_aux		*/
	N_rom         :  1,	/* is text in ROM?			*/
	N_auxtype     :  7,	/* specify purpose of aux STE		*/
	N_no_ext      :  1,	/* if on, link table must be just
				 * before static data because
				 * there is no external addressing */
	unused2       : 15;
   } N_aux;			/* auxiliary type information		*/
 } N_ty;
#define n_type		N_ty.N_type
#define n_etype		N_ty.N_tf.N_etype
#define n_last		N_ty.N_tf.N_last
#define n_ext		N_ty.N_tf.N_ext
#define n_more		N_ty.N_tf.N_more
#define n_typestr	N_ty.N_tf.N_typestr
#define n_begin		N_ty.N_at.N_begin
#define n_proc		N_ty.N_at.N_proc
#define n_hllmod1	N_ty.N_at.N_hllmod1
#define n_hllmod2	N_ty.N_at.N_hllmod2
#define n_hllptype	N_ty.N_at.N_hllptype
#define n_bfield	N_ty.N_at.N_bfield
#define n_nargs		N_ty.N_at.N_nargs
#define n_typeinfo	N_ty.N_aux.N_typeinfo
#define n_initialized	N_ty.N_aux.N_initialized
#define n_rom		N_ty.N_aux.N_rom
#define n_auxtype	N_ty.N_aux.N_auxtype
#define n_no_ext	N_ty.N_aux.N_no_ext


 union {			/* Third doubleword:	*/
   long N_value;	/* disp for addressing mode	*/
   struct {		/* 2 disps for some addr modes	*/
     short N_disp1,
	   N_disp2;
   } N_vf;
 } N_v;				   /* symbol value	*/
#define n_value		N_v.N_value
#define n_disp1		N_v.N_vf.N_disp1
#define n_disp2		N_v.N_vf.N_disp2
#define n_ldisp2	N_v.N_value
};


/*
 * A macro and masks to access type info and string offsets from
 * n_type or n_name fields.  Using these may be more efficient
 * than accessing via specific fields.
 */
#define N_STRX(x) \
   ((x) / 64)		/* gives the offset into the string table	*/
#define N_AMODE 0x1f	/* mask for addressing mode			*/
#define N_EXT   0x10	/* external bit, or'ed in			*/
#define N_MORE  0x20	/* more bit, or'ed in				*/
#define N_ETYPE 0x07	/* mask for entry type bits			*/

/*
 * addressing modes (in n_addrmode field):
 */
#define A_R0	0x00	/* register 0 - r0				*/
#define A_R0R	0x08	/* register 0 relative - disp(r0)		*/
#define A_R1R	0x09	/* register 1 relative - disp(r1)		*/
#define A_R2R	0x0a	/* register 2 relative - disp(r2)		*/
#define A_R3R	0x0b	/* register 3 relative - disp(r3)		*/
#define A_R4R	0x0c	/* register 4 relative - disp(r4)		*/
#define A_R5R	0x0d	/* register 5 relative - disp(r5)		*/
#define A_R6R	0x0e	/* register 6 relative - disp(r6)		*/
#define A_R7R	0x0f	/* register 7 relative - disp(r7)		*/
#define A_FPR	0x10	/* frame memory relative - disp2(disp1(FP))	*/
#define A_SPR	0x11	/* stack memory relative - disp2(disp1(SP))	*/
#define A_SBR	0x12	/* static memory relative - disp2(disp1(SB))	*/
#define A_IMM	0x14	/* immediate - value				*/
#define A_ABS	0x15	/* absolute - @disp - not relocated		*/
#define A_EX    0x16    /* external - EXTERNAL(disp1)+disp2		*/
#define A_TOS	0x17	/* top of stack - TOS				*/
#define A_FP	0x18	/* frame memory - disp(FP)			*/
#define A_SP	0x19	/* stack memory - disp(SP)			*/
#define A_SB    0x1a    /* static memory - disp(SB)			*/
#define A_PC    0x1b    /* program memory - disp(PC) 
			 * PC at beginning of module			*/

/*
 * There are several kinds of information that ld(1)
 * must recognize that might be in an STE:
 *	NM - n_namestr points to a string.
 *	TN - n_typestr points to the name of a type.
 *	AM - n_addrmode specifies NS16032 addressing mode and
 *	     n_value contains displacement(s) for addressing mode.
 *
 * A main symbol table entry (MSTE) defines a symbol (always has name).
 * Possible values of n_etype field:
 */
#define N_MAT   0  /* (MSTE, NM, AM), uses N_at				*/
#define N_MTN   1  /* (MSTE, NM, AM, TN), symbol is of type specified	*/
#define N_MPTN  2  /* (MSTE, NM, AM, TN), symbol is ptr to type specd	*/
#define N_METN  3  /* (MSTE, NM, AM, TN), symbol uses type specd	*/
#define N_SLINE 4  /* (NM, AM), source line number instead of TN	*/
#define N_SCHAR 5  /* (NM, AM), source character number instead of TN	*/
#define N_MAUX  6  /* (MSTE, NM), auxiliary info - uses N_aux		*/
#define N_AUX   7  /* auxiliary info - uses N_aux			*/
		   /* aux STEs use N_at if n_typeinfo is on		*/

/*
 * masks for accessing atomic type fields from n_type
 */
#define N_BEGIN    0x00000040	/* begin lexical scope			*/
#define N_PROC     0x00000100	/* procedure entry point		*/
				/* three fields specify the high level
				 * language type of a symbol		*/
#define N_NARGS    0x0000fe00	/* number of doublewords of args	*/
#define N_HLLMOD1  0x00030000	/* first type modifier			*/
#define N_HLLMOD2  0x000c0000	/* second type modifier			*/
#define N_HLLPTYPE 0x00f00000	/* primitive type of symbol		*/
#define N_BFIELD   0xff000000	/* bit field specification		*/

/*
 * type modifier values (n_hllmod1 and n_hllmod2 fields)
 */
#define M_NONE	0	/* no modifier		*/
#define M_PTR	1	/* ptr to		*/
#define M_FUN	2	/* function returning	*/
#define M_ARRAY	3	/* array of		*/

/*
 * high level language primitive types (n_hllptype field)
 */
#define T_UNDEF		 0	/* undefined or named type	*/
#define T_INT		 1	/* signed integer		*/
#define T_CHAR		 2	/* character variable		*/
#define T_UINT		 3	/* unsigned integer		*/
#define T_SETE		 4	/* set element			*/
#define T_SET		 5	/* set: defines a scope		*/
#define T_FLOAT		 6	/* 32-bit float			*/
#define T_DFLOAT	 7	/* 64-bit float			*/
#define T_STRUCT	 8	/* structure: defines a scope	*/
#define T_UNION		 9	/* union: defines a scope	*/
#define T_ENUM		10	/* enumeration: defines a scope	*/
#define T_ENUME		11	/* enumeration element		*/
#define T_BOOL		12	/* boolean			*/
#define T_MORE		13	/* more type info is needed	*/

/*
 * masks for accessing auxiliary STE fields from n_type
 */
#define N_TYPEINFO	0x0080	/* use atomic type fields, not
				 * the auxiliary STE fields.
				 * n_auxtype is not valid	*/
#define N_INIT		0x0040	/* data segment is initialized	*/
#define N_ROM		0x0100	/* text segment is in rom	*/
#define N_AUXTYPE	0xfe00	/* type of N_AUX or N_MAUX STE	*/
#define N_NO_EXT	0x010000/* no external addressing-use sb*/

/*
 * values of n_auxtype field
 */
#define N_MODTYPE	0	/* module MSTE				*/
#define N_TXTINFO	1	/* text segment and link table size	*/
#define N_DATINFO	2	/* data segment size			*/
#define N_COMMON	4	/* last MSTE is external for common
				 * n_value gives size in bytes		*/
#define N_LDISP		5	/* uses n_ldisp1 and n_ldisp2		*/
#define N_LIFE		6	/* lifetime of MSTE			*/
#define N_ARBND		7	/* array bounds				*/

/*
 * masks and macro to recognize a module MSTE
 */
#define N_MODMASK	0xfe87
#define N_MOD		0x0006
#define N_MODULE(x)	\
      (((x).n_type & N_MODMASK) == N_MOD)
