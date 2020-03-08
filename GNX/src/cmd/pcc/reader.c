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
 * reader.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)reader.c	5.1 (NSC) 9/8/83";
# endif

/* jima 6/10/82: Comments only */
/* jima 6/18/82: Cerror in order() to prevent rewrite of non-temps to ASG op*/
/* jima 6/23/82: cbranch modified to reflect removal of use of bn.label */
/* jima 6/28/82: debug prints ch from oct to decimal for sdb convenience */
/* jima 7/14/82,7/28: CCODES revisions, SCONV rewrite added */
/* jima 7/17/82: order() changed: If nextcook() gives cookie which works but
		 doesn't include anything in the original cook, rewrites are
		 allowed in subsequent ("final") matching. */

/* jima 8/13/82: added old_callseq and -z[no] option */
/* JZ 9/25/82: changed the check in order to see if MUSTDO's were done to check and see
	if it was a fake NAME node for coercing a register, as it might be */
/* JZ 10/8/82: Added something to FLD case of order to force it into a register
   since otherwise it would die not knowing what to do.  Still does not do 
   assignment correctly */
/*JZ 10-27-82: Added a check in BITYPE for a signed division and mod. This
is to prevent wrongly match the rewrite case for unsigned division and mod.
if it is signed it actually generates the code and cleans up after itself.
This needs to be changed to have a seperate rewrite case for template 89-1
since that is the only one this applies on.  It should come immediately
before the BITYPE case so that when it really is UNSIGNED division it will
fall through.*/
/*JZ 11/11/82: Changed order QUEST to keep the type of the result the same
        as that of the largest node instead of the right side. Deleted a line
	assigning the type from the right side or the last assignment into
	the remaining node.*/
/*JZ 11/12/82: Changed order SCONV to allow the sizes to be the same as they
		might be with the '?' operator.*/
/*JZ 11/15/82: Changed QUEST to allow PTR types to be returned*/
/*JZ 11/23/82: Changed order INCR/DECR to rewrite as asgop when their child is
		an UNARY MUL node and the INCR/DECR has a mustdo as otherwise
		the wrong registers will be used.  This probably should be
		changed so that a different register is used for the child
		and setexpr rewrites thing so that 2 registers are used,
		since this will be slightly more efficient although it will
		require more registers.  Sucomp should be checked to make
		sure the registers will be available. Either that or all
		operations should be done with the asg rewrite for U*.
/*JZ 11/24/82: Changed order ASSIGN to allow FLD types to match tables
	       since they are addressable for extract instructions.*/
/*JZ 11/24/82: Changed p2init to use 'f' for non fatal error flag and
		F for keeping common in local static*/
/*JZ 11/29/82: Changed order ASSIGN to not allow FLD types to match tables
		since it causes extsd to memory instructions to be generated
		which do not work.  This is now dependant on INSSBUG being
		undefined.*/
/*JZ 12/13/82:  Change matchdebug to extern so it will compile since it is 
		most often used in the place it is defined*/
/*JZ 12/27/82:  Changed rewrite case for STASG to realize that if the 
		right side is a function setstr will have done the 
		assignment allready*/	
/*JZ 1/4/82:	Changed canon() to avoid trying to make OREG's on the 
		UNARY MUL left kid of an STASG node whose right side is a
		STCALL since address of the left side will be needed to pass
		to the function which will do the actual assignment.*/
/*JZ  1/7/83:   Changed order FORCE to recognise the cases of STASG and
		STCALL and do the apropriate things which is call setstr
		with STCALL so it will pass the correct pointer and call
		genscall with STASG with the cookie FORARG so it will save
		the return value. */
/*JZ  1/28/83:  Changed the BITYPE rewrite rule for signed asg div/mod
		to only return the left side as that is all that is safe
		Also added some comments.*/
/*JZ  2/1/83:   Changed BITYPE rewrite rule for signed asg div/mod to
		do a tcopy of the left side if we need to force the left side
		into a temporary register.  This is so that the register will
		remain busy and not be reallocated to some other purpose.*/
/*JZ  2/9/83:   Removed all the kluge code in BITYPE to handle signed /= or %=
		with an unsigned right side and made setexpr() just rewrite
		the right side instead.  This allows all the regular code to
		do its job instead of having to hand compile it.*/
/*JZ 4/20/83:   Added fcon_alloc for allocating fcons.  Changed rewrite CCODES
		to check for double operands and generate the correct type*/
/*JZ 4/23/83:   Added maxtbreg to p2bbeg for setting maximum number of
		floating temporary registers*/
/* JZ 4/25/83:  Changed rewrite CCODES to check for FLOAT operands and change
		constant size to FLOAT.  Since 0.0 is identical at any size.*/
/* JZ 4/25/83:  Removed the code in canon() that tried to prevent OREGs from
		being made for the left side of STASG nodes where the right
		side was a function call.  It is allright now since addrlxpd
		knows what to do.*/
/* JZ 5/6/83:   Changed fcon_alloc to use X-support float stuff */
/* JZ 5/18/83:  Changed order FORCE to truncate constants to be returned as
		FLOAT to float sized. */
# include "mfile2"

/*	some storage declarations */

# ifndef ONEPASS
NODE node[TREESZ];
char filename[100] = "";  /* the name of the file */
int ftnno;  /* number of current function */
int lineno;
# else
# define NOMAIN
#endif

extern char *rnames[];

int nrecur;
int lflag;
extern int Wflag;   /* suppress warnings */
extern int NFEflag; /* make errors non-fatal (for interactive debugging) */
extern int old_callseq;  /* see force_rxp0() (jima) */
int edebug = 0;
int xdebug = 0;
int udebug = 0;
int vdebug = 0;
int Fflag =0;/* no common flag if set all non extern global defs reserve space*/

/* checking-code generation flags (jima) */
int scheck = 0;  /* stack alignment checking */


OFFSZ tmpoff;  /* offset for first temporary, in bits for current block */
OFFSZ maxoff;  /* maximum temporary offset over all blocks in current ftn, in bits */
int maxtreg;	/* highest non used register.  Can be used to implement varying
		 * numbers of temporary registers */
int maxtbreg;	/* same as above but for B (floating) registers */

NODE *stotree;
int stocook;

OFFSZ baseoff = 0;
OFFSZ maxtemp = 0;

extern int matchdebug;  /* tracing in match.  Moved from scan.c (jima) */

