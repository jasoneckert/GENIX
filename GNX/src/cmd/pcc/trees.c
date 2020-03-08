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
 * trees.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)trees.c	5.1 (NSC) 9/8/83";
# endif

/* jima 5/17/82: buildtree modified so fields are accessed from nearest byte */
/* jima 5/18/82: SREF bit set. */
/* jima 5/24/82: Floating point unimplemented msg _ to avoid asm error */
/* JZ   3/10/83: Reimplemented floating point */
/* JZ   5/5/83:  Added floating point X-support*/
/* JZ   5/6/83:  All constant collapsing is done by X-support programs except
		 if ns16000 in which case it is done in native mode.
		 Also prtdcon is commented out so that floating point
		 constants will now be generated in line. */
/* JZ  5/10/83:  Changed p2tree to do nothing in the case of FCON.  It was
		 previously defaulting and overwriting the NODE */
/* JZ  5/26/83:  Added constant collapsing of floating conditional
   		 expressions.  Added constant collapsing of && and || done
   		 carefully so that it will work even if 1 side is a variable.
		 */
/* JZ  6/7/83:   Fixed floating point conditionals to change to int.  Made
		 FCON have the value NONAME. Fixed conval to make the correct
		 checks.*/
/* JZ  6/8/83:   Fixed floating point ANDAND and OROR to work in all contexts
		 Does constant collapsing in simple case but not allways
		 when there are arbritrary levels of ops.  Need to change
		 to recognize all cases where it can collapse constants and
		 to recognize possibility of collapsing INT conditionals as
		 well. This is all in conval().  Perhaps this should be moved
		 to optim(). */
# include "mfile1"
	    /* corrections when in violation of lint */

/*	some special actions, used in finding the type of nodes */
# define NCVT 01
# define PUN 02
# define TYPL 04
# define TYPR 010
# define TYMATCH 040
# define LVAL 0100
# define CVTO 0200
# define CVTL 0400
# define CVTR 01000
# define PTMATCH 02000
# define OTHER 04000
# define NCVTR 010000

/* node conventions:

	NAME:	rval>0 is stab index for external
		rval<0 is -inlabel number
		lval is offset in bits
	ICON:	lval has the value
		rval has the STAB index, or - label number,
			if a name whose address is in the constant
		rval = NONAME means no name
	REG:	rval is reg. identification cookie

	*/

int bdebug = 0;
extern ddebug;

