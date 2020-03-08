/*
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
 */

/*
 * table.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)table.c	5.1 (NSC) 9/8/83";
# endif

/* jima 5/17/82: fix of exss/inss operand reversal - rules 62 & 63 */
/* jima 5/20/82: bill's cxpd fixes */
/* jima 5/21/82: bill's coml fix (rule 78) */
/* jima 6/7/82:  fixes for unsigned arithmetic; hack for dei/mei; rule 70a */ 
/* --- backup copy made here (table.1.c) --- */
/* jima 6/16/82:  temp aregs now retain real size (short/char) tag;  But all
    register ops uniformly use 32-bit precision, except for *,/, and %, which 
    use extended precision for [long] unsigneds (only) */
/*  jima 6/20/82: made goals which include INAREG always also include INTAREG*/
/*               ASG OPANY put in for all cases.  OPLOG FORCC rewrite chg. */
/*  jima 6/30/82: FORARG goal now handled by zzzcode('W'), thru 114 & 115 */
/*  jima 7/ 5/82: UNARY MINUS rewrite put back - rewrites to 0-<expr> */
/*  jima 7/12/82: ZW usages changed to require addressable branches with regs*/
/* jima 7/16/82: rule 42 fix to rewrite only with r0, not the temp */
/* jima 7/17/82: SACON used in 86e.  Floating point fixups. ZW NASL bugfix. */
/* jima 7/26/82: INIT fixes from Jay; 7/29: Jay's STASG fix */
/* jima 7/30/82: Removed RS nodes, per change in local(). */
/* jima 8/02/82: Right-shifts of signed sign-extend, per Jay's suggestion */
/* jima 8/03/82: ZW templates now 3 sets, only INxTREG demands addressability */
/* jima 8/03/82: COMPL nodes - which were wrong - commented out */
/* jima 8/03/82: Fixed ASSIGNs to return rhs only for temps & constants */
/* jima 8/15/82, 8/17: replaced most cookie strings with ANYCOOK, ZPs added. */
/* JZ   9/8/82: Added ZC escape to cxpd tables in order to generate the proper
	adjspb instruction */
/* JZ   9/8/82:  Added NASL  for cxpd calls to allow r0 or f0 to be
	shared with the return value */
/* jima 9/13/82: INSSBUG versions of inss templates */
/* JZ 9/24/82:  Changed 20 to generate adjspb w or d depending on its operands
	size. */
/* JZ 9/24/82:  Changed 81s and 81u They were reversing the order of dividend
	and remainder for unsigned integers.*/
/* JZ 9/26/82: Changed 20a to allow its argument to be SNAME .  Since all the
	temporary registers get trashed we cannot allow anything to be in a
	temporary register for large structures.  This should be allright as
	a structure assignment in an expression cannot return anything except
	a structure.  However I wonder what happens in conditional expressions.
	I am not sure sucomp is correct for structures and I wonder what
	happens if you are assigning a structure that is a member of an
	array. Here you might need temporary registers to store indexes.
	I think we might need a seperate template for large structures. since
	they require all 3 temporary registers while the other STASG states do
	not.  Sucomp might be wrong here also since it does not appear to
	check the size.*/
/* JZ 9/28/82: Changed above back temporarily for this delta since more needs to
	be done on it. */
/* JZ 9/28/82: Changed 86d to generate addr con(rx) instead of addr @con(rx) ,
	which is syntactically incorrect.*/
/* JZ 9/28/82: Changed 20 -22a To temporarily fix structure assignment problems.
	Changes which allow a shared register may not be correct.
	The cookies had to be expanded since the next cook routine no
	longer changes the cookie around in order to get things to possibly
	match. Therefore INAREG and INTAREG were not matching, causing
	multiple assignments to die.  All the code here is hopelessly
	kludged from the vax 11 and pdp 11 code.  The entire structure
	assignment needs to be rewritten to match the 16k s more symnetrical
	instruction set.  Eventually it should be rewritten to be handled
	basically the same as normal assignments.
	All these changes are from Jima.  */
/*JZ 10/11/82: changed templates 81 to eliminate multiplication from unsigned
	operations since it can be done correctly by the normal
	multiplication routines.*/
/*JZ 10/19/82: moved the new rewrite cookies 89a-2 and 89a-1 to come before
	the other rewrite rules so they will match first and not cause
	everything to be thrown around willy nill */
/*JZ 10/19/82: Changed the resource returned by mod in 81 to be RLEFT
	instead of RESC2 since those are the same resource and the
	compiler will try and reclaim RLEFT otherwise*/
/*JZ 10/19/82: Changed the type of 89-1and2a to be TANYUNSIGNED so it will
	rewrite all the left sides of any unsigned div or mod into a
	register, eventually char needs to be sign extended
	into a register to match the left side.*/
/* jima 10/21/82: deleted INTAREG ZW entries for binops */
/*JZ 10/25/82: Changed template 105r by removing the T so the size of
	registers will no longer be preserved since an arbritrary
	operation can increase the size of the register up to double
	word size. Also changed 82 and 86b,d and e 87b and c 105tfor
	the same reason.
	Removed the PRL from the right side of 105r since
	the operation is a double operation any way and double
	operations are done on temporary registers by definition.
	However if reg and char variables are allowed this should
	be changed to match them.  Perhaps through matching shorter
	types also.
	Also removed the T from 105t since the result can be greater
	than 32 bits.*/
/*JZ 10/25/82: removed the +NASL|NAREG needs for the 81 unsigned div/mod
		templates since it seems to cause the wrong resource to
		be assigned.  It should still work fine since the left
		side must allready be in a register anyway. 
		NOTE The code can be further optimised by matching the
		2 cases where the left operand is in memory and is 2X the
		size of the right.  */
/*JZ 10/25/82:  Added templates for integer operands for the 81 unsigned
		templates.  These will eventually have to be special
		cased in the table as they are presently taking up
		too many templates*/
/*JZ+Jima:   Changed templates 98 and 100 so that divides will not be done
		shorter precision than their longest operand since that is
		incorrect. Added 2 templates for that. */
/*JZ 11/1/82: Added templates that were missing for shorter integers in
		UNSIGNED mod/div.  Also remembered that the meaning of
		the 89-1and 89-2 templates has changed since the rewrite rule
		automatically emits a quo or rem instruction in the case where
		it is a real equals op.*/
/*JZ 11/1/82: Added templates 50a and 50b in front of old 50a and 50b
		These templates do a lshd or ashd when the left side is
		STAREG.  This is because we want all operations into
		temporary registers to affect all 32 bits since that is
		the way c is defined.  Also removed the T for these
		since a shift does not always preserve the register
		size.*/
/*JZ 11/1/82: Reversed the rewritten resources in 81 templates since
		the operands were being put in the wrong registers.
		By shifting the mustdo's the resources will automatically
		be switched so that the resources must also be switched
		to keep the same registers.*/
/*JZ 11/1/82: Changed 100b to be OB instead of OW,  it was causing the
		wrong operation to be done*/
/*JZ 11/19/82: Added template 49c for allowing fast LS's for temporary 
		registers */
/*JZ 11/23/82: Changed template for extsd to allow the extsd to share a
		register with the left side since that is the only side
		likely to have one to share since FLD is a unary op.*/
/*JZ 12/14/82: Changed template for Assign extsd to reverse the order of
	       the 2 operands. It was assigning the left to the right which of
	       course screwed everything up. */
/* JZ 3/03/83:  Removed template 86d since it was doing addr r1[r2:b],r2
		in a vain attempt to do a three address add. */
/* JZ 3/16/83:  Switch 114g and 114h so that it will match floating point first
		so that if the left side is floating point it will put it into
		a floating register instead of a regular register.*/
/* JZ 3/16/83:  114g and 114h had their left and right sides reversed.  Since
		a unary op is matched against the tables with the left side
		being its real left side and the right side being the op itself.
		It is necessary for the right side to be the type the REGISTER
		will be, since it is the type of the op itself,  however the
		shape must match anything since its real shape is an operator.
		The left side must match its real left child which could be
		anything that can be in memory.*/
/* JZ 3/16/83:  Changed 114b to only match FIXED point so that floating point
		operands will be placed in a floating point register.  Any 
		floating point that needs to go into a fixed register will
		have an SCONV node to convert them so this node doesn't have
		to worry about that.*/