p2init( argc, argv ) char *argv[];{
	/* set the values of the pass 2 arguments */

	register int c;
	register char *cp;
	register files;

	allo0();  /* free all regs */
	files = 0;

	for( c=1; c<argc; ++c ){
		if( *(cp=argv[c]) == '-' ){
			while( *++cp ){
				switch( *cp ){

				case 'C':  /* checking-code flags (jima) */
				case 'X':  /* pass1 flags */
					while( *++cp ) { /* VOID */ }
					--cp;
					break;

				case 'a':  /* rallo */

					++radebug;
					break;

				case 'e':  /* expressions */
					++edebug;
					break;
				case 'F':/* keep common in local static*/
					++Fflag;
					break;

				case 'f':  /* make errors non-fatal */
					++NFEflag;
					break;

				case 'l':  /* linenos */
					++lflag;
					break;

				case 'm':
					++matchdebug;
					break;

				case 'o':  /* orders */
					++odebug;
					break;

				case 'r':  /* register allocation */
					++rdebug;
					break;

				case 's':  /* shapes */
					++sdebug;
					break;

				case 't':  /* ttype calls */
					++tdebug;
					break;

				case 'u':  /* Sethi-Ullman testing (machine dependent) */
					++udebug;
					break;

				case 'v':
					++vdebug;
					break;

				case 'w':
				case 'W':  /* shut up warnings */

					++Wflag;
					break;

				case 'x':  /* general machine-dependent debugging flag */
					++xdebug;
					break;

				case 'z':  /* use old (zo) or new (zn) */
					   /* calling convention.  The old */
					   /* uses rxp n (and no adjsp) for */
					   /* all functions except printf etc.*/
					switch( *++cp ) { 
					case 'o': old_callseq=1;
						  break;
					case 'n': old_callseq=0;
						  break;
					default: printf("use -z[no] for new or old calling sequence\n");
					}
					break;

				default:
					printf( "2nd pass options are -aeflmorstuvwx -z[no]\n");
					cerror( "bad option: %c", *cp );
					}
				}
			}
		else files = 1;  /* assumed to be a filename */
		}

	mkdope();
	setrew();
	return( files );

	}

# ifndef NOMAIN

unsigned int caloff();
unsigned int offsz;
mainp2( argc, argv ) char *argv[]; {
	register files;
	register temp;
	register c;
	register char *cp;
	register NODE *p;

	offsz = caloff();
	files = p2init( argc, argv );
	tinit();

	reread:

	if( files ){
		while( files < argc && argv[files][0] == '-' ) {
			++files;
			}
		if( files > argc ) return( nerrors );
		freopen( argv[files], "r", stdin );
		}
	while( (c=getchar()) > 0 ) switch( c ){
	case ')':
	default:
		/* copy line unchanged */
		if ( c != ')' )
			PUTCHAR( c );  /*  initial tab  */
		while( (c=getchar()) > 0 ){
			PUTCHAR(c);
			if( c == '\n' ) break;
			}
		continue;

	case BBEG:
		/* beginning of a block */
		temp = rdin(10);  /* ftnno */
		tmpoff = baseoff = (unsigned int) rdin(10); /* autooff for block gives max offset of autos in block */
		maxtreg = rdin(10);
		if( getchar() != '\n' ) cerror( "intermediate file format error");

		if( temp != ftnno ){ /* beginning of function */
			maxoff = baseoff;
			ftnno = temp;
			maxtemp = 0;
			}
		else {
			if( baseoff > maxoff ) maxoff = baseoff;
			/* maxoff at end of ftn is max of autos and temps
			   over all blocks in the function */
			}
		setregs(); /* Set fregs based on these things, that is the
			    * number of temporary registers we will allow in
			    * this procedure.*/
		continue;

	case BEND:  /* end of block */
		SETOFF( maxoff, ALSTACK );
		eobl2();
		while( (c=getchar()) != '\n' ){
			if( c <= 0 ) cerror( "intermediate file format eof" );
			}
		continue;

	case EXPR:
		/* compile code for an expression */
		lineno = rdin( 10 );
		for( cp=filename; (*cp=getchar()) != '\n'; ++cp ) ; /* VOID, reads filename */
		*cp = '\0';
		if( lflag ) lineid( lineno, filename );

		tmpoff = baseoff;  /* expression at top level reuses temps */
		p = eread();

# ifndef BUG4
		if( edebug ) fwalk( p, eprint, 0 );
# endif

# ifdef MYREADER
		MYREADER(p);  /* do your own laundering of the input */
# endif

		nrecur = 0;
		delay( p );  /* expression statement  throws out results */
		reclaim( p, RNULL, 0 );

		allchk();
		tcheck();
		continue;

	default:
		cerror( "intermediate file format error" );

		}

	/* EOF */
	if( files ) goto reread;
	return(nerrors);

	}

# endif

# ifdef ONEPASS

p2compile( p ) NODE *p; {

	if( lflag ) lineid( lineno, filename );
	tmpoff = baseoff;  /* expression at top level reuses temps */
	/* generate code for the tree p */
# ifndef BUG4
	if( edebug ) fwalk( p, eprint, 0 );
# endif

# ifdef MYREADER
	MYREADER(p);  /* do your own laundering of the input */
# endif
	nrecur = 0;
	delay( p );  /* do the code generation */
	reclaim( p, RNULL, 0 );
	allchk();
	/* can't do tcheck here; some stuff (e.g., attributes) may be around from first pass */
	/* first pass will do it... */
	}

p2bbeg( aoff, myreg, mybreg) {
	static int myftn = -1;

	tmpoff = baseoff = (unsigned int) aoff;
	maxtreg = myreg;
	if( myftn != ftnno ){ /* beginning of function */
		maxoff = baseoff;
		myftn = ftnno;
		maxtemp = 0;
		}
	else {
		if( baseoff > maxoff ) maxoff = baseoff;
		/* maxoff at end of ftn is max of autos and temps over all blocks */
		}
	setregs();
	}

p2bend(){
	SETOFF( maxoff, ALSTACK );
	eobl2();
	}

# endif

NODE *deltrees[DELAYS];
int deli;

delay( p ) register NODE *p; {
	/* look in all legal places for COMOP's and ++ and -- ops to delay */
	/* note; don't delay ++ and -- within calls or things like
	/* getchar (in their macro forms) will start behaving strangely */
	register i;

	/* look for visible COMOPS, and rewrite repeatedly */

	while( delay1( p ) ) { /* VOID */ }

	/* look for visible, delayable ++ and -- */

	deli = 0;
	delay2( p );
	codgen( p, FOREFF );  /* do what is left */
	for( i = 0; i<deli; ++i ) codgen( deltrees[i], FOREFF );  /* do the rest */
	}

delay1( p ) register NODE *p; {  /* look for COMOPS */
	register o, ty;

	o = p->in.op;
	ty = optype( o );
	if( ty == LTYPE ) return( 0 );
	else if( ty == UTYPE ) return( delay1( p->in.left ) );

	switch( o ){

	case QUEST:
	case ANDAND:
	case OROR:
		/* don't look on RHS */
		return( delay1(p->in.left ) );

	case COMOP:  /* the meat of the routine */
		delay( p->in.left );  /* completely evaluate the LHS */
		/* rewrite the COMOP */
		{ register NODE *q;
			q = p->in.right;
			ncopy( p, p->in.right );
			q->in.op = FREE;
			}
		return( 1 );
		}

	return( delay1(p->in.left) || delay1(p->in.right ) );
	}