NODE *
buildtree( o, l, r ) register NODE *l, *r; {
	register NODE *p, *q;
	register actions;
	register opty;
	register struct symtab *sp;
	register NODE *lr, *ll;
	int i;
	extern int eprint();

# ifndef BUG1
	if( bdebug ) printf( "buildtree( %s, %o, %o )\n", opst[o], l, r );
# endif
	opty = optype(o);

	/* check for constants */

	if( opty == UTYPE && l->in.op == ICON){

		switch( o ){

		case NOT:
			if( hflag ) werror( "constant argument to NOT" );
		case UNARY MINUS:
		case COMPL:
			if( conval( l, o, l ) ) return(l);
			break;

			}
		}

	else if( opty ==UTYPE && l->in.op==FCON ){
		switch (o)
		{
		    case NOT:
			if (hflag) werror ("constant argument to NOT");
		    case UNARY MINUS:
		    	if (conval(l,o,l)) return(l);
		    	break;
		}
		
	}

	else if( o==QUEST && l->in.op==ICON ) {
		l->in.op = FREE;
		r->in.op = FREE;
		if( l->tn.lval )
		{
			tfree( r->in.right );
			return( r->in.left );
		}
		else {
			tfree( r->in.left );
			return( r->in.right );
			}
		}
	else if (o == QUEST && l->in.op == FCON)
	{
	    l->in.op = FREE;
	    r->in.op = FREE;
# if ns16000
	    if (l->fpn.dval)
# else
	    if (!(int)float_ceq(int_double(0),l->fpn.dval))
# endif
	    {
		tfree( r->in.right);
		return(r->in.left);
	    }
	    else
	    {
		tfree(r->in.left);
		return( r->in.right );
	    }
	}
	

	else if( (o==ANDAND || o==OROR) && (l->in.op==ICON||r->in.op==ICON||
		   l->in.op == FCON || r->in.op == FCON) )
	    goto ccwarn;

	else if( opty == BITYPE && (l->in.op == ICON || l->in.op == FCON)
		 && (r->in.op == ICON|| r->in.op == FCON) ){

		switch( o ){
		case CBRANCH:
		    if (l->in.op == FCON)
		    {
		        l->in.op = ICON;
# if ns16000
			l->tn.lval = l->fpn.dval? 1:0;
# else
			l->tn.lval =(int)float_ceq(int_double(0),l->fpn.dval)==0;
# endif
			l->in.type = INT;
			l->tn.rval = NONAME;
			break;
		    }

		case ULT:
		case UGT:
		case ULE:
		case UGE:
		case LT:
		case GT:
		case LE:
		case GE:
		case EQ:
		case NE:
		case ANDAND:
		case OROR:

		ccwarn:
			if( hflag ) werror( "constant in conditional context" );

		case PLUS:
		case MINUS:
		case MUL:
		case DIV:
		case MOD:
		case AND:
		case OR:
		case ER:
		case LS:
		case RS:
			if( conval( l, o, r ) ) {
				r->in.op = FREE;
				return(l);
				}
			break;
			}
		}

	else if( opty == BITYPE && (l->in.op==FCON||l->in.op==ICON) &&
		(r->in.op==FCON||r->in.op==ICON) ){
 		switch(o){
 		case PLUS:
 		case MINUS:
 		case MUL:
 		case DIV:
 			if( l->in.op == ICON ){
# if ns16000
 				l->fpn.dval = l->tn.lval;
# else
				l->fpn.dval = int_double(l->tn.lval);
# endif
 				}
 			if( r->in.op == ICON ){
# if ns16000
 				r->fpn.dval = r->tn.lval;
# else
				r->fpn.dval = int_double(r->tn.lval);
# endif;
 				}
 			l->in.op = FCON;
 			l->in.type = l->fn.csiz = DOUBLE;
 			r->in.op = FREE;
 			switch(o){
 			case PLUS:
#if ns16000
 				l->fpn.dval += r->fpn.dval;

#else
 				l->fpn.dval = float_add(l->fpn.dval,
					      r->fpn.dval);
#endif
				
 				return(l);
 			case MINUS:
#if ns16000
 				l->fpn.dval -= r->fpn.dval;
#else
 				l->fpn.dval = float_sub( l->fpn.dval,
				             r->fpn.dval);
#endif
			 	return(l);
 			case MUL:
#if ns16000
 				l->fpn.dval *= r->fpn.dval;
# else
				l->fpn.dval = float_mul(l->fpn.dval,r->fpn.dval);
#endif
 				return(l);
 			case DIV:
#if ns16000
  				if(r->fpn.dval == 0) uerror( "division by 0." );
 				else l->fpn.dval /= r->fpn.dval;
#else
				if ((int)float_ceq(int_double(0),r->fpn.dval))
				    uerror( "division by 0." ); 	
				else l->fpn.dval = float_div(l->fpn.dval,r->fpn.dval);
#endif
 				return(l);
 				}
 			}
		}

	/* its real; we must make a new node */

	p = block( o, l, r, INT, 0, INT );

	actions = opact(p);

	if( actions&LVAL ){ /* check left descendent */
		if( notlval(p->in.left) ) {
			uerror( "illegal lhs of assignment operator" );
			}
		}

	if( actions & NCVTR ){
		p->in.left = pconvert( p->in.left );
		}
	else if( !(actions & NCVT ) ){
		switch( opty ){

		case BITYPE:
			p->in.right = pconvert( p->in.right );
		case UTYPE:
			p->in.left = pconvert( p->in.left );

			}
		}

	if( (actions&PUN) && (o!=CAST||cflag) ){
		chkpun(p);
		}

	if( actions & (TYPL|TYPR) ){

		q = (actions&TYPL) ? p->in.left : p->in.right;

		p->in.type = q->in.type;
		p->fn.cdim = q->fn.cdim;
		p->fn.csiz = q->fn.csiz;
		}

	if( actions & CVTL ) p = convert( p, CVTL );
	if( actions & CVTR ) p = convert( p, CVTR );
	if( actions & TYMATCH ) p = tymatch(p);
	if( actions & PTMATCH ) p = ptmatch(p);

	if( actions & OTHER ){
		l = p->in.left;
		r = p->in.right;

		switch(o){

		case NAME:
			sp = &stab[idname];
			/* The SREF bit was not used in PDP11 version.     */
			/*  Use added 5/19/82 (jima).  See ejobcode.       */
		        sp->sflags |= SREF;    /* show item is referenced. */
			if( sp->stype == UNDEF ){
#ifndef FLEXNAMES
				uerror( "%.8s undefined", sp->sname );
#else
				uerror( "%s undefined", sp->sname );
#endif
				/* make p look reasonable */
				p->in.type = p->fn.cdim = p->fn.csiz = INT;
				p->tn.rval = idname;
				p->tn.lval = 0;
				defid( p, SNULL );
				break;
				}
			p->in.type = sp->stype;
			p->fn.cdim = sp->dimoff;
			p->fn.csiz = sp->sizoff;
			p->tn.lval = 0;
			p->tn.rval = idname;
			/* special case: MOETY is really an ICON... */
			if( p->in.type == MOETY ){
				p->tn.rval = NONAME;
				p->tn.lval = sp->offset;
				p->fn.cdim = 0;
				p->in.type = ENUMTY;
				p->in.op = ICON;
				}
			break;

		case ICON:
			p->in.type = INT;
			p->fn.cdim = 0;
			p->fn.csiz = INT;
			break;

		case STRING:
			p->in.op = NAME;
			p->in.type = CHAR+ARY;
			p->tn.lval = 0;
			p->tn.rval = NOLAB;
			p->fn.cdim = curdim;
			p->fn.csiz = CHAR;
			break;

		case FCON:
			p->tn.lval = 0;
			p->tn.rval = NONAME; /* Constants have no name */
			p->in.type = DOUBLE;
			p->fn.cdim = 0;
			p->fn.csiz = DOUBLE;
			break;

		case STREF:
			/* p->x turned into *(p+offset) */
			/* rhs must be a name; check correctness */

			i = r->tn.rval;
			if( i<0 || ((sp= &stab[i])->sclass != MOS && sp->sclass != MOU && !(sp->sclass&FIELD)) ){
				uerror( "member of structure or union required" );
				}else
			/* if this name is non-unique, find right one */
			if( stab[i].sflags & SNONUNIQ &&
				(l->in.type==PTR+STRTY || l->in.type == PTR+UNIONTY) &&
				(l->fn.csiz +1) >= 0 ){
				/* nonunique name && structure defined */
				char * memnam, * tabnam;
				register k;
				int j;
				int memi;
				j=dimtab[l->fn.csiz+1];
				for( ; (memi=dimtab[j]) >= 0; ++j ){
					tabnam = stab[memi].sname;
					memnam = stab[i].sname;
# ifndef BUG1
					if( ddebug>1 ){
#ifndef FLEXNAMES
						printf("member %.8s==%.8s?\n",
#else
						printf("member %s==%s?\n",
#endif
							memnam, tabnam);
						}
# endif
					if( stab[memi].sflags & SNONUNIQ ){
#ifndef FLEXNAMES
						for( k=0; k<NCHNAM; ++k ){
							if(*memnam++!=*tabnam)
								goto next;
							if(!*tabnam++) break;
							}
#else
						if (memnam != tabnam)
							goto next;
#endif
						r->tn.rval = i = memi;
						break;
						}
					next: continue;
					}
				if( memi < 0 )
#ifndef FLEXNAMES
					uerror("illegal member use: %.8s",
#else
					uerror("illegal member use: %s",
#endif
						stab[i].sname);
				}
			else {
				register j;
				if( l->in.type != PTR+STRTY && l->in.type != PTR+UNIONTY ){
					if( stab[i].sflags & SNONUNIQ ){
						uerror( "nonunique name demands struct/union or struct/union pointer" );
						}
					else werror( "struct/union or struct/union pointer required" );
					}
				else if( (j=l->fn.csiz+1)<0 ) cerror( "undefined structure or union" );
				else if( !chkstr( i, dimtab[j], DECREF(l->in.type) ) ){
#ifndef FLEXNAMES
					werror( "illegal member use: %.8s", stab[i].sname );
#else
					werror( "illegal member use: %s", stab[i].sname );
#endif
					}
				}

			p = stref( p );
			break;

		case UNARY MUL:
			if( l->in.op == UNARY AND ){
				p->in.op = l->in.op = FREE;
				p = l->in.left;
				}
			if( !ISPTR(l->in.type))uerror("illegal indirection");
			p->in.type = DECREF(l->in.type);
			p->fn.cdim = l->fn.cdim;
			p->fn.csiz = l->fn.csiz;
			break;

		case UNARY AND:
			switch( l->in.op ){

			case UNARY MUL:
				p->in.op = l->in.op = FREE;
				p = l->in.left;
			case NAME:
				p->in.type = INCREF( l->in.type );
				p->fn.cdim = l->fn.cdim;
				p->fn.csiz = l->fn.csiz;
				break;

			case COMOP:
				lr = buildtree( UNARY AND, l->in.right, NIL );
				p->in.op = l->in.op = FREE;
				p = buildtree( COMOP, l->in.left, lr );
				break;

			case QUEST:
				lr = buildtree( UNARY AND, l->in.right->in.right, NIL );
				ll = buildtree( UNARY AND, l->in.right->in.left, NIL );
				p->in.op = l->in.op = l->in.right->in.op = FREE;
				p = buildtree( QUEST, l->in.left, buildtree( COLON, ll, lr ) );
				break;

# ifdef ADDROREG
			case OREG:
				/* OREG was built in clocal()
				 * for an auto or formal parameter
				 * now its address is being taken
				 * local code must unwind it
				 * back to PLUS/MINUS REG ICON
				 * according to local conventions
				 */
				{
				extern NODE * addroreg();
				p->in.op = FREE;
				p = addroreg( l );
				}
				break;

# endif
			default:
				uerror( "unacceptable operand of &" );
				break;
				}
			break;

		case LS:
		case RS:
		case ASG LS:
		case ASG RS:
			if(tsize(p->in.right->in.type, p->in.right->fn.cdim, p->in.right->fn.csiz) > SZINT)
				p->in.right = makety(p->in.right, INT, 0, INT );
			break;

		case RETURN:
		case ASSIGN:
		case CAST:
			/* structure assignment */
			/* take the addresses of the two sides; then make an
			/* operator using STASG and
			/* the addresses of left and right */

			{
				register TWORD t;
				register d, s;

				if( l->fn.csiz != r->fn.csiz ) uerror( "assignment of different structures" );

				r = buildtree( UNARY AND, r, NIL );
				t = r->in.type;
				d = r->fn.cdim;
				s = r->fn.csiz;

				l = block( STASG, l, r, t, d, s );

				if( o == RETURN ){
					p->in.op = FREE;
					p = l;
					break;
					}

				p->in.op = UNARY MUL;
				p->in.left = l;
				p->in.right = NIL;
				break;
				}
		case COLON:
			/* structure colon */

			if( l->fn.csiz != r->fn.csiz ) uerror( "type clash in conditional" );
			break;

		case CALL:
			p->in.right = r = strargs( p->in.right );
		case UNARY CALL:
			if( !ISPTR(l->in.type)) uerror("illegal function");
			p->in.type = DECREF(l->in.type);
			if( !ISFTN(p->in.type)) uerror("illegal function");
			p->in.type = DECREF( p->in.type );
			p->fn.cdim = l->fn.cdim;
			p->fn.csiz = l->fn.csiz;
			if( l->in.op == UNARY AND && l->in.left->in.op == NAME &&
				l->in.left->tn.rval >= 0 && l->in.left->tn.rval != NONAME &&
				( (i=stab[l->in.left->tn.rval].sclass) == FORTRAN || i==UFORTRAN ) ){
				p->in.op += (FORTCALL-CALL);
				}
			if( p->in.type == STRTY || p->in.type == UNIONTY ){
				/* function returning structure */
				/*  make function really return ptr to str., with * */

				p->in.op += STCALL-CALL;
				p->in.type = INCREF( p->in.type );
				p = buildtree( UNARY MUL, p, NIL );

				}
			break;

		default:
			cerror( "other code %d", o );
			}

		}

	if( actions & CVTO ) p = oconvert(p);
	p = clocal(p);

# ifndef BUG1
	if( bdebug ) fwalk( p, eprint, 0 );
# endif

	return(p);

	}

NODE *
strargs( p ) register NODE *p;  { /* rewrite structure flavored arguments */

	if( p->in.op == CM ){
		p->in.left = strargs( p->in.left );
		p->in.right = strargs( p->in.right );
		return( p );
		}

	if( p->in.type == STRTY || p->in.type == UNIONTY ){
		p = block( STARG, p, NIL, p->in.type, p->fn.cdim, p->fn.csiz );
		p->in.left = buildtree( UNARY AND, p->in.left, NIL );
		p = clocal(p);
		}
	return( p );
	}

chkstr( i, j, type ) TWORD type; {
	/* is the MOS or MOU at stab[i] OK for strict reference by a ptr */
	/* i has been checked to contain a MOS or MOU */
	/* j is the index in dimtab of the members... */
	int k, kk;

	extern int ddebug;

# ifndef BUG1
#ifndef FLEXNAMES
	if( ddebug > 1 ) printf( "chkstr( %.8s(%d), %d )\n", stab[i].sname, i, j );
#else
	if( ddebug > 1 ) printf( "chkstr( %s(%d), %d )\n", stab[i].sname, i, j );
#endif
# endif
	if( (k = j) < 0 ) uerror( "undefined structure or union" );
	else {
		for( ; (kk = dimtab[k] ) >= 0; ++k ){
			if( kk >= SYMTSZ ){
				cerror( "gummy structure" );
				return(1);
				}
			if( kk == i ) return( 1 );
			switch( stab[kk].stype ){

			case STRTY:
			case UNIONTY:
				if( type == STRTY ) continue;  /* no recursive looking for strs */
				if( hflag && chkstr( i, dimtab[stab[kk].sizoff+1], stab[kk].stype ) ){
					if( stab[kk].sname[0] == '$' ) return(0);  /* $FAKE */
					werror(
#ifndef FLEXNAMES
					"illegal member use: perhaps %.8s.%.8s?",
#else
					"illegal member use: perhaps %s.%s?",
#endif
					stab[kk].sname, stab[i].sname );
					return(1);
					}
				}
			}
		}
	return( 0 );
	}

conval( p, o, q ) register NODE *p, *q; {
	/* apply the op o to the lval part of p; if binary, rhs is val */
	/* Floating point and && and || added 5/26 by JZ */
	int i, u;
	CONSZ val;
# if ns16000
	double fval;
# else
	quad_wrd fval;
# endif
	if (p->in.op == FCON || q->in.op == FCON)
	{
		
 			if( p->in.op == ICON ){
# if ns16000
 				p->fpn.dval = p->tn.lval;
# else
				p->fpn.dval = int_double(p->tn.lval);
# endif
 				}
 			if( q->in.op == ICON ){
# if ns16000
 				q->fpn.dval =q->tn.lval;
# else
				q->fpn.dval = int_double(q->tn.lval);
# endif;
 			}
	    if (p->tn.op== ICON)
	    {
 	    	p->in.op = FCON;
 	    	p->in.type = p->fn.csiz = DOUBLE;
	    }
 	    if (q->tn.op== ICON) q->in.op = FCON;

	    fval = q->fpn.dval;
	    switch(o)
	    {
		case PLUS:
# if ns16000
		    p->fpn.dval += fval;
# else
		    p->fpn.dval = float_add( p->fpn.dval,fval);
# endif
		    break;

		case MINUS:
# if ns16000
		    p->fpn.dval -= fval;
# else
		    p->fpn.dval = float_sub( p->fpn.dval,fval);
# endif
		    break;

		case MUL:
# if ns16000
		    p->fpn.dval *= fval;
# else
		    p->fpn.dval = float_mul( p->fpn.dval,fval);
# endif
		    break;

		case DIV:
		    if
# if ns16000			/* which part of the if is done depends on
				   the machine we are using */
		      (fval == 0) uerror ( "division by zero");
		    else  p->fpn.dval /= fval;
# else				/* running cross support */
		      (float_ceq(int_double(0),fval)) uerror ("division by zero");
		    else  p->fpn.dval = float_div( p->fpn.dval,fval);
# endif
		    break;


	        case UNARY MINUS:
#if ns16000
		    p->fpn.dval = - p->fpn.dval;
# else
		    p->fpn.dval = float_neg(p->fpn.dval);
# endif	
		    break;

		case NOT:
# if ns16000
		    p->tn.lval = !p->fpn.dval;
# else
		    p->tn.lval = (int)float_ceq( int_double(0),p->fpn.dval);
# endif
		    p->in.op = ICON;
		    p->in.type = INT;
		    break;
		/*  REMEMBER the order of evaluation is guaranteed so we    */
		/*  can NOT optimise by changing a || 1 into 1,  since "a"  */
		/*  might cause a side effect or might be an I. O. register.*/

		case OROR:	/* The cases of constant collapsing include: */
				/* 1. There are 2 variables and no constants.*/
				/*    In this case we just return */
				/* 2. One op is a constant and one a variable*/
				/*    a. The value of the constant is 0     */
				/*       or the second op is the constant.  */
				/*       Here we change the constant to an  */
				/*       int so it can be recognized by     */
				/*       setexpr.			    */
				/*    b. If the first op is non 0 then the  */
				/*       value of the expression is 1       */
				/* 3. If both ops are constants the value of*/
				/*    the expression is of course the || of */
				/*    the 2 constants */
		    
		    if (p->tn.op != FCON|| q->tn.op != FCON)
		    {
		    	int notconval; /* is dval non zero? */
		        NODE *tmp;
		    	if (q->tn.op != FCON&& p->tn.op != FCON) return(0);
			tmp = q->tn.op == FCON? q:p;
		        
# if ns16000
			
		    	if ( (notconval = !tmp->fpn.dval)|| q == tmp) 
# else
			if ((notconval=(int)float_ceq(int_double(0),
			     tmp->fpn.dval))|| q == tmp)
# endif
		    	{/* Change the type to INT so setexpr is happy */
		    	    tmp->tn.lval = !notconval;
			    tmp->in.op = ICON;
			    tmp->in.type = INT;
			    return 0;
		    	}
		    }
# if ns16000
		    p->tn.lval = (p->fpn.dval)? 1: (fval|| 0) ;
		     
		    
# else
		    if( !(int)float_ceq( int_double(0),p->fpn.dval))
   		         p->tn.lval = 1;
		    else p->tn.lval =  !(int)float_ceq(int_double(0),fval) ;
# endif

		    p->in.op = ICON;
		    p->in.type = INT;/* conditional expressions are always int 
		    			no matter what their operands */
		    break;

		case ANDAND:	/* The comment for OROR aplies here with the */
				/* semantics changed to apply to && */
		    if (p->tn.op != FCON|| q->tn.op != FCON)
		    {
		        NODE *tmp;
			int condval;/* 1 if dval is nonzero 0 otherwise */
		    	if (p->tn.op != FCON && q->tn.op != FCON) return(0);
			tmp = p->tn.op == FCON? p : q;
# if ns16000
		    	if ((condval =tmp->fpn.dval)|| tmp == q)
# else
			if ((condval= !float_ceq(int_double(0),tmp->fpn.dval))
			    || tmp ==q)
# endif
		    	{/* Change the type to INT so setexpr is happy */
		    	    tmp->tn.lval = condval;
			    tmp->in.op = ICON;
			    tmp->in.type = INT;
			    return 0;
		    	}
		    }
# if ns16000
		    p->tn.lval = p->fpn.dval? (fval || 0) : 0;
		     
		    
# else
		    if( (int)float_ceq( int_double(0),p->fpn.dval))
   		         p->tn.lval = 0;
		    else p->tn.lval =  !(int)float_ceq(int_double(0),fval) ;
# endif

		    p->in.op = ICON;
		    p->in.type = INT;/* conditional expressions are always int 
		    			no matter what their operands */
		    break;		    
		case LT:
# if ns16000
		    p->tn.lval = p->fpn.dval < fval;
# else
		    p->tn.lval = (int)float_clt( p->fpn.dval,fval);
# endif
		    p->in.op = ICON;
		    p->in.op = INT;
		    break;
		case LE:
# if ns16000
		    p->tn.lval = p->fpn.dval <= fval;
# else
		    p->tn.lval = (int)float_cle( p->fpn.dval,fval);
# endif
		    p->in.op = ICON;
		    p->in.type = INT;
		    break;

		case GT:
# if ns16000
		    p->tn.lval = p->fpn.dval >fval;
# else
		    p->tn.lval = (int)float_cgt( p->fpn.dval,fval);
# endif
		    p->in.op = ICON;
		    p->in.type = INT;
		    break;
		case GE:

# if ns16000
		    p->tn.lval = p->fpn.dval >= fval;
# else
		    p->tn.lval = (int)float_cge( p->fpn.dval,fval);
# endif
		    p->in.op = ICON;
		    p->in.type = INT;
		    break;

		case EQ:
# if ns16000
		    p->tn.lval = p->fpn.dval == fval;
# else
		    p->tn.lval = (int)float_ceq( p->fpn.dval,fval);
# endif
		    p->in.op = ICON;
		    p->in.type = INT;
		    break;
		case NE:
# if ns16000
		    p->tn.lval = p->fpn.dval != fval;
# else
		    p->tn.lval = !(int)float_ceq( p->fpn.dval,fval);
# endif
		    p->in.op = ICON;
		    p->in.type = INT;
		    break;
		default:
		    return 0;
	    }
	}
	
	else
	{
	    val = q->tn.lval;
	    u = ISUNSIGNED(p->in.type) || ISUNSIGNED(q->in.type);
	    if( u && (o==LE||o==LT||o==GE||o==GT)) o += (UGE-GE);

	    if( p->tn.rval != NONAME && q->tn.rval != NONAME ) return(0);
	    if( q->tn.rval != NONAME && o!=PLUS ) return(0);
    	    if( p->tn.rval != NONAME && o!=PLUS && o!=MINUS ) return(0);

	    switch( o ){

	    case PLUS:
		p->tn.lval += val;
		if( p->tn.rval == NONAME ){
			p->tn.rval = q->tn.rval;
			p->in.type = q->in.type;
			}
		break;
	    case MINUS:
		p->tn.lval -= val;
		break;
	    case MUL:
		p->tn.lval *= val;
		break;
	    case DIV:
		if( val == 0 ) uerror( "division by 0" );
		else p->tn.lval /= val;
		break;
	    case MOD:
		if( val == 0 ) uerror( "division by 0" );
		else p->tn.lval %= val;
		break;
	    case AND:
		p->tn.lval &= val;
		break;
	    case OR:
		p->tn.lval |= val;
		break;
	    case ER:
		p->tn.lval ^=  val;
		break;
	    case LS:
		i = val;
		p->tn.lval = p->tn.lval << i;
		break;
	    case RS:
		i = val;
		p->tn.lval = p->tn.lval >> i;
		break;

	    case UNARY MINUS:
		p->tn.lval = - p->tn.lval;
		break;
	    case COMPL:
		p->tn.lval = ~p->tn.lval;
		break;
	    case NOT:
		p->tn.lval = !p->tn.lval;
		break;
	    case OROR:
		if (p->in.op != ICON) return(0);
		if (p->tn.lval) p->tn.lval = 1;
		else if (q->tn.op != ICON)return(0);
		else  p->tn.lval = p->tn.lval || val;
		break;
	    case ANDAND:
	    	if (p-> in.op != ICON)return (0);
		if ( !p->tn.lval)p->tn.lval =0;
		else if (q->tn.op != ICON) return(0);
		else    p->tn.lval = p->tn.lval && val;
		break;
	    case LT:
		p->tn.lval = p->tn.lval < val;
		break;

	    case LE:
		p->tn.lval = p->tn.lval <= val;
		break;
	    case GT:
		p->tn.lval = p->tn.lval > val;
		break;
	    case GE:
		p->tn.lval = p->tn.lval >= val;
		break;
	    case ULT:
		p->tn.lval = (p->tn.lval-val)<0;
		break;
	    case ULE:
		p->tn.lval = (p->tn.lval-val)<=0;
		break;
	    case UGE:
		p->tn.lval = (p->tn.lval-val)>=0;
		break;
	    case UGT:
		p->tn.lval = (p->tn.lval-val)>0;
		break;
	    case EQ:
		p->tn.lval = p->tn.lval == val;
		break;
	    case NE:
		p->tn.lval = p->tn.lval != val;
		break;
	    default:
		return(0);
		}
    }
    return(1);

}
	
chkpun(p) register NODE *p; {

	/* checks p for the existance of a pun */

	/* this is called when the op of p is ASSIGN, RETURN, CAST, COLON, or relational */

	/* one case is when enumerations are used: this applies only to lint */
	/* in the other case, one operand is a pointer, the other integer type */
	/* we check that this integer is in fact a constant zero... */

	/* in the case of ASSIGN, any assignment of pointer to integer is illegal */
	/* this falls out, because the LHS is never 0 */

	register NODE *q;
	register t1, t2;
	register d1, d2;

	t1 = p->in.left->in.type;
	t2 = p->in.right->in.type;

	if( t1==ENUMTY || t2==ENUMTY ) { /* check for enumerations */
		if( logop( p->in.op ) && p->in.op != EQ && p->in.op != NE ) {
			uerror( "illegal comparison of enums" );
			return;
			}
		if( t1==ENUMTY && t2==ENUMTY && p->in.left->fn.csiz==p->in.right->fn.csiz ) return;
		werror( "enumeration type clash, operator %s", opst[p->in.op] );
		return;
		}

	if( ISPTR(t1) || ISARY(t1) ) q = p->in.right;
	else q = p->in.left;

	if( !ISPTR(q->in.type) && !ISARY(q->in.type) ){
		if( q->in.op != ICON || q->tn.lval != 0 ){
			werror( "illegal combination of pointer and integer, op %s",
				opst[p->in.op] );
			}
		}
	else {
		d1 = p->in.left->fn.cdim;
		d2 = p->in.right->fn.cdim;
		for( ;; ){
			if( t1 == t2 ) {;
				if( p->in.left->fn.csiz != p->in.right->fn.csiz ) {
					werror( "illegal structure pointer combination" );
					}
				return;
				}
			if( ISARY(t1) || ISPTR(t1) ){
				if( !ISARY(t2) && !ISPTR(t2) ) break;
				if( ISARY(t1) && ISARY(t2) && dimtab[d1] != dimtab[d2] ){
					werror( "illegal array size combination" );
					return;
					}
				if( ISARY(t1) ) ++d1;
				if( ISARY(t2) ) ++d2;
				}
			else break;
			t1 = DECREF(t1);
			t2 = DECREF(t2);
			}
		werror( "illegal pointer combination" );
		}

	}

NODE *
stref( p ) register NODE *p; {

	TWORD t;
	int d, s, dsc, align;
	OFFSZ off;
	register struct symtab *q;

	/* make p->x */
	/* this is also used to reference automatic variables */

	q = &stab[p->in.right->tn.rval];
	p->in.right->in.op = FREE;
	p->in.op = FREE;
	p = pconvert( p->in.left );

	/* make p look like ptr to x */

	if( !ISPTR(p->in.type)){
		p->in.type = PTR+UNIONTY;
		}

	t = INCREF( q->stype );
	d = q->dimoff;
	s = q->sizoff;

	p = makety( p, t, d, s );

	/* compute the offset to be added */

	off = q->offset;
	dsc = q->sclass;

	if( dsc & FIELD ) {  /* normalize offset */
	        /* access fields from nearest preceeding byte, for inss/exss */
		/*   instructions - jima 5/17/82 */
		align = ALCHAR;  /* was: align = ALINT; (jima)*/
		s = INT;
		off = (off/align)*align;
		}
	if( off != 0 ) p = clocal( block( PLUS, p, offcon( off, t, d, s ), t, d, s ) );

	p = buildtree( UNARY MUL, p, NIL );

	/* if field, build field info */

	if( dsc & FIELD ){
		p = block( FLD, p, NIL, q->stype, 0, q->sizoff );
		p->tn.rval = PKFIELD( dsc&FLDSIZ, q->offset%align );
		}

	return( clocal(p) );
	}

notlval(p) register NODE *p; {

	/* return 0 if p an lvalue, 1 otherwise */

	again:

	switch( p->in.op ){

	case FLD:
		p = p->in.left;
		goto again;

	case UNARY MUL:
		/* fix the &(a=b) bug, given that a and b are structures */
		if( p->in.left->in.op == STASG ) return( 1 );
		/* and the f().a bug, given that f returns a structure */
		if( p->in.left->in.op == UNARY STCALL ||
		    p->in.left->in.op == STCALL ) return( 1 );
	case NAME:
	case OREG:
		if( ISARY(p->in.type) || ISFTN(p->in.type) ) return(1);
	case REG:
		return(0);

	default:
		return(1);

		}

	}

NODE *
bcon( i ){ /* make a constant node with value i */
	register NODE *p;

	p = block( ICON, NIL, NIL, INT, 0, INT );
	p->tn.lval = i;
	p->tn.rval = NONAME;
	return( clocal(p) );
	}

NODE *
bpsize(p) register NODE *p; {
	return( offcon( psize(p), p->in.type, p->fn.cdim, p->fn.csiz ) );
	}

OFFSZ
psize( p ) NODE *p; {
	/* p is a node of type pointer; psize returns the
	   size of the thing pointed to */

	if( !ISPTR(p->in.type) ){
		uerror( "pointer required");
		return( SZINT );
		}
	/* note: no pointers to fields */
	return( tsize( DECREF(p->in.type), p->fn.cdim, p->fn.csiz ) );
	}

NODE *
convert( p, f )  register NODE *p; {
	/*  convert an operand of p
	    f is either CVTL or CVTR
	    operand has type int, and is converted by the size of the other side
	    */

	register NODE *q, *r;

	q = (f==CVTL)?p->in.left:p->in.right;

	r = block( PMCONV,
		q, bpsize(f==CVTL?p->in.right:p->in.left), INT, 0, INT );
	r = clocal(r);
	if( f == CVTL )
		p->in.left = r;
	else
		p->in.right = r;
	return(p);

	}

econvert( p ) register NODE *p; {

	/* change enums to ints, or appropriate types */

	register TWORD ty;

	if( (ty=BTYPE(p->in.type)) == ENUMTY || ty == MOETY ) {
		if( dimtab[ p->fn.csiz ] == SZCHAR ) ty = CHAR;
		else if( dimtab[ p->fn.csiz ] == SZINT ) ty = INT;
		else if( dimtab[ p->fn.csiz ] == SZSHORT ) ty = SHORT;
		else ty = LONG;
		ty = ctype( ty );
		p->fn.csiz = ty;
		MODTYPE(p->in.type,ty);
		if( p->in.op == ICON && ty != LONG ) p->in.type = p->fn.csiz = INT;
		}
	}

NODE *
pconvert( p ) register NODE *p; {

	/* if p should be changed into a pointer, do so */

	if( ISARY( p->in.type) ){
		p->in.type = DECREF( p->in.type );
		++p->fn.cdim;
		return( buildtree( UNARY AND, p, NIL ) );
		}
	if( ISFTN( p->in.type) )
		return( buildtree( UNARY AND, p, NIL ) );

	return( p );
	}

NODE *
oconvert(p) register NODE *p; {
	/* convert the result itself: used for pointer and unsigned */

	switch(p->in.op) {

	case LE:
	case LT:
	case GE:
	case GT:
		if( ISUNSIGNED(p->in.left->in.type) || ISUNSIGNED(p->in.right->in.type) )  p->in.op += (ULE-LE);
	case EQ:
	case NE:
		return( p );

	case MINUS:
		return(  clocal( block( PVCONV,
			p, bpsize(p->in.left), INT, 0, INT ) ) );
		}

	cerror( "illegal oconvert: %d", p->in.op );

	return(p);
	}

NODE *
ptmatch(p)  register NODE *p; {

	/* makes the operands of p agree; they are
	   either pointers or integers, by this time */
	/* with MINUS, the sizes must be the same */
	/* with COLON, the types must be the same */

	TWORD t1, t2, t;
	int o, d2, d, s2, s;

	o = p->in.op;
	t = t1 = p->in.left->in.type;
	t2 = p->in.right->in.type;
	d = p->in.left->fn.cdim;
	d2 = p->in.right->fn.cdim;
	s = p->in.left->fn.csiz;
	s2 = p->in.right->fn.csiz;

	switch( o ){

	case ASSIGN:
	case RETURN:
	case CAST:
		{  break; }

	case MINUS:
		{  if( psize(p->in.left) != psize(p->in.right) ){
			uerror( "illegal pointer subtraction");
			}
		   break;
		   }
	case COLON:
		{  if( t1 != t2 ) uerror( "illegal types in :");
		   break;
		   }
	default:  /* must work harder: relationals or comparisons */

		if( !ISPTR(t1) ){
			t = t2;
			d = d2;
			s = s2;
			break;
			}
		if( !ISPTR(t2) ){
			break;
			}

		/* both are pointers */
		if( talign(t2,s2) < talign(t,s) ){
			t = t2;
			s = s2;
			}
		break;
		}

	p->in.left = makety( p->in.left, t, d, s );
	p->in.right = makety( p->in.right, t, d, s );
	if( o!=MINUS && !logop(o) ){

		p->in.type = t;
		p->fn.cdim = d;
		p->fn.csiz = s;
		}

	return(clocal(p));
	}

int tdebug = 0;

NODE *
tymatch(p)  register NODE *p; {

	/* satisfy the types of various arithmetic binary ops */

	/* rules are:
		if assignment, op, type of LHS
		if any float or doubles, make double
		if any longs, make long
		otherwise, make int
		if either operand is unsigned, the result is...
	*/

	register TWORD t1, t2, t, tu;
	register o, u;

	o = p->in.op;

	t1 = p->in.left->in.type;
	t2 = p->in.right->in.type;
	if( (t1==UNDEF || t2==UNDEF) && o!=CAST )
		uerror("void type illegal in expression");

	u = 0;
	if( ISUNSIGNED(t1) ){
		u = 1;
		t1 = DEUNSIGN(t1);
		}
	if( ISUNSIGNED(t2) ){
		u = 1;
		t2 = DEUNSIGN(t2);
		}

	if( ( t1 == CHAR || t1 == SHORT ) && o!= RETURN ) t1 = INT;
	if( t2 == CHAR || t2 == SHORT ) t2 = INT;

	if( t1==DOUBLE || t1==FLOAT || t2==DOUBLE || t2==FLOAT ) t = DOUBLE;
	else if( t1==LONG || t2==LONG ) t = LONG;
	else t = INT;

	if( asgop(o) ){
		tu = p->in.left->in.type;
		t = t1;
		}
	else {
		tu = (u && UNSIGNABLE(t))?ENUNSIGN(t):t;
		}

	/* because expressions have values that are at least as wide
	   as INT or UNSIGNED, the only conversions needed
	   are those involving FLOAT/DOUBLE, and those
	   from LONG to INT and ULONG to UNSIGNED */

	if( t != t1 ) p->in.left = makety( p->in.left, tu, 0, (int)tu );

	if( t != t2 || o==CAST ) p->in.right = makety( p->in.right, tu, 0, (int)tu );

	if( asgop(o) ){
		p->in.type = p->in.left->in.type;
		p->fn.cdim = p->in.left->fn.cdim;
		p->fn.csiz = p->in.left->fn.csiz;
		}
	else if( !logop(o) ){
		p->in.type = tu;
		p->fn.cdim = 0;
		p->fn.csiz = t;
		}

# ifndef BUG1
	if( tdebug ) printf( "tymatch(%o): %o %s %o => %o\n",p,t1,opst[o],t2,tu );
# endif

	return(p);
	}

NODE *
makety( p, t, d, s ) register NODE *p; TWORD t; {
	/* make p into type t by inserting a conversion */

	if( p->in.type == ENUMTY && p->in.op == ICON ) econvert(p);
	if( t == p->in.type ){
		p->fn.cdim = d;
		p->fn.csiz = s;
		return( p );
		}

	if( t & TMASK ){
		/* non-simple type */
		return( block( PCONV, p, NIL, t, d, s ) );
		}

	if( p->in.op == ICON ){
		if( t==DOUBLE||t==FLOAT ){
			p->in.op = FCON;
			if( ISUNSIGNED(p->in.type) ){
#if ns16000
				p->fpn.dval = /* (unsigned CONSZ) */ p->tn.lval;
#else
				p->fpn.dval = int_double(p->tn.lval);
#endif
				}
			else {
#if ns16000
				p->fpn.dval = p->tn.lval;
#else 
				p->fpn.dval = int_double(p->tn.lval);
#endif
				}

			p->in.type = p->fn.csiz = t;
			return( clocal(p) );
			}
		}

	return( block( SCONV, p, NIL, t, d, s ) );

	}

NODE *
block( o, l, r, t, d, s ) register NODE *l, *r; TWORD t; {

	register NODE *p;

	p = talloc();
	p->in.op = o;
	p->in.left = l;
	p->in.right = r;
	p->in.type = t;
	p->fn.cdim = d;
	p->fn.csiz = s;
	return(p);
	}

icons(p) register NODE *p; {
	/* if p is an integer constant, return its value */
	int val;

	if( p->in.op != ICON ){
		uerror( "constant expected");
		val = 1;
		}
	else {
		val = p->tn.lval;
		if( val != p->tn.lval ) uerror( "constant too big for cross-compiler" );
		}
	tfree( p );
	return(val);
	}

/* 	the intent of this table is to examine the
	operators, and to check them for
	correctness.

	The table is searched for the op and the
	modified type (where this is one of the
	types INT (includes char and short), LONG,
	DOUBLE (includes FLOAT), and POINTER

	The default action is to make the node type integer

	The actions taken include:
		PUN	  check for puns
		CVTL	  convert the left operand
		CVTR	  convert the right operand
		TYPL	  the type is determined by the left operand
		TYPR	  the type is determined by the right operand
		TYMATCH	  force type of left and right to match, by inserting conversions
		PTMATCH	  like TYMATCH, but for pointers
		LVAL	  left operand must be lval
		CVTO	  convert the op
		NCVT	  do not convert the operands
		OTHER	  handled by code
		NCVTR	  convert the left operand, not the right...

	*/

# define MINT 01  /* integer */
# define MDBI 02   /* integer or double */
# define MSTR 04  /* structure */
# define MPTR 010  /* pointer */
# define MPTI 020  /* pointer or integer */
# define MENU 040 /* enumeration variable or member */

opact( p )  NODE *p; {

	register mt12, mt1, mt2, o;

	mt12 = 0;

	switch( optype(o=p->in.op) ){

	case BITYPE:
		mt12=mt2 = moditype( p->in.right->in.type );
	case UTYPE:
		mt12 &= (mt1 = moditype( p->in.left->in.type ));

		}

	switch( o ){

	case NAME :
	case STRING :
	case ICON :
	case FCON :
	case CALL :
	case UNARY CALL:
	case UNARY MUL:
		{  return( OTHER ); }
	case UNARY MINUS:
		if( mt1 & MDBI ) return( TYPL );
		break;

	case COMPL:
		if( mt1 & MINT ) return( TYPL );
		break;

	case UNARY AND:
		{  return( NCVT+OTHER ); }
	case INIT:
	case CM:
	case NOT:
	case CBRANCH:
	case ANDAND:
	case OROR:
		return( 0 );

	case MUL:
	case DIV:
		if( mt12 & MDBI ) return( TYMATCH );
		break;

	case MOD:
	case AND:
	case OR:
	case ER:
		if( mt12 & MINT ) return( TYMATCH );
		break;

	case LS:
	case RS:
		if( mt12 & MINT ) return( TYMATCH+OTHER );
		break;

	case EQ:
	case NE:
	case LT:
	case LE:
	case GT:
	case GE:
		if( (mt1&MENU)||(mt2&MENU) ) return( PTMATCH+PUN+NCVT );
		if( mt12 & MDBI ) return( TYMATCH+CVTO );
		else if( mt12 & MPTR ) return( PTMATCH+PUN );
		else if( mt12 & MPTI ) return( PTMATCH+PUN );
		else break;

	case QUEST:
	case COMOP:
		if( mt2&MENU ) return( TYPR+NCVTR );
		return( TYPR );

	case STREF:
		return( NCVTR+OTHER );

	case FORCE:
		return( TYPL );

	case COLON:
		if( mt12 & MENU ) return( NCVT+PUN+PTMATCH );
		else if( mt12 & MDBI ) return( TYMATCH );
		else if( mt12 & MPTR ) return( TYPL+PTMATCH+PUN );
		else if( (mt1&MINT) && (mt2&MPTR) ) return( TYPR+PUN );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+PUN );
		else if( mt12 & MSTR ) return( NCVT+TYPL+OTHER );
		break;

	case ASSIGN:
	case RETURN:
		if( mt12 & MSTR ) return( LVAL+NCVT+TYPL+OTHER );
	case CAST:
		if(o==CAST && mt1==0)return(TYPL+TYMATCH);
		if( mt12 & MDBI ) return( TYPL+LVAL+TYMATCH );
		else if( (mt1&MENU)||(mt2&MENU) ) return( LVAL+NCVT+TYPL+PTMATCH+PUN );
		else if( mt12 == 0 ) break;
		else if( mt1 & MPTR ) return( LVAL+PTMATCH+PUN );
		else if( mt12 & MPTI ) return( TYPL+LVAL+TYMATCH+PUN );
		break;

	case ASG LS:
	case ASG RS:
		if( mt12 & MINT ) return( TYPL+LVAL+OTHER );
		break;

	case ASG MUL:
	case ASG DIV:
		if( mt12 & MDBI ) return( LVAL+TYMATCH );
		break;

	case ASG MOD:
	case ASG AND:
	case ASG OR:
	case ASG ER:
		if( mt12 & MINT ) return( LVAL+TYMATCH );
		break;

	case ASG PLUS:
	case ASG MINUS:
	case INCR:
	case DECR:
		if( mt12 & MDBI ) return( TYMATCH+LVAL );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+LVAL+CVTR );
		break;

	case MINUS:
		if( mt12 & MPTR ) return( CVTO+PTMATCH+PUN );
		if( mt2 & MPTR ) break;
	case PLUS:
		if( mt12 & MDBI ) return( TYMATCH );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+CVTR );
		else if( (mt1&MINT) && (mt2&MPTR) ) return( TYPR+CVTL );

		}
	uerror( "operands of %s have incompatible types", opst[o] );
	return( NCVT );
	}

