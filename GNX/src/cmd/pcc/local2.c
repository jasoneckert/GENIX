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
 * local2.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)local2.c	5.1 (NSC) 9/8/83";
# endif

/* jima 5/18/82: Added BJ's changes to zzzcode for structure assignment */
/* jima 5/18/82: made size-1 be size on movm */
/* jima 5/18/82: put BJ's revised conput, adrput, acon */
/* jima 5/20/82: zzzcode 'A' changed to generated lxpd,not addr, for fcns 
                 docontext patched to ALWAYS save & restore R2, for EPascal.
                 'V' code replaced with BJ's version (initcode). */
/* jima 5/27/82: addrlxpd routine added, removed pass2 symtab references */
/* jima 6/3/82:  Make unsigned arithmetic not call external functions. */
/* jima 6/11/82: Fixed recursive zzzcode call which passed too few args */
/* jima 6/16/82: Modified offset() to allow any fixed-point index types */
/*               NOTE: I don't why (of if) even these checks are necessary. */
/* jima 6/20/82: Modified adjsp hack to include scanf/fscanf/sscanf.
                 zzzcode 'B' commented out (obsoleted by rewrite action) */
/* jima 6/30/82: optim2 modified to collapse U&->NAME to ICON for calls */
/* jima 7/ 6/82: add opsuffic(),shltype() handle NIL p, lastchance(FORCC) */
/* jima 7/15/82: zzzcode(W): rw to ASGops when lhs is treg; nextcook; */
/* jima 7/19/82: hopcode - chaged div to quo and mod to rem */
/* jima 7/26/82: zzzcode(V) - replaced with Jay's version for INIT fix */
/* jima 7/30/82: ccbranches[] - bug fixed in ULE condition name (from Jay) */
/* jima 8/3/82:  callreg() made to return F0 for float/double. */
/* jima 8/3/82:  put in stack-alignment checking feature (scheck flag) */
/* jima 8/9/82:  Much of mycanon folded into optim2 */
/* jima 8/13/82: in base(): if( !ISPTR(p) ) return(-1); */
/* JZ   8-31-82: in addrlxpd and zzzcode V  removed DECREF since it gets a NAME
                not an ICON now */
/* jima 8/31/82: Fixed CCODES (rval!=0) bug which reversed sense */
/* jima 9/7/82: gencall - not force all ftn ptrs to regs; optim2 index on left
                always check to see if this works in complex cases.*/
/* JZ   9/9/82: change default to new call save return sequence */
/* JZ  9/15/82: Changed the hack for s instructions in zzzcode A to generate
		branch around both moves since an initial move to tos does not
		work.  Possible general problem with tos mode. forarg should be
		carefully checked.*/
/*JZ 9/24/82: Change zzzcode T to generate adjsp[b w d] depending on size */
/*JZ 9/24/82: Change optim2 to fold constants for all  NAME nodes since
              tn.lval is being used for double indirect modes for fp
	      relative variables and as an offset for externals.
	      This should eventually be changed when we get lexical scoping to
	      adding the double indirect directly to the name node and having
	      the offset be in tn.lval for both of them.
	      Then we can make sure we aren't triple indirecting by checking
	      for the leading digit.*/
/*JZ 9/25/82: Changed optim2 to also fold statics the same way as externs. Is
	      this right?  It doesn't seem  to try and fold statics into
	      indirects although it should. This will need to be changed once
	      we have lexical scoping since we won't be	able to recognize
	      statics this way.*/
/*JZ 9/26/82: Changed zzzcode S to accept NAME node since general addressing
	      modes can be accepted and the tables will now pass it through.
	      Put in several ASSERTS to make sure that sucomp gets
	      the right values for structure assignment and that neither side
	      is in a temp register.
	      I will need to check everything as I am not sure that it is all
	      correct for large structures */
/*JZ 9/28/82: removed above  structure fix for this delta since more needs
	      to be done on it.*/
/*JZ 9/29/82: Above fixes can not be implemented yet since front end creates
	    a U& node over the right kid of these nodes.  The entire  structure
	    assignment system needs to be rewritten with a check for size of
	    the structure, and a new template to match large size structures>
	    32 bytes.  Also we need to do 2 movms instead of movs.  When we do
	    a movs we need to mustdo the sides into r1 and r2 as we are doing
	    now in rallo.  All stasgs should be handled by setasg or a
	    modification of it as assigns are now handled in general.from jima
	    The fix we have is rallo()  mustdoing right side  into r0 and left
	    side into  r2.  structures are checked as to their size and if
	    they are greater than 16 that is done.  In zzzcode S we have
	    added  checks to see if the regs are correct.*/
/*JZ 10/5/82: Changed zzzcode S to do assignments into r1 if the source is
           either a register variable or r0
	   if it has been forced into R0 by FORCE in rallo we need to move it
	   into r1.  This won't hurt anything since the code being
	   generated here will not overwrite r0 until after the assignment. 
	   The actual address to be passed will be added later by efcode .The
	   code in efcode is totally kluged since it generates the tree with
	   the assumption that r0 will be allocated, if it is not the tree
	   will still think that the assignment was into r0 no matter where it
	   really was.*/
/*JZ 11/16/82: Changed docontext to stop saving R2 unless the -Zzo switch is
		on for the old call save return sequence.*/
/*JZ 12/1/82:  Generate S instruction kludge only on definition of SBUG since
		rev G cpu seems to fix that problem. */
/*JZ 12/13/82: Changed reg_use to external since it is originally defined in
	       code.c*/
/*JZ 12/17/82: Added an extra argument to gencall: extraargsize. This is so
               that when a function ignores the return value of a structure
	       returning function, it will pop the returned structure off the
	       stack along with its arguments. All call sites were also 
	       changed to use the extra argument*/
/*JZ 12/30/82: Changed genscall to use 0(sp) instead of (sp) since that is
	       all that is accepted by the assembler*/
/*JZ 12/30/82: Changed genscall to do handle the case *struct = structfn()
	       which is just to remove the UNARY MUL and pass the pointer */
/*JZ 1/6/83:   Changed addstrucarg to check for pointer to sp and store it
	       so that the location will not change as arguments are added*/
/*JZ 1/18/83:  Changed MINUS case of optim2 so that it will fall through to
	       PLUS case and will attempt the optimisations of PLUS on 
	       subtraction as well since it was failing on initializations
	       involving addresses and negative offsets*/
/*JZ 1/20/83:  Changed mycanon to not put constants on the left side if the
	       right side is a shorter type, such as short or char.  This
	       is necessary since shorts or chars must be extended into a 
	       register so we can do 32 bit precision operations.*/
/*JZ 2/4/83:   Added break after call cases in optim2() as it falls through
	       otherwise which can cause the MINUS case to be execute and
	       disaster!!*/
/*JZ 3/8/83:   Changed .routines to .importp in stack checking code.*/
/*JZ 3/17/83:  Changed movs instructions to use correct size, which is the
	       number of units being moved.  That is the number of int, chars or
	       shorts not the number of bytes as I had thought */
/*JZ 3/15/83:  Changed hopcode() to generate correct floating point instructions*/
/*JZ 3/19/83:  Changed zzzcode 'W' to recognise FLOAT SCONV nodes and do 2
	       assignments, 1 into the temp location as FLOAT and then to
	       assign the destination to itself while extending from float to
	       double.  This preserves the semantics of allowing FLOAT types
	       and functions to return FLOAT while still actually having them
	       return DOUBLE values. */
/* JZ 4/8/83:  Change optim2() UNARY MUL  to not switch the sides of the
	       PLUS operator unless the right side used at least as many
	       registers as the right.  Previously the code falsely assumed
	       that the sides would be switched back if the su numbers 
	       indicated that they should.  However that was done before not
	       after so certain pointer offset operations were failing.
	       An indexed OREG can only be set up if the BASE can be addressed
	       as is. */
/* JZ 4/21/83: Changed docontext to save and restore f4 to f7.  The code is
	       special cased for the present case.  When 2 temporary registers
	       are used it will need to be changed and when variable numbers
	       of temporary and register variables are used it will need to
	       be changed again. */
/* JZ 4/25/83: Changed the assert in addrlxpd to allow OREGs, since it is legal
	       to take an Address of an OREG if you want to.  This may even be
	       a simple way of implementing the general OREG capability, if
	       you want to use addr for its effects just generate a UNARY AND
	       above an OREG and that will do it.  */
/* JZ 4/25/83: Added fconput for printing floating constants. */
/* JZ 5/6/83:  Changed fconput to print output in hex whether for X-support or
	       native mode. Also change it to print out float format if
	       constant is FLOAT size even though constant is internally
	       stored in hex. Added FCON to canaddr.*/
/* JZ 5/10/83: Changed fconput to only do long format float always since that 
	       is all the assembler presently accepts.*/
/* JZ 5/10/83: Changed fconput to switch the sides of its operands */
/* JZ 6/08/83: Fixed zzzcode 'W' to make tos always be DOUBLE if an arg is
	       either DOUBLE or FLOAT. */
# include "mfile2"
# include "ctype.h"

/* Garbage for static init hack */
/* This redeclares things normally needed only in pass1... */
/*typedef unsigned int TWORD; */
# define STATIC	3
# define FTN  040
# define TMASK 060
# define ISFTN(x)  (( (x)&TMASK)==FTN)  /* is x a function type */
struct symtab {
#ifndef FLEXNAMES
	char sname[NCHNAM];
#else
	char *sname;
#endif
	TWORD stype;  /* type word */

	char sclass;  /* storage class */
	char slevel;  /* scope level */
	char sflags;  /* flags for set, use, hidden, mos, etc. */
	int offset;  /* offset or value */
	short dimoff; /* offset into the dimension table */
	short sizoff; /* offset into the size table */
	short suse;  /* line number of last use of the variable */
	};

extern struct symtab stab[];
/* End of Garbage */
/* a lot of the machine dependent parts of the second pass */

# define BITMASK(n) ((1L<<n)-1)

bjobcode()
{
   printf(".program\n");
}

where(c) {
	fprintf( stderr, "%s, line %d: ", filename, lineno );
}

lineid(l, fn) char *fn; {
	/* identify line l and file fn */
	printf( ";	line %d, file %s\n", l, fn );
}