delay2( p ) register NODE *p; {

	/* look for delayable ++ and -- operators */

	register o, ty;
	o = p->in.op;
	ty = optype( o );

	switch( o ){

	case NOT:
	case QUEST:
	case ANDAND:
	case OROR:
	case CALL:
	case UNARY CALL:
	case STCALL:
	case UNARY STCALL:
	case FORTCALL:
	case UNARY FORTCALL:
	case COMOP:
	case CBRANCH:
		/* for the moment, don't delay past a conditional context, or
		/* inside of a call */
		return;

	case UNARY MUL:
		/* if *p++, do not rewrite */
		if( autoincr( p ) ) return;
		break;

	case INCR:
	case DECR:
		if( deltest( p ) ){
			if( deli < DELAYS ){
				register NODE *q;
				deltrees[deli++] = tcopy(p);
				q = p->in.left;
				p->in.right->in.op = FREE;  /* zap constant */
				ncopy( p, q );
				q->in.op = FREE;
				return;
				}
			}

		}

	if( ty == BITYPE ) delay2( p->in.right );
	if( ty != LTYPE ) delay2( p->in.left );
	}

codgen( p, cookie ) NODE *p; {

	/* generate the code for p;
	   order may call codgen recursively */
	/* cookie is used to describe the context */

	for(;;){
		canon(p);  /* creats OREG from U* if possible and does sucomp */
		stotree = NIL;
# ifndef BUG4
		if( edebug ){
			printf( "codgen: calling store on:\n" );
			fwalk( p, eprint, 0 );
			}
# endif
		store(p);
		if( stotree==NIL ) break;

		/* because it's minimal, can do w.o. stores */

		order( stotree, stocook );
		}

	order( p, cookie );

	}

# ifndef BUG4
char *cnames[] = { /* names of cookie bits */
	"SANY",
	"SAREG",
	"STAREG",
	"SBREG",
	"STBREG",
	"SCC",
	"SNAME",
	"SCON",
	"SFLD",
	"SOREG",
# ifdef WCARD1
	"WCARD1",
# else
	"STARNM",
# endif
# ifdef WCARD2
	"WCARD2",
# else
	"STARREG",
# endif
	"INTEMP",
	"FORARG",
	"SWADD",
	0,
	};

prcook( cookie ){

	/* print a nice-looking description of cookie */

	int i, flag;

	if( cookie & SPECIAL ){
		if( cookie == SZERO ) printf( "SZERO" );
		else if( cookie == SONE ) printf( "SONE" );
		else if( cookie == SMONE ) printf( "SMONE" );
		else printf( "SPECIAL+%d", cookie & ~SPECIAL );
		return;
		}

	flag = 0;
	for( i=0; cnames[i]; ++i ){
		if( cookie & (1<<i) ){
			if( flag ) printf( "|" );
			++flag;
			printf( cnames[i] );
			}
		}

	}
# endif
NODE *
fcon_alloc(val)
#if ns16000
double val;
#else
quad_wrd val;
#endif
{ /* allocate a new node and make it a floating constant */
		  /* added 4/6/83 (JZ)  */
	register NODE *p;

	p = talloc();
	p->fpn.op = FCON;
	p->fpn.type = DOUBLE;
	p->fpn.dval = val;
	p->tn.rval = 0;
	p->tn.lval = 0;
	return(p);
	}
NODE *
icon_alloc(val) { /* allocate a new node and make it an integer constant */
		  /* added 7/6/82 (jima)  */
	register NODE *p;

	p = talloc();
	p->tn.op = ICON;
	p->tn.type = INT;
	p->tn.name = 0;
	p->tn.lval = val;
	p->tn.rval = 0;
	return(p);
	}

int odebug = 0;