moditype( ty ) TWORD ty; {

	switch( ty ){

	case TVOID:
	case UNDEF:
		return(0); /* type is void */
	case ENUMTY:
	case MOETY:
		return( MENU );

	case STRTY:
	case UNIONTY:
		return( MSTR );

	case CHAR:
	case SHORT:
	case UCHAR:
	case USHORT:
		return( MINT|MPTI|MDBI );
	case UNSIGNED:
	case ULONG:
	case INT:
	case LONG:
		return( MINT|MDBI|MPTI );
	case FLOAT:
	case DOUBLE:
		return( MDBI );
	default:
		return( MPTR|MPTI );

		}
	}

NODE *
doszof( p )  register NODE *p; {
	/* do sizeof p */
	int i;

	/* whatever is the meaning of this if it is a bitfield? */
	i = tsize( p->in.type, p->fn.cdim, p->fn.csiz )/SZCHAR;

	tfree(p);
	if( i <= 0 ) werror( "sizeof returns 0" );
	return( bcon( i ) );
	}

# ifndef BUG2
/* Note: This eprint is used only in "pass1" modules.  In the 2nd pass,
 *  the name "eprint" is redefined as "e2print" by mfile2.  The other eprint
 *  (which is thus actually compiled as e2print) is in reader.c  (jima)
 */
