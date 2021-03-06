/*
 * Copyright (C) 1983 by National Semiconductor Corporation
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

/* static char sccsid[] = "@(#)c22.c	2.56" */

#include "c2.h"
#define readonly

readonly struct optab optab[] = {
"br",BR,
"beq",T(CBR,JEQ),
"bne",T(CBR,JNE),
"ble",T(CBR,JLE),
"bge",T(CBR,JGE),
"blt",T(CBR,JLT),
"bgt",T(CBR,JGT),
"bls",T(CBR,JLOS),
"bhs",T(CBR,JHIS),
"blo",T(CBR,JLO),
"bhi",T(CBR,JHI),
"bcc",T(CBR,JBCC),
"bcs",T(CBR,JBCS),
"bfc",T(CBR,JBFC),
"bfs",T(CBR,JBFS),

"acbb",T(ACB,BYTE),
"acbw",T(ACB,WORD),
"acbd",T(ACB,LONG),

"absb",T(ABS,BYTE),
"absd",T(ABS,LONG),
"absw",T(ABS,WORD),
"adjspb",T(ADJSP,BYTE),
"adjspd",T(ADJSP,LONG),
"adjspw",T(ADJSP,WORD),
"comb",T(COM,BYTE),
"comd",T(COM,LONG),
"comw",T(COM,WORD),
"negb",T(NEG,BYTE),
"negl",T(NEG,DOUBLE),
"negf",T(NEG,FLOAT),
"negd",T(NEG,LONG),
"negw",T(NEG,WORD),
"notb",T(NOT,BYTE),
"notw",T(NOT,WORD),
"notd",T(NOT,LONG),

"addb",T(ADD,U(BYTE,OP2)),
"addw",T(ADD,U(WORD,OP2)),
"addd",T(ADD,U(LONG,OP2)),
"addl",T(ADD,U(DOUBLE,OP2)),
"addf",T(ADD,U(FLOAT,OP2)),
"addcb",T(ADDC,U(BYTE,OP2)),
"addcw",T(ADDC,U(WORD,OP2)),
"addcd",T(ADDC,U(LONG,OP2)),
"addpb",T(ADDP,U(BYTE,OP2)),
"addpw",T(ADDP,U(WORD,OP2)),
"addpd",T(ADDP,U(LONG,OP2)),
"addqb",T(ADDQ,U(BYTE,OP2)),  /* converted back to addq by output() */
"addqw",T(ADDQ,U(WORD,OP2)),
"addqd",T(ADDQ,U(LONG,OP2)),
"subl",T(SUB,U(DOUBLE,OP2)),
"subf",T(SUB,U(FLOAT,OP2)),
"subb",T(SUB,U(BYTE,OP2)),
"subw",T(SUB,U(WORD,OP2)),
"subd",T(SUB,U(LONG,OP2)),
"subcb",T(SUBC,U(BYTE,OP2)),
"subcw",T(SUBC,U(WORD,OP2)),
"subcd",T(SUBC,U(LONG,OP2)),
"subpb",T(SUBP,U(BYTE,OP2)),
"subpw",T(SUBP,U(WORD,OP2)),
"subpd",T(SUBP,U(LONG,OP2)),
"xorb",T(XOR,U(BYTE,OP2)),
"xord",T(XOR,U(LONG,OP2)),
"xorw",T(XOR,U(WORD,OP2)),
"orb",T(OR,U(BYTE,OP2)),
"orw",T(OR,U(WORD,OP2)),
"ord",T(OR,U(LONG,OP2)),
"andb",T(AND,U(BYTE,OP2)),
"andw",T(AND,U(WORD,OP2)),
"andd",T(AND,U(LONG,OP2)),
"ashb",T(ASH,U(BYTE,OP2)),
"ashw",T(ASH,U(WORD,OP2)),
"ashd",T(ASH,U(LONG,OP2)),
"lshb",T(LSH,U(BYTE,OP2)),
"lshw",T(LSH,U(WORD,OP2)),
"lshd",T(LSH,U(LONG,OP2)),
"rotb",T(ROT,U(BYTE,OP2)),
"rotw",T(ROT,U(WORD,OP2)),
"rotd",T(ROT,U(LONG,OP2)),
"mulf",T(MUL,U(FLOAT,OP2)),
"mull",T(MUL,U(DOUBLE,OP2)),
"mulb",T(MUL,U(BYTE,OP2)),
"mulw",T(MUL,U(WORD,OP2)),
"muld",T(MUL,U(LONG,OP2)),
"quob",T(QUO,U(BYTE,OP2)),
"quow",T(QUO,U(WORD,OP2)),
"quod",T(QUO,U(LONG,OP2)),
"remb",T(REM,U(BYTE,OP2)),
"remw",T(REM,U(WORD,OP2)),
"remd",T(REM,U(LONG,OP2)),
"rem",COMMENT,  /* special remark which is passed through */
"divb",T(DIV,U(BYTE,OP2)),
"divw",T(DIV,U(WORD,OP2)),
"divd",T(DIV,U(LONG,OP2)),
"divf",T(DIV,U(FLOAT,OP2)),
"divl",T(DIV,U(DOUBLE,OP2)),
"modb",T(MOD,U(BYTE,OP2)),
"modw",T(MOD,U(WORD,OP2)),
"modd",T(MOD,U(LONG,OP2)),
"meib",T(MEI,BYTE),
"meiw",T(MEI,WORD),
"meid",T(MEI,LONG),
"deib",T(DEI,BYTE),
"deiw",T(DEI,WORD),
"deid",T(DEI,LONG),
"bicb",T(BIC,U(BYTE,OP2)),
"bicd",T(BIC,U(LONG,OP2)),
"bicw",T(BIC,U(WORD,OP2)),
"ffsb",T(FFS,U(BYTE,OP2)),
"ffsd",T(FFS,U(LONG,OP2)),
"ffsw",T(FFS,U(WORD,OP2)),

"cbitb",T(CBIT,BYTE),
"cbitd",T(CBIT,LONG),
"cbitw",T(CBIT,WORD),
"sbitb",T(SBIT,BYTE),
"sbitd",T(SBIT,LONG),
"sbitw",T(SBIT,WORD),
"tbitb",T(TBIT,BYTE),
"tbitd",T(TBIT,LONG),
"tbitw",T(TBIT,WORD),
"cxp",CXP,
"cxpd",CXPD,
"bsr",BSR,
"jsr",JSR,
"caseb",T(CASE,BYTE),
"cased",T(CASE,LONG),
"casew",T(CASE,WORD),
"checkb",T(CHECK,BYTE),
"checkw",T(CHECK,WORD),
"checkd",T(CHECK,LONG),
"indexb",T(INDEX,BYTE),
"indexw",T(INDEX,WORD),
"indexd",T(INDEX,LONG),
"cmpl",T(CMP,DOUBLE),
"cmpf",T(CMP,FLOAT),
"cmpb",T(CMP,BYTE),
"cmpd",T(CMP,LONG),
"cmpw",T(CMP,WORD),
"cmpqb",T(CMPQ,BYTE),  /* converted back to cmpq at output */
"cmpqd",T(CMPQ,LONG),
"cmpqw",T(CMPQ,WORD),
/* All data-type conversions among integer and floating types are encoded
   as CVT.  Unsigned extensions are encoded as MOVZ.  */
"movbl",T(CVT,U(BYTE,DOUBLE)),
"movbf",T(CVT,U(BYTE,FLOAT)),
"movxbd",T(CVT,U(BYTE,LONG)),
"movxbw",T(CVT,U(BYTE,WORD)),
"movwl",T(CVT,U(WORD,DOUBLE)),
"movwf",T(CVT,U(WORD,FLOAT)),
"movwb",T(CVT,U(WORD,BYTE)),
"movxwd",T(CVT,U(WORD,LONG)),
"movdl",T(CVT,U(LONG,DOUBLE)),
"movdf",T(CVT,U(LONG,FLOAT)),
"movdw",T(CVT,U(LONG,WORD)),
"movdb",T(CVT,U(LONG,BYTE)),
"movfl",T(CVT,U(FLOAT,DOUBLE)),
"truncfb",T(CVT,U(FLOAT,BYTE)),
"truncfd",T(CVT,U(FLOAT,LONG)),
"truncfw",T(CVT,U(FLOAT,WORD)),
"movlf",T(CVT,U(DOUBLE,FLOAT)),
"trunclb",T(CVT,U(DOUBLE,BYTE)),
"truncld",T(CVT,U(DOUBLE,LONG)),
"trunclw",T(CVT,U(DOUBLE,WORD)),

"extsb",T(EXTS,BYTE), /* NOTE: Operand order not same as on vax */
"extsw",T(EXTS,WORD),
"extsd",T(EXTS,LONG),
"extb",T(EXT,BYTE),
"extw",T(EXT,WORD),
"extd",T(EXT,LONG),
"inssb",T(INSS,BYTE),
"inssw",T(INSS,WORD),
"inssd",T(INSS,LONG),
"insb",T(INS,BYTE),
"insw",T(INS,WORD),
"insd",T(INS,LONG),
"ibitb",T(IBIT,BYTE),
"ibitw",T(IBIT,WORD),
"ibitd",T(IBIT,LONG),
"jump",JMP,
"addr",T(ADDR,LONG),  /* Always gives 32-bit result */
"lxpd",T(LXPD,LONG),  /* Always gives 32-bit result */
"movb",T(MOV,BYTE),
"movw",T(MOV,WORD),
"movd",T(MOV,LONG),
"movl",T(MOV,DOUBLE),
"movf",T(MOV,FLOAT),
"movqb",T(MOVQ,BYTE),  /* converted to MOV during internal processing */
"movqw",T(MOVQ,WORD),
"movqd",T(MOVQ,LONG),
"movzbd",T(MOVZ,U(BYTE,LONG)),
"movzbw",T(MOVZ,U(BYTE,WORD)),
"movzwd",T(MOVZ,U(WORD,LONG)),
"movmb",T(MOVM,BYTE),
"movmw",T(MOVM,WORD),
"movmd",T(MOVM,LONG),
"cmpmb",T(CMPM,BYTE),
"cmpmw",T(CMPM,WORD),
"cmpmd",T(CMPM,LONG),
"movsub",T(MOVSU,BYTE),
"movsuw",T(MOVSU,WORD),
"movsud",T(MOVSU,LONG),
"movusb",T(MOVUS,BYTE),
"movusw",T(MOVUS,WORD),
"movusd",T(MOVUS,LONG),
"movsb",T(MOVS,BYTE),
"movsw",T(MOVS,WORD),
"movsd",T(MOVS,LONG),
"movsbt",T(MOVST,BYTE),
"movswt",T(MOVST,WORD),
"movsdt",T(MOVST,LONG),
"cmpsb",T(CMPS,BYTE),
"cmpsw",T(CMPS,WORD),
"cmpsd",T(CMPS,LONG),
"cmpsbt",T(CMPST,BYTE),
"cmpswt",T(CMPST,WORD),
"cmpsdt",T(CMPST,LONG),
"skpsb",T(SKPS,BYTE),
"skpsw",T(SKPS,WORD),
"skpsd",T(SKPS,LONG),
"skpsbt",T(SKPST,BYTE),
"skpswt",T(SKPST,WORD),
"skpsdt",T(SKPST,LONG),
"setcfg",SETCFG,
"rxp",T(BR,RXP),
"ret",T(BR,RET),
"rett",T(BR,RETT),
"reti",T(BR,RETI),
"lmr",T(LMR,LONG), /* data operand is always double-word */
"smr",T(SMR,LONG),
"lprb",T(LPR,BYTE),
"lprw",T(LPR,WORD),
"lprd",T(LPR,LONG),
"sprb",T(SPR,BYTE),
"sprw",T(SPR,WORD),
"sprd",T(SPR,LONG),
".export",EROU,  /* routine export decl.  "EROU" means end of routine,	*/
		 /* used to process the program in pieces.  An EROU with*/
		 /* empty op string is artificially inserted by getline	*/
		 /* before double-colon labels ("_foo::"), since ccom	*/
		 /* no longer generates .exports.  Yes, this means sets */
		 /* of static routines will be processed all at once, 	*/
		 /* possibly quite slowly.			(jima)	*/
".module",MODULE,
".program",PROGRAM,
".static",STATIC,
".endseg",ENDSEG,
".byte",BGEN,
".word",WGEN,
".double",LGEN,
".long",DGEN,
".float",FGEN,
".comm",COMM,
"rdval",T(RDVAL,BYTE),
"wrval",T(WRVAL,BYTE),
"bicpsrb",T(BICPSR,BYTE),
"bicpsrw",T(BICPSR,WORD),
"bispsrb",T(BISPSR,BYTE),
"bispsrw",T(BISPSR,WORD),
"save",SAVE,
"restore",RESTORE,
"enter",ENTER,
"exit",EXIT,
"flag",FLAG,
"seqd",T(CSETD,JEQ),
"sned",T(CSETD,JNE),
"sled",T(CSETD,JLE),
"sged",T(CSETD,JGE),
"sltd",T(CSETD,JLT),
"sgtd",T(CSETD,JGT),
"slsd",T(CSETD,JLOS),
"shsd",T(CSETD,JHIS),
"slod",T(CSETD,JLO),
"shid",T(CSETD,JHI),
"sccd",T(CSETD,JBCC),
"scsd",T(CSETD,JBCS),
"sfcd",T(CSETD,JBFC),
"sfsd",T(CSETD,JBFS),
"seqw",T(CSETW,JEQ),
"snew",T(CSETW,JNE),
"slew",T(CSETW,JLE),
"sgew",T(CSETW,JGE),
"sltw",T(CSETW,JLT),
"sgtw",T(CSETW,JGT),
"slsw",T(CSETW,JLOS),
"shsw",T(CSETW,JHIS),
"slow",T(CSETW,JLO),
"shiw",T(CSETW,JHI),
"sccw",T(CSETW,JBCC),
"scsw",T(CSETW,JBCS),
"sfcw",T(CSETW,JBFC),
"sfsw",T(CSETW,JBFS),
"seqb",T(CSETB,JEQ),
"sneb",T(CSETB,JNE),
"sleb",T(CSETB,JLE),
"sgeb",T(CSETB,JGE),
"sltb",T(CSETB,JLT),
"sgtb",T(CSETB,JGT),
"slsb",T(CSETB,JLOS),
"shsb",T(CSETB,JHIS),
"slob",T(CSETB,JLO),
"shib",T(CSETB,JHI),
"sccb",T(CSETB,JBCC),
"scsb",T(CSETB,JBCS),
"sfcb",T(CSETB,JBFC),
"sfsb",T(CSETB,JBFS),
	0,	0};