order(p,cook) NODE *p; {

	register o, ty, m;
	int m1;
	int cookie;
	NODE *p1, *p2, *newleft, *newright;
# ifdef DOASSERTS
	register original_rall = p->in.rall;  /* to be sure MUSTDOs are done */
# endif

	cookie = cook;
	rcount();
	canon(p);
	rallo( p, p->in.rall );
	goto first;
	/* by this time, p should be able to be generated without stores;
	   the only question is how */

	again:

	if ( p->in.op == FREE )
		return;		/* whole tree was done */
	cookie = cook;
	rcount();
	canon(p);
	rallo( p, p->in.rall );
	/* if any rewriting and canonicalization has put
	 * the tree (p) into a shape that cook is happy
	 * with (exclusive of FOREFF, FORREW, and INTEMP)
	 * then we are done.
	 * this allows us to call order with shapes in
	 * addition to cookies and stop short if possible.
	 */
	if( tshape(p, cook &(~(FOREFF|FORREW|INTEMP))) )return;

	first:
# ifndef BUG4
	if( odebug ){
		printf( "order( 0t%d, ", p );
		prcook( cookie );
		printf( " )\n" );
		fwalk( p, eprint, 0 );
		}
# endif

	o = p->in.op;
	ty = optype(o);

	/* first of all, for most ops, see if it is in the table */

	/* However, skip the table search for those ops which never match */


	switch( m = p->in.op ){

	default:
		/* look for op in table */
	      use_table:	
# ifndef MEMRMWOK
		/* Kludge to avoid read-modify-write of memory - bug in 
		   revF (and at least the first rev-G) cpus.  Write goes into 
		   supervisor space if an interrupt occurs during rmw 
		   instruction!!! (jima 8/24/82) */
		if ((dope[m]&ASGOPFLG) && p->in.left->in.op!=REG) {
			/* Force the rewrite action, which will force 
			   the left operand (at least) into into temporary 
			   register */
			if (odebug) 
				printf("\tForcing REGs for RMW cpu bug...\n");
			m = BITYPE;
		        break;
		}
# endif
		for(;;) {
			if((m=match(p,cookie))==MDONE) {
				/* Next line added to allow further rewrites
				   after subgoal evaluation (jima) */
				if (!(cookie&cook)) goto again;
				goto cleanup;
				}
			else if(m==MNOPE) {
				if(!(cookie=nextcook(p,cookie)))
					goto nomat;
				continue;
				} 
			else break;
			}
		break;

	case ASSIGN: 
		/* table only works with simple RHS */
		if (canaddr(p->in.right)
#ifndef INSSBUG
		||(p->in.right->in.op==FLD&&flshape(p->in.right->in.left))
#endif
		)
		        goto use_table;
		if (p->in.right->in.op==CCODES) goto use_table; 
		break;
		
	case COMPL:   			/* jima */
	case UNARY MINUS:   	
		/* Tables only have ZW entries for U- and ~ and U* */
		if (cookie&(INTAREG|INTBREG)) goto use_table;
		break;
	case UNARY MUL:
	/* U* should not use the tables if the underlying op is a UNARY
	 * MUL or OREG because that will prevent register sharing since
	 * it will match STARNM templates*/ 

		if (cookie&(INTAREG|INTBREG)&&(p->in.left->in.op!=UNARY MUL
		)&&p->in.left->in.op != OREG) goto use_table; 
		break; 
		
	case INCR:
	case DECR:			/* jima */
		/* Always use rewrite rule if no value needed (converts
		   directly to += or -=) */
		if (cookie&FOREFF) break;
		/* If the underlying op is a unary mul and the register is
		   required,  we must rewrite
		   this as an asgop -= 1 so that the operation can be done
		   in 1 register.  Perhaps this should be changed to allow
		   more registers and reserve the mustdo register so that
		   we can save the extra addqd .
		   JZ*/
		if (p->in.rall&MUSTDO&&p->in.left->in.op==UNARY MUL) break;
		goto use_table;
	case ANDAND: 
	case OROR:  
	case NOT:  
	case COMOP:
	case FORCE:
	case CBRANCH:
	case QUEST:
	case UNARY CALL:
	case CALL:
	case UNARY STCALL:
	case STCALL:
	case UNARY FORTCALL:
	case FORTCALL:
		/* don't even go near the table, but use "rewrite" rules... */
		;

		}
	/* get here to do rewriting if no match or fall through from above 
	   if we know a priori that a rewrite is appropriate.  m is rewrite 
	   specification from match, or p->in.op if fell through. */

	p1 = p->in.left;
	if( ty == BITYPE ) p2 = p->in.right;
	else p2 = NIL;
	
# ifndef BUG4
	if( odebug ){
		printf( "order( 0t%d, ", p );
		prcook( cook );
		printf( " ), cookie " );
		prcook( cookie );
		printf( ", rewrite case %d = \"%s\"\n", 
			m, (m==BITYPE) ? "BITYPE" : opst[m]) ;
		}
# endif
	switch( m ){
	default:
	      nomat:
		fwalk(p,eprint,0);
		prcook(cook); printf("\n" );
		cerror( "no table entry for above op & cookie" );

	case COMOP:  /* comma operator */
		codgen( p1, FOREFF );
		p2->in.rall = p->in.rall;
		codgen( p2, cook );  /* right expression gives result val */
/* was:		codgen( p2, cookie );   (jima) */
		ncopy( p, p2 );
		p2->in.op = FREE;
		goto cleanup;

	case FORCE:
	{
		NODE *l =p->in.left;
		/* recurse, letting the work be done by rallo */
		if (l->in.op== UNARY STCALL || l->in.op== STCALL)
		{
			setstr(p);
			return;
		}
		if (p->in.type == FLOAT && l->in.op == FCON)
		{		/*  Truncate to float sized constant */
# if ns16000
		    l->fpn.dval = (float) l->fpn.dval;
# else
		    l->fpn.dval = float_double(double_float(l->fpn.dval));
# endif
		}
		 p = l;


		cook = 
		  p->in.type==FLOAT || p->in.type==DOUBLE ? INTBREG:INTAREG;
		goto again;
	}
	case CBRANCH:
		o = p2->tn.lval;
		cbranch( p1, -1, o );
		p2->in.op = FREE;
		p->in.op = FREE;
		return;

	case QUEST:
		{int p_type=p->in.type,type1,type2;

		cbranch( p1, -1, m=getlab() );
		p2->in.left->in.rall = p->in.rall;
		codgen( p2->in.left, INTAREG|INTBREG );
		type1=p2->in.left->in.type;
		/* force right to compute result into same reg used by left */
		p2->in.right->in.rall = p2->in.left->tn.rval|MUSTDO;
		reclaim( p2->in.left, RNULL, 0 );
		cbgen( 0, m1 = getlab(), 'I' );
		deflab( m );
		codgen( p2->in.right, INTAREG|INTBREG );
		type2=p2->in.right->in.type;
		deflab( m1 );
		p->in.op = REG;  /* set up node describing result */
		p->tn.lval = 0;
		/* The type of the result should be accurate to 32 bits
		   If the both sides are shorter than 32 bits then the
		   result is known to be no larger than the larger of
		   the 2.  Knowing this can allow some operands to be done
		   smaller precision.
		   If one is signed and the other is unsigned the type
		   must be the type of the ? operator for correctness
		   since that will be either UNSIGNED or INT whichever
		   is correct for this expression.If there are any
		   problems with this code it should simply be
		   allowed to default since there is some question
		   as to whether this is worth this much effort*/
		ASSERT((p_type==DOUBLE|| p_type==UNSIGNED||
			p_type==LONG||p_type==ULONG||p_type==INT||
			ISPTR(p_type)),
			"QUEST:order unexpected type");
		if (p_type != DOUBLE &&
			((ISUNSIGNED(type1))==(ISUNSIGNED(p_type)))
			&&((ISUNSIGNED(type1))==(ISUNSIGNED(type2))))
		 	switch (type1)
			{	
				case CHAR:
				case UCHAR:if (type2==CHAR||type2==UCHAR)
					   {
						p_type=type2;
						break;
					   }/*else*/
				case SHORT:
				case USHORT:if (type2==SHORT||type2==USHORT) 
					    {
						p_type=type2;
						break;
					    }/*else*/
		 		/* either side INTEGER or *
		 		 * UNSIGNED or PTR default to type of '?'*/
				default:break;
			}/*end switch on types*/
		p->in.type=p_type;
		p->tn.rval = p2->in.right->tn.rval;
		tfree( p2->in.right );
		p2->in.op = FREE;
		goto cleanup;
		}/*QUEST*/


	/* Evaluate a compound logical expression with conditional
	   branching.  The result is a special CCODES node with rval<>0,
	   which is the internal label branched to in the false (0 result)
	   case.  A subsequent template ASSIGNment (possibly after rewrite
	   into ASSIGN to a temporary) emits a movq/branch/movq sequence to
	   put a 0 or 1 in the desired place.

	   (When rval==0 in a CCODES node, an S instruction is used to
	   directly generate a 0 or 1 from the condition codes).

	   The table are bypassed completely for these ops.
	   [Note: This is a new way of handling CCODES - jima]
	*/
	case ANDAND:
	case OROR:
	case NOT:
		cbranch( p, -1, m=getlab() );
		ASSERT(p->in.op==FREE,"order:NOT");
 		p->tn.op = CCODES;
 		p->tn.rval = m;
 		p->tn.lval = 0;
		if (cook&FORCC) goto cleanup;	/* jima */
 		goto again;			/* jima */

	case FLD:	/* fields of funny type */
		if ( p1->in.op == UNARY MUL ){
			offstar( p1->in.left, cook );
			goto again;
			}
			/* added JZ 10/8/82 if nothing works throw it in a register*/
		order(p,INTAREG|INTBREG);
		goto again;

/* Not needed - templates should handle this without forced temporary (jima)
/*	case NAME:
/*		/* all leaves end up here ... */
/*		if( o == REG ) goto nomat;
/*		order( p, INTAREG|INTBREG );
/*		goto again;
/* */

	case CCODES:  	/* OPLTYPE,FORCC template with needs=REWRITE */
			/* rewrite as (0 or 0.0 != <the leaf>)    (jima)    */
			if (!(cookie&FORCC) || optype(o)!=LTYPE)
			   cerror("rewrite CCODES");
			p2 = talloc();	
			ncopy(p2,p);
			p->in.right = p2;
#if ns16000
#define ZEROF 0.0
#else
#define ZEROF (int_double(0))
#endif

			p->in.left = (p->in.type==DOUBLE||p->in.type == FLOAT)?
			    fcon_alloc(ZEROF):icon_alloc(0);
			if (p->in.type == FLOAT) p->in.left->in.type =FLOAT;
			p->in.op = NE;  /* Now have LHS=ICON zero,RHS=old p */
			goto again;
				
	case SCONV:	/* Scalar down-conversions:
			   Just demote the node to be the the smaller type...
			   16K addressing allows this (the address doesn't 
			   change).  The upper bits will not be used when
			   the datum is referenced (usually an assigment).

			   This action would also apply to registers, except
			   that registers are always assumed to have all 
			   32 bits valid, regardless of their nominal type.
			   So regs are rewritten as fake NAME memory nodes.
			   This may seem like a kludge, but it really just 
			   reflects the fact that explicit conversions are 
			   the only case where registers must be considered 
			   to have smaller than 32-bit size.       (jima) */

			ASSERT( tlen(p)<=tlen(p1) && canaddr(p1) &&
				p->in.type!=FLOAT && !mixtypes(p,p1),
				"order:SCONV");
			{
			int p1_type = p1->in.type;
			p1->in.type = p->in.type;  /* demote the type */
			ncopy(p,p1);  /* delete the SCONV node */
			p1->in.op=FREE;
			if (p->tn.op==REG && tlen(p) < 4 &&
			    p1_type != p->in.type) { 
				/* convert REGs to fake NAMEs */
				p->tn.op = NAME;
		    		p->tn.name = rnames[p->tn.rval];
		    		p->tn.lval = p->tn.rval = 0;
				/* NOTE: An OREG had better not be made from
				   this! (it won't because the name doesn't
				   begin with an '_' or '-'; see oreg2) */
				if (xdebug)
				   printf("REG %d converted to fake NAME\n",p);
				}
			}

			goto again;
	case INIT:
		uerror( "illegal initialization" );
		return;

	case UNARY FORTCALL:
		p->in.right = NIL;
	case FORTCALL:
		o = p->in.op = UNARY FORTCALL;
		if( genfcall( p, cookie,0 ) ) goto nomat;
		goto cleanup;

	case UNARY CALL:
		p->in.right = NIL;
	case CALL:
		o = p->in.op = UNARY CALL;
		if( gencall( p, cookie,0 ) ) goto nomat;
		goto cleanup;

	case UNARY STCALL:
		p->in.right = NIL;
	case STCALL:
		o = p->in.op = UNARY STCALL;
		if( genscall( p, cookie ) ) goto nomat;
		goto cleanup;

		/* if arguments are passed in register, care must be taken 
		   that reclaim not throw away the register which now has 
		   the result... */

	case UNARY MUL:
		if( cook == FOREFF ){
			/* do nothing */
			order( p->in.left, FOREFF );
			p->in.op = FREE;
			return;
			}
		offstar( p->in.left, cook );
		goto again;

	case INCR:  /* INCR and DECR */
	case DECR:

		if( setincr(p, cook) ) goto again;
		/* x++ becomes (x += 1) -1; */

		if( cook & FOREFF ){  /* result not needed so inc or dec and be done with it */
			/* x++ => x += 1 */
			p->in.op = (p->in.op==INCR)?ASG PLUS:ASG MINUS;
			goto again;
			}
		p1 = tcopy(p);
		reclaim( p->in.left, RNULL, 0 );
		p->in.left = p1;
		p1->in.op = (p->in.op==INCR)?ASG PLUS:ASG MINUS;
		p->in.op = (p->in.op==INCR)?MINUS:PLUS;
		goto again;

	case STASG:
		if(p->in.right->in.op==STCALL||p->in.right->in.op==UNARY STCALL)
			{if(setstr(p,cook))
			 	{
				 goto cleanup;
				}
			}
		else if( setstr( p, cook ) ) goto again;
		goto nomat;

/* Not needed - BITYPE case handles offstar calls, etc.   (jima)
/*	case ASG PLUS:  /* and other assignment ops */
/*		if( setasop(p, cook) ) goto again;
/*
/*		/* there are assumed to be no side effects in LHS */
/*
/*		p2 = tcopy(p);
/*		p->in.op = ASSIGN;
/*		reclaim( p->in.right, RNULL, 0 );
/*		p->in.right = p2;
/*		canon(p);
/*		rallo( p, p->in.rall );
/*
/*# ifndef BUG4
/*		if( odebug ) fwalk( p, eprint, 0 );
/*# endif
/*
/*		order( p2->in.left, INTBREG|INTAREG );
/*		order( p2, INTBREG|INTAREG );
/*		goto again;
/* */

	case ASSIGN:
		if( setasg( p, cook ) ) goto again;
		goto nomat;


	case UNARY MINUS: 
	case COMPL: 
	case BITYPE:
/*The following commented out code was a hack to catch and generate the
/*code for the case of int /= unsigned.  It has been simplified to simply
/*forcing the right side into a register if it is a shorter type and
/*changing its type to int to match the int asg div/mod rules.  If it is
/*necessary to force the left side into a register we change the size of
/*the right to long and the rewrite rules will automatically do that, saving
/*all the bother of doing it by hand and using present code for that.  This
/*is all done in setexpr*/
	/* When we are doing a real int /= unsigned we will still get this
	/* rewrite rule as the templates cannot tell the difference between
	/* that and the fake assignment op created by setasg. The only
	/* difference between the 2 is the type of the /= is INT. Therefor
	/* we check here and do the signed operation if the /= is signed.*/
	/* The reason for this confusing case is the rule that in an 
	/* assignment op the type of the operation is the type of the
	/* LEFT side !! So even thought the right side is UNSIGNED
	/* the operation must be done INT.  If either operand is shorter
	/* than INT it is forced into a temporary register unless it
	/* allready is a register.  If the left side is a register variable
	/* that is shorter than INT it is sign extended into itself 
	/* afterwards to preserve its size. 
	/* The rewrite rule is done if it is a fake node for dei*/
	/* This needs to eventually have its own rewrite rule, perhaps
	   UNSIGNED and have template 89-1 use that rewrite case. Then
	   we won't have to have that long if statement that checks to
	   see if it really is the case we want. Then all we need
	   to check is if the type  of the /=or %= is unsigned.*/
/*		{register NODE *r=p->in.right; 
/*		register  NODE *l=p->in.left; 
/*		int p_op = p->in.op;
/*		if ((p_op== ASG DIV||p_op==ASG MOD)
/*				&&!(ISUNSIGNED(p->in.type)||p->in.type==FLOAT||
/*				p->in.type==DOUBLE)&&canaddr(r)&&
/*				canaddr(l)&&ISUNSIGNED(r->in.type))
/*				{switch (tlen(l)){
/*				case 4:
/*				if (tlen(r)!=4 &&r->in.op!=REG)
/*					order(r,INTAREG);
/*				{
/*					printf((p_op==ASG DIV)?"\tquod\t":
/*								"\tremd\t");
/*					adrput(r);
/*					printf(",");
/*					adrput(l);
/*					printf("\n");
/*					break;
/*				}
/*				case 1:
/*				case 2:
/*				/* This cannot be done shorter precision since
/*				   the semantics of c say that the unsigned 
/*				   quantity must be zero extended before the
/*				   operation is done to keep the sign positive
/*				   if possible*/
/*				{NODE *temp = tcopy(l);
/*					/*if (temp->in.op== UNARY MUL)
/*					{/* Keep the pointers from tangling */
/*					/*	temp->in.left=talloc();
/*					/*	ncopy(temp->in.left,l->in.left);
/*					}*/
/*					if(temp->in.op!=REG)
/*						order(temp,INTAREG);
/*					/* Don't put it in a register if it is*/
/*
/*
/*					/* already there*/
/*					if(r->in.op!=REG)
/*						order(r,INTAREG);
/*					printf((p_op==ASG DIV)?"\tquod\t":
/*								"\tremd\t");
/*					adrput(r);
/*					printf(",");
/*					adrput(temp);
/*					printf("\n");
/*					/*reclaim(p->in.right,RNULL,FOREFF);*/
/*					/*The following checks are for when*/
/*					/*char and short registers are allowed*/
/*
/*					if (l->in.op!=REG)
/*						ord_assign(l,p,temp);
/*					else if (!istnode(l))
/*					{
/*						printf("\tmovx");
/*						prtype(l);
/*						printf("d\t");
/*						adrput(l);
/*						printf(",");
/*						adrput(l);
/*					}
/*					break;
/*				}/*case 1 and 2*/
/*				default:
/*					cerror("wrong type for divide in BITYPE");
/*			}/*switch tlen(l)*/
/*
/*					if (cookie &FOREFF)
/*						reclaim(p,RNULL,cookie);
/*					else
/*					/* RLEFT is the only reasonable thing
/*					   to return since the other side 
/*					   contains the non divided operand*/
/*						reclaim(p,RLEFT,cookie);
/*					return;
/*		}}/*signed div*/


		if( setexpr( p, cook ) ) goto again;
		/* try to replace binary ops by =ops */
		switch(o){

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
			if ( istnode(p->in.left) ) {
				p->in.op = ASG o;
				goto again;
			} 
			else 	/* Can't do it if lhs is not temporary reg */
				cerror("rewrite ASGop");   /* (jima) */
		}
		goto nomat;

		}

	cleanup:

	/* if it is not yet in the right state, put it there */

	if( cook & FOREFF ){
		reclaim( p, RNULL, 0 );
		return;
		}

	if( p->in.op==FREE ) return;

	/* if cook is INTAREG,INTBREG, or INTEMP, tshape will return true
	   if the tree satisfies these requirements.  N.b. INTEMP may be
	   satisfied here by other than an official temp memory cell - 
	   see shtemp() and tshape().  */
	if( tshape( p, cook ) ) return;

	/* One last chance to generate code with what is left */
	/* after all "rewrite" processing is completed.       */
	/* remember fake NAME registers however */
	if( (m=match(p,cook) ) == MDONE ) {
		ASSERT( !(original_rall&MUSTDO) ||
			(original_rall&(~MUSTDO))==p->tn.rval||
			(p->tn.op== NAME && REAL_REG(p->tn.name[1]) == (original_rall&(~MUSTDO))),
			"MUSTDO not done");
		return;
	}

	/* we are in bad shape, try one last chance */
	if( lastchance( p, cook ) ) goto again;

	goto nomat;
	}