char	reglist[] = "r3,r4,r5,r6,r7";
extern int	reg_use;

extern ncntargs;		/* The number of arguments.  This is not the
				 * number of double words however as each
				 * structure only counts one. */

extern max_used_breg;		/* The highest used floating point register.
       				 * It is set in freereg() */


docontext(s,i)
char *s;
{
	extern do_rxpn;
	extern minbvar;
	/*       There is old code to have r2 not be a temporary register.
	 *	 This code should be removed eventually.
	 *       This is done with the -Zzo flag which sets the old call save
	 *	 return sequence for the kernel.  The rest of the time R2 is 
	 *	 not saved or restored.
	 */
	if (s[1]=='x')
	{			/* restore floating point registers f4,f6 */
	register save_breg;
						
	/* The following is wrong. It needs to be done in reverse order */
	
	    for (save_breg = max_used_breg; save_breg >= F4; save_breg -= 2)
	    {
		printf("\tmovl	tos,%s\n",rnames[save_breg]);
	    }
	    if (minbvar <  MINBVAR)   /* A kludge to check FLOATS for now*/
	        printf("\tmovl	tos,%s\n",rnames[MINBVAR]);
	}
	
	    
	if( (MAXRVAR-reg_use) > 0 )
	        printf(do_rxpn?"\t%s\t[R2,%s]":"\t%s\t[%s]", s, &reglist[(reg_use-MINRVAR)*3+3]);
	else	printf(do_rxpn?"\t%s\t[R2]":"\t%s\t", s);

	if(s[1]!='x')
	{
	register save_breg;
	
	    if( (MAXRVAR-reg_use) > 0 ||do_rxpn)
		printf(",");
	    printf("%d\n", i);
	    if (minbvar <  MINBVAR)   /* A kludge to check FLOATS for now*/
	        printf("\tmovl	%s,tos\n",rnames[MINBVAR]);
	    for (save_breg = max_used_breg; save_breg >= F4; save_breg -= 2)
	    {
		printf("\tmovl	%s,tos\n",rnames[save_breg]);
	        max_used_breg =0;	/* reset for next function
					 * Since entry is done after exit
					 * it must be reset here */
	    }
	}
	else
	    printf("\n\trxp\t%d\n", (do_rxpn ? ncntargs*(SZINT/SZCHAR):0));
}

eobl2(){
	OFFSZ spoff;	/* offset from stack pointer */
#ifdef FORT
	spoff = maxoff;
	if( spoff >= AUTOINIT ) spoff -= AUTOINIT;
	spoff /= SZCHAR;
	SETOFF(spoff,4);
#ifndef FLEXNAMES
	printf( ".F%d:	.equ	%ld\n", ftnno, spoff );
#else
	/* SHOULD BE L%d ... ftnno but must change pc/f77 */
	printf( "LF%d:	.equ	%ld\n", ftnno, spoff );
#endif
#else
	extern int ftlab1, ftlab2, stk_area;

	spoff = maxoff;
	if( spoff >= AUTOINIT ) spoff -= AUTOINIT;
	SETOFF(spoff,4);
	printf( "L%d:\n", ftlab1);
	docontext("enter",spoff/SZCHAR);
	printf( "	br 	L%d\n", ftlab2);
#endif
	maxargs = -1;
}

struct hoptab { int opmask; char * opstring; } ioptab[] = {

	ASG PLUS,	"add",
	ASG MINUS,	"sub",
	ASG MUL,	"mul",
	ASG DIV,	"quo",
	ASG MOD,	"rem",
	ASG OR,		"or",
	ASG ER,		"xor",
	ASG AND,	"and",
	PLUS,		"add",
	MINUS,		"sub",
	MUL,		"mul",
	DIV,		"quo",
	MOD,		"rem",
	OR,		"or",
	ER,		"xor",
	AND,		"and",
	-1, ""    };

hopcode( f, o ){
	/* output the appropriate string from the above table */

	register struct hoptab *q;
	if ((f =='F' || f == 'L')&& (o == DIV || o == ASG DIV ||
			    o == MOD && o== ASG MOD))
	{
	    if (o== DIV||o==ASG DIV)
	 	printf("div");
	    else if (o== MOD || o== ASG MOD)
		printf("mod");
	    else cerror("illegal floating operation\n");
	}
	else
	{
	    for( q = ioptab;  q->opmask>=0; ++q ){
		if( q->opmask == o ){ /* floating divides have dif names */
		
		    printf( "%s", q->opstring );
		    goto printsize;
		}
	    }
	cerror( "no hoptab for %s", opst[o] );
	}
	printsize:
	switch( f ) {
		case 'D':
		case 'W':
		case 'B':
		case 'L':
		case 'F':
		    printf("%c", tolower(f));
		    break;

		}
/* tbl */
	return;
}

char *
rnames[] = {  /* keyed to register number tokens */
	"r0", "r1", "r2", "f0", "f2", "f4", "f6",
	"r3", "r4", "r5", "r6", "r7", "fp", "sp"

};
/* Changed f6 to register variable */
int rstatus[] = {
	SAREG|STAREG,	SAREG|STAREG,	/* r0, r1 	*/
	SAREG|STAREG,			/* r2		*/
	SBREG|STBREG,	SBREG|STBREG,	/* f0+f1, f2+f3 */
	SBREG|STBREG,	SBREG,	/* f4+f5, f6+f7 */
	SAREG,				/* r3		*/
	SAREG, 		SAREG,		/* r4, r5	*/
	SAREG, 		SAREG,		/* r6, r7	*/
	0,		0,		/* fp, sp	*/
};

tlen(p) NODE *p;
{
	switch(p->in.type) {
		case CHAR:
		case UCHAR:
			return(1);

		case SHORT:
		case USHORT:
			return(2);

		case DOUBLE:
			return(8);

		default:
			return(4);
		}
}

mixtypes(p, q) NODE *p, *q;
{
	register tp, tq;

	tp = p->in.type;
	tq = q->in.type;

	return( (tp==FLOAT || tp==DOUBLE) !=
		(tq==FLOAT || tq==DOUBLE) );
}

prtype(n) NODE *n;
{
	switch (n->in.type)
		{
		case DOUBLE:
			printf("l");
			return;

		case FLOAT:
			printf("f");
			return;

		case LONG:
		case ULONG:
		case INT:
		case UNSIGNED:
			printf("d");
			return;

		case SHORT:
		case USHORT:
			printf("w");
			return;

		case CHAR:
		case UCHAR:
			printf("b");
			return;

		default:
			if ( !ISPTR( n->in.type ) ) cerror("zzzcode- bad type");
			else {
				printf("d");
				return;
				}
		}
}

/* THIS IS NOT USED ANY MORE (jima)
/* /*
/*  * Get a thing into a register and change it to a shorter type.
/*  *
/*  * Comments are NOT authoritative.
/*  *
/*  */
/* regtrunc(l,r)
/* register NODE *l,*r;
/* {
/* 
/* /*
/*  * char/short <- float/double
/*  * int (long) <- double
/*  *
/*  * Not called on ``int <- float'', since that is exact and can be done
/*  * (if at all) in one instruction.
/*  *
/*  * First truncate the value
/*  */
/* 	printf("mov");
/* 	prtype(r);
/* 	prtype(l);
/* 	printf("	");
/* 	adrput(r);
/* 	printf(",");
/* 	adrput(l);
/* 	/*
/* 	 * Now extend thru the register
/* 	 */
/* 	if(tlen(l)!=4) {
/* 		printf("mov");
/* 		prtype(l);
/* 		printf("d");
/* 		printf("	");
/* 		adrput(l);
/* 		printf(",");
/* 		adrput(l);
/* 	}
/* }
 */

/* Emit "addr\t" or "lxpd\t" according to the type of an ICON node.
 * In 2nd pass trees, function references are pointers to functions.
 * (6/2/82 jima)  Allowed OREG's to be passed here since if you really want
 *		  the address of an OREG it is perfectly legal to get it.
 */
addrlxpd(name_node) NODE *name_node; { 
	ASSERT(name_node->in.op==NAME|| name_node->in.op==OREG,"addrlxpd");
  	printf( ISFTN( name_node->in.type ) ? "lxpd\t" : "addr\t" ); 
}
/* 8-31-82: JZ removed DECREF since it gets a name not an ICON now */


/* Determine "operator suffix" information.

   Called when generating NEG[bwdfl], and COMi (emit_uop).  
   Someday zzzcode('A') could probably use this also.

   Returns information about the relative sizes of the operands, and what
   conversions are needed,if any.  If <dest> is a register, it's size
   is always taken as 32 bits ("d").  A pointer to a string is returned:

	"b", "w", "d", "f", "l"  means the sizes are both as indicated,
		or they are scalars and <src> is larger than <dest> 
		and truncation presumably should occur.  
		If <dest> is float/double and <src> is not a constant
		or the same size, 0 is returned.

	"xbw", "xbd", "xwd" means <src> is smaller than <dest> and
		sign-extension should be used.

	"zbw", "zbd", "zwd" means <src> is smaller than <dest> and
		zero-fill extension should be used.

	"a" means <src> is an address constant (addr/lxpd needed).  The
		size of <dest> is not checked in this case.

	"qb", "qw", or "qd" means <src> is a quick-sized constant and
		<dest> is the indicated size.

	0 means none of the above (i.e., float<->int or 
	  float<->double conversions).

(jima) */

/* 1,2,4,8 corresp to b,w,d,l; 5=f */
char *opsu_simple[] = { 0, "b",  "w",   0, "d",  "f", 0, 0, "l" };
char *opsu_xb[] =     { 0, 0,    "xbw", 0, "xbd", 0,  0, 0, 0   };
char *opsu_zb[] =     { 0, 0,    "zbw", 0, "zbd", 0,  0, 0, 0   };
char *opsu_q[] =      { 0, "qb", "qw",  0, "qd",  0,  0, 0, 0   };

