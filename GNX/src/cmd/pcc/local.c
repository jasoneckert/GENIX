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
 * local.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)local.c	5.1 (NSC) 9/8/83";
# endif

/* jima 5/19/82: commdec modified; 6/28/82: %o -> %d in stabs prints */
/* jima 6/30/82: andable always returns false now. */
/* jima 7/30/82: local() changes RS/ASG RS ops to LS/ASG LS */
/* jima 8/3/82: noinit() bogus parameter removed (stack adjust error) */
/* JZ 9/15/82: Added ifndef MESA for a number of stabs not in mesa defines */
/* JZ 11/24/82: Added check in noinit() for Fflag so that we can have all
		declarations use local static base*/
/* JZ 3/15/83: Changed isitfloat to convert the string to floating point so
		that floating point code can be generated.*/
/* JZ 3/18/83: Changed clocal to pass through the SCONV nodes for floating
	       point so that coercion will work correctly. However we continue
	       to throw away SCONV nodes for constants since for now it will
	       be allright to simply truncate them.*/
/* JZ 4/22/83: Changed cisreg() to allow doubles as register variables */
/* JZ 5/6/83:  Changed fincode to print floating point in hex for initializations*/
# include "mfile1"
extern char *rnames[];

/*	this file contains code which is dependent on the target machine */

NODE *
cast( p, t ) register NODE *p; TWORD t; {
	/* cast node p to type t */

	p = buildtree( CAST, block( NAME, NIL, NIL, t, 0, (int)t ), p );
	p->in.left->in.op = FREE;
	p->in.op = FREE;
	return( p->in.right );
}

NODE *
clocal(p) NODE *p;
{	

	/* this is called to do local transformations on
	   an expression tree preparatory to its being
	   written out in intermediate code.
	*/

	/* the major essential job is rewriting the
	   automatic variables and arguments in terms of
	   REG and OREG nodes */
	/* conversion ops which are not necessary are also clobbered here */
	/* in addition, any special features (such as rewriting
	   exclusive or) are easily handled here as well */

	register struct symtab *q;
	register NODE *r;
	register o;
	register m, ml;
	extern eprint();
/*
	printf("clocal\n");
		fwalk(p, eprint, 0); */
	switch( o = p->in.op ){

	case NAME:
		if( p->tn.rval < 0 ) { /* already processed; ignore... */
			return(p);
			}
		q = &stab[p->tn.rval];
		switch( q->sclass ){
	TWORD t;
	int d, s ;

		case AUTO:
		case PARAM:
			/* fake up a structure reference */
/*
			r = block( REG, NIL, NIL, PTR+STRTY, 0, 0 );
			r->tn.lval = 0;
			r->tn.rval = FP;
			p = stref( block( STREF, r, p, 0, 0, 0 ) );
	printf("clocal\n");
		fwalk(p, eprint, 0);
*/

	/* make p look like ptr to x */

	/*
	if( ISARY( p->in.type) ){ 
		p->in.type = DECREF( p->in.type );
		++p->fn.cdim;
	}
	if( !ISPTR(p->in.type)){
		p->in.type = PTR+UNIONTY;
		}
	t = INCREF( q->stype );
	d = q->dimoff;
	s = q->sizoff;

	p = makety( p, t, d, s );

	p = buildtree( UNARY MUL, p, NIL ); */

			break;

		case ULABEL:
		case LABEL:
		case STATIC:
			if( q->slevel == 0 ) break;
			p->tn.lval = 0;
			p->tn.rval = -q->offset;
			break;

		case REGISTER:
			p->in.op = REG;
			p->tn.lval = 0;
			p->tn.rval = q->offset;
			break;

			}
		break;

	case PCONV:
		/* do pointer conversions for char and longs */
		ml = p->in.left->in.type;
		if( ( ml==CHAR || ml==UCHAR || ml==SHORT || ml==USHORT )
		&& !( p->in.left->in.op == ICON /*|| p->in.left->in.op == NAME*/))
			break;

		/* pointers all have the same representation; the type is inherited */

	inherit:
		p->in.left->in.type = p->in.type;
		p->in.left->fn.cdim = p->fn.cdim;
		p->in.left->fn.csiz = p->fn.csiz;
		p->in.op = FREE;
		return( p->in.left );

	case SCONV:
		m = (p->in.type == FLOAT || p->in.type == DOUBLE );
		ml = (p->in.left->in.type == FLOAT || p->in.left->in.type == DOUBLE );
		/* The following allows coercion from double to float. */
		if (p->in.type==FLOAT && p->in.left->in.type == DOUBLE &&
		    p->in.left->in.op!=FCON) break;
		if( m != ml ) break;

		/* now, look for conversions downwards */

		m = p->in.type;
		ml = p->in.left->in.type;
		if( p->in.left->in.op == ICON ){ /* simulate the conversion here */
			CONSZ val;
			val = p->in.left->tn.lval;
			switch( m ){
			case CHAR:
				p->in.left->tn.lval = (char) val;
				break;
			case UCHAR:
				p->in.left->tn.lval = val & 0XFF;
				break;
			case USHORT:
				p->in.left->tn.lval = val & 0XFFFFL;
				break;
			case SHORT:
				p->in.left->tn.lval = (short)val;
				break;
			case UNSIGNED:
				p->in.left->tn.lval = val & 0xFFFFFFFFL;
				break;
			case INT:
				p->in.left->tn.lval = (int)val;
				break;
				}
			p->in.left->in.type = m;
			}
		else {
			/* meaningful ones are conversion of int to char, int to short,
			   and short to char, and unsigned version of them */
			if( m==CHAR || m==UCHAR ){
				if( ml!=CHAR && ml!= UCHAR ) break;
				}
			else if( m==SHORT || m==USHORT ){
				if( ml!=CHAR && ml!=UCHAR && ml!=SHORT && ml!=USHORT ) break;
				}
			}

		/* clobber conversion */
		if( tlen(p) == tlen(p->in.left) ) goto inherit;
		p->in.op = FREE;
		return( p->in.left );  /* conversion gets clobbered */

	case PVCONV:
	case PMCONV:
		if( p->in.right->in.op != ICON ) cerror( "bad conversion", 0);
		p->in.op = FREE;
		return( buildtree( o==PMCONV?MUL:DIV, p->in.left, p->in.right ) );

	/* Negative the shift count for right shifts */
	/* A recursive call was sometimes causing double-negation, so the op 
	   is now changed to always be LS (possibly negative).    (jima) */
	case RS:
		ASSERT(p->in.op==RS,"RS");
	 	p->in.op = LS;	
		goto negate_shiftcount;
	case ASG RS:
		ASSERT(p->in.op==ASG RS,"ASG RS");
	 	p->in.op = ASG LS;	

	      negate_shiftcount:
		p->in.right = buildtree( UNARY MINUS, p->in.right, NIL );
		break;

	case FLD:
		/* make sure that the second pass does not make the
		   descendant of a FLD operator into a doubly indexed OREG */

		if(p->in.left->in.op == UNARY MUL
		&&(r=p->in.left->in.left)->in.op == PCONV)
			if(r->in.left->in.op == PLUS
			|| r->in.left->in.op == MINUS ) 
				if( ISPTR(r->in.type) ) {
					if( ISUNSIGNED(p->in.left->in.type) )
						p->in.left->in.type = UCHAR;
					else
						p->in.left->in.type = CHAR;
				}
		break;
		}

	return(p);
	}