int callflag;
int fregs;

store( p ) register NODE *p; {

	/* find a subtree of p which should be stored */

	register o, ty;

	o = p->in.op;
	ty = optype(o);

	if( ty == LTYPE ) return;

	switch( o ){

	case UNARY CALL:
	case UNARY FORTCALL:
	case UNARY STCALL:
		++callflag;
		break;

	case UNARY MUL:
		if( asgop(p->in.left->in.op) ) stoasg( p->in.left, UNARY MUL );
		break;

	case CALL:
	case FORTCALL:
	case STCALL:
		store( p->in.left );
		stoarg( p->in.right, o );
		++callflag;
		return;

	case COMOP:
		markcall( p->in.right );
		if( p->in.right->in.su > fregs ) SETSTO( p, INTEMP );
		store( p->in.left );
		return;

	case ANDAND:
	case OROR:
	case QUEST:
		markcall( p->in.right );
		if( p->in.right->in.su > fregs ) SETSTO( p, INTEMP );
	case CBRANCH:   /* to prevent complicated expressions on the LHS from being stored */
	case NOT:
		constore( p->in.left );
		return;

		}

	if( ty == UTYPE ){
		store( p->in.left );
		return;
		}

	if( asgop( p->in.right->in.op ) ) stoasg( p->in.right, o );

	if( p->in.su>fregs ){ /* must store */
		mkadrs( p );  /* set up stotree and stocook to subtree
				 that must be stored */
		}

	store( p->in.right );
	store( p->in.left );
	}