char *
opsuffix(dest,src) NODE *dest, *src; {
	register src_lval = src->tn.lval;
	register dest_type = dest->tn.type;
	register srclen,destlen;
	register char *answer;
	char srcexpandable;  /* register or absolute ICON */

	ASSERT( canaddr(src) && canaddr(dest), "opsuffix:args");

	srclen = tlen(src);
	if (src->tn.type==FLOAT) srclen=5;  /* pseudo size for table lookup*/
	srcexpandable = (src->in.op==REG);  /* set below if absolute ICON */

	if (dest->in.op==REG)
		/* Full registers always used, regardless of "type" */
		destlen = (dest_type==DOUBLE ? 8:4);
	else
		destlen = tlen(dest);
	if (dest->tn.type==FLOAT) destlen=5;  /* pseudo size for table lookup*/

	if (src->in.op==ICON && dest_type!=FLOAT && dest_type!=DOUBLE) { 
		/* src is a scalar CONSTANT */
		if(src->in.name[0]=='\0') {
			/* src is an absolute constant */
			if (tshape(src,SQICK)) {
				return( opsu_q[destlen] ); /* qb,qw,qd */
				}
			srcexpandable = 1;  /* can be any larger size */
			if (tshape(src,SCCON)) srclen=1;
			else
			if (tshape(src,SSCON)) srclen=2;
			else
			                       srclen=4;
		} else {
			/* src is a relocatable address constant */
			return("a");
		}
	}
	else
	if (mixtypes(dest,src)) {
		/* floating-scalar mixture */
		if (src->tn.op!=ICON)
			return(0);
		/* else fall through - <src> is constant, <dest> is float/dou.*/
	}
	else {
		/* not floating-scalar mixture */
		if ((destlen > srclen) && (src->tn.op!=REG)) {
			/* extension to larger size */
			if(ISUNSIGNED(src->tn.type))
				if (srclen==2) return("zwd");
				else return(opsu_zb[destlen]);
			else
				if (srclen==2) return("xwd");
				else return(opsu_xb[destlen]);
			}
		else {
			; /* same size or truncation - fall through */
		}
	} 

	/* If we get here, destlen & srclen are set for a simple b,w,d,l,f */
	/* This is the case for same-size or destlen<srclen (truncation).  */
	/* If src is a constant or register, its nominal size is a minimum,*/
	/* and any larger size may be used (registers always have all bits */
	/* defined).                               			   */

	if ( (srclen<destlen) && !srcexpandable ) cerror("opsuffix");
	return(opsu_simple[destlen]);
}

int old_callseq = 0;  /* default; reset by -z[no] option - see initp2 */
force_rxp0( name ) register char *name; { /* FOR HACKED VARARGS - use rxp 0? */
	if (old_callseq) {
		return (strcmp(name,"_printf")  == 0 || 
		        strcmp(name,"_sprintf") == 0 || 
		        strcmp(name,"_fprintf") == 0 || 
		        strcmp(name,"_scanf") == 0   || 
		        strcmp(name,"_fscanf") == 0  || 
		        strcmp(name,"_sscanf") == 0  ||
			/* TO COMPILE COMPILER: */
		        strcmp(name,"_uerror") == 0  ||
		        strcmp(name,"_werror") == 0  
		       );
	}
	else
		return(1);
}