extern blevel;

/* called from optim() to see if &p can be made into an ICON */
andable( p ) NODE *p; {
	/* The Mesa linker does not support relocatable constants of any kind.
	   So an addr or lxpd instruction is always needed to generate
	   "addresses" of things (external procedure descriptors for
	   functions).  Refusing to make ptr ICONs causes the U& operator
	   to remain explicitly in the tree, to be handled by the
	   ASSIGN rewrite rule [setasg()] and UNARY AND rewrite rule. (jima) */

	/* WAS: if ( blevel >= 2) return(ISFTN(p->in.type)); */
	return(0);
	}

cendarg(){ /* at the end of the arguments of a ftn, set the automatic offset */
	autooff = AUTOINIT;
	}
/* Allowed doubles as register variables */
cisreg( t ) TWORD t; { /* is an automatic variable of type t OK for a register variable */

#ifdef TRUST
	if( t==INT || t==UNSIGNED || t==LONG || t==ULONG	/* tbl */
		|| t==CHAR || t==UCHAR || t==SHORT 		/* tbl */
		|| t==USHORT || ISPTR(t)|| t == DOUBLE) return(1);/* tbl */
#else
	if( t==INT || t==UNSIGNED || t==LONG || t==ULONG	/* wnj */
		|| ISPTR(t)|| t == DOUBLE) return (1);		/* wnj */
#endif
	return(0);
	}

NODE *
offcon( off, t, d, s ) OFFSZ off; TWORD t; {

	/* return a node, for structure references, which is suitable for
	   being added to a pointer of type t, in order to be off bits offset
	   into a structure */

	register NODE *p;

	/* t, d, and s are the type, dimension offset, and sizeoffset */
	/* in general they  are necessary for offcon, but not on H'well */

	p = bcon(0);
	p->tn.lval = off/SZCHAR;
	return(p);

	}


static inwd	/* current bit offsed in word */;
static word	/* word being built from fields */;
static naddrcnt;	/* count of addrs init's */