constore( p ) register NODE *p; {

	/* store conditional expressions */
	/* the point is, avoid storing expressions in conditional
	   conditional context, since the evaluation order is predetermined */

	switch( p->in.op ) {

	case ANDAND:
	case OROR:
	case QUEST:
		markcall( p->in.right );
	case NOT:
		constore( p->in.left );
		return;

		}

	store( p );
	}

markcall( p ) register NODE *p; {  /* mark off calls below the current node */

	again:
	switch( p->in.op ){

	case UNARY CALL:
	case UNARY STCALL:
	case UNARY FORTCALL:
	case CALL:
	case STCALL:
	case FORTCALL:
		++callflag;
		return;

		}

	switch( optype( p->in.op ) ){

	case BITYPE:
		markcall( p->in.right );
	case UTYPE:
		p = p->in.left;
		/* eliminate recursion (aren't I clever...) */
		goto again;
	case LTYPE:
		return;
		}

	}

stoarg( p, calltype ) register NODE *p; {
	/* arrange to store the args */

	if( p->in.op == CM ){
		stoarg( p->in.left, calltype );
		p = p->in.right ;
		}
	if( calltype == CALL ){
		STOARG(p);  /* does nothing on 16K (jima) */
		}
	else if( calltype == STCALL ){
		STOSTARG(p);
		}
	else {
		STOFARG(p);
		}
	callflag = 0;
	store(p);
# ifndef NESTCALLS
	if( callflag ){ /* prevent two calls from being active at once  */
		SETSTO(p,INTEMP);
		store(p); /* do again to preserve bottom up nature....  */
		}
#endif
	}

/* Return the negative of a relational op */
int negrel[] = { NE, EQ, GT, GE, LT, LE, UGT, UGE, ULT, ULE } ;  
/* for ops:      EQ  NE  LE  LT  GE  GT  ULE  ULT  UGE  UGT  */
# define negrelop(op) negrel[op-EQ]