eprint( p, down, a, b ) register NODE *p; int *a, *b; {
	register ty;

	*a = *b = down+1;
	while( down > 1 ){
		printf( "\t" );
		down -= 2;
		}
	if( down ) printf( "    " );

	ty = optype( p->in.op );

	printf("%o) %s, ", p, opst[p->in.op] );
	if( ty == LTYPE ){
		printf( CONFMT, p->tn.lval );
		printf( ", %d, ", p->tn.rval );
		}
	tprint( p->in.type );
	printf( ", %d, %d\n", p->fn.cdim, p->fn.csiz );
	}
# endif

prtdcon( p ) register NODE *p; {
	int i;
/* This is now removed so constants will be generated in line */
/*	if( p->in.op == FCON ){
		locctr( DATA );
		defalign( ALDOUBLE );
		deflab( i = getlab() );
		fincode( p, SZDOUBLE );
		p->tn.lval = 0;
		p->tn.rval = -i;
		p->in.type = DOUBLE;
		p->in.op = NAME;
		} */
	}


int edebug = 0;
ecomp( p ) register NODE *p; {
# ifndef BUG2
	if( edebug ) fwalk( p, eprint, 0 );
# endif
	if( !reached ){
		werror( "statement not reached" );
		reached = 1;
		}
	p = optim(p);
	walkf( p, prtdcon );
	locctr( PROG );
	ecode( p );
	tfree(p);
	}