int ninitdef = 0;
extern initfile;
zzzcode(p,c,cookie)
register NODE *p;
{
	extern int xdebug;
	CONSZ val;
	if (xdebug) {
		/* eprint(p, 0, &val, &val); */
		printf("zzz `%c': p->in.op is `%s' cookie is `", 
			c, opst[p->in.op]);
		prcook(cookie); printf("'\n");
		fwalk(p,eprint,0);
	}
	switch( c ) {

	case 'A': {  /* assign Left(or treg) = Right */
		register NODE *l, *r;
		register right_lval;
		register TWORD real_lhs_type; 
		register TWORD real_rhs_type; 

	        /* if (xdebug) eprint(p, 0, &val, &val); */
		ASSERT(p->in.op==ASSIGN,"zzzA op");
		l = p->in.left;		/* was: getlr(p,'L'); */ 
		r = p->in.right;	/* was: getlr(p,'R'); */ 
		real_rhs_type = r->in.type;
		real_lhs_type = l->in.type;

		right_lval = r->tn.lval;

		/*  For extending scalar conversions, sign- or zero- 
		    extension is determined by the RHS type.

		    All regs always have all bits valid, regardless  
		    of their nominal type.  

		    HOWEVER,      
		    truncation or extending must be done as if the LHS
		    was really it's nominal type.  The semantics must be
		    as if an "proper" (possibly extending) move was done
		    based on the nominal types, followed by an extending
		    move from the LHS type to 4 bytes, to fill out the
		    register.  In all cases except USHORT=CHAR this can
		    be optimized into a single instruction.  (jima)
		*/
/* This may not be quite correct...
   Operations on "shorts" which are in registers modify all 32 bits.  
   So should LONG=SHORT do a movwd?  Complete vax compatibility appears 
   possible only if registers have sizes like memory (or short/char
   registers are not used).  

   Truncation behavior can be preserved if assignements into register
   variables are done so that the effect is equivalent to a transfer
   assuming the nominal size, then sign/zero extended to 32 bits.  This can
   be done with a single (extending) move except in the case of short=USHORT.

   (jima)
*/

		if (l->in.op!=REG && r->in.op==REG && tlen(r) < 4) 
			r->in.type = (ISUNSIGNED(r->in.type)?ULONG:LONG);
		if (l->in.op==REG && tlen(l) < 4) 
			l->in.type = (ISUNSIGNED(l->in.type)?ULONG:LONG);

		if (r->in.op == CCODES) { /* evaluate for 1-0 result */ 
			if (r->tn.rval==0) {
				/* rval==0 means no branching, lval=logop */
				/* Emit simple S instruction to get 1-0   */
				/* lval is the OPLOG used to do the cmp.  */
#ifdef SBUG
/* . . . . . TEMP HACK TO AVOID revF CPU CHIP BUG IN S INSTRUCTION (jima)  */
				register truelab; /* branch to if true */
				register contlab; /* continue */

				printf("\t;**TEMP HACK FOR RevF CPU BUG\n\tb"); 
				csgen(r->tn.lval);
				printf("	L%d\t\t;**\n\tmovq",
				       (truelab=getlab())); prtype(l); 
				printf("	0,");adrput(l); 
				printf("\n\tbr	L%d\n",(contlab=getlab()));
				printf("\tL%d:\t",truelab);
				printf("	movq"); prtype(l);
				printf("	1,");adrput(l);
				printf("\t;**\nL%d:\t\t\t\t;** WILL BE:\n;",
				       contlab);
/* . . . . . END OF TEMP HACK (leaves ';' so the S instr will be a comment) */
#endif
				
				printf("	s");
				csgen( r->tn.lval );  /* eq,ne, etc. */
				prtype(l);
				printf("	");
				adrput(l);
				}
			else { register falselab;
				/* Conditional branching was used; rval is   */
				/* the label number branched to in the false */
				/* (0-return) case.  */
				printf( "	movq"); 
				prtype(l); printf("	1,");
				adrput(l); printf("	;zzzA\n");
				cbgen( 0, falselab=getlab(),'I'); /*uncond br*/
				deflab( r->tn.rval );
				printf( "	movq"); 
				prtype(l); printf("	0,");
				adrput(l); printf("\n");
				deflab( falselab );
				/* NOTE: Someday emit one move before
				   evaluating the conditional, so that
				   only a single cond. br. around a move
				   of the other value is needed here.
				   ONLY when we are not using tos!!!!! */
		 		}
			goto zzzA_return;
			} /* rhs is CCODES */

		if (r->in.op == ICON) {
			if(r->in.name[0] == '\0') {
				if (-8<=right_lval && right_lval<=7
				&& l->in.type != FLOAT
				&& l->in.type != DOUBLE) {
					printf("	movq");
					prtype(l);
					goto print_operands;
					}
				if ( tshape(r,SACON) && 
				     (tlen(p)==4 || ISPTR(p->in.type))) {
					printf("	addr\t@");
					adrput(r); printf(","); adrput(l);
					goto zzzA_return;
					}

				/* mark the ICON for best size in mov[xz] */
				r->in.type = (right_lval < 0 ?
						(right_lval >= -128 ? CHAR
						: (right_lval >= -32768 ? SHORT
						: INT )) : real_rhs_type);
				r->in.type = (right_lval >= 0 ?
						  ( right_lval <= 127 ? CHAR
						: (right_lval <= 255 ? UCHAR
						: (right_lval <= 32767 ? SHORT
						: (right_lval <= 65535 ? USHORT
						: INT)))) : r->in.type );
				}
			else {
			 	cerror("zzzA: relocatable ICON");	
				/* These should never occur. All relocatable
				   constants keep their U& -> NAME form,
				   except when used with cxp (jima). */
				}
			} /* ICON */

		/* ICON types have been edited; do the move */

		/* NOTE: The FPU document "MD-026 15-Sep-81" implies in sec.
		   2.3.1.8 (page 10) that the nmemonics MOVFF and MOVLL
		   are correct.  But the present assembler only supports
		   MOVF and MOVL, so that is what is generated (8-82 jima) */
	/*   Change floating constants to smaller size if possible */
		if (r->in.op == FCON)
		{		/* ( */
		    if ((l->in.type == FLOAT)||
# if ns16000
			r->fpn.dval == (float) r->fpn.dval)
#else
	(int)(		float_ceq(r->fpn.dval,
				  float_double(
				  	double_float(r->fpn.dval)))))
#endif
		    	real_rhs_type = r->in.type = FLOAT;
		    }
		/*  Except for coercion which is handled by templates,
		    we can never have float sized temporary registers*/
		if (istnode(l) && l->in.type == FLOAT)
			real_lhs_type= l->in.type = DOUBLE;
		if (mixtypes(l,r)) {
			if (real_lhs_type==FLOAT || real_lhs_type==DOUBLE) {
				/* FLOAT|DOUBLE = scalar */
				printf("	mov");
			} else {
				/* scalar = FLOAT|DOUBLE */
				printf("	trunc"); 
			}  
			prtype(r);
		}
		else {	/* both scalar, or both floating-point */
			printf("	mov");
			if (real_lhs_type==FLOAT || real_lhs_type==DOUBLE){
				/* DOUBLE=FLOAT or FLOAT=DOUBLE */
				if (real_lhs_type!=real_rhs_type) prtype(r);
			}
			else
			if( tlen(l) > tlen(r) ) {
				/* scalar extension */
				if( ISUNSIGNED(r->in.type) )
					printf("z");
				else
					printf("x");
				prtype(r);
			}
			/* else same size or scalar truncation */
		    }
		
	      
		    
		prtype(l);
	print_operands:
		printf("\t");
		adrput(r); printf(","); adrput(l);

              zzzA_return: /* All cases end up here when finished */
		/* Return the types to their true values (jima) */
		l->in.type = real_lhs_type;
		r->in.type = real_rhs_type;
		return;
		} /* case 'A' */

	case 'C':	/* num bytes pushed on arg stack */
		{
		extern int gc_numbytes;
		extern int xdebug;
		extern scheck; /* stack alignment checking flag (jima) */
		register NODE *l;

		if (gc_numbytes) {
			/* Remove the arg bytes */
			/* BEGIN GROSS AND HORRIBLE HACK */
			if (!force_rxp0(p->in.left->in.name))
				  printf("\t;"); /*comment it out for now*/
			/* END GROSS AND HORRIBLE HACK */

		 	ASSERT((gc_numbytes % 4)==0,"args len?");
			printf( 
			  (gc_numbytes<=128 ?"\tadjspb\t%d\n":"\tadjspw\t%d\n"),
			   -gc_numbytes );
			}
		if (scheck)
		    printf("\taddr\t0(sp),tos\t;zzzC\n\tcxp\t_check_postpop\n");
		return;
	}

	case 'D':	/* INCR and DECR */
		zzzcode(p->in.left, 'A', cookie); /* assign to temp first */
		printf("\n	");
		/* fall through... */

	case 'E': {	/* INCR and DECR, FOREFF */
		int	howmuch = p->in.right->tn.lval;
		ASSERT(howmuch>=0, "zzz:E"); /* bad addq template ? */
		if (howmuch <= 7 || (p->in.op!=INCR && howmuch <= 8)) {
			printf("addq");
			prtype(p->in.left);
			printf("\t");
			printf("%d,", (p->in.op == INCR ?  howmuch
							: -howmuch));
			adrput(p->in.left);
			return;
		}
		printf("%s", (p->in.op == INCR ? "add" : "sub") );
		prtype(p->in.left);
		printf("\t");
		adrput(p->in.right);
		printf(",");
		adrput(p->in.left);
		return;
	}
	case 'F':	/* register type of right operand */
		{
		register NODE *n;
		extern int xdebug;
		register int ty;

		n = getlr( p, 'R' );
		ty = n->in.type;

		if (xdebug) printf("->%d<-", ty);

		if ( ty==DOUBLE) printf("l");
		else if ( ty==FLOAT ) printf("f");
		else printf("d");
		return;
		}

	case 'L':	/* type of left operand */
	case 'R':	/* type of right operand */
		{
		register NODE *n;
		extern int xdebug;

		n = getlr ( p, c);
		if (xdebug) printf("->%d<-", n->in.type);

		prtype(n);
		return;
		}

	case 'N':  /* logical ops, turned into 0-1 */
                cerror("zzz:N");  /* not used at present (jima) */
				/* should be removed;  Newly added
				   rules of OPANY,INTAREG implement
				   all intermediate term computation (into
				   temporary regs) via explicit ASSIGNs to
				   the temporary.  All evaluation of
				   logical ops for 0-1 ends up happening in
				   setasg().   See zzz 'W'. (jima) */
		/* THE OLD CODE:
		/* An OPLOG,FORCC template using zzzcode('P') have emitted a 
		 *  conditional branch for the "false" condition (note:
		 *  the sense of the original op was reversed to use cbgen).
		 *  The associated rewrite yielded a CCODES node, which
		 *  matched the present CCODES template.
		 *
		 *  This template placed a 1 in the RESC1 register, for the true
		 *  case.  Branch around the false case, and emit the false 
		 *  case code, namely, place a 0 in the register. (jima) */
		/*
		 *cbgen( 0, m=getlab(), 'I' );  /* unconditional br */
		/*deflab( p->bn.label );        /* drop label here  */
		/*printf( "\tmovqd\t0,%s\n", rnames[getlr( p, '1' )->tn.rval] );
		 *deflab( m );
		 *return;
		 */

/* Should not be needed with new OPLOG/CCODES handling (jima)
 *	case 'I':
 *	case 'P':
 *		if(cookie==FORARG)
 *			csgen(p->in.op); printf('d');
 *		else if(cookie==FORCC)
 *			cbgen( p->in.op, p->bn.label, c );
 *		else	cerror("zzz: cookie");
 *		return;
 */
	case 'S':  /* structure assignment */
		{
			register NODE *l, *r;
			register size;
			int	 movm;
			int      remsize;/*  remaining bytes to be moved */

			if( p->in.op == STASG ){
				l = p->in.left;
				r = p->in.right;

				}
			else if( p->in.op == STARG ){  /* store an arg into a temporary */
				l = getlr( p, '3' );
				r = p->in.left;
				}
			else cerror( "STASG bad" );

			if( r->in.op == ICON ) r->in.op = NAME;
	/*		else if (r->in.op == NAME);   This should be implemented some day*/
			else if( r->in.op == REG ) r->in.op = OREG;
			else if( r->in.op != OREG ) cerror( "STASG-r" );

			size = p->stn.stsize;

			if( size <= 0 ) /* > 64K now works */
				cerror("structure size <0=0");

			movm = 0;
			/*
			 * for the future...
			 *
			 * Determine the alignment, use optimal bwd size.
			 *
			 * Replace the movd for long strings with an optimally
			 * sized movz[bw]d.
			 *
			 * For very long and possibly mis-aligned things,
			 * insert a run-time alignment check.
			 *
			 * Use optimal combination of instructions.  For ex:
			 * 2 movmd are faster than 1 movsd so we can replace
			 * the movsd by movmd upto 32.  Also movsb and movmb
			 * are very slow so that it pays for large strings to
			 * do a movmd or movsd and a movb instead of a movmb or
			 * movsb.  
			 */
			
			 switch(size)
			 {
				case 1:
					printf("	movb	");
					break;
				case 2:
					printf("	movw	");
					break;
				case 4:
					printf("	movd	");
					break;
				default:
				
				/* The encoding for the block move instructions
				 * is the number of objects to be moved, not
				 * the number of bytes.  So a d instruction
				 * tells it the number of double words to be
				 * moved.
				 */
				    if(size<=16)
				    {
					if ((size % (SZINT/SZCHAR))==0 )
					{
					    printf("movmd\t");
					    adrput(r);
					    printf(",");
					    adrput(l);
					    printf(",%d\t",size/(SZINT/SZCHAR));
					}
				    
					else if (size % (SZSHORT/SZCHAR) ==0)
					{
					    printf("movmw\t");
					    adrput(r);
					    printf(",");
					    adrput(l);
					    printf(",%d\t",size/(SZSHORT/SZCHAR));
					}
					
					else
					{
					    printf("\tmovmb\t");
					    adrput(r);
					    printf(",");
					    adrput(l);
					    printf(",%d\t", size);
					}
					
				    }
				    
				    else
				    {
					ASSERT(r->tn.op==OREG &&
					      (!istreg(r->tn.rval)||r->tn.rval==R1||(r->tn.rval==R0)),
					       "zzz:movs r1?");
					if (!istreg(r->tn.rval)||r->tn.rval==R0)
					{
					    printf("\taddr\t");
					    adrput(r);
					    printf(",r1\n");
					}
					if(l->tn.op!=OREG ||
					   l->tn.rval!=R2)
					{
					    printf("\taddr\t");
					    adrput(l);
					    printf(",r2\n");
					} /* jima */
					printf("\taddr\t@");
					if (size%(SZINT/SZCHAR)==0)
					    printf("%d,r0\n\tmovsd\t",
					            size/(SZINT/SZCHAR));
					else if (size%(SZSHORT/SZCHAR)==0)
					    printf("%d,r0\n\tmovsw\t",
					           size/(SZSHORT/SZCHAR));
					else printf("%d,r0\n\tmovsb\t",size);
				    }
				    
				goto xx;
			    }
			    
			    
			    adrput(r);
			printf(",");
			adrput(l);
			printf("\n");
xx:
			if( r->in.op == NAME ) r->in.op = ICON;
			else if( r->in.op == OREG ) r->in.op = REG;

			}
		break;

	case 'T':	/* rounded structure length for arguments */
		{
		int size;

		size = p->stn.stsize;
		SETOFF( size, 4);
		printf((size <= 127 ? "b	%d" : (size <=32767  ?"w	%d" : "d	%d")), size);
		return;
		}

	case 'V':	/* Emit initcode instructions for addressable constants
			   Modified 8/31/82 for U&->NAME func. refs (JZ,jima)*/
		{ register NODE *l;
		printf("S%d:\t.blkb 4\n", ninitdef);
		l = p->in.left;
		if  (l->in.op == UNARY AND && l->in.left->in.op==NAME)
		{
			l = l ->in.left; 
			if  (l->tn.lval == 0)    /* l is the leaf node JZ  */
				fprintf(initfile,
					(ISFTN(l->in.type) ? 
					 "\tlxpd %s,S%d\n":"\taddr %s,S%d\n"),
					l->in.name,ninitdef);
			else                /* Is an array,structure or union */
			{
				fprintf(initfile,"\taddr %s+",l->in.name);
				fprintf(initfile,CONFMT,l->tn.lval);
				fprintf(initfile,", S%d\n",ninitdef);
			}
				
		}
		/* This case should never occur now that the UNARY AND is 
		   always passed through.  It never did before either.
		* else if( l->tn.lval )
		* 	fprintf(initfile,"\taddr L%d,S%d\n",l->tn.lval,
		* 		ninitdef);
		* else
		* 	fprintf(initfile,"\taddr %s,S%d\n",l->in.name,
		* 		ninitdef); */
		 else
			uerror("illegal constant in  pointer initialization");
		ninitdef++;
		}
		break;

	case 'W':  /* (6/22/82 jima) */
		/* Used with default rules when computing 
		   INTAREG|INTBREG|INTEMP|FORARG.

		   The matched node is rewritten to be an ASSIGNment of the 
		   node (whatever it is) to the temporary allocated as 
		   resource #1, or a fake variable named "tos" if FORARG.
		   However, if the LHS of the expression is already a temp 
		   register, the op is converted to an ASGop if possible.
		   
		   The templates have a rewrite specification of RNOP, which 
		   causes reclaim to do nothing, not rewriting or
		   reclaiming resources.  The subtree is completely evaluated
		   here by a recursive call to order().  A REG or temp-mem 
		   node results except for FORARG, in which case nothing
		   is returned.

		   Usually the ASSIGN rewrite rule (and setasg()) end up
		   actually evaluating the expression.  In particular, 
		   OPLOG expressions with ||,&&,! which require branching 
		   can directly place a 0 or 1 in the temp (which is the 
		   LHS of the ASSIGN).                (jima) */

		{ register NODE *dest, *newright;

		if( canasgop(p->in.op) && istnode(p->in.left) ) {
			/* lhs is already a temp reg, and the op has an */
			/* assignment form.   Do the assign-op first. */
			/* Ordinarily changing the op to an asgop could
			   alter the order of evaluation, and thus invalidate
			   the sethi-ullman numbers (so codgen would be 
			   called to do any necessary stores).  But the
			   LHS is a simple temp reg, so the RHS must be
			   evaluated now in any case.  */
			p->in.op = ASG p->in.op;
			order(p,INTAREG|INTBREG);

			/* If a temp reg is all that's wanted, we are done */
			if (cookie&(INTAREG|INTBREG)) return;
			}
		
		/* Make explicit ASSIGNment */
		if (cookie&FORARG) {
			/* create fake name node with name "tos" */
 			dest = talloc();
			dest->tn.op = NAME;
			dest->tn.lval = 0;
			dest->tn.rval = 0;
#ifndef FLEXNAMES
			strcpy( dest->tn.name, "tos" );
#else
			dest->tn.name = "tos";
#endif
			}
		else {
			/* LHS of ASSIGN to be temp reg or memory */
			dest = tcopy(getlr(p,'1'));  /* RESC1 */
			}
		/* Preserve the type if reasonable in registers.  Otherwise,
		   always make it 4 or 8 bytes, for scalar or floating.
		   However if we are forcing floating point to be FLOAT
		   we first truncate it to FLOAT and then expand it to
		   DOUBLE.  This will be done for functions that return FLOAT
		   and expressions that are coerced to FLOAT.*/
		   
		if (cookie&INTAREG) {
			dest->in.type = p->in.type;
			if (p->in.op==ICON) {
				if (tshape(p,SCCON)) dest->in.type = CHAR;
				else
				if (tshape(p,SSCON)) dest->in.type = SHORT;
			}
		}
		else
		if( tlen(p) < 4 ) 
			dest->in.type = (ISUNSIGNED(p->in.type) ? ULONG:LONG);
			/* tos should always be DOUBLE by definition */
		else    if (dest -> tn.name[0] == 't' && p->in.type == FLOAT)
			dest->in.type = DOUBLE;
		else
			dest->in.type = p->in.type;

		/* Preserve MUSTDO information for ord_assign */
		dest->in.rall = p->in.rall;
		
		/* do ASSIGNment of p into dest, returning dest in p */
		
		ord_assign( dest, p, p );
		p->in.su=dest->in.su;/*Total registers needed can be 11/9
					no more than required by dest JZ*/
		ASSERT(dest->in.op==FREE,"zzzW; !FREE");
		if (cookie&FORARG) reclaim(p,RNULL,0); /* free result */
		return;

/*		newright = talloc(); 
/*		ncopy(newright,p);    /* newright := orig. tree node */
/*		p->in.op = ASSIGN;
/*		p->in.left = dest;
/*		p->in.right = newright;
/*		p->in.type = dest->in.type;
/*
/*		/* Do the ASSIGN */
/*		/* Choose cookie so reclaim will return what is desired */
/*		if (cookie&INTEMP) cookie=SOREG; /*returns LHS preferentially*/
/*		else
/*		if (cookie&INTAREG) cookie=INAREG;
/*		else
/*		if (cookie&INTBREG) cookie=INBREG;
/*		else
/*			cookie=FOREFF; /* discard result when FORARG */
/*		order(p,cookie);
/*		ASSERT( !(cookie&FORARG && p->in.op!=FREE),"zzzW");
/*		return;
 */
		}
     
	case 'X':
		/* panic. This table entry shouldn't have matched */
		cerror("Table panic.\n");
	case 'Z':	/* complement mask for bit instr */
		printf("$%ld", ~p->in.right->tn.lval);
		return;

	case ':':	/* Output apropriate b,w,d,q in index exp on
			 * sneaky shift */
		{ register NODE *r;
		r = getlr(p, 'R');
		ASSERT( r->in.op == ICON && (r->tn.lval >=0 && r->tn.lval <= 3),
			"zzz: :" );
		printf("%c","bwdq"[r->tn.lval]);
		}
		break;
	default:
		cerror( "illegal zzzcode" );
	}
}