/* JZ 3/17/83:  Changed ASG OPFLOAT templates to match temporary registers as
		well as regular B registers since setexpr now changes all ops
		to assignment ops.
		Changed OPUNARY 114g and h to match all cases except allready
		being in the temp register of the type wanted, since this is
		also used by SCONV.*/
/* JZ 5/18/83:  Added a new SCONV 2c that will convert from double to float
		while keeping it in double format. */
/* JZ 6/8/83:   Fixed SCONV 2c to check to make sure its operand is addressable
		before matching it.*/
/* JZ 6/8/83:   Fixed floating point ops to only use registers as double, never
		float.  We will never have float sized temporary registers.*/
/* JZ 6/16/83:  Fixed 68a to accept SBREG so double reg++; will work.  */
# include "mfile2"

# define WPTR TPTRTO|TINT|TLONG|TFLOAT|TDOUBLE|TPOINT|TUNSIGNED|TULONG
# define AWD SNAME|SOREG|SCON|STARNM|STARREG
/* tbl */
# define TANYSIGNED TPOINT|TINT|TLONG|TSHORT|TCHAR
# define TANYUNSIGNED TUNSIGNED|TULONG|TUSHORT|TUCHAR
# define TANYFIXED TANYSIGNED|TANYUNSIGNED
# define TANYWORD TINT|TUNSIGNED|TPOINT|TLONG|TULONG
# define NIAWD SNAME|SCON|STARNM
/* tbl */

struct optab  table[] = {

PCONV,	INAREG|INTAREG,
	SAREG|AWD,	TCHAR|TSHORT,
	SANY,		TPOINT,
	NAREG|NASL,	RESC1,
	"	movxZLd	AL,A1	;1\n",

PCONV,	INAREG|INTAREG,
	SAREG|AWD,	TUCHAR|TUSHORT,
	SANY,		TPOINT,
	NAREG|NASL,	RESC1,
	"	movzZLd	AL,A1	;2\n",

	/* the following entry is to fix a problem with
	   the manner that the first pass handles the
	   type of a shift expression                 */
PCONV,	INAREG|INTAREG,
	SAREG|AWD,	TINT|TUNSIGNED,
	SANY,		TPOINT,
	NAREG|NASL,	RLEFT,
	";2a\n",

SCONV,	INTBREG,
	SBREG,	TDOUBLE,
	SANY,	TDOUBLE,
	0,	RLEFT,
	";2b\n",
/* Added to convert from double to float*/
/* unlike fixed point which can be changed in size by simply reclassifying */
/* the size of the memory location, the different sizes of floating point */
/* have different formats so they must be physically converted through a */
/* register.*/
SCONV,	ANYCOOK,
	AWD|SAREG|STAREG|STBREG,	TDOUBLE,
	SANY,	TFLOAT,
	NBREG|NBSL,	RESC1,
	"\tmovlf	AL,A1\n\
	movfl	A1,A1	;2c\n",

#ifdef FORT
\*SCONV,	INTBREG,
\*	SAREG|AWD,	TANYSIGNED|TUNSIGNED|TULONG,
\*	SANY,		TFLOAT,
\*	NBREG,		RESC1,
\*	"	movZLf	AL,TA1	;3\n",
\*
\*SCONV,	INTBREG,
\*	SBREG|AWD,	TFLOAT,
\*	SANY,		TFLOAT,
\*	NBREG,		RESC1,
\*	"	movZLf	AL,TA1	;4\n",
\*
\*SCONV,	INTBREG,
\*	SAREG|AWD,	TUCHAR|TUSHORT,
\*	SANY,		TFLOAT,
\*	NAREG|NASL|NBREG|NBSR,	RESC2,
\*	"	movzZLd	AL,A1	;5\n	movdf	A1,TA2\n",
 */
#endif

/* conversions involving floating point... */

/* should not be needed with code in setasg() and ZW */
/* SCONV,	INTBREG,
/* 	SAREG|AWD,	TANYSIGNED|TUNSIGNED|TULONG,
/* 	SANY,		TFLOAT|TDOUBLE,
/* 	NBREG,		RESC1,
/* 	"	movZLl	AL,A1	;6\n",
/* 
/* SCONV,	INTBREG,
/* 	SBREG|AWD,	TFLOAT,
/* 	SANY,		TFLOAT|TDOUBLE,
/* 	NBREG,		RESC1,
/* 	"	movZLl	AL,A1	;7\n",
/* 
/* SCONV,	INTBREG,
/* 	SAREG|AWD,	TUCHAR|TUSHORT,
/* 	SANY,		TFLOAT|TDOUBLE,
/* 	NAREG|NBREG,	RESC2,
/* 	"	movzZLd	AL,A1	;8\n	movdl	A1,A2\n",
/* 
/* SCONV,	INTAREG,
/* 	SBREG|AWD,	TFLOAT|TDOUBLE,
/* 	SANY,		TANYFIXED,
/* 	NAREG|NASL,	RESC1,
/* 	"	movZLZF	AL,A1	;9\n",
 */

/* Scalar down-conversions or changes between signed and unsigned are 
   handled entirely by the SCONV rewrite rule, which generally just rewrites 
   the source datum node to appear to be the desired type.

   For down-conversion of registers, however, the REG node is replaced with
   a fake NAME (with the register name as the symbol), because REGs are
   always treated as if all 32 bits are valid.

   Usually an extending move results when the converted datum is subsequently
   used.   (jima) */

/* - obsolete - see SCONV rewrite rule
/*SCONV,	INTAREG,		
/*	SAREG,	TANY,
/*	SANY,	TANYUNSIGNED,
/*	NAREG|NASL,	RESC1,
/*	"	movzZRd	AL,TA1	;10\n",	/* 'T' means keep smaller type */
/*
/*SCONV,	INTAREG,		
/*	SAREG,	TANY,
/*	SANY,	TANYSIGNED,
/*	NAREG|NASL,	RESC1,
/*	"	movxZRd	AL,TA1	;11\n",
/*
/*SCONV,	INTAREG,
/*	SNAME|SCON|STARNM,	TANY,
/*	SANY,		TANYUNSIGNED,
/*	NAREG|NASL,	RESC1,
/*	"	movzZRd	AL,TA1	;10x\n",
/*
/*SCONV,	INTAREG,
/*	SSOREG,	TANY,
/*	SANY,		TANYUNSIGNED,
/*	NAREG|NASL,	RESC1,
/*	"	movzZRd	AL,TA1	;11x\n",
/*
/*SCONV,	INTAREG,
/*	SNAME|SCON|STARNM,	TANY,
/*	SANY,	TANYSIGNED,
/*	NAREG|NASL,	RESC1,
/*	"	movxZRd	AL,TA1	;12x\n",
/*
/*SCONV,	INTAREG,
/*	SSOREG,	TANY,
/*	SANY,	TANYSIGNED,
/*	NAREG|NASL,	RESC1,
/*	"	movxZRd	AL,TA1	;13\n",
 */


INIT,	FOREFF,
	SCON,	TANY,
	SANY,	TANYWORD,
	0,	RNOP,
	"	.double	CL\t; 14\n",


INIT,	FOREFF,
	SCON,	TANY,
	SANY,	TSHORT|TUSHORT,
	0,	RNOP,
	"	.word	CL\t; 15\n",

INIT,	FOREFF,
	SCON,	TANY,
	SANY,	TCHAR|TUCHAR,
	0,	RNOP,
	"	.byte	CL\t; 16\n",

INIT,	FOREFF,
	/*SNAME,	TANY,*/
	SANY,	TANY,   /* changed to match U & nodes which come */
	SANY,	TANYWORD,
	0,	RNOP,
	"ZV \t;16a\n",  /*was 14b */