# ifdef STDPRTREE
# ifndef ONEPASS

prtree(p) register NODE *p; {

	register struct symtab *q;
	register ty;

# ifdef MYPRTREE
	MYPRTREE(p);  /* local action can be taken here; then return... */
#endif

	ty = optype(p->in.op);

	printf( "%d\t", p->in.op );

	if( ty == LTYPE ) {
		printf( CONFMT, p->tn.lval );
		printf( "\t" );
		}
	if( ty != BITYPE ) {
		if( p->in.op == NAME || p->in.op == ICON ) printf( "0\t" );
		else printf( "%d\t", p->tn.rval );
		}

	printf( "%o\t", p->in.type );

	/* handle special cases */

	switch( p->in.op ){

	case NAME:
	case ICON:
		/* print external name */
		if( p->tn.rval == NONAME ) printf( "\n" );
		else if( p->tn.rval >= 0 ){
			q = &stab[p->tn.rval];
			printf(  "%s\n", exname(q->sname) );
			}
		else { /* label */
			printf( LABFMT, -p->tn.rval );
			}
		break;

	case STARG:
	case STASG:
	case STCALL:
	case UNARY STCALL:
		/* print out size */
		/* use lhs size, in order to avoid hassles with the structure `.' operator */

		/* note: p->in.left not a field... */
		printf( CONFMT, (CONSZ) tsize( STRTY, p->in.left->fn.cdim, p->in.left->fn.csiz ) );
		printf( "\t%d\t\n", talign( STRTY, p->in.left->fn.csiz ) );
		break;

	default:
		printf(  "\n" );
		}

	if( ty != LTYPE ) prtree( p->in.left );
	if( ty == BITYPE ) prtree( p->in.right );

	}