incode( p, sz ) register NODE *p; {

	/* generate initialization code for assigning a constant c
		to a field of width sz */
	/* we assume that the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* we also assume sz  < SZINT */
	if ( p->in.op == NAME )
	{
		printf("S%d:	.blkd 1\nmagic:\taddr %s,S%d\n",
		naddrcnt, exname(stab[p->tn.rval].sname), naddrcnt++);
		return;
	}

	if((sz+inwd) > SZINT) cerror("incode: field > int");
	word |= ((unsigned)(p->tn.lval<<(SZINT-sz))) >> (SZINT-sz-inwd);
	inwd += sz;
	inoff += sz;
	if(inoff%SZINT == 0) {
		printf( "	.double	x'%x\n", word);
		word = inwd = 0;
		}
	}

fincode( p, sz )
NODE *p;
{
	/* output code to initialize space of size sz to the value d */
	/* the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* write it out in hex! */
	/* Since C reverses the order of arguments on the stack this is the
	   correct way to print the numbers */

	printf("	%s	",
		sz == SZDOUBLE ? ".long" : ".float" );
	fconput(p);
	printf("\n");
	inoff += sz;
}

cinit( p, sz ) NODE *p; {
	/* arrange for the initialization of p into a space of
	size sz */
	/* the proper alignment has been opbtained */
	/* inoff is updated to have the proper final value */
	ecode( p );
	inoff += sz;
	}

vfdzero( n ){ /* define n bits of zeros in a vfd */

	if( n <= 0 ) return;

	inwd += n;
	inoff += n;
	if( inoff%ALINT ==0 ) {
		printf( "	.double	x'%x\n", word );
		word = inwd = 0;
		}
	}

char *
exname( p ) char *p; {
	/* make a name look like an external name in the local machine */

#ifndef FLEXNAMES
	static char text[NCHNAM+1];
#else
	static char text[BUFSIZ+1];
#endif

	register i;

	text[0] = '_';
#ifndef FLEXNAMES
	for( i=1; *p&&i<NCHNAM; ++i )
#else
	for( i=1; *p; ++i )
#endif
		{
		text[i] = *p++;
		}

	text[i] = '\0';
#ifndef FLEXNAMES
	text[NCHNAM] = '\0';  /* truncate */
#endif

	return( text );
	}

ctype( type ){ /* map types which are not defined on the local machine */
	switch( BTYPE(type) ){

	case LONG:
		MODTYPE(type,INT);
		break;

	case ULONG:
		MODTYPE(type,UNSIGNED);
		}
	return( type );
	}
extern Fflag;

noinit( ) { /* curid is a variable which is defined but
	is not initialized (and not a function );
	This routine returns the storage class for an uninitialized 
	declaration */
	if (Fflag)
	return(EXTDEF);
	/* else */
	return(EXTERN);


	}

/* ------------------------------------------------------------------------
 * Make a common declaration.  Called from nidcl (non-initialized var decl)
 *
 * This now just sets the SCOMM bit in the symtab entry.  This causes
 * firstref to put out a .comm declaration instead of .import. (jima 5/19/82)
 */
commdec( id ) { /* make a common declaration for id, if reasonable */
	/* register struct symtab *q;
	 *OFFSZ off, tsize();
 	 *q = &stab[id];
 	 *off = tsize( q->stype, q->dimoff, q->sizoff );
	 *if(off <= 0) return;
	 * ...emit common declaration...
	 */
	stab[id].sflags |= SCOMM;
	}

isitlong( cb, ce ){ /* is lastcon to be long or short */
	/* cb is the first character of the representation, ce the last */

	if( ce == 'l' || ce == 'L' ||
		lastcon >= (1L << (SZINT-1) ) ) return (1);
	return(0);
	}

isitfloat( s ) char *s; {
#if ns16000
	double atof();
	dcon = atof(s);
#else
	quad_wrd natof();	/* defined in fpops.c */
	dcon = natof(s);	/* ns16000 format atof */
#endif
	return( FCON );
	}

ecode( p ) NODE *p; {

	/* walk the tree and write out the nodes.. */

	if( nerrors ) return;
	p2tree( p );
	p2compile( p );
	}
	
#include <sys/types.h>
#include <a.out.h>
#include <stab.h>
extern int ddebug;
extern int gdebug;

fixarg(p)
struct symtab *p; {
}
int	stabLCSYM;

outstab(p)
struct symtab *p; {
	register TWORD ptype;
	register char *pname;
	register char pclass;
	register int poffset;

	/*if (!gdebug) return; a*/

	ptype = p->stype;
	pname = p->sname;
	pclass = p->sclass;
	poffset = p->offset;

	if (ISFTN(ptype)) {
		return;
	}
	