	/* for the use of fortran only */

GOTO,	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
	0,	RNOP,
	"	br	CL	;17\n",

GOTO,	FOREFF,
	AWD,	TANY,
	SANY,	TANY,
	0,	RNOP,
	"	jump	AL	;18\n",

GOTO,	FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
	0,	RNOP,
	"	jump	AL	;19\n",

STARG,	FORARG,
	SCON|SOREG,	TANY,
	SANY,	TANY,
	NTEMP+2*NAREG,	RESC3,
	"ZS;19a\n",

STASG,	FORARG,
	SNAME|SOREG,	TANY,
	SCON|SAREG,	TANY,
	0,	RNULL,
	"	adjspZT	;20\nZS\n",

/* Structure assignments always get the ADDRESS of a structure on the RHS,
   and the destination sturcture itself on the LHS.  If necessary, the
   first pass puts in U& NAME, which is evaluated into a register (with
   addr x,R) before matching any STASG templates.  Someday this should be
   changed to avoid this wastefulness. (jima) */
   
   
/* INTAREG is (now)included to handle nested assignments and evaluation
   into a temporary (as required to implement the MUSTDOs for returns,
   and as specified in rallo() to help the movs instruction) (jima) */
STASG,	FOREFF,
	SNAME|SOREG,	TANY,
	SCON|SAREG,	TANY,  /* used to include SNAME - bug! */
	0,	RNOP,
	"ZS;20a\n",

/* The following seems to be from the vax, which supports relocatable
/* ICONs.  The movd after the assignement resulted (presumably) in
/* movd $foobar,Rx...  (jima)
/*STASG,	INAREG,
/*	SNAME|SOREG,	TANY,
/*	SCON,	TANY,
/*	NAREG,	RESC1,
/*	"ZS	movd	AR,A1	;21\n",
 */

STASG,	INTAREG,  /* INTAREG added - jima */
	SNAME|SOREG,	TANY,
	SAREG,	TANY,
	NAREG|NASR,	RESC1,
"	movd	AR,tos	;22a\n\
ZS\n\
	movd	tos,A1\n",

STASG,	INAREG,
	SNAME|SOREG,	TANY,
	SAREG,	TANY,
	0, 	RRIGHT,
"	movd	AR,tos	;22b\n\
ZS\n\
	movd	tos,AR\n",

STASG,	INTAREG,  /* INTAREG added - jima */
	SNAME|SOREG,	TANY,
	SAREG,	TANY,
	NAREG|NASR,	RESC1,  /* was 0,RRIGHT (jima) */
"	movd	AR,tos	;22a\n\
ZS\n\
	movd	tos,A1\n",

FLD,	INAREG|INTAREG,
	SANY,	TANY,
	SFLD,	TANYFIXED,
	NAREG|NASR|NASL,	RESC1,
	"	extsd	AR,A1T,H,S	;23\n",

FLD,	FORARG,
	SANY,	TANY,
	SFLD,	TANYFIXED,
	0,	RNULL,
	"	extsd	AR,tos,H,S	;24\n",

/* CBRANCH nodes, used for if-then-else, etc., are caught initially 	*/
/* inside order() in the CBRANCH "rewrite" rule.  cbranch() is called	*/
/* to really do the work.  In this case, 0-1 values are never actually 	*/
/* generated, but just "short-circuit" branching to the ultimate 	*/
/* destinations.  All other uses of OPLOGs (==,<,||,&&,etc.) are to 	*/
/* generate a 0-1 value, either directly into memory or a register.  In */
/* that case, the 16000 S instruction is used.			(jima)	*/

/* First the small-constant optimizations, then the others */
OPLOG,	FORCC,
	SQICK,		TANY,
	AWD|SAREG,	TANYFIXED,
	0,	RESCC,
	"	cmpqZR	AL,AR	;35\n",

OPLOG,	FORCC,
	SCCON,		TANY,		/* CHAR-sized constant */
	SAREG|AWD,	TCHAR|TUCHAR,
	0,	RESCC,
	"	cmpb	PLRAL,AR	;36\n",

OPLOG,	FORCC,
	SSCON,		TANY, 		/* SHORT-sized constant */
	SAREG|AWD,	TSHORT|TUSHORT,
	0,	RESCC,
	"	cmpw	PLRAL,AR	;37a1\n",

OPLOG,	FORCC,
	SACON,		TANY, 		/* small-displacement addr con */
	SAREG|AWD,	TANYWORD,
	NAREG,	RESCC,
"	addr	@CL,A1\n\
	cmpd	A1,AR	;37a2\n",

OPLOG,	FORCC,
	SSCON,		TANY,
	SAREG,	TSHORT|TUSHORT|TCHAR|TUCHAR,  /* all regs have 32 bits valid */
	0,	RESCC,
	"	cmpw	PLRAL,AR	;37b\n",

OPLOG,	FORCC,
	AWD|SAREG,	TANYWORD,
	AWD|SAREG,	TANYWORD,
	0,	RESCC,
	"	cmpd	AL,AR	;38\n",

OPLOG,	FORCC,
	AWD|SAREG,	TSHORT|TUSHORT,
	AWD|SAREG,	TSHORT|TUSHORT,
	0,	RESCC,
	"	cmpw	PLRAL,PRLAR	;39a\n",

OPLOG,	FORCC,
	AWD|SAREG,	TCHAR|TUCHAR,
	AWD|SAREG,	TCHAR|TUCHAR,
	0,	RESCC,
	"	cmpb	PLRAL,PRLAR	;39b\n",

/* Registers always have all 32 bits valid, so can compare things with 	*/
/* relatively smaller registers using the larger-sized compare.        	*/

/* Note: mycanon() puts registers preferentially on the LHS, so only   	*/
/*       that case is provided for here.				*/
OPLOG,	FORCC,
	SAREG,		TSHORT|TUSHORT|TCHAR|TUCHAR,
	AWD|SAREG,	TSHORT|TUSHORT,
	0,	RESCC,
	"	cmpw	AL,PRLAR	;39c\n",

OPLOG,	FORCC,
	SAREG,		TANY,
	AWD|SAREG,	TANYWORD,
	0,	RESCC,
	"	cmpd	AL,AR	;39f\n",

/* can always compare registers with 32-bit precision, regardless of "sizes" */
OPLOG,	FORCC,
	SAREG,	TANYFIXED,
	SAREG,	TANYFIXED,
	0,	RESCC,
	"	cmpd	AL,AR	;39e\n",

/* If the above don't match, the shorter operand will be put into a 
   temporary register, so it will match the above. */

OPLOG,	FORCC,
	SBREG|AWD,	TDOUBLE,
	SBREG|AWD,	TDOUBLE,
	0,	RESCC,
	"	cmpl	AL,AR	;40\n",

OPLOG,	FORCC,
	SBREG|AWD,	TDOUBLE,
	AWD,		TFLOAT,
	NBREG,		RESCC,
	"	movfl	AR,A1	;41\n	cmpl	AL,A1\n",

OPLOG,	FORCC,
	AWD,		TFLOAT,
	SBREG|AWD,	TDOUBLE,
	NBREG,		RESCC,
	"	movfl	AL,A1	;42\n	cmpl	A1,AR\n",

OPLOG,	FORCC,
	AWD,	TFLOAT,
	AWD,	TFLOAT,
	0,	RESCC,
	"	cmpf	AL,AR	;43\n",

UNARY CALL,	INTAREG,
	SCON,	TANY,
	SANY,	TANYWORD|TCHAR|TUCHAR|TSHORT|TUSHORT,
	NAREG,	RESC1, /* should be register 0 */
	"	cxp	CL	;44a\nZC",

UNARY CALL,	INTAREG,
	AWD,	TANY, /* descriptor in memory - NOT in a register */
	SANY,	TANYWORD|TCHAR|TUCHAR|TSHORT|TUSHORT,
	NAREG|NASL,	RESC1,	/* should always be r0 (marked MUSTDO r0) */
	"	cxpd	AL	; 44b\nZC",

UNARY CALL,	INTBREG,
	SCON,	TANY,
	SANY,	TFLOAT|TDOUBLE,
	NBREG,	RESC1, /* should be f0 */
	"	cxp	CL	;45a\nZC",

UNARY CALL,	INTBREG,
	AWD,	TANY, /* descriptor in memory - NOT in a register */
	SANY,	TFLOAT|TDOUBLE,
	NBREG|NBSL,	RESC1,	/* should always be f0 (marked MUSTDO f0) */
	"	cxpd	AL	; 45b\nZC",

/* getcall() forces register function ptrs INTEMP before doing the call, */
/* to accomodate the cxpd quirk (the "addr" operand mode).               */

/*UNARY CALL,	INTAREG,
/*	SAREG,	TANY,
/*	SANY,	TANYWORD|TCHAR|TUCHAR|TSHORT|TUSHORT,
/*	(NAREG|NASL)+NTEMP,	RESC1,	/* resource No.1 should be r0 */
/*"	movd	AL,A2\n\
/*	cxpd	A2	; 46\n",
/* */

/*
 * Right shifts are converted to left shifts in the first pass (with negated
 *  shift count) - see local().		  		(jima)
 */

/*ASG RS,	ANYCOOK,	/* signed right shift */
/*	SAREG|AWD,	TANYSIGNED,
/*	SAREG|AWD,	TANYFIXED,
/*	0,	RLEFT,
/*	"	ashZL	AR,TAL	;48\n",
/*
/*ASG RS,	ANYCOOK,	/* unsigned right shift */
/*	SAREG|AWD,	TANYUNSIGNED,
/*	SAREG|AWD,	TANYFIXED,
/*	0,	RLEFT,
/*	"	lshZL	AR,TAL	;49\n",
 */

/* Signed operands are arithmetically shifted, while unsigneds are shifted
   with zero-fill.  This is not strictly required by the C language (it is
   only necesary that unsigneds zero-fill). 			(jima) */

/* LS is converted to assignment-op */
/*LS,	INTAREG,
/*	SAREG,	TANYWORD,
/*	SSHFT,	TANY,
/*	NASL|NAREG,	RESC1,
/*	"	addr	@0[AL:Z:],TA1	;fast ls 	;49a\n",
 */
/* This template will need to be changed when TRUST is redefined since
   this will destroy the upper bits in register variables if they aren't LONG*/
ASG LS,	ANYCOOK,
	SAREG,	TANYWORD,
	SSHFT,	TANY,
	0,	RLEFT,
	"	addr	@0[AL:Z:],AL	;fast AL <<= CR ;49b\n",

ASG LS,	ANYCOOK,
	STAREG,	TANY, /* any temporary register size is allright*/
	SSHFT,	TANY,
	0,	RLEFT,
	"	addr	@0[AL:Z:],AL	;fast AL <<= CR ;49c\n",

ASG LS,	ANYCOOK,
	STAREG,	TANYUNSIGNED,
	SAREG|AWD,	TANYFIXED,
	0,	RLEFT,
	"	lshd	AR,AL	;50a\n",

ASG LS,	ANYCOOK,
	STAREG,	TANYSIGNED,
	SAREG|AWD,	TANYFIXED,
	0,	RLEFT,
	"	ashd	AR,AL	;50b\n",

ASG LS,	ANYCOOK,
	SAREG|AWD,	TANYUNSIGNED,
	SAREG|AWD,	TANYFIXED,
	0,	RLEFT,
	"	lshZL	AR,TAL	;50c\n",

ASG LS,	ANYCOOK,
	SAREG|AWD,	TANYSIGNED,
	SAREG|AWD,	TANYFIXED,
	0,	RLEFT,
	"	ashZL	AR,TAL	;50d\n",

/* INCR & DECR done in rewrite rule, which rewrites as +=, preceeded by
   putting the operand into a temp reg if not FOREFF   (jima) */
/*INCR,	FOREFF,
/*	SAREG|AWD,	TANY,
/*	SCON,	TANY,
/*	0,	RLEFT,
/*	"	ZE	T;51\n",
/*
/*DECR,	FOREFF,
/*	SAREG|AWD,	TANY,
/*	SCON,	TANY,
/*	0,	RLEFT,
/*	"	ZE	T;52\n",
/*
/* /* Rules 51 & 52 used to have SAREG only for left side.  Rules 53 & 54
/* *  should no longer be needed using SAREG|AWD.
/* *INCR,	FOREFF,
/* *	SAREG,	TANYWORD,
/* *	SCON,	TANY,
/* *	0,	RLEFT,
/* *	"	ZE	T;53\n",
/* *
/* *DECR,	FOREFF,
/* *	SAREG,	TANYWORD,
/* *	SCON,	TANY,
/* *	0,	RLEFT,
/* *	"	ZE	T;54\n",
/* */
/*
/* /* jwf INCR and DECR for SAREG TCHAR|TSHORT matched by ASG PLUS etc */
/*
/* Rules 55 through 58 are removed experimentally.  They were used to
/* place something into a reg before incrementing it - meaning auto
/*  post-incr/decr.
/*  
/*INCR,	INAREG|INTAREG,
/*	AWD,	TANY,
/*	SCON,	TANY,
/*	NAREG,	RESC1,
/*	"	ZD	T;55\n",
/*
/*DECR,	INAREG|INTAREG,
/*	AWD,	TANY,
/*	SCON,	TANY,
/*	NAREG,	RESC1,
/*	"	ZD	T;56\n",
/*
/*INCR,	INAREG|INTAREG,
/*	SAREG,	TANYWORD,
/*	SCON,	TANY,
/*	NAREG,	RESC1,
/*	"	ZD	T;57\n",
/*
/*DECR,	INAREG|INTAREG,
/*	SAREG,	TANYWORD,
/*	SCON,	TANY,
/*	NAREG,	RESC1,
/*	"	ZD	T;58a\n",
 */

/* zzzcode('A') does assignments between various memory & register locations.
   The rewrite rule returns the left or right (preferably a register), in case
   the value is needed for some other purpose, such as a surrounding 
   expression.  Assignement to registers always fill all 32 bits.

   NOTE: Presently the RHS is returned only if it is a temp
   register or constant.  It is unclear whether cases like 
   "X = (A=B) / (B++)" are defined in C, but they are forced left-to-right
   by never allowing "B" to be returned as the value of (A=B) here.
   
   The goals include FORARG|INTEMP|INTAREG|INTBREG to accomodate imbedded
   assignments in such cases (the final match call in order will put the 
   result where desired).  This is also important because setasg()
   assumes that the tree is too complicated, but does not know anything
   about cookies.  The zzzcode(W) templates do not match ASSIGNs. (jima) */

ASSIGN,	INAREG|INTAREG|INBREG|INTBREG|AWD|FORCC|FORARG|INTEMP|FOREFF,
	SAREG|SBREG|AWD,	TANYFIXED|TFLOAT|TDOUBLE,
	STAREG|STBREG|SCON,	TANYFIXED|TFLOAT|TDOUBLE,
	0,	RLEFT|RRIGHT, /* can return rhs if temp or constant*/
	"PRLZA	T;59a\n",

ASSIGN,	INAREG|INTAREG|INBREG|INTBREG|AWD|FORCC|FORARG|INTEMP|FOREFF,
	SAREG|SBREG|AWD,	TANYFIXED|TFLOAT|TDOUBLE,
	SAREG|SBREG|AWD,	TANYFIXED|TFLOAT|TDOUBLE,
	0,	RLEFT,		/* be safe - return lhs only */
	"PRLZA	T;59b\n",

/* CCODES nodes are evaluated with special code in zzzcode(A) */
ASSIGN,	INAREG|INTAREG|INBREG|INTBREG|AWD|FORCC|FORARG|INTEMP|FOREFF,
	SAREG|AWD,	TANYFIXED,
	SCC,		TANY,  /* SCC means CCODES */
	0,	RLEFT,
	"PRLZA	T;59d\n",

/* obsolete...
/*ASSIGN,	INAREG|INTAREG|FORARG|FOREFF|INTEMP,
/*	SAREG|AWD,	TANYFIXED,
/*	SAREG|AWD,	TANYFIXED,
/*	0,	RLEFT,
/*	"ZA	T;59x\n",
/*
/*ASSIGN,	INBREG|FOREFF,
/*	SBREG|AWD,	TDOUBLE|TFLOAT,
/*	SAREG|AWD,	TANYFIXED,
/*	0,	RLEFT,
/*	"ZA	;60\n",
/*
/*ASSIGN,	INAREG|INTAREG|FOREFF,
/*	SAREG|AWD,	TANYFIXED,
/*	SBREG|AWD,	TDOUBLE|TFLOAT,
/*	0,	RLEFT,
/*	"ZA	;60a\n",
/*
 */

# ifdef INSSBUG
ASSIGN,	INAREG|INTAREG|INBREG|INTBREG|AWD|FORCC|FORARG|INTEMP|FOREFF,
	SFLD,	TANY,
	STAREG,	TANY,
	NAREG,	RRIGHT, 	/* return rhs if temp reg */
"	movd	TAL,A1		; **temp to avoid inss bug and rmw**\n\
	inssZR	AR,A1,H,S	;62a\n\
	movd	A1,TAL\n",

ASSIGN,	SCON,
	SFLD,	TANY,
	SCON,	TANY,
	NAREG,	RRIGHT, 	/* return rhs if constant is acceptable */
"	movd	TAL,A1		; **temp to avoid inss bug and rmw**\n\
	inssZR	AR,A1,H,S	;62b\n\
	movd	A1,TAL\n",

ASSIGN,	INAREG|INTAREG|INBREG|INTBREG|AWD|FORCC|FORARG|INTEMP|FOREFF,
	SFLD,	TANY,
	SAREG|AWD,	TANY,
	NAREG,	RLEFT,		/* be safe - return lhs here */
"	movd	TAL,A1		; **temp to avoid inss bug and rmw**\n\
	inssZR	AR,A1,H,S	;62c\n\
	movd	A1,TAL\n",
# endif
# ifndef INSSBUG

ASSIGN,	INAREG|INTAREG|INBREG|INTBREG|AWD|FORCC|FORARG|INTEMP|FOREFF,
	SFLD,	TANY,
	STAREG,	TANY,
	0,	RRIGHT, 	/* return rhs if temp reg */
        "	inssZR	AR,TAL,H,S	;62a\n",

ASSIGN,	SCON,
	SFLD,	TANY,
	SCON,	TANY,
	0,	RRIGHT, 	/* return rhs if constant is acceptable */
        "	inssZR	AR,TAL,H,S	;62b\n",

ASSIGN,	INAREG|INTAREG|INBREG|INTBREG|AWD|FORCC|FORARG|INTEMP|FOREFF,
	SFLD,	TANY,
	SAREG|AWD,	TANY,
	0,	RLEFT,		/* be safe - return lhs here */
        "	inssZR	AR,TAL,H,S	;62c\n",
#endif

ASSIGN,	INAREG|INTAREG|INBREG|INTBREG|AWD|FORCC|FORARG|INTEMP|FOREFF,
	SAREG|AWD,	TANYFIXED,
	SFLD,		TANYFIXED,
	0,	RLEFT,
	"	extsZL	TAR,AL,H,S	;63\n",  /* jima 5/17/82 */

/* dummy UNARY MUL entry to get U* to possibly match OPLTYPE */
/*  (because searching starts with 1st entry with matching op - see match() */
/* ...I don't think this is true any more; see setrew(), which uses the 
      regular matching algorithm... (jima) */
/*UNARY MUL,	FOREFF,
/*	SCC,	TANY,
/*	SCC,	TANY,
/*	0,	RNULL,
/*	"!HELP! 64\n",
 */
/*REG,	INTEMP,
/*	SANY,	TANY,
/*	SBREG,	TDOUBLE,
/*	2*NTEMP,	RESC1,
/*	"	movl	AR,A1	;65\n",
/*
/*REG,	INTEMP,
/*	SANY,	TANY,
/*	SAREG,	TANY,
/*	NTEMP,	RESC1,
/*	"	movZF	AR,A1	;66\n",
 */

#ifdef FORT
/*REG,	FORARG,
/*	SANY,	TANY,
/*	SBREG,	TFLOAT,
/*	0,	RNULL,
/*	"	movfl	AR,tos	;67\n",
/*
/*REG,	FORARG,
/*	SANY,	TANY,
/*	SBREG,	TDOUBLE,
/*	0,	RNULL,
/*	"	movZR	AR,tos	;68\n",
 */
#endif

OPLEAF,	FOREFF,
	SANY,		TANY,
	SAREG|SBREG|AWD,	TANY,
	0,		RLEFT,
	";68a\n",	/* nothing to do (e.g., with comma operator) */

/* obsoleted by rewrite to ASSIGN and zzzcode(W)   (jima)
/*OPLTYPE,INBREG|INTBREG,
/*	SBREG|AWD,	TFLOAT|TDOUBLE,
/*	SBREG|AWD,	TFLOAT|TDOUBLE,
/*	NBREG|NBSR,	RESC1,
/*	"ZA	;69\n",
 */

/*OPLTYPE,INAREG|INTAREG,        /* Added to avoid casting unsigneds to 32bits */
/*	SANY,	TUSHORT|TUCHAR,/*  when placing in registers, to to avoid    */
/*	SANY,	TUSHORT|TUCHAR,/*  using dei/mei except for ulongs(=unsigned)*/
/*	NAREG|NASR,	RESC1, /* (6/7/82 jima)                              */
/*	"TZA	;70a\n",
/*
/*OPLTYPE,INAREG|INTAREG,
/*	SANY,	TANYSIGNED|TUNSIGNED|TULONG,  /* was: SANY,	TANYFIXED */
/*	SANY,	TANYSIGNED|TUNSIGNED|TULONG,
/*	NAREG|NASR,	RESC1,
/*	"ZA	;70b\n",
/* */

/* This should be unnecessary with the default rule for OPANY,INTAREG (114)
*OPLTYPE,INAREG|INTAREG,
*	SANY,	TANYFIXED,
*	SANY,	TANYFIXED,
*	NAREG,	RESC1,
*	"ZA	T;70\n",
*/

/* should be obsoleted by setasg() rewrite actions &/or NAME rewrite (jima)
/*OPLTYPE,FORCC,   		/* NOTE: A rewrite rule should be added */
/*	SANY,	TANYFIXED,	/* which detects quick-sized ICONS on the */
/*	SANY,	TANYFIXED,	/* LHS of OPLOGs, and switches the branches */
/*	0,	RESCC,		/* while reversing the sense of the OPLOG */
/*				/* AND, do away with OPLTYPE,FORCC completely,*/
/*				/* generating explicit comparisons with 0 */
/*				/* when necessary.  (jima) */
/*	"	cmpZR	AR,0	;71\n",  
/*
/*OPLTYPE,	FORARG,
/*	SANY,	TANY,
/*	SANY,	TANYWORD,
/*	0,	RNULL,
/*	"	ZG	;71a\n",
/*
/*OPLTYPE,	FORARG,
/*	SANY,	TANY,
/*	SANY,	TCHAR|TSHORT,
/*	0,	RNULL,
/*	"	movxZRd	AR,tos	;72\n",
/*
/*OPLTYPE,	FORARG,
/*	SANY,	TANY,
/*	SANY,	TUCHAR|TUSHORT,
/*	0,	RNULL,
/*	"	movzZRd	AR,tos	;73\n",
/*
/*OPLTYPE,	FORARG,
/*	SANY,	TANY,
/*	SANY,	TDOUBLE,
/*	0,	RNULL,
/*	"	movl	AR,tos	;74\n",
/*
/*OPLTYPE,	FORARG,
/*	SANY,	TANY,
/*	SANY,	TFLOAT,
/*	0,	RNULL,
/*	"	movfl	AR,tos	;75\n",
*/

/* unary minus and complement have code generated in setasg() -
   via emit_uop (jima) */
/*UNARY MINUS,	INTAREG,
/*	SAREG,	TANYFIXED,
/*	SANY,	TANY,
/*	NAREG|NASL,	RESC1,
/*	"	negd	AL,TA1	;76a\n",	/* regs ALWAYS use 32 bits */
/*
/*UNARY MINUS,	INTAREG,
/*	AWD,	TANYWORD,	/* shorts & chars are 1st forced into regs  */
/*	SANY,	TANY,           /*  (so all 32 register bits will be valid) */
/*	NAREG|NASL,	RESC1,
/*	"	negd	AL,TA1	;76b\n",
/*
/*COMPL,	INTAREG,
/*	SAREG,	TANYFIXED,
/*	SANY,	TANY,
/*	NAREG|NASL,	RESC1,
/*	"	comd	AL,TA1	;77a\n",	/* regs ALWAYS use 32 bits */
/*
/*COMPL,	INTAREG,
/*	AWD,	TANYWORD,	/* shorts & chars are 1st forced into regs  */
/*	SANY,	TANY,           /*  (so all 32 register bits will be valid) */
/*	NAREG|NASL,	RESC1,
/*	"	comd	AL,TA1	;77b\n",
 */

/* Here is where it becomes useful to have the real sizes of register datums
 *  available.  For operations other than /, and %,  use of registers is
 *  always with 32-bit precision, regardless of the size associated with the 
 *  registers.  The C language generally wants all intermediate results to be 
 *  done with int-sized (32-bit) precision.
 *
 * But the line is drawn for unsigned /, and %, which require use of the 
 *  dei instruction for correct implementation with 32 bit precision.
 */ 
 
/* Special entries for 32-bit unsigned /, and %:          

   The LHS is required to be in a temp register, which will be r1 for the
   dei instruction (rallo puts in MUSTDO|R1). 

   NOTE: Since the operands are unsigned, the upper register bits are 
   always ZERO; since the result of DIV and MOD can never be larger than the 
   size of the LHS, byte and word operations can be done on the LHS register.
   When the operands are signed they must first be sign extended into a
   register and the they must do a deid operation.
   
   In the following, "in REG" means the RHS must be in a register to match
   any template (a rewrite rule puts it there if necessary):

   LHS type	RHS type	required dei operation precision
   ---------	----------	--------------------------------
   ULONG	ULONG		d
   ULONG	USHORT in REG	d
   ULONG	UCHAR in REG	d
   LONG		ULONG		d
   LONG		USHORT in REG	d
   LONG		UCHAR in REG	d
   ULONG	LONG		d
   ULONG	SHORT in REG	d
   ULONG	CHAR in REG	d

   USHORT	ULONG		d (N.b. LHS is in a 32-bit register)
   USHORT	USHORT		w
   USHORT	UCHAR in REG	w or d (w is faster)
   SHORT	ULONG		d (N.b. LHS is in a 32-bit register)
   SHORT	USHORT		d
   SHORT	UCHAR in REG	d 
   USHORT	LONG		d (N.b. LHS is in a 32-bit register)
   USHORT	SHORT in REG	d
   USHORT	CHAR in REG	d 

   UCHAR	ULONG		d (N.b. LHS is in a 32-bit register)
   UCHAR	USHORT		w
   UCHAR	UCHAR 		b
   CHAR		ULONG		d (N.b. LHS is in a 32-bit register)
   CHAR		USHORT		d
   CHAR		UCHAR 		d
   UCHAR	LONG		d (N.b. LHS is in a 32-bit register)
   UCHAR	SHORT in REG	d
   UCHAR	CHAR in REG	d
   Preserve the size of registers since the answer cannot be larger
   than the LHS.
*/

ASG DIV,	ANYCOOK,
	STAREG,	TUNSIGNED|TULONG|TUSHORT|TUCHAR, /* always in r1 */
	SAREG|AWD,	TUNSIGNED|TULONG|TINT|TLONG,
	NAREG,			RESC1,
"	movqd	0,A1	;should be r1\n\
	deid	AR,TAL	;81s div result is r1\n",

ASG DIV,	ANYCOOK,
	STAREG,	TANYFIXED, /* always in r1 */
	SAREG|AWD,	TUNSIGNED|TULONG,
	NAREG,			RESC1,
"	movqd	0,A1	;should be r1\n\
	deid	AR,TAL	;81s1 div result is r1\n",

ASG MOD,	ANYCOOK,
	STAREG,	TUNSIGNED|TULONG|TUSHORT|TUCHAR, /* always in r1 */
	SAREG|AWD,	TINT|TLONG|TUNSIGNED|TULONG,
	NAREG,	RLEFT,
" 	movqd	0,A1	;should be r1\n\
	deid	AR,TAL	;81t mod result is r0\n",

ASG MOD,	ANYCOOK,
	STAREG,	TANYFIXED, /* always in r1 */
	SAREG|AWD,	TUNSIGNED|TULONG,
	NAREG,	RLEFT,
" 	movqd	0,A1	;should be r1\n\
	deid	AR,TAL	;81t1 mod result is r0\n",

ASG DIV,	ANYCOOK,
	STAREG,	TUSHORT|TUCHAR, /* always in r1 */
	SAREG|AWD,	TUSHORT,
	NAREG,			RESC1,
"	movqd	0,A1	;should be r1\n\
	deiw	AR,TAL	;81u div result is r1\n",


ASG MOD,	ANYCOOK,
	STAREG,	TUSHORT|TUCHAR, /* always in r1 */
	SAREG|AWD,	TUSHORT,
	NAREG,	RLEFT,
" 	movqd	0,A1	;should be r1\n\
	deiw	AR,TAL	;81v the mod is in r0\n",


ASG DIV,	ANYCOOK,
	STAREG,	TUNSIGNED|TULONG, /* always in r1 */
	SAREG,	TUSHORT|TUCHAR|TSHORT|TCHAR,
	NAREG,			RESC1,
"	movqd	0,A1	;should be r0\n\
	deid	AR,TAL	;81w div result is r1\n",

ASG DIV,	ANYCOOK,
	STAREG,	TINT|TLONG|TULONG|TUNSIGNED|TCHAR|TSHORT, /* always in r1 */
	SAREG,	TUSHORT|TUCHAR,
	NAREG,			RESC1,
"	movqd	0,A1	;should be r0\n\
	deid	AR,TAL	;81w1 div result is r1\n",


ASG MOD,	ANYCOOK,
	STAREG,	TUNSIGNED|TULONG|TINT|TLONG|TCHAR|TSHORT, /* always in r1 */
	SAREG,	TUSHORT|TUCHAR,
	NAREG,	RLEFT,
" 	movqd	0,A1	;should be r0\n\
	deid	AR,TAL	;81x the mod is in r0\n",

ASG MOD,	ANYCOOK,
	STAREG,	TUNSIGNED|TULONG, /* always in r1 */
	SAREG,	TUSHORT|TUCHAR|TCHAR|TSHORT,
	NAREG,	RLEFT,
" 	movqd	0,A1	;should be r0\n\
	deid	AR,TAL	;81x1 the mod is in r0\n",

ASG DIV,	ANYCOOK,
	STAREG,	TUSHORT,/* always in r1 */
	SAREG,	TUCHAR,
	NAREG,			RESC1, /* result is r1 */
"	movqd	0,A1	;should be r0\n\
	deiw	AR,TAL	;(w ok for unsigned;div result is r1)81y\n",


ASG MOD,	ANYCOOK,
	STAREG,	TUSHORT, /* always in r1 */
	SAREG,	TUCHAR,
	NAREG,	RLEFT,
" 	movqd	0,A1	;should be r0\n\
	deiw	AR,TAL	;(w ok for unsigned;mod result is r0)81z\n",

ASG DIV,	ANYCOOK,
	STAREG,		TUCHAR, /* always in r1 */
	SAREG|AWD,	TUCHAR,
	NAREG,			RESC1,
"	movqd	0,A1	;should be r0\n\
	deib	AR,TAL	;81zz div result is r1\n",

ASG MOD,	ANYCOOK,
	STAREG,		TUCHAR, /* always in r1 */
	SAREG|AWD,	TUCHAR,
	NAREG,	RLEFT,
" 	movqd	0,A1	;should be r0\n\
	deib	AR,TAL	;81zz1 mod result is r0\n",

ASG DIV,	ANYCOOK,
	STAREG,		TUSHORT|TUCHAR, /* always in r1 */
	SAREG,	TSHORT|TCHAR,
	NAREG,			RESC1,
"	movqd	0,A1	;should be r0\n\
	deid	AR,TAL	;81zzz div result is r1\n",

ASG MOD,	ANYCOOK,
	STAREG,		TUSHORT|TUCHAR, /* always in r1 */
	SAREG,	TSHORT|TCHAR,
	NAREG,	RLEFT,
" 	movqd	0,A1	;should be r0\n\
	deid	AR,TAL	;81zzz1 mod result is r0\n",

ASG PLUS,	ANYCOOK,
	SAREG,	TANYFIXED,	/* registers ALWAYS hold 32 bits */
	SQICK,	TANYFIXED,
	0,	RLEFT,
	"	addqd	AR,AL	;82\n",

ASG PLUS,	ANYCOOK,
	AWD,	TANYFIXED,  /* ...but memory can be any size */
	SQICK,	TANYFIXED,
	0,	RLEFT,
	"	addqZL	AR,TAL	;83\n",

ASG PLUS,	FORADDR|INTAREG|INTBREG|FORCC|FOREFF, /*any reason*/
	SANY,	TANY,
	SZERO,	TANY,
	0,	RLEFT,
	";86a (add zero)\n",

ASG PLUS,	ANYCOOK,
	SAREG,	TANYFIXED,
	SQICK,	TANYFIXED,   /* was: TINT|TLONG, (jima)*/
	0,	RLEFT,
	"	addqd	AR,AL	;86b\n",	/* 32-bit registers */

ASG PLUS,	ANYCOOK,
	AWD,	TANYFIXED,
	SQICK,	TANYFIXED,
	0,	RLEFT,
	"	addqZL	AR,TAL	;86c\n",


/* NOTE: The following should be removed and replaced with a more general
   scheme in which oreg2() recognizes REG+constant and forms an OREG.
   Further, rule 86 could then match an SSOREG (singly-indexed offset from
   register) on the right, allowing things like LHS = Rx + 123 + Ry to
   be done with "addr 123(Rx)[Ry:b],LHS"     			(jima) */
#ifdef garbage			/* The following is a sub optimisation */
ASG PLUS,	ANYCOOK,
	SAREG,	TANYWORD,
	SAREG,	TANYWORD,
	0,	RLEFT,
	"	addr	(AR)[AL:b],AL	;86d  AL += AR\n",
#endif

/* Use the "addr" instruction is possible for constants because it encodes */
/* them as a "displacement" - which is variable-sized.			   */
ASG PLUS,	ANYCOOK,
	SAREG,	TANYFIXED,
	SACON,	TANYFIXED,
	0,	RLEFT,
	"	addr	CR(AL),AL	;86e  AL += CR\n",

ASG MINUS,	ANYCOOK,
	SANY,	TANY,
	SZERO,	TANY,
	0,	RLEFT,
	";87a (subtract zero)\n",
	
ASG MINUS,	ANYCOOK,
	SAREG,	TANYFIXED,
	SQICK,	TANYFIXED,
	0,	RLEFT,
	"	addqd	-AR,AL	;87b\n",	/* 32-bit*/

ASG MINUS,	ANYCOOK,
	AWD,	TANYFIXED,
	SQICK,	TANYFIXED,
	0,	RLEFT,
	"	addqZL	-AR,AL	;87c\n",

/* |=  with mixed-size operands can be done in the smaller precision because 
   the upper bits wouldn't change even with extended precision.  (jima) */

ASG OR,	ANYCOOK,
	SAREG|AWD,	TUCHAR,
	SAREG|AWD,	TANYUNSIGNED,
	0,	RLEFT,
	"	OB	PRLAR,TAL	;88d\n",

ASG OR,	ANYCOOK,
	SAREG|AWD,	TANYUNSIGNED,
	SAREG|AWD,	TUCHAR,
	0,	RLEFT,
	"	OB	PRLAR,TAL	;88e\n",

ASG OR,	ANYCOOK,
	SAREG|AWD,	TUSHORT,
	SAREG|AWD,	TUSHORT|TUNSIGNED|TULONG,
	0,	RLEFT,
	"	OW	PRLAR,TAL	;88f\n",

ASG OR,	ANYCOOK,
	SAREG|AWD,	TSHORT|TUSHORT|TANYWORD,
	SAREG|AWD,	TSHORT|TUSHORT,
	0,	RLEFT,
	"	OW	PRLAR,TAL	;88g\n",

/* The left side needs to be put in a temporary register here */
/* This now matches any unsigned since that is easier to do*/
/* dei is quicker than quo and the only problem is an extra
   register is used which isn't too bad hopefully*/
/* Char still needs to be fixed up properly*/
ASG OPDIV,	ANYCOOK,
	SAREG|AWD,	TANYUNSIGNED,
	SANY,	TANY,
	REWRITE,BITYPE,
	"unsigned rw 89a-2",

/* This will match either a asg op created by setasg for unsigned division or
   a real assignment op that needs to do signed division.  In that case the
   rewrite rule will simply emit the div or quo instruction and return.*/
ASG OPDIV,	ANYCOOK,
	SAREG|AWD,	TANY,
	SANY,	TANYUNSIGNED,
	REWRITE,BITYPE,
	"unsigned rw 89a-1",

/* Unsigned shorts and chars must be extended into registers before multiply
   divide or mod operations, because these must be done in longer precision
   (e.g., 32-bits) to preserve the "sign" bit.   The following rewrite
   templates catch such cases and force the operands into registers. */

ASG OPDIV,	ANYCOOK,
	AWD,	TUSHORT|TUCHAR,
	SANY,	TANY,
	REWRITE,BITYPE,
	"unsigned rw 89a",

ASG OPDIV,	ANYCOOK,
	SANY,	TANY,
	AWD,	TUSHORT|TUCHAR,
	REWRITE,BITYPE,
	"unsigned rw 89b",

/*ASG MUL,	ANYCOOK,
*	AWD,	TUSHORT|TUCHAR,
*	SANY,	TANY,
*	REWRITE,BITYPE,
*	"unsigned rw 90a",
* Unsigned multiplication is equivalent to signed multiplication
*ASG MUL,	ANYCOOK,
*	SANY,	TANY,
*	AWD,	TUSHORT|TUCHAR,
*	REWRITE,BITYPE,
*	"unsigned rw 90b", */
/* Here is where non-quick assignment operators are done for other than /%*   */

/* Assignment operations into memory can be byte or word precision.
 * If the result is going to be truncated, and the operation does not
 * depend on the high operand bits for correct computation, the operation
 * can just be done in the smaller size.  N.b. DIV and MOD require the
 * higher bits! Example: ( (char)3 /= 0x101 ) is zero, and the operation
 * must be done in larger precision to be correct.  (jima)
 */
ASG OPANY,	ANYCOOK,
	AWD,	TSHORT|TUSHORT,
	SAREG|AWD,	TSHORT|TUSHORT,
	0,	RLEFT,
	"	OW	PRLAR,TAL	;98a\n",

ASG OPANY,	ANYCOOK,
	AWD,	TCHAR|TUCHAR,
	SAREG|AWD,	TCHAR|TUCHAR,
	0,	RLEFT,
	"	OB	PRLAR,TAL	;99\n",

ASG OPSIMP,	ANYCOOK, /*  PLUS,MINUS,AND,OR,EOR  */
	AWD,	TSHORT|TUSHORT,
	SAREG|AWD,	TANYWORD, /* truncated! */
	0,	RLEFT,
	"	OW	PRLAR,TAL	;100a\n",

ASG OPSIMP,	ANYCOOK, /*  PLUS,MINUS,AND,OR,EOR  */
	AWD,	TCHAR|TUCHAR,
	SAREG|AWD,	TSHORT|TUSHORT|TANYWORD, /* truncated! */
	0,	RLEFT,
	"	OB	PRLAR,TAL	;100b\n",

/* If none of the shorter forms apply, do 32-bit precision:   */

ASG OPANY,	ANYCOOK,
	AWD,	TANYWORD,
	AWD,	TANYWORD,
	0,	RLEFT,
	"	OD	PRLAR,AL	;105q\n",

ASG OPANY,	ANYCOOK,
	SAREG,	TANYFIXED,
	AWD,	TANYWORD,
	0,	RLEFT,
	"	OD	AR,AL	;105r\n",  /* 32-bit registers */

ASG OPANY,	ANYCOOK,
	AWD,	TANYWORD,
	SAREG,	TANYFIXED,
	0,	RLEFT,
	"	OD	AR,AL	;105s\n",  /* 32-bit registers */

ASG OPANY,	ANYCOOK,
	SAREG,	TANYFIXED,
	SAREG,	TANYFIXED,
	0,	RLEFT,
	"	OD	AR,AL	;105t\n",  /* 32-bit registers */


ASG OPFLOAT,INTBREG|INBREG|FOREFF|FORCC,
	STBREG|SBREG|AWD,	TDOUBLE,
	STBREG|SBREG|AWD,	TDOUBLE,
	0,	RLEFT,
	"	OL	AR,AL	;107\n",
/*  This template should probably be removed since floating should be done
    double precision */

ASG OPFLOAT,INBREG|FOREFF|FORCC,
	AWD,	TFLOAT,
	AWD,	TFLOAT,
	0,	RLEFT,
	"	OF	AR,AL	;108\n",

ASG OPFLOAT,INTBREG|INBREG|FOREFF|FORCC,
	SBREG|AWD,	TDOUBLE,
	SBREG|AWD,	TFLOAT,
	NBREG|NBSR,	RLEFT,
	"	movfl	AR,A1	;109\n	OL	A1,AL\n",

ASG OPFLOAT,INBREG|INTBREG|FOREFF|FORCC,
	SBREG|AWD,	TFLOAT,
	SBREG|AWD,	TDOUBLE,
	NBREG,	RLEFT|RESC1,
	"	movfl	AL,A1	;110\n	OL	AR,A1\n	movlf	A1,AL\n",
/*  The following templates are probably no longer used*/
OPFLOAT,INBREG|INTBREG,
	STBREG,		TDOUBLE,
	STBREG|SBREG|AWD,TDOUBLE,
	0,	RLEFT,
	"	OL	AR,AL	;111\n",


OPFLOAT,INBREG|INTBREG,
	SBREG|AWD,	TFLOAT,
	SBREG|AWD,	TDOUBLE,
	NBREG|NBSL,	RESC1,
	"	movfl	AL,A1	;112\n	OL	AR,A1\n",

/* OPFLOAT,INBREG|INTBREG,
	STBREG,		TFLOAT,
	STBREG|SBREG|AWD,TFLOAT,
	0,	RLEFT,
	"	OF	TAR,AL	;113\n",*/

/* Default actions for hard trees ... */


OPLTYPE,	FORCC, 		/* rewrites as 0!=<leaf> */
	SANY,	TANY,
	SANY,	TANY,
	REWRITE,	CCODES,
	"rw 113d",

/* Default rules when goal is to put into a temp regs, mem, or tos      */
/* FORARG.  For temp regs, the object must be addressable, so that any  */
/* function calls will be completed (temp regs must not be busy then).  */
/*  zzzcode('W') constructs and explicit assignment and evaluates it by */
/*  a recursive call to call to order().       				*/
/*									*/
/* The "rewrite" specificaion is RNOP, which does nothing (all the work */
/* is completed in zzzcode).					(jima)	*/

/* Note: These get used only when the tree is simplified as much as     */
/*       possible.  Any ASGops have usually already been done.          */

/*** TEMP REGISTERS: ***/

/* Special match for addresses - even addresses of floats go into AREGs */
UNARY AND,	INTAREG,	
	AWD,	TANY,
	SANY,	TANY,
	NAREG|NASL,	RNOP,
	"\t;& AL intareg 114a...\nZW",
 
/* Allow sharing of register with REGs and OREGs, but not with op branches */
OPLEAF,	INTAREG,	
	AWD|SAREG,	TANYFIXED, /* NOT FLOATING POINT*/
	SANY,		TANY,
	NAREG|NASL,	RNOP,
	"\t;AL intareg(nasl) 114b...\nZW",
 
OPLEAF,	INTBREG,	
	AWD|SBREG,	TFLOAT|TDOUBLE,
	SANY,		TANY,
	NBREG|NBSL,	RNOP,
	"\t;AL intbreg(nbsl) 114c...\nZW",
 
 
/* OPUNARY cases handle U- and ~and U* and U& and SCONV.
   The type of the operator   appears to  be the correct type of the operand,
   so it is still correct for 
   zzzcode(W) to make the temp reg have the type of the "node" for (U- or
   ~or U&). Howerver for U* the register should be assigned at the
   bottom level first so you can do register sharing which is required
   by sucomp. The recursion will be done by setexpr however. JZ*/
OPUNARY,	INTBREG,	/* Any item to B-reg or tos */
	STAREG|SAREG|SBREG|AWD,		TANY,
	SANY,		TFLOAT|TDOUBLE, /* RIGHT side is the op!!!*/
	NBREG,	RNOP,
	"\t;intbreg 114g...\nZW",

OPUNARY,	INTAREG,	/* any item to A-reg or tos */
	SBREG|STBREG|SAREG|AWD,		TANY,
	SANY,		TANYFIXED, /* RIGHT side is the op!!*/
	NAREG|NASL,	RNOP,
	"\t;intareg 114h...\nZW",



/* INCR or DECR do not allow register sharing because the LHS will be used
   immediately afterwards (see setexpr) */
INCR,	INTAREG,
	SAREG|AWD,	TANYFIXED,
	SCON,		TANY,
	NAREG,		RNOP,
	"\t;intareg 114i...\nZW",

DECR,	INTAREG,
	SAREG|AWD,	TANYFIXED,
	SCON,		TANY,
	NAREG, 		RNOP,
	"\t;intareg 114j...\nZW",

/* zzzcode'A', which does ASSIGNs, is where CCODES nodes are finally	*/ 
/* evaluated into a 0 or 1, using a movq 1/branch/movq 0/label sequence,*/
/* or an S instruction.   				jima		*/
CCODES,	INTAREG,
	SANY,	TANY,
	SANY,	TANY,
	NAREG,	RNOP,
	"\t;ccodes,intareg 114q...\nZW",

/*** FUNCTION ARGUMENTS: ***/

UNARY AND,	FORARG,	
	SANY,	TANY,
	SANY,	TANY,
	0,	RNOP,
	"\t;unary and,forarg 115a...\nZW",
 
/* Non-leafs okay, so that setasg can use the "tos" on the LHS as an
   accumulator (jima) */
OPANY,	FORARG,	/* fixed-point item to A-reg or tos */
	SANY,	TANYFIXED,  /* SANYs here allow matching unary or leaf */
	SANY,	TANYFIXED,
	0,	RNOP,
	"\t;forarg 115d...\nZW",

OPANY,	FORARG,	
	SANY,	TFLOAT|TDOUBLE,
	SANY,	TANY,			/* mixed? */
	0,	RNOP,
	"\t;forarg 115e...\nZW",
 
OPANY,	FORARG,	/* to fp reg or tos */
	SANY,	TANY,			/* mixed? */
	SANY,	TFLOAT|TDOUBLE,
	0,	RNOP,
	"\t;intemp 115f...\nZW",

INCR,	FORARG,
	SANY,	TANYFIXED,
	SCON,	TANY,
	0,	RNOP,
	"\t;forarg 115i...\nZW",

DECR,	FORARG,
	SANY,	TANYFIXED,
	SCON,	TANY,
	0,	RNOP,
	"\t;forarg 115j...\nZW",

/*** TEMPORARY MEMORY: ***/

OPANY,	INTEMP,	/* fixed-point item to A-reg or tos */
	SANY,	TANYFIXED,  /* SANYs here allow matching unary or leaf */
	SANY,	TANYFIXED,
	NTEMP,	RNOP,
	"\t;intemp 116a...\nZW",

OPANY,	INTEMP,	
	SANY,	TFLOAT|TDOUBLE,
	SANY,	TANY,			/* mixed? */
	2*NTEMP,RNOP,
	"\t;intemp 116e...\nZW",
 
OPANY,	INTEMP,	/* to fp reg or tos */
	SANY,	TANY,			/* mixed? */
	SANY,	TFLOAT|TDOUBLE,
	2*NTEMP,	RNOP,
	"\t;intemp 116f...\nZW",

INCR,	INTEMP,
	SANY,	TANYFIXED,
	SCON,	TANY,
	0,	RNOP,
	"\t;intemp 116i...\nZW",

DECR,	INTEMP,
	SANY,	TANYFIXED,
	SCON,	TANY,
	0,	RNOP,
	"\t;intemp 116j...\nZW",

/*added 11/15 JZ: This is supposed to cause the compiler to fall out of its
 *recursion so it can match what should be a STARNM probably. */
/*UNARY MUL,	INTAREG|INTBREG,
	SANY,	TPOINT,
	SANY,	TPOINT,
	0,	RNOP,
	"\t;starnm 117\nZW",*/


# define DF(rew_case) FORREW,SANY,TANY,SANY,TANY,REWRITE,rew_case,"rw rew_case"

UNARY MUL, DF( UNARY MUL ),

INCR,	FOREFF,		/* rewrite as +=.  Other cookies handled by setexpr */
	SANY,	TANY,	/*		   (which evaluates INTAREG, etc.)  */
	SANY,	TANY,   
	REWRITE,	INCR,
	"",

DECR,	FOREFF,
	SANY,	TANY,
	SANY,	TANY,
	REWRITE,	INCR,
	"",

/* The ASSIGN rewrite rule calls setasg(), which handles cases where 
   knowledge of the LHS is needed at the time the operator on the RHS is
   handled (frequently, storing a value directly into the left without
   a temporary register). */
ASSIGN, DF(ASSIGN),

STASG, DF(STASG),

FLD, DF(FLD),

/* Down-conversions of memory by re-classification */
SCONV,	ANYCOOK, 
	AWD|SAREG,	TANYFIXED,
	SANY,	TSHORT|TUSHORT|TCHAR|TUCHAR,
	REWRITE,	SCONV,
	"rwSCONV",

/* The "BITYPE" rewrite rule calls setexpr(), which handles most kinds of
   things.  Ops are converted to ASGops here. */
ASG OPANY, DF(BITYPE),
OPANY, 	   DF(BITYPE),
INCR, 	   DF(BITYPE), 
DECR, 	   DF(BITYPE),

INIT, DF(INIT),		/* diagnoses an error (invalid initialization) */

FREE,	FREE,	
	FREE,	FREE,	
	FREE,	FREE,	
	FREE,	FREE,	
	"!help!;999\n"
};