rmove( rt, rs, t ) {
	printf( "	%s	%s,%s\n",
		(t==FLOAT ? "movf" : (t==DOUBLE ? "movl" : "movd")),
		rnames[rs], rnames[rt] );
}

/* Reclaim resource determination: A single resource is used to replace the
   tree in reclaim(), choosen from the set of resources specified in the
   "rw" (rewrite) argument.  For each table entry below with "cform" in
   the cookie, tshape() is called with the corresponding shape "mform" and 
   each candidate resource.  The resource which conforms to any respref
   entry is used. */

/* This was changed so that most things can be used to rewrite, allowing
   the most efficient side of ASSIGNments to be returne for use in
   surrounding expressions (RLEFT|RRIGHT specified).

   However, if a temp register is involved it is always returned, as required
   by ord_assign().				(jima) */

# define MISCCK FOREFF|FORCC|INTEMP|FORARG
# define ALLCK MISCCK|SCON|INAREG|INTAREG|INBREG|INTBREG|SOREG|SNAME|STARNM|STARREG
struct respref  /* { int cform; int mform; } */
respref[] = {
	/* in order of preference... */
	ALLCK,	INTAREG|INTBREG,  /* return temp regs whenever possible */
	/* The above is essential; the following are just for efficiency */
	ALLCK,	INTAREG|INTBREG,
	ALLCK,	INAREG|INBREG,
	ALLCK,	SQICK,   /* 0-byte constants (frequently - when in movq,addq)*/
	ALLCK,	SCCON,   /* 1-byte constants
	ALLCK,	STARREG, /* 1-displacement indirect, usually 1-byte */
	ALLCK,	SNAME,   /* usually 1-byte displacement off sb/fp */
	ALLCK,	SSOREG,  /* other 1-displacement OREG */
	ALLCK,	SOREG,   /* arbitrary (2-offset &/or indexing) OREG */
	ALLCK,	STARNM,  /* 2-displacement off sb/fp */
	ALLCK,	SSCON,   /* 2-byte constants */
	ALLCK,	ICON,    /* larger constants */
	0,		0 };

/* The old table WAS:   
/*respref[] = {
/*	INTAREG,	INTAREG,
/*	INTBREG,	INTBREG,
/*	INAREG,		INAREG|SOREG|STARREG|STARNM|SNAME|SCON,
/*	INBREG,		INBREG|SOREG|STARREG|STARNM|SNAME|SCON,
/*	INTEMP,		INTEMP,
/*	FORARG,		FORARG,
/*	INTEMP,		INTAREG|INAREG|INTBREG|INBREG|SOREG|STARREG|STARNM,
/*	0,	0 };
 */

setregs()
{ /* set up temporary registers */
	fregs = 3;
}

/*** may need this later
szty(t) {
	return( (t==DOUBLE||t==FLOAT) ? 2 : 1 );
}
***/

/*
 * FPU registers are defined as 'B' registers, each able to hold
 * a DOUBLE (actually, each "B register" is always a fp register pair).
 *
 * float/double values can't ever get into scalar regs.		(jima)
 */

szty(t) { /* size, in registers, needed to hold thing of type t */
	return 1;
}

rewfld( p ) NODE *p; {
	return(1);
}

callreg(p) NODE *p; {
	return( (p->in.type==FLOAT || p->in.type==DOUBLE ? F0 : R0) );
}

base( p ) register NODE *p; {
	register int o = p->in.op;

	if (xdebug)
	{
		printf("BASE\n");
		fwalk(p, eprint, 0);
	}
	if( !ISPTR(p->in.type) ) return(-1);
	if( (o==ICON && p->in.name[0] != '\0'))
		return( 100 ); /* ie no base reg */
	if( o==UNARY AND && p->in.left->in.op == NAME)
		return( 100 ); /* ie no base reg */
	if( o==NAME  && (p->in.name[0]=='-' || isdigit(p->in.name[0])) )
                /* Bases can be locals (symbol="-n(fp)")
		 *  or arguments (symbol="n(fp)").
		 *  This causes a node of the form 0(-n(fp))[r?:d] to be
		 *  generated.
		 *  We need to figure out some way of allowing a second offset.
		           *
			   +
			+       <<
		 -n(fp)   n2  r?  2

		 *  if it is already indirect what is
		 *  wanted is its value, not its address so that a third
		 *  level of indirection would be necessary
		 */
		 /* Used to return 100 in the true case but that caused 
		 *  a level of indirection to be lost */
		return(p->tn.lval?-1: 100 + ( 0200*1)); /* Indirect name */
	if( o==REG )
		return( p->tn.rval );
	/* SHOULDN'T  SINGLE-OFFSET OREGS BE RETURNED??? (jima) */
	/* ...for instance (this has never been tried):
	/* if ( o==OREG && !R2TEST(p->tn.rval) )
	/* 	return( p->tn.rval + 0200 ); /* 2nd indirect OREG */
	return( -1 );
}/*base*/


