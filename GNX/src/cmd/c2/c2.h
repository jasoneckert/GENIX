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

/* static char sccsid[] = "@(#)c2.h	2.56" */

/*
 * Header for object code improver
 */

/* Option defines (set in Makefile):
	MULTIVAL - enables multiple entries in regs[], so rmove() can
		   retain the original value when the register was
		   loaded, and the locations the register is stored
		   into.  Should be on.  Off speeds up c2. (jima).
	SPACE    - Speed-space tradeoffs are made for space iff defined.
	REGISTER - Used in lieu of "register" in argument and local variable
		   declarations.  Should be defined as "register" in
		   production.  May be defined as null during debugging to
		   keep everything in memory, as sdb(1) apparently wants.
	COPYCODE - Not fully understood (jima), but appears to enable
		   copying the code text of branch operands when they
		   are irregular (don't reference L<num> labels).
(jima)*/

/* BR..FNBRPS must be contiguous starting from 1, for use by equop().	*/
/* (they are branches and "not real instruction" ops).  (jima) 		*/
#define	BR	1
#define	CBR	2
#define	JMP	3
#define	LABEL	4
#define	DLABEL	5
#define	EROU	7
#define	JSW	9 /* CASE jump index, ".word Lxxx-Lcasebase", xxx=labno   */
	 	  /* ...unless subop!=0, in which case it's ".double Lxxx" */
	 	  /*    (used with F77 - see input() and output()). (jima) */

#define FNBRPS (JSW+1) /* first which is not branch or pseudo - used by equop */
/* 	#define	spare 10 */
#define	CLR	11
#define	INC	12
#define	DEC	13
#define	TST	14
#define	PUSH	15
#define CVT 	16  /* MOVX and conversions involving floating point */
#define	CVTP 	17
#define	SUB	18
#define	BIT	19
#define	BIC	20
/* AND, OR, XOR should be kept in sequence */
#define	AND	21
#define	OR	22 
#define	XOR	23

#define	COM	24
#define	NEG	25
#define	MUL	26
#define	DIV	27
#define	ASH	28
#define INS	29
#define INSS	30
#define EXT	31
#define EXTS	32
#define	CXP 	33
#define	CXPD 	34
#define RXP	35
#define RETT	36
#define RETI	37

#define	CASE	38
#define ADDC	39
#define	ABS	40
#define FFS	41
#define	ALIGN	42
#define	END	43
#define MOVZ 	44
#define ACB 	45
#define CHECK 	46
/* #define spare 47 */
/* #define spare 48 */
/* #define spare 49 */
/* #define spare 50 */
#define SET 51
#define JSR 52
#define RET 53
#define BSR 54
#define SMR 55
#define LMR 56
#define SPR 57
#define LPR 58

#define	SBIT	60
#define	CBIT	61
#define	IBIT	62
#define	TBIT	63
#define	LSH	64
#define	QUO	65
#define	REM	66
#define	MOD	67
#define	NOT 	68
#define	ROT	69
#define	SUBC	70
#define SUBP	71
#define ADDP	72
#define INSV	73 /* VAX - obsolete */
#define	ADDR	74
#define	LXPD	75
#define RDVAL 	76
#define WRVAL 	77
#define MOVSU	78
#define MOVUS	79
#define MOVM	80
#define CMPM	81
#define ADJSP	82
#define SETCFG	83
#define INDEX 	84
#define BICPSR 	85
#define BISPSR 	86
#define SAVE 	87
#define RESTORE	88
#define ENTER 	89
#define EXIT	90

/* These 6 must be in order for conversion by input() and output(): */
#define	MOV	91
#define	MOVQ	92
#define	CMP	93
#define	CMPQ	94
#define	ADD	95
#define	ADDQ	96
#define DEI	97
#define MEI	98
#define FLAG	99
#define	LCOMM 	100
#define	COMM 	101
#define	MODULE 	102
#define	PROGRAM	103
#define	STATIC	104
#define	BSS	105
#define	ENDSEG	106
#define	COMMENT 107  /* comment which is not removed (used in c2 test progs) */
#define MOVS	108
#define MOVST	109
#define CMPS	110
#define CMPST	111
#define SKPS	112
#define SKPST	113
#define CSETB	114  /* The three CSETs must be in order */
#define CSETW	115  /* The three CSETs must be in order */
#define CSETD	116  /* The three CSETs must be in order */

#define BGEN 	117
#define WGEN 	118
#define LGEN 	119
#define FGEN    120	
#define DGEN 	121

/* CBR subops are used to index revbr[] and negbr[].  Unsigneds = signeds+6.
   Order is also assumed by HASEQ. 				(jima) */