cbranch( p, true, false ) NODE *p; {
	/* evaluate p for truth value, and branch to true or false
	/* accordingly: label <0 means fall through */

	register o, lab, flab, tlab;

	lab = -1;

	switch( o=p->in.op ){

	case EQ:
	case NE:
	case LE:
	case LT:
	case GE:
	case GT:
	case ULE:
	case ULT:
	case UGE:
	case UGT:
		if( true < 0 ){
			o = p->in.op = negrelop(o);
			true = false;
			false = -1;
			}

		/* Issue warnings for meaningless unsigned zero comparisons */
		if( p->in.left->in.op == ICON && p->in.left->tn.lval == 0 && 
		    p->in.left->in.name[0] == '\0' )
			switch( o ){
			case UGE:
			case UGT:
				werror("questionable comparison - unsigneds can never be negative.");
				break;
			case ULE:
				werror("meaningless comparison (unsigned>=0)");
			}

		codgen( p, FORCC );  /* WAS: p->bn.label=true in old scheme */
		/* reclaim() set lval of CCODES to the op (which   */
		/* may have been reversed - so "o" is no good now) */
		cbgen( p->tn.lval, true, 'I' ); /* conditional branch */
		
		if( false>=0 ) cbgen( 0, false, 'I' );	/* uncond. br */
		reclaim( p, RNULL, 0 );			/* Finished. */
		return;

	case ANDAND:
		lab = false<0 ? getlab() : false ;
		cbranch( p->in.left, -1, lab );
		cbranch( p->in.right, true, false );
		if( false < 0 ) deflab( lab );
		p->in.op = FREE;
		return;

	case OROR:
		lab = true<0 ? getlab() : true;
		cbranch( p->in.left, lab, -1 );
		cbranch( p->in.right, true, false );
		if( true < 0 ) deflab( lab );
		p->in.op = FREE;
		return;

	case NOT:
		cbranch( p->in.left, false, true );
		p->in.op = FREE;
		break;

	case COMOP:
		codgen( p->in.left, FOREFF );
		p->in.op = FREE;
		cbranch( p->in.right, true, false );
		return;

	case QUEST:
		flab = false<0 ? getlab() : false;
		tlab = true<0 ? getlab() : true;
		cbranch( p->in.left, -1, lab = getlab() );
		cbranch( p->in.right->in.left, tlab, flab );
		deflab( lab );
		cbranch( p->in.right->in.right, true, false );
		if( true < 0 ) deflab( tlab);
		if( false < 0 ) deflab( flab );
		p->in.right->in.op = FREE;
		p->in.op = FREE;
		return;

	case ICON:
		if( p->in.type != FLOAT && p->in.type != DOUBLE ){

			if( p->tn.lval || p->in.name[0] ){
				/* addresses of C objects are never 0 */
				if( true>=0 ) cbgen( 0, true, 'I' );
				}
			else if( false>=0 ) cbgen( 0, false, 'I' );
			p->in.op = FREE;
			return;
			}
		/* fall through to default with other strange constants */


	default: /* Arithmetic expressions or leaves other than ICONs (jima) */
		/* get condition codes */
		codgen( p, FORCC );
		/* A comparison with zero (or vice versa) has been done */
		if( true >= 0 ) cbgen( NE, true, 'I' );
		if( false >= 0 ) cbgen( true >= 0 ? 0 : EQ, false, 'I' );
		reclaim( p, RNULL, 0 );
		return;

		}

	}

rcount(){ /* count recursions */
	if( ++nrecur > NRECUR ){
		cerror( "expression causes compiler loop: try simplifying" );
		}

	}

# ifndef BUG4
/* NOTE: If ONEPASS, then "eprint" is redefined as "eprint2" in mfile2.
 *  Thus, this and other "2nd-pass" modules call this routine, which is
 *  actually compiled as "eprint2".  First-pass modules call the plain
 *  eprint in trees.c ("eprint" is not redefined in mfile1).
 * I don't know why this eprint isn't permanently called eprint2... (jima)
 */
eprint( p, down, a, b ) NODE *p; int *a, *b; {

	*a = *b = down+1;
	while( down >= 2 ){
		printf( "\t" );
		down -= 2;
		}
	if( down-- ) printf( "    " );


	printf( "0t%d> %s", p, opst[p->in.op] );
	switch( p->in.op ) { /* special cases */

	case REG:
		{ register rval = p->tn.rval;
		if (rval<R0 || rval>SP) printf(" ILLEGALrval=0%o",rval);
		else                    printf( " %s", rnames[rval] );
		break;
		}

	case ICON:
	case NAME:
	case OREG:
		printf( " " );
		adrput( p );
		break;

	case STCALL:
	case UNARY STCALL:
	case STARG:
	case STASG:
		printf( " size=%d", p->stn.stsize );
		printf( " align=%d", p->stn.stalign );
		break;
		}

	printf( ", " );
	tprint( p->in.type );
	printf( ", " );
	if( p->in.rall == NOPREF ) printf( "NOPREF" );
	else {
		register rall = p->in.rall;
		if( rall & MUSTDO ) printf( "MUSTDO " );
		else printf( "PREF " );
		rall = rall&~MUSTDO;
		if (rall<R0 || rall>SP) printf("ILLEGALrall=0%o",p->in.rall);
		else                    printf( "%s", rnames[rall] );
		/* printf( "%s", rnames[p->in.rall&~MUSTDO]); */
		}
	printf( ", SU= %d\n", p->in.su );

	}

dump2( p ) NODE *p; { /* print out tree p (jiam) */
	/* This routine was inserted to allow dumping trees from sdb(1) */
	/* It could also be called in lieu of fwalk,etc., to save code. */
	fwalk(p, eprint, 1);
}
# endif

# ifndef NOMAIN
NODE *
eread(){

	/* call eread recursively to get subtrees, if any */

	register NODE *p;
	register i, c;
	register char *pc;
	register j;

	i = rdin( 10 );

	p = talloc();

	p->in.op = i;

	i = optype(i);

	if( i == LTYPE ) p->tn.lval = rdin( 10 );
	if( i != BITYPE ) p->tn.rval = rdin( 10 );

	p->in.type = rdin(8 );
	p->in.rall = NOPREF;  /* register allocation information */

	if( p->in.op == STASG || p->in.op == STARG || p->in.op == STCALL || p->in.op == UNARY STCALL ){
		p->stn.stsize = (rdin( 10 ) + (SZCHAR-1) )/SZCHAR;
		p->stn.stalign = rdin(10) / SZCHAR;
		if( getchar() != '\n' ) cerror( "illegal \n" );
		}
	else {   /* usual case */
		if( p->in.op == REG ) rbusy( p->tn.rval, p->in.type );  /* non usually, but sometimes justified */
#ifndef FLEXNAMES
		for( pc=p->in.name,j=0; ( c = getchar() ) != '\n'; ++j ){
			if( j < NCHNAM ) *pc++ = c;
			}
		if( j < NCHNAM ) *pc = '\0';
#else
		{ char buf[BUFSIZ];
		for( pc=buf,j=0; ( c = getchar() ) != '\n'; ++j ){
			if( j < BUFSIZ ) *pc++ = c;
			}
		if( j < BUFSIZ ) *pc = '\0';
		p->in.name = tstr(buf);
		}
#endif
		}

	/* now, recursively read descendents, if any */

	if( i != LTYPE ) p->in.left = eread();
	if( i == BITYPE ) p->in.right = eread();

	return( p );

	}