offset( p, tyl ) register NODE *p; int tyl; {
	extern xdebug;
	register TWORD top_type  = p->in.type;
	register TWORD left_type;

	if (xdebug)
	{
		printf("OFFSET\n");
		fwalk(p, eprint, 0);
	}
	if( tyl==1   /* 1-byte element size, no index adjustment needed */
	    && p->in.op==REG 
	    && !isbreg(p->tn.rval) /* probably unnecessary (jima) */

/* was:	    && (top_type==INT					  jima*/
/* was:		|| p->in.type==UNSIGNED || ISPTR(p->in.type))	  jima*/

	  ) return(p->tn.rval);

	if(p->in.op==LS) {
		left_type = p->in.left->in.type;
		if ( p->in.left->in.op==REG
	    	     && !isbreg(p->in.left->tn.rval) /*prob. unnecessary(jima)*/

/* was:	 	     && (left_type==INT 			  jima*/
/*      		 || p->in.left->in.type==UNSIGNED         jima*/
/* 			 || ISPTR(p->in.type))     ?old bug?      jima*/

	             && (p->in.right->in.op==ICON 
	                 && p->in.right->in.name[0]=='\0')
	             && (1 << p->in.right->tn.lval)==tyl
		   ) return( p->in.left->tn.rval + 0200*2*p->in.right->tn.lval );
		}
	return( -1 );
} /*offset*/

makeor2( p, q, b, o) register NODE *p, *q; register int b, o; {
	register NODE *t;
	register int i;
	NODE *f; extern xdebug;

	if (xdebug)
	{
		printf("MAKEOR2:\n");
		printf("MAKEOR2: base\n");
		fwalk(q, eprint, 0);
		printf("MAKEOR2: offset\n");
		fwalk(p, eprint, 0);
	}
	p->in.op = OREG;
	f = p->in.left; 	/* have to free this subtree later */
	/* init base */
	switch (q->in.op) {
		case NAME:
		case ICON:
		case REG:
		case OREG:
			t = q;
			break;
		case UNARY MUL:
			t = q->in.left->in.left;
			break;

		case UNARY AND:
			t = q->in.left;
			q->in.op = FREE;
			break;

		default:
			cerror("illegal makeor2");
	}

	p->tn.lval = t->tn.lval;
#ifndef FLEXNAMES
	for(i=0; i<NCHNAM; ++i)
		p->in.name[i] = t->in.name[i];
#else
	p->in.name = t->in.name;
#endif

	/* init offset */
	p->tn.rval = R2PACK( (b & 0177), (o & 0177), ((o|b)>>7) );

	tfree(f);
	if(xdebug) {
		fwalk(p, eprint, 0);
	}
	return;
}

canaddr( p ) NODE *p; {
	register int o = p->in.op;

	if( o==NAME || o==REG || o==ICON || o== FCON || o==OREG
	|| (o==UNARY MUL && shumul(p->in.left)) ) return(1);
	return(0);
	}

shltype( o, p ) register NODE *p; {
	if (o==UNARY MUL) {
		if (p==NIL) return(1); /* call from setrew()      (jima)*/
		else        return(shumul(p->in.left));
		}
	return( o== REG || o == NAME || o == ICON || o == OREG );
	}

flshape( p ) register NODE *p; {
	return( p->in.op == REG || p->in.op == NAME || p->in.op == ICON ||
		(p->in.op == OREG && (!R2TEST(p->tn.rval) || tlen(p) == 1)) );
	}

shtemp( p ) register NODE *p; {
	if( p->in.op == STARG ) p = p->in.left;
	return( p->in.op==NAME || p->in.op ==ICON || p->in.op == OREG || (p->in.op==UNARY MUL && shumul(p->in.left)) );
	}

/* Match complex shapes built using unary mul nodes */
shumul( p ) register NODE *p; {
	register o;
	extern int xdebug;

	if (xdebug) {
		printf("SHUMUL\n");
		fwalk(p, eprint, 0);
	}
	o = p->in.op;
	if( ((o==NAME) && (p->in.name[0] != '_' ) && (p->tn.lval==0)) ) {
		/* Match names only if not externals (actually, the underscore
		 * check discriminates against any global).  This is because
		 * there is no indirect addressing mode for externals.
		 * [Maybe later this could let statics through].
		 *
		 * The lval==0 check insures that the name is not already 
		 * memory relative [e.g., accessed with disp2(disp1(reg))], 
		 * which would result in an attempt to do triple indexing.
		 * In this case lval would be disp2.
		 */
		return(STARNM);
	}
	return( 0 );
}

adrcon( val ) CONSZ val; {
	printf( CONFMT, val );
}

conput( p ) register NODE *p; { /* Constant format */

		if( p->in.name[0] == '\0' ){
			printf( CONFMT, p->tn.lval);
		}
		else if( p->tn.lval == 0 ) {
#ifndef FLEXNAMES
			printf( "%.8s", p->in.name );
#else
			printf( "%s", p->in.name );
#endif
		} else if (p->in.name[0]== '_'|| p->in.name[0] == 'L'){
#ifndef FLEXNAMES
			printf( "%.8s+", p->in.name );
#else
			printf( "%s+", p->in.name );
#endif
			printf( CONFMT, p->tn.lval );
		} else {
#ifndef FLEXNAMES	/* if the variable is local then the lval field means*/
			/* this is an offset from another level of indirection*/
			/* instead of just an offset from that memory location*/
			printf( "%d(%.8s)", p->tn.lval,p->in.name );
#else
			printf( "%d(%s)", p->tn.lval,p->in.name );
#endif
	}
    }
    
typedef union Switch_sides { 
#if ns16000
				double conval;
#else
				quad_wrd conval;
#endif
				struct {
				int lo,hi; /* int is as good as anything */
				} switchpart;
				} switch_sides;
fconput( p )
register NODE *p;
{ /*Floating constant format */
	switch_sides tmp;
	tmp.conval = p->fpn.dval;
/*  The assembler presently only accepts long format float numbers. When it 
 *  accepts short format floating point the commented out section should be
 *  reinstated in the code.*/
/*		    if (p->in.type == DOUBLE)*/
		    {
			printf( DCONFMT, tmp.switchpart.hi,tmp.switchpart.lo);
			return;
		    }
/*		    else
*		    if (p->in.type == FLOAT)
*		    {		/* since only doubles can be passed as
*				   parameters; we force our floats to int
*				   in order to keep them in float format */
/*#if ns16000
*			union { float fl; int nt; } tmp;
*			tmp.fl = p->fpn.dval;
*			printf(FCONFMT,tmp.nt);
*#else
*			printf(FCONFMT,double_float(p->fpn.dval));
*#endif
*			return;
*		    }*/
		    
		    
	}
insput( p ) register NODE *p; {
	cerror( "insput" );
	}

upput( p ) register NODE *p; {
	cerror( "upput" );
	}

adrput( p )
register NODE *p;
{
	register int r;
	/* output an address, with offsets, from p */

	if( p->in.op == FLD ) {
		p = p->in.left;
	}
	switch( p->in.op ) {

	case NAME:
		acon( p );
		return;

	case ICON:
		/* addressable value of the constant */
		conput( p );
		return;
	case FCON:
		fconput(p);
		return;

	case REG:
		printf( "%s", rnames[p->tn.rval] );
		return;

	case OREG:
		r = p->tn.rval;
		if( R2TEST(r) ) { /* double indexing */
			register int flags;

			/* NOTE: I assume that UPK3 will give me at least
				three bits -- watch out on pdp11's */

			flags = R2UPK3(r);
			if( flags & 1 )
				printf("0(");
			if( p->tn.lval != 0 || p->in.name[0] != '\0' )
			{
				conput(p);
				if( R2UPK1(r) != 100)
					printf( "(%s)", rnames[R2UPK1(r)] );
			} else if( R2UPK1(r) != 100)
					printf( "0(%s)", rnames[R2UPK1(r)] );
			if( flags & 1)
				printf(")");
			ASSERT( flags <= 07, "bad double index mode word");
			printf( "[%s:%c]", rnames[R2UPK2(r)],"bwdq"[flags>>1] );
			return;
		}
		if( r == FP ) {  /* in the argument region */
			printf( CONFMT, p->tn.lval );
			printf( "(fp)" );
			return;
		}
		if( p->tn.lval != 0 || p->in.name[0] != '\0')
			conput( p );
		else	printf("0");
		printf("(%s)", rnames[p->tn.rval] );
		return;

	case UNARY MUL:
		/* STARNM found */
		if( tshape(p, STARNM) ) {
			printf("0(");
			adrput(p->in.left);
			printf(")");
		} else
			cerror("auto inc or dec"); /* STARREG */
		return;

	default:
		cerror( "illegal address" );
		return;

		}

	}

acon (p) register   NODE * p; {	/* print out a addressable constant */

    if (p -> in.name[0] == '\0') {
	printf ("@%ld", p -> tn.lval);
    }
    else
	if (p -> tn.lval == 0) {
#ifndef FLEXNAMES
	    printf ("%.8s", p -> in.name);
#else
	    printf ("%s", p -> in.name);
#endif
	}
	else
	    if (p -> in.name[0] == '-') {
		printf (CONFMT, p -> tn.lval);
#ifndef FLEXNAMES
		printf ("(%.8s)", p -> in.name);
#else
		printf ("(%s)", p -> in.name);
#endif
	    }
	    else {
#ifndef FLEXNAMES
		printf ("%.8s+", p -> in.name);
#else
		printf ("%s+", p -> in.name);
#endif
		printf (CONFMT, p -> tn.lval);
	    }
}