# else

p2tree(p) register NODE *p; {
	register ty;

# ifdef MYP2TREE
	MYP2TREE(p);  /* local action can be taken here; then return... */
# endif

	ty = optype(p->in.op);

	switch( p->in.op ){

	case NAME:
	case ICON:
#ifndef FLEXNAMES
		if( p->tn.rval == NONAME ) p->in.name[0] = '\0';
#else
		if( p->tn.rval == NONAME ) p->in.name = "";
#endif
		else if( p->tn.rval >= 0 ){ /* copy name from exname */
			register char *cp;
			register i;
/* */			register struct symtab *spp = &stab[p->tn.rval];
			char bufr[20];
			if ( spp->sclass == AUTO || spp->sclass == PARAM){
				sprintf(bufr,"%d(fp)", (spp->offset)/SZCHAR);
				cp = bufr;
			}
/* */			else
				cp = exname( stab[p->tn.rval].sname );
#ifndef FLEXNAMES
			for( i=0; i<NCHNAM; ++i ) p->in.name[i] = *cp++;
#else
			p->in.name = tstr(cp);
#endif
			}
#ifndef FLEXNAMES
		else sprintf( p->in.name, LABFMT, -p->tn.rval );
#else
		else {
			char temp[32];
			sprintf( temp, LABFMT, -p->tn.rval );
			p->in.name = tstr(temp);
		}
#endif
		break;

	case STARG:
	case STASG:
	case STCALL:
	case UNARY STCALL:
		/* set up size parameters */
		p->stn.stsize = (tsize(STRTY,p->in.left->fn.cdim,p->in.left->fn.csiz)+SZCHAR-1)/SZCHAR;
		p->stn.stalign = talign(STRTY,p->in.left->fn.csiz)/SZCHAR;
		break;
	case FCON:
		break;/* nothing needs to be done */

	case REG:
		rbusy( p->tn.rval, p->in.type );

	default:
#ifndef FLEXNAMES
		p->in.name[0] = '\0';
#else
		p->in.name = "";
#endif
		}

	p->in.rall = NOPREF;

	if( ty != LTYPE ) p2tree( p->in.left );
	if( ty == BITYPE ) p2tree( p->in.right );
	}

# endif
# endif