CONSZ
rdin( base ){
	register sign, c;
	CONSZ val;

	sign = 1;
	val = 0;

	while( (c=getchar()) > 0 ) {
		if( c == '-' ){
			if( val != 0 ) cerror( "illegal -");
			sign = -sign;
			continue;
			}
		if( c == '\t' ) break;
		if( c>='0' && c<='9' ) {
			val *= base;
			if( sign > 0 )
				val += c-'0';
			else
				val -= c-'0';
			continue;
			}
		cerror( "illegal character `%c' on intermediate file", c );
		break;
		}

	if( c <= 0 ) {
		cerror( "unexpected EOF");
		}
	return( val );
	}
# endif

#ifndef FIELDOPS
	/* do this if there is no special hardware support for fields */

ffld( p, down, down1, down2 ) NODE *p; int *down1, *down2; {
	 /* look for fields that are not in an lvalue context, and rewrite them... */
	register NODE *shp;
	register s, o, v, ty;

	*down1 =  asgop( p->in.op );
	*down2 = 0;

	if( !down && p->in.op == FLD ){ /* rewrite the node */

		if( !rewfld(p) ) return;

		ty = (szty(p->in.type) == 2)? LONG: INT;
		v = p->tn.rval;
		s = UPKFSZ(v);
# ifdef RTOLBYTES
		o = UPKFOFF(v);  /* amount to shift */
# else
		o = szty(p->in.type)*SZINT - s - UPKFOFF(v);  /* amount to shift */
#endif

		/* make & mask part */

		p->in.left->in.type = ty;

		p->in.op = AND;
		p->in.right = talloc();
		p->in.right->in.op = ICON;
		p->in.right->in.rall = NOPREF;
		p->in.right->in.type = ty;
		p->in.right->tn.lval = 1;
		p->in.right->tn.rval = 0;
#ifndef FLEXNAMES
		p->in.right->in.name[0] = '\0';
#else
		p->in.right->in.name = "";
#endif
		p->in.right->tn.lval <<= s;
		p->in.right->tn.lval--;

		/* now, if a shift is needed, do it */

		if( o != 0 ){
			shp = talloc();
			shp->in.op = RS;
			shp->in.rall = NOPREF;
			shp->in.type = ty;
			shp->in.left = p->in.left;
			shp->in.right = talloc();
			shp->in.right->in.op = ICON;
			shp->in.right->in.rall = NOPREF;
			shp->in.right->in.type = ty;
			shp->in.right->tn.rval = 0;
			shp->in.right->tn.lval = o;  /* amount to shift */
#ifndef FLEXNAMES
			shp->in.right->in.name[0] = '\0';
#else
			shp->in.right->in.name = "";
#endif
			p->in.left = shp;
			/* whew! */
			}
		}
	}
#endif

oreg2( p ) register NODE *p; {

	/* look for situations where we can turn * into OREG */

	NODE *q;
	register i;
	register r;
	register char *cp;
	register NODE *ql, *qr;
	CONSZ temp;

	if( p->in.op == UNARY MUL) {
		q = p->in.left;
		if( q->in.op == REG ){
			temp = q->tn.lval;
			r = q->tn.rval;
			cp = q->in.name;
			goto ormake;
			}
		/*if (q->in.op==NAME&& q->in.name[0]=='-'||
		    (q->in.name[0] >='0'&&q->in.name[0]<='9'))
		    if (tn.rval ==0)*/
			

		if( q->in.op != PLUS && q->in.op != MINUS ) return;
		ql = q->in.left;
		qr = q->in.right;

#ifdef R2REGS
		if (xdebug) {
			printf("OREG2:");
			fwalk(p, eprint, 0);
			}
		/* look for doubly indexed expressions */

		if( q->in.op == PLUS) {
			if( (r=base(ql))>=0 && (i=offset(qr, tlen(p)))>=0) {
				makeor2(p, ql, r, i);
				return;
			} else if( (r=base(qr))>=0 && (i=offset(ql, tlen(p)))>=0) {
				makeor2(p, qr, r, i);
				return;
				}
			}


#endif
		/* swap sides if needed to recognize con(Reg)    (jima) */
		if( q->in.op==PLUS && ql->in.op==ICON && qr->in.op==REG ) {
			q->in.left = qr;
			q->in.right = ql;
			ql = qr;
			qr = q->in.right;
			}

		if( (q->in.op==PLUS || q->in.op==MINUS)
		&& (qr->in.op == ICON ) &&
				ql->in.op==REG && szty(qr->in.type)==1) {
			temp = qr->tn.lval;
			if( q->in.op == MINUS ) temp = -temp;
			r = ql->tn.rval;
			temp += ql->tn.lval;
			cp = qr->in.name;
			if( *cp && ( q->in.op == MINUS || *ql->in.name ) ) return;
			if( !*cp ) cp = ql->in.name;

			ormake:
			if( notoff( p->in.type, r, temp, cp ) ) return;
			p->in.op = OREG;
			p->tn.rval = r;
			p->tn.lval = temp;
#ifndef FLEXNAMES
			for( i=0; i<NCHNAM; ++i )
				p->in.name[i] = *cp++;
#else
			p->in.name = cp;
#endif
			tfree(q);
			return;
			}
		}

	}

canon(p) NODE *p; {
	/* put p in canonical form */
	int oreg2(), sucomp();
	register NODE *l=p->in.left,*r=p->in.right;
#ifndef FIELDOPS
	int ffld();
	fwalk( p, ffld, 0 ); /* look for field operators */
# endif
/*  The following comment and commented out code were removed as a UNARY
    AND node should be created which will be handled correctly by addrlxpd.
    This will eventually be the method of implementing general use of the
    addr instruction */
	/*  If the op is an STASG op its parent is a UNARY MUL with*/
	/*  a cookie of FOREFF.  We have to check for the case of a*/
	/*  structure assignment where the right side is a function*/
	/*  since in that case we will only want the address, not  */
	/*  the thing on the left itself which is all that the OREG presently*/
	/*  allows.  Perhaps we will need a different category of  */
	/*  OREG for that case.*/
/*	if (p->in.op == UNARY MUL && l->in.op == STASG &&
 *	    l->in.left->in.op == UNARY MUL &&
 *	    (l->in.right->in.op==STCALL||l->in.right->in.op==UNARY STCALL))
 *	{
 *		walkf(l->in.left->in.left,oreg2);
 *		walkf(l->in.right,oreg2);/* look on right side also */
/*	}
 *	else if (p->in.op == STASG && l->in.op == UNARY MUL &&
 *	        ( l->in.right->in.op==STCALL || p->in.right->in.op==UNARY STCALL))
 *	{
 *		walkf(l->in.left,oreg2);
 *		walkf(p->in.right,oreg2);
 *	}
 *	else */
 	walkf( p, oreg2 );  /* look for and create OREG nodes */
#ifdef MYCANON
	MYCANON(p);  /* your own canonicalization routine(s) */
#endif
	walkf( p, sucomp );  /* do the Sethi-Ullman computation */

	}