/* This function is passed 3 nodes :
/*   1. The result node is where the rewritten trees root belongs.
/*   2. The presargs node is where the STCALL node is 
/*   3. newarg is the new first argument of the function, a pointer to where
/*      the structure is to be returned to.
/*  The argument tree of a CALL or STCALL node has 3 possible forms as follows
/*    1. No args
/*		UNARY CALL
/*		  /    \
/*	      ICON     NULL
/*	      fn. name
/*       All other forms are rewritten to this once the args are generated 
/*    2. One arg
/*		    CALL
/*		  /      \
/* 	      ICON      arg
/*	      fn. name
/*    3. multiple args
/*                  CALL
/*               /       \
/*            ICON       CM
/*	      fn.name  /    \
/*		      CM    arg 3
/*		     /   \
/*		  arg 1  arg 2
/* When the function is a function pointer then there is no ICON and the
/* left side is instead  whatever
/* the function pointer is: REG, U* or NAME, otherwise there is no difference
/* from the above.
/* When the tree is printed by the debugging flags CM apears as ",," .
/* The arg tree is of course left recursive.*/
#line 1540
addstrucarg(result,presargs,newarg)
register NODE *result,*presargs,*newarg;
{ 
   NODE * tmp = talloc(), * newnode , *l, *p; 

   if (newarg->in.op == UNARY MUL)
   {/*eliminate the  UNARY MUL make it a pointer */
   	newarg->in.op = FREE;
	newnode = newarg->in.left;
	newarg->in.right =NIL;
	newarg->in.left=NIL;
   }
   else
   {/* create a pointer */
   	newnode = talloc();
   	newnode->in.op = UNARY AND;
   	newnode -> in.left = newarg;
   	newnode -> in.su = 0;
   	newnode -> in.type = INCREF (newarg->in.type);/* pointer to type returned */
   	newnode->in.name="";
   	newnode->in.right=NULL;
   	newnode->in.rall=NOPREF;
   }

   if (result != presargs) /* result is STASG node */
   {
	ASSERT(result->in.op==STASG,"addstrucarg, bad op");
	ncopy (result, presargs); /* replace assignment node with call node */
	presargs -> in.op = FREE;
   }
   if (result->in.right)  /* are there any args? */
   {
   	ncopy(tmp,result->in.right);    /* The new top of the arg tree */
	result->in.right->in.op = CM;
	result->in.right->in.left=tmp;
	result->in.right->in.right=NIL;
	if (strcmp(newarg->in.name,"0(sp)")==0)/* Should we also allow INTAREG*/
		order(newnode,INTEMP);/*store the sp location so it doesn't change*/
		
	p = tmp; /*p points at the arg or CM the top of arg tree*/
	if (p->in.op == CM) /* are there  more than one args? */
			   /* Traverse the tree moving the last arguments up */
			   /* one notch to make room for a new first arg */
	{
		result->in.right->in.right=tmp->in.right;/* The last arg
							    is moved up 1 */

		result->in.right->in.left  = p;      /* The old tree is here */
		while ( p->in.left->in.op == CM)
		{
			p->in.right=p->in.left->in.right;
			p = p->in.left;
		}
		p->in.right = p->in.left; /* make room for the new first arg*/
		p->in.left=newnode; /* add the new #1 argument */
	}
	else 
	{
		result->in.right->in.right=tmp; /*The only arg becomes last*/
		p=result->in.right;
		p->in.left=newnode; /* add the new #1 argument */
	}
   }   else  /* No args yet. we will now add one */
   {
   	p = result;
	p->in.op = STCALL; /* no longer UNARY STCALL since we added arg */
	p->in.right = newnode; /* single argument goes on the right */
   }

}
genscall( p, cookie ) register NODE *p;
{
	/* structure valued call */ 
int size=p->stn.stsize;
	if (cookie != FORSTRUCT)
	{
	NODE *l,*tmp = talloc();
		SETOFF(size,4);/* should be double word aligned */
		/*Save room on the stack for the soon to be returned structure*/
		printf(size<128 ?"\tadjspb\t%d\n":size<32768?
		"\tadjspw\t%d\n":"\tadjspd\t%d\n",size  );

		l=tmp;
		l->in.type = STRTY;
		/* since the sp register is basically an address */
		l->in.op=NAME; /* NAME is apropriate in this case */
		l->tn.name="0(sp)";/* generate addr (sp),tos. sp changes after*/
		l->tn.rval = l->tn.lval = 0;
		addstrucarg(p,p,tmp);
		if (cookie != FORARG) /*throw away return value, its not used*/
			return(gencall(p,FOREFF,size));
		else return(gencall(p,FOREFF,0));/*save return value for call*/
		
		
	}
	else cookie=FOREFF;/* The only real reason for doing a structure
			    * return is for effect. */
	return( gencall( p, cookie,0 ) );
}

/* tbl */
int gc_numbytes;
/* tbl */

gencall( p, cookie,extraargsize ) register NODE *p; {
	/* generate the call given by p */
	register NODE *p1, *ptemp;
	register temp, temp1;
	register m;
	extern scheck; /* stack alignment checking flag (jiam) */

	if (scheck) {
		if (scheck>0)  { /* first time */
			printf(
		      "\t.importp _check_prepush\n\t.importp _check_postpop\n");
			scheck = -1;
			}
		printf("\taddr\t0(sp),tos\t;gencall\n\tcxp\t_check_prepush\n");
		}

	if( p->in.right ) temp = argsize( p->in.right );
	else temp = 0;

	if( p->in.op == STCALL || p->in.op == UNARY STCALL ){
		/* set aside room for structure return */

		if( p->stn.stsize > temp ) temp1 = p->stn.stsize;
		else temp1 = temp;
		}

	if( temp > maxargs ) maxargs = temp;
	SETOFF(temp1,4);

	if( p->in.right ){ /* make temp node, put offset in, and generate args */
		ptemp = talloc();
		ptemp->in.op = OREG;
		ptemp->tn.lval = -1;
		ptemp->tn.rval = SP;
#ifndef FLEXNAMES
		ptemp->in.name[0] = '\0';
#else
		ptemp->in.name = "";
#endif
		ptemp->in.rall = NOPREF;
		ptemp->in.su = 0;
		genargs( p->in.right, ptemp );
		ptemp->in.op = FREE;
		}

	p1 = p->in.left;
if (xdebug) {
   printf("gencall:p=%d p1=%d p1->in.op=%d p1->tn.rval=%d\n",
                   p,   p1,   p1->in.op,   p1->tn.rval );
   }


/* Only has to be addressable... (jima)  9/7/82
/*	if( p1->in.op != ICON ){
/*		if( p1->in.op != REG ){
/*			if( p1->in.op != OREG || R2TEST(p1->tn.rval) ){
/*				if( p1->in.op != NAME ){
/*					order( p1, INAREG );
/*					}
/*				}
/*			}
/*		}
 */
	if (!canaddr(p1)) order(p1,FORADDR);   /*jima*/
/* p1 is now either a "relocatable" ICON or a pointer for use with cxpd */

	/* If a function pointer (procedure descriptor) ended up in a 
	   register, store it into temp memory to accomodate the 16K cxpd
	   instruction (which always treats registers as pointers to 
	   descriptors, never descriptors themselves). */
	if( p1->in.op == REG ) order(p->in.left,INTEMP);

	gc_numbytes = temp+extraargsize;  /* set up for zzzcode(C) 	tbl */

	p->in.op = UNARY CALL;
	m = match( p, INTAREG|INTBREG );
	return(m != MDONE);
	}

/* tbl */
char ccbranches[][2] = {
	"eq", "ne", "le", "lt", "ge", "gt", "ls", "lo", "hs", "hi", };
/* ops:  EQ    NE    LE    LT    GE    GT    ULE   ULT   UGE   UGT  */
/* tbl */

csgen(o)
{
    ASSERT (simlogop (o), "csgen");
    printf ("%.2s", ccbranches[o - EQ]);
}
	  
cbgen(o, lab, mode)
{ /*   print conditional and unconditional branches */

/* tbl */
	if(o==0)
		printf("	br	L%d\n", lab);
/* tbl */
	else {
		ASSERT( simlogop(o), "cbgen");
		printf("	b%.2s	L%d\n", ccbranches[o-EQ], lab );
	}
}

/* Changed 6/24/82, 7/17/82 (jima) */
nextcook( p, cookie ) NODE *p; {
	/* we have failed to match p with cookie; try another */
	if (logop(p->in.op) && !(cookie&FORCC)) return(FORCC);  /* jima */
	if (cookie == INTEMP) return(FORADDR);	/* jima */
	if( cookie == FORREW ) return( 0 );  /* hopeless! */
	return( FORREW );
	}

lastchance( p, cook ) NODE *p; {
	if (cook&FORCC) return(1);  /* multiple steps to get CCODES  */
				    /* (first in a reg, etc.) (jima) */
	/* forget it! */
	return(0);
	}