	switch (pclass) {
	
	case AUTO:
		printf(";%s:	.EQU	%d(fp)\n", exname(pname),
				(poffset)/SZCHAR);
		return;
	
	case EXTDEF:
		printf("; %s == external\n", pname);
		return;

	case EXTERN:
		printf("; %s == undefined\n", pname);
		return;
			
	case STATIC:
		if (p->slevel > 1) {
			printf("; L%d ==  %s\n", poffset, pname);
		} else {
			printf(";%s == %d(sb)\n", exname(pname), poffset);
		}
		return;
	
	case REGISTER:
		printf(";%s:	.EQU %s\n", exname(pname), rnames[poffset]);
		return;
	
	case MOS:
	case MOU:
		/*printf("0,%d,%d\n", ptype, poffset/SZCHAR);*/
		return;
	
	
	default:
		if (ddebug) printf("	No .stab for %s\n", pname);
		
	}
}

pstab(name, type)
char *name;
int type; {
	register int i;
	register char c;
	if (!gdebug) return;
	/* locctr(PROG);  /* .stabs must appear in .text for c2 */
#ifdef ASSTRINGS
	if ( name[0] == '\0')
		printf("\t.stabn\t");
	else
#ifndef FLEXNAMES
		printf("\t.stabs\t\"%.8s\", ", name);
#else
		printf("\t.stabs\t\"%s\", ", name);
#endif
#else
	printf("	.stab	");
	for(i=0; i<8; i++) 
		if (c = name[i]) printf("'%c,", c);
		else printf("0,");
#endif
	printf("%d,", type);
}

#ifdef STABDOT
pstabdot(type, value)
	int	type;
	int	value;
{
	if ( ! gdebug) return;
	/* locctr(PROG);  /* .stabs must appear in .text for c2 */
	printf("\t.stabd\t");
	printf("%d,0,%d\n",type, value);
}
#endif

poffs(p)
register struct symtab *p; {
	int s;
	if (!gdebug) return;
	if ((s = dimtab[p->sizoff]/SZCHAR) > 1) {
#ifndef MESA
		pstab(p->sname, N_LENG);
#endif
		printf("1,0,%d\n", s);
	}
}

extern char NULLNAME[8];
extern int  labelno;
extern int  fdefflag;

psline() {
	static int lastlineno;
	register char *cp, *cq;
	register int i;
	
	if (!gdebug) return;

	cq = ititle;
	cp = ftitle;

	while ( *cq ) if ( *cp++ != *cq++ ) goto neq;
	if ( *cp == '\0' ) goto eq;
	
neq:	for (i=0; i<100; i++)
		ititle[i] = '\0';
	cp = ftitle;
	cq = ititle;
	while ( *cp )  
		*cq++ = *cp++;
	*cq = '\0';
	*--cq = '\0';
#ifndef FLEXNAMES
	for ( cp = ititle+1; *(cp-1); cp += 8 ) {
#ifndef MESA
		pstab(cp, N_SOL);
#endif
		if (gdebug) printf("0,0,LL%d\n", labelno);
		}
#else
#ifndef MESA
	pstab(ititle+1, N_SOL);
#endif
	if (gdebug) printf("0,0,LL%d\n", labelno);
#endif
	*cq = '"';
	/*printf("LL%d:\n", labelno++);*/

eq:	if (lineno == lastlineno) return;
	lastlineno = lineno;

	if (fdefflag) {
#ifdef STABDOT
		pstabdot(N_SLINE, lineno);
#else
		pstab(NULLNAME, N_SLINE);
		printf("0,%d,LL%d\n", lineno, labelno);
		printf("LL%d:\n", labelno++);
#endif
		}
	}
	
plcstab(level) {
	if (!gdebug) return;
#ifdef STABDOT
#ifndef MESA
	pstabdot(N_LBRAC, level);
#endif
#else
#ifndef MESA
	pstab(NULLNAME, N_LBRAC);
#endif
	printf("0,%d,LL%d\n", level, labelno);
	printf("LL%d:\n", labelno++);
#endif
	}
	
prcstab(level) {
	if (!gdebug) return;
#ifdef STABDOT
#ifndef MESA
	pstabdot(N_RBRAC, level);
#endif
#else
#ifndef MESA
	pstab(NULLNAME, N_RBRAC);
#endif
	printf("0,%d,LL%d\n", level, labelno);
	printf("LL%d:\n", labelno++);
#endif
	}
	
pfstab(sname) 
char *sname; {
	if (!gdebug) return;
#ifndef MESA
	pstab(sname, N_FUN);
#endif
#ifndef FLEXNAMES
	printf("0,%d,_%.7s\n", lineno, sname);
#else
	printf("0,%d,_%s\n", lineno, sname);
#endif
}

#ifndef ONEPASS
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
#endif