#define	JEQ	0
#define	JLE	1
#define	JGE	2
#define	JLT	3
#define	JGT	4
#define	JNE	5
/* (dummy for unsigned JEQ - 6) */
#define	JLOS	7
#define	JHIS	8
#define	JLO	9
#define	JHI	10
/* (dummy for unsigned JNE - 11) */
#define HASEQ(o) (o<=JGE || o==JLOS || o==JHIS)

#define JBFC 12 /* 16k F bit */
#define JBFS 13 /* 16k F bit */
#define JBCC 14 /* 16K Carry */
#define JBCS 15 /* 16k Carry */
#define NULBR 99 /* undefined value */

/* Types, stored in node.subopt low nibble:			(jima) */
#define	BYTE	1 /* must be in this order - indexes bitsize in c21.c */
#define	WORD	2
#define LONG	3
#define	FLOAT	4 /* must follow scalar types for compat() */
#define	DOUBLE	5

/* Modifiers, stored in node.subopt high nibble:		(jima) */
#define OP2	7	/* 2-operand instruction - operands are src,dest */
#define OP3	8	/* VAX 3-operand instruction */

/* Used by bflow in reg[r][0] (type field) for registers used as a (base) 
   or [index], respecitvely.  Stored in upper nibble. */
#define OPB 9
#define OPX 10

#define T(a,b) (a|((b)<<8))
#define U(a,b) (a|((b)<<4))

#define C2_ASIZE 160		/* Size of fixed buffers - line,regs[],reads[]*/
#define MAX_STRLEN (C2_ASIZE-2)	/* Maximum length of a string which might be 
				   in a C2_ASIZE-size buffer.  Two bytes are 
				   reserved for other info, as in reads[]. */
#define OPNLEN 9

struct optab {
	char	opstring[OPNLEN];
	short	opcode;
} optab[];

struct node {
	char	op;
	char	subop;
	short	refc;
	struct	node	*forw;
	struct	node	*back;
	struct	node	*ref;
	char	*code;
	struct	optab	*pop;
	long	labno;
	short	seq;
};

#define COMBOP(p) *(short *)(&((p)->op))  /* access op & subop together */

/* Use of the following will be supplanted by the above define (jima) */
struct {
	short	combop;
};

# define LINESZ 512
char	line[LINESZ];
struct	node	first;
char	*curlp;
int	nbrbr;
int	nsaddr;  /* No. of simplified addresses */
int	ncommut; /* No. of commuted operands to eliminate moves */
int	nregsub; /* No. of laternate register substitutions */
int	redunm;
int	equivnop;
int	iaftbr;
int	njp1;
int	nrlab;
int	nxjump;
int	ncmot;
int	nnegbr;
int	loopiv;
int	nredunj;
int	nskip;
int	ncomj;
int	nsob;
int	nrtst;
int nbj;
int nfield;

int	nchange;
int	isn;
int	tdebug, debug, gdebug, xdebug, ioflag, ckflag;
char	*lasta;
char	*lastr;
char	*firstr;
#define FIRSTFREG 8    /* First FPU register number, after input encoding */
#define	NREG	16     /* Note: f0-f7 are mapped to register numbers 8-15 */
char	*regs[NREG+6]; /* r0-r7,f0-f7, 5 for operands, 1 for running off end */
#define	RT1	NREG
#define	RT2	(NREG+1)
#define RT3 	(NREG+2)
#define RT4 	(NREG+3)
#define RT5 	(NREG+4)
char	conloc[C2_ASIZE];
char	conval[C2_ASIZE];
char	negbr[];
/* "pass" enables (used for debugging and validation) */
char enable_bmove, enable_rmove, enable_iterate, enable_comjump;

#define	LABHS	127

struct { char lbyte; };

char *copy();
char *copy2();
char *isimm();
char *isfimm();
long getnum();
struct node *codemove();
struct node *insertl();
struct node *insert_znode();
struct node *nonlab();
struct node *alloc();

/* OPERAND CLASS WORD - returned by opclasses() for a given opcode */
typedef unsigned long CLASSWORD;
# define CLREAD 1
# define CLWRITE 2
# define CLADDR 4
# define CLREG 8
# define CLCON 16
# define CL(classword) (classword & 0x1F)  /* 5 bits */
# define CLSPECIAL (-1)  /* special cases, e.g., CVT, MOVZ */
# define ADVANCE_CL(classword) (classword) >>= 5
# define CL_DEF2(p1,p2)          ( ((p2)<<5) | (p1) )
# define CL_DEF3(p1,p2,p3)       ( (CL_DEF2(p2,p3)<<5) | (p1) )
# define CL_DEF4(p1,p2,p3,p4)    ( (CL_DEF3(p2,p3,p4)<<5) | (p1) )
# define CL_DEF5(p1,p2,p3,p4,p5) ( (CL_DEF4(p2,p3,p4,p5)<<5) | (p1) )