/* do local tree transformations and optimizations */
/* called from myreader via walkf (bottom-up) */
optim2(p) register NODE *p; {
	register int op = p->in.op;
	register NODE *l = p->in.left;
	register NODE *r = p->in.right;
	register NODE *temp;

	switch(op) {

/*[ Change this to insert a and/not node someday ]
/*	case AND:
/*		/* commute L and R to eliminate compliments and constants */
/*		if((p->in.left->in.op==ICON
/*		&&  p->in.left->in.name[0]==0)
/*		||  p->in.left->in.op==COMPL) {
/*			r = p->in.left;
/*			p->in.left = p->in.right;
/*			p->in.right = r;
/*		}
/*	case ASG AND:
/*		r = p->in.right;
/*		if(r->in.op==COMPL) { 
/*			r->in.op = FREE;
/*			p->in.right = r->in.left;
/*		}
 */

	case CALL:
	case UNARY CALL:
	case STCALL:
	case UNARY STCALL:
		/* if the target of a call is a plain constant function, it 
		   will appear as &X, where X is a "pointer to function" name.
		   In "ordinary" systems, which support relocatable constants, 
		   U&->NAME combinations are collapsed into ICONs by optim(), 
		   as directed by andable().  But Mesa does not support such, 
		   so they are left uncollapsed to facilitate generation of 
		   addr or lxpd instructions where needed.  But in this case 
		   (a call), an ICON is safe (will not be used except with a 
		   cxp), and is convenient for template matching.   (jima) */

		   if (l->in.op == UNARY AND)
		       	if (l->in.left->in.op == NAME) {
				l->in.left->in.op = ICON;
				p->in.left = l->in.left;
				l->in.op = FREE;
				goto fooo;
			}
			break;/* used to fall through but it never should*/
	case MINUS: /* added 8/11/82 - jima */
	     /*JZ  moved so that it will fall through to PLUS optimisations*/
		if (r->in.op==ICON && r->tn.name[0]=='\0' )
		{
			/* convert subtraction of constants to addition */
			/* (eliminates templates and some checks in offstar) */
			p->in.op = PLUS;
			r->tn.lval = -(r->tn.lval);
		}
				
	case PLUS:
		if(ISPTR(p->in.type) && l->in.op == UNARY AND 
		&& l->in.left->in.op == NAME) {
/* This is wrong for Mesa - ICONs can't be relocatable (except with cxp)(jima)
/*			if( r->in.op == REG ) {
/*				l->in.op = FREE;
/*				p->in.left = l->in.left;
/*				p->in.left->in.op = ICON;
/*				goto fooo;
/*			} else 
 */			if( r->in.op == ICON ) /* allow statics also since they are the same?*/
			if (l->in.left->tn.name[0] == '_'||(l->in.left->tn.name[0]=='L')) { 
				l->in.left->tn.lval += r->tn.lval;
				p->in = l->in;
				l->in.op = FREE;

				r->in.op = FREE;
				goto fooo;
				

			}
			else if (l->in.left->tn.name[0] != 'r')
			{
			char *malloc();

			register char *ptr= malloc(SZINT+4);/* KLUDGE since I am not sure how it should be done */
			register offset;         /* This should eventually be done above or in pass 1 */
				offset = atoi(l->in.left->tn.name) + r->tn.lval;
				ASSERT((offset != r->tn.lval),"optim2 +: non fp relative name");
				sprintf(ptr,"%d(fp)",offset);
				l->in.left->tn.name = ptr;
				p->in = l->in;
				l->in.op = FREE;
				r->in.op = FREE;
				goto fooo;
			 }
		}
		break;

	case UNARY MUL:
		if(l->in.op==PLUS && ISPTR(l->in.type)
		   /*&& !ISPTR(DECREF(l->in.type)) */ ) {

			/* Put index expressions on left, to be put into a
			   temp register first (by offstar) - left-to-right
			   evaluation order under plus.  But not if it's
			   an ICON, in which case con(basereg) will be
			   formed.  However if the su numbers indicate the
			   left side requires more registers than the right
			   we want the left put in a register first as that
			   will require the least stores. The left side will
			   be put in a register first except in certain cases
			   in offstar().
			    */
			if(ISPTR(l->in.left->in.type) && 
			   l->in.right->in.op!=ICON && l->in.right->in.su >=
			   l->in.left->in.su ) {

/*			/* Put scaled index expressions on lft of + for
/*			   offstar, which will put the unscaled index into
/*			   a register to help form (base)[Reg:bwdq] OREGs.
			   base must be either a NAME or be able to
			   be made into a STARNM to be able to be used.
			   Either way it's su number should be 0 */
/*		        if(l->in.right->in.op==LS) {added'}' to balance.
*/
				/* swap sides under the PLUS */
		  		temp = l->in.left;
		  		l->in.left = l->in.right;
		  		l->in.right = temp;
			}

			/*  Collapse *(`-n(fp)' + ICON) to NAME con(`-n(fp')) */
			/* This should be changed to actually change the string
			   to con(-n(fp)) so that tn.lval can be used for offsets */
			if(l->in.left->in.op == NAME && 
			   l->in.left->in.name[0] == '-' && 
			   l->in.right->in.op == ICON && 
			   l->in.right->tn.lval != 0 &&  /* why this? (jima)*/
			   l->in.left->tn.lval == 0) {
					l->in.left->tn.lval = 
						l->in.right->tn.lval;
					l->in.right->in.op = FREE;
					goto smashtypes;
			}
		} /*  U* -> PLUS PTR  */

		/* collapse *(&Name) to just Name */
		if(l->in.op == UNARY AND && l->in.left->in.op == NAME){
	      smashtypes:
				l->in.left->in.type = p->in.type;
				p->tn = l->in.left->tn;
				l->in.left->in.op = FREE;
				l->in.op = FREE;
			fooo:	if(xdebug) {
					printf("OPTIM2:\n");
					fwalk(p, eprint, 0);
				}
		}
		
			
	} /* switch */

	/* swap sides to avoid stores, based on initial sethi-ullman numbers */
	if ((canasgop(op) && comop(op)) || simlogop(op)) {
		if (r->in.su > l->in.su) {
			if (simlogop(op)) p->in.op = op = revrelop(op);
		  	p->in.left = r;
		  	p->in.right = l;
			r = l;
			l = p->in.left;
		}
	}

	/* compute initial sethi-ullman number (jima) */
	sucomp(p);
}

NODE * addroreg(l)
				/* OREG was built in clocal()
				 * for an auto or formal parameter
				 * now its address is being taken
				 * local code must unwind it
				 * back to PLUS/MINUS REG ICON
				 * according to local conventions
				 */
{
	cerror("address of OREG taken");
}



# ifndef ONEPASS
main( argc, argv ) char *argv[]; {
	return( mainp2( argc, argv ) );
	}
# endif


/* added by jwf */
/* made conditional on UHARD (6/82 jima) */
#ifdef UHARD
struct functbl {
	int fop;
	TWORD ftype;
	char *func;
	} opfunc[] = {
	DIV,		TANY,	"udiv",
	MOD,		TANY,	"urem",
	ASG DIV,	TANY,	"udiv",
	ASG MOD,	TANY,	"urem",
	0,	0,	0 };
#endif

hardops(p)  register NODE *p; {
	/* change hard to do operators into function calls.  */
#ifdef UHARD    /* unsigned operators are hard */
	register NODE *q;
	register struct functbl *f;
	register o;
	register TWORD t;

	o = p->in.op;
	t = p->in.type;
	if( t!=UNSIGNED && t!=ULONG ) return;

	for( f=opfunc; f->fop; f++ ) {
		if( o==f->fop ) goto convert;
		}
	return;

	/* need to rewrite tree for ASG OP */
	/* must change ASG OP to a simple OP */
	convert:
	if( asgop( o ) ) {
		q = talloc();
		switch( p->in.op ) {
			case ASG DIV:
				q->in.op = DIV;
				break;
			case ASG MOD:
				q->in.op = MOD;
				break;
		}
		q->in.rall = NOPREF;
		q->in.type = p->in.type;
		q->in.left = tcopy(p->in.left);
		q->in.right = p->in.right;
		p->in.op = ASSIGN;
		p->in.right = q;
		zappost(q->in.left); /* remove post-INCR(DECR) from new node */
		fixpre(q->in.left);	/* change pre-INCR(DECR) to +/-	*/
		p = q;

	}

	/* build comma op for args to function */
	q = talloc();
	q->in.op = CM;
	q->in.rall = NOPREF;
	q->in.type = INT;
	q->in.left = p->in.left;
	q->in.right = p->in.right;
	p->in.op = CALL;
	p->in.right = q;

	/* put function name in left node of call */
	p->in.left = q = talloc();
	q->in.op = ICON;
	q->in.rall = NOPREF;
	q->in.type = INCREF( FTN + p->in.type );
#ifndef FLEXNAMES
	strcpy( q->in.name, f->func );
#else
	q->in.name = f->func;
#endif
	q->tn.lval = 0;
	q->tn.rval = 0;

	return;
#endif   /* ifdef UHARD */

	}

zappost(p) NODE *p; {
	/* look for ++ and -- operators and remove them */

	register o, ty;
	register NODE *q;
	o = p->in.op;
	ty = optype( o );

	switch( o ){

	case INCR:
	case DECR:
			q = p->in.left;
			p->in.right->in.op = FREE;  /* zap constant */
			ncopy( p, q );
			q->in.op = FREE;
			return;

		}

	if( ty == BITYPE ) zappost( p->in.right );
	if( ty != LTYPE ) zappost( p->in.left );
}

fixpre(p) NODE *p; {

	register o, ty;
	o = p->in.op;
	ty = optype( o );

	switch( o ){

	case ASG PLUS:
			p->in.op = PLUS;
			break;
	case ASG MINUS:
			p->in.op = MINUS;
			break;
		}

	if( ty == BITYPE ) fixpre( p->in.right );
	if( ty != LTYPE ) fixpre( p->in.left );
}

/* called from mainp2 just before calling delay (and codgen, etc.) */
myreader(p) register NODE *p; {
	walkf( p, hardops );	/* convert ops to function calls */
	canon( p );		/* expands r-vals for fileds */
	if (xdebug) {printf("pre optim2\n");dump2(p);}/* DEBUG CODE */
	walkf( p, optim2 );
	if (xdebug) {printf("post optim2\n");dump2(p);}/* DEBUG CODE */
	/* jwf toff = 0;  /* stack offset swindle */
	}

/* called from canon after oreg2 just before sucomp */
mycanon(p) NODE *p; {
	register NODE *r = p->in.right;
	register NODE *l = p->in.left;
	register op = p->in.op;

	if (optype(op)!=BITYPE || !canaddr(l) || !canaddr(r)) return;

	/* Swap sides of binary ops to get ICONs and REGs on best side. */
	
	/* However if the op is an add  and one side is an ICON and the other
	/* is a short or char, the short or char should be on the left side
	/* since it must be thrown into a register anyway so a double operation
	/* can be done on it.  This is also true for a multiply */
	/* Both sides are presently addressable, so function calls
	   have been completed and swapping will not cause running out
	   of registers. */

  	/* If the RHS of a relational is any REG or ICON, put it 
	   on the left.  There are OPLOG templates for ICONs, and 
	   for REGs involving mixed types, on the left (only). 

	   Otherwise, put the smallest type mem ref on the LHS;
	   if it is put in a temp register, it will always compare
	   directly with the (larger type) other side. */

	if (simlogop(op) && (l->in.op!=ICON) && 
	    (r->in.op==REG || r->in.op==ICON ||
	     (l->in.op!=REG && tlen(r)<tlen(l)))) { 
		p->in.op = op = revrelop(op); /* reverse sense */
		goto swap;
	}
		
	/* For ops which will become assignment-ops, put quick constants
	   on the LHS, as they are the cheapest thing to put in a temp reg.
	   Exception: put quicks on RIGHT for +, so addq can be used.

	   Otherwise put the smallest type on the left, as it should go
	   into a register first to resolve mixed-type problems.  */

	if( comop(op) && canasgop(op) && !istnode(l) &&
	    ((op==PLUS && l->in.op==ICON && tshape(l,SQICK)) ||
	     (r->in.op==ICON && tshape(r,SQICK)&&l->in.type != SHORT &&
	     l->in.type!=USHORT && l->in.type != CHAR && l->in.type !=UCHAR) ||
	     tlen(r)<tlen(l)) ){
	 /***/ swap: /***/
	  	p->in.left = r;
	  	p->in.right = l;
		r = l;
		l = p->in.left;
		if (odebug) {
			printf("MYCANON: Swapped sides, result is:\n");
			fwalk(p, eprint, 1);	
		}
	}
}
			
