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
 * allo.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)allo.c	5.1 (NSC) 9/8/83";
# endif

/* jima 6/22/82: Reclaim now makes CCODES results have generating op in lval */
/* jima 8/13/82: Reclaim */
/* JZ 9/17/82: recl2 changed to check NAME nodes to see if they are actually registers and should be freed*/
/* JZ 9/17/82: recl2 Changed again since the register number is destroyed when a name node was
               created. It is taken from the name */
/* JZ 9/20/82: recl2  Changed to call a new macro REAL_REG which returns the
	real internal representation or the registers for calling rfree
	since that is what rfree uses. I assume that floating
	point will not have this problem. The macro is in mac2defs*/
/* JZ 10/25/82: changed reclaim to not rewrite register variables,
		they should retain their size I believe*/
/* 11/8/82: changed allo to generate name nodes with -n(fp) for tmp
		memory locations instead of generating OREG s, since
		it is the same as local variable it should be treated
		the same.*/
/*  JZ 4/21/83: Changed freereg to keep track of the maximum floating point
		register used as a temporary register so we can save and
		restore them correctly in docontext(). */
/*  JZ 6/8/83:  Changed allo to check to rewrite all float registers to double,
		since we really don't have float registers.*/

# include "mfile2"

NODE resc[3];

int busy[REGSZ];

int maxa, mina, maxb, minb;

# ifndef ALLO0
allo0(){ /* free everything */

	register i;

	maxa = maxb = -1;
	mina = minb = 0;

	REGLOOP(i){
		busy[i] = 0;
		if( rstatus[i] & STAREG ){
			if( maxa<0 ) mina = i;
			maxa = i;
			}
		if( rstatus[i] & STBREG ){
			if( maxb<0 ) minb = i;
			maxb = i;
			}
		}
	}
# endif

# define TBUSY 01000

# ifndef ALLO
allo( p, q ) NODE *p; struct optab *q; {

	register n, i, j;
	int either;

	n = q->needs;
	either = ( EITHER & n );
	i = 0;

	while( n & NACOUNT ){
		resc[i].in.op = REG;
		resc[i].tn.rval = freereg( p, n&NAMASK );
		resc[i].tn.lval = 0;
#ifdef FLEXNAMES
		resc[i].in.name = "";
#else
		resc[i].in.name[0] = '\0';
#endif
		n -= NAREG;
		++i;
	}

	if (either) { /* all or nothing at all */
		for(j=0; j<i; j++)
			if( resc[j].tn.rval < 0 ) { /* nothing */
				i = 0;
				break;
			}
		if( i != 0 )
			goto ok; /* all */
	}

	while( n & NBCOUNT ){
		resc[i].in.op = REG;
		resc[i].tn.rval = freereg(p, n&NBMASK);
		resc[i].tn.lval = 0;
#ifdef FLEXNAMES
		resc[i].in.name = "";
#else
		resc[i].in.name[0] = '\0';
#endif
		n -= NBREG;
		++i;
		}
	if (either) { /* all or nothing at all */
		for( j = 0; j < i; j++ ) {
			if( resc[j].tn.rval < 0 ) { /* nothing */
				i = 0;
				break;
			}
		}
		if( i != 0 )
			goto ok; /* all */
	}

	if( n & NTMASK ){
	     char buffr[20];
	     char *tstr();
		resc[i].in.op = NAME;
		if(p->in.op == STCALL
		|| p->in.op == STARG
		|| p->in.op == UNARY STCALL
		|| p->in.op == STASG) {
			sprintf(buffr,"%d(fp)",BITOOR(freetemp( (SZCHAR*p->stn.stsize + (SZINT-1))/SZINT )));
			}
		else {
			sprintf(buffr,"%d(fp)",BITOOR(freetemp( (n&NTMASK)/NTEMP )));
			}
#ifdef FLEXNAMES
		resc[i].in.name = tstr(buffr);
#else
		strcpy(resc[i].in.name,buffr) ;
#endif
		resc[i].tn.lval=0;/* No offset*/
		resc[i].tn.su=0;/* No registers needed since it is addressable*/

		++i;
		}

	/* turn off "temporarily busy" bit */

ok:
	REGLOOP(j)
		busy[j] &= ~TBUSY;

	for(j=0; j<i; ++j)
		if( resc[j].tn.rval < 0 )
			return(0);
	return(1);

	}
# endif

extern unsigned int offsz;
freetemp( k ){ /* allocate k integers worth of temp space */
	/* we also make the convention that, if the number of words is more than 1,
	/* it must be aligned for storing doubles... */

# ifndef BACKTEMP
	int t;

	if( k>1 ){
		SETOFF( tmpoff, ALDOUBLE );
		}

	t = tmpoff;
	tmpoff += k*SZINT;
	if( tmpoff > maxoff ) maxoff = tmpoff;
	if( tmpoff >= offsz )
		cerror( "stack ovf" );
	if( tmpoff-baseoff > maxtemp ) maxtemp = tmpoff-baseoff;
	return(t);

# else
	tmpoff += k*SZINT;
	if( k>1 ) {
		SETOFF( tmpoff, ALDOUBLE );
		}
	if( tmpoff > maxoff ) maxoff = tmpoff;
	if( tmpoff >= offsz )
		cerror( "stack ovf" );
	if( tmpoff-baseoff > maxtemp ) maxtemp = tmpoff-baseoff;
	return( -tmpoff );
# endif
	}
int max_used_breg;		/* The largest floating point register to be
				   saved and restored */
freereg( p, n ) NODE *p; {
	/* allocate a register of type n */
	/* p gives the type, if floating */

	register j;

	/* not general; means that only one register (the result) OK for call */
	if( callop(p->in.op) ){
		j = callreg(p);
		if( usable( p, n, j ) ) return( j );
		/* have allocated callreg first */
		}
	j = p->in.rall & ~MUSTDO;
	if( j!=NOPREF && usable(p,n,j) ){ /* needed and not allocated */
		return( j );
		}
	if( n&NAMASK ){
		for( j=mina; j<=maxa; ++j ) if( rstatus[j]&STAREG ){
			if( usable(p,n,j) ){
				return( j );
				}
			}
		}
	else if( n &NBMASK ){
		for( j=minb; j<=maxb; ++j ) if( rstatus[j]&STBREG ){
			if( usable(p,n,j) ){
				if (j> max_used_breg) max_used_breg = j;
				return(j);
				}
			}
		}

	return( -1 );
	}

# ifndef USABLE
usable( p, n, r ) NODE *p; {
	/* decide if register r is usable in tree p to satisfy need n */

	/* checks, for the moment */
	ASSERT( istreg(r), "usable:nontemp" );

	if( busy[r] > 1 ) return(0);
	if( isbreg(r) ){
		if( n&NAMASK ) return(0);
		}
	else {
		if( n & NBMASK ) return(0);
		}
	if( (n&NAMASK) && (szty(p->in.type) == 2) ){ /* only do the pairing for real regs */
		if( r&01 ) return(0);
		if( !istreg(r+1) ) return( 0 );
		if( busy[r+1] > 1 ) return( 0 );
		if( busy[r] == 0 && busy[r+1] == 0  ||
		    busy[r+1] == 0 && shareit( p, r, n ) ||
		    busy[r] == 0 && shareit( p, r+1, n ) ){
			busy[r] |= TBUSY;
			busy[r+1] |= TBUSY;
			return(1);
			}
		else return(0);
		}
	if( busy[r] == 0 ) {
		busy[r] |= TBUSY;
		return(1);
		}

	/* busy[r] is 1: is there chance for sharing */
	return( shareit( p, r, n ) );

	}
# endif

shareit( p, r, n ) NODE *p; {
	/* can we make register r available by sharing from p
	   given that the need is n */
	if( (n&(NASL|NBSL)) && ushare( p, 'L', r ) ) return(1);
	if( (n&(NASR|NBSR)) && ushare( p, 'R', r ) ) return(1);
	return(0);
	}

ushare( p, f, r ) NODE *p; {
	/* can we find a register r to share on the left or right
		(as f=='L' or 'R', respectively) of p */
	p = getlr( p, f );
	if( p->in.op == UNARY MUL ) p = p->in.left;
	if( p->in.op == OREG ){
		if( R2TEST(p->tn.rval) ){
			return( r==R2UPK1(p->tn.rval) || r==R2UPK2(p->tn.rval) );
			}
		else return( r == p->tn.rval );
		}/* I think we should check if this is a NAME JZ */
	if( p->in.op == REG ){
		return( r == p->tn.rval || ( szty(p->in.type) == 2 && r==p->tn.rval+1 ) );
		}
	return(0);
	}

recl2( p ) register NODE *p; {
	register r = p->tn.rval;
#ifndef OLD
	int op = p->in.op;
	if (op == REG && r >= REGSZ)
		op = OREG;
  /* JZ Sometimes NAMES are registers however the their names are not always the same as the internal
	compiler labels*/
	if (op == NAME && p->in.name[0] == 'r' ) rfree( REAL_REG(p->in.name[1]-'0'), p->in.type );
	else if( op == REG) rfree( r, p->in.type );
	else if( op == OREG ) {
		if( R2TEST( r ) ) {
			if( R2UPK1( r ) != 100 ) rfree( R2UPK1( r ), PTR+INT );
			rfree( R2UPK2( r ), INT );
			}
		else {
			rfree( r, PTR+INT );
			}
		}
#else
	if( p->in.op == REG ) rfree( r, p->in.type );
	else if( p->in.op == OREG ) {
		if( R2TEST( r ) ) {
			if( R2UPK1( r ) != 100 ) rfree( R2UPK1( r ), PTR+INT );
			rfree( R2UPK2( r ), INT );
			}
		else {
			rfree( r, PTR+INT );
			}
		}
#endif
	}

int rdebug = 0;

# ifndef RFREE
rfree( r, t ) TWORD t; {
	/* mark register r free, if it is legal to do so */
	/* t is the type */

# ifndef BUG3
	if( rdebug ){
		printf( "rfree( %s ), size %d\n", rnames[r], szty(t) );
		}
# endif

	if( istreg(r) ){
		if( --busy[r] < 0 ) cerror( "reg overfreed");
		if( szty(t) == 2 ){
			if( (r&01) || (istreg(r)^istreg(r+1)) ) cerror( "ill free" );
			if( --busy[r+1] < 0 ) cerror( "reg overfreed" );
			}
		}
	}
# endif

# ifndef RBUSY
rbusy(r,t) TWORD t; {
	/* mark register r busy */
	/* t is the type */

# ifndef BUG3
	if( rdebug ){
		printf( "rbusy( %s ), size %d\n", rnames[r], szty(t) );
		}
# endif

	if( istreg(r) ) ++busy[r];
	if( szty(t) == 2 ){
		if( istreg(r+1) ) ++busy[r+1];
		if( (r&01) || (istreg(r)^istreg(r+1)) ) cerror( "ill reg pair freed" );
		}
	}
# endif

# ifndef BUG3
rwprint( rw ){ /* print rewriting rule */
	register i, flag;
	static char * rwnames[] = {

		"RLEFT",
		"RRIGHT",
		"RESC1",
		"RESC2",
		"RESC3",
		0,
		};

	if( rw == RNULL ){
		printf( "RNULL" );
		return;
		}

	if( rw == RNOP ){
		printf( "RNOP" );
		return;
		}

	flag = 0;
	for( i=0; rwnames[i]; ++i ){
		if( rw & (1<<i) ){
			if( flag ) printf( "|" );
			++flag;
			printf( rwnames[i] );
			}
		}
	}
# endif

/* CHANGED TO RECORD p->in.op IN CCODES NODES (in lval).  Was: lval=rval=0 
   This is to facilitate subsequent generation of cond. branches or 
   the 16032 "s" instruction solely from the CCODES node, after the
   parent structure (probably a OPLOG) is gone             (jima 6/22/82) */

reclaim( p, rw, cookie ) NODE *p; {
	register NODE **qq;
	register NODE *q;
	register i;
	NODE *recres[5];
	struct respref *r;

	/* get back stuff */

# ifndef BUG3
	if( rdebug ){
		printf( "reclaim( %d rw=", p );
		rwprint( rw );
		printf( " cookie=" );
		prcook( cookie );
		printf( " )\n" );
		fwalk(p,eprint,0);
		}
# endif

	if( rw == RNOP || ( p->in.op==FREE && rw==RNULL ) ) return;  /* do nothing */

	walkf( p, recl2 );

	if( callop(p->in.op) ){
		/* check that all scratch regs are free */
		callchk(p);  /* ordinarily, this is the same as allchk() */
		}

	if( rw == RNULL || (cookie&FOREFF) ){ /* totally clobber, leaving nothing */
		tfree(p);
		return;
		}

	/* handle condition codes specially */

	if( (cookie & FORCC) && (rw&RESCC)) { register o;
		/* result is CC register */
		o = p->in.op;
		tfree(p);
		p->in.op = CCODES;
		p->tn.rval = 0;
		p->tn.lval = o;  /* save op for future use (jima) */
		return;
		}

	/* locate results */

	qq = recres;

	if( rw&RLEFT ) *qq++ = getlr( p, 'L' );;
	if( rw&RRIGHT ) *qq++ = getlr( p, 'R' );
	if( rw&RESC1 ) *qq++ = &resc[0];
	if( rw&RESC2 ) *qq++ = &resc[1];
	if( rw&RESC3 ) *qq++ = &resc[2];

	if( qq == recres ){
		cerror( "ill reclaim");
		}

	*qq = NIL;

	/* Now, select the single proper result among those mentioned in rw,
	   based on the cookie.

	   The respref array (defined in local2.c) contains <cform,mform>
	   pairs; (mform) gives shapes which match resources to be 
	   choosen when (cform) contains things in cookie.  
	   */

	for( r=respref; r->cform; ++r ){
		if( cookie & r->cform ){
			for( qq=recres; (q= *qq) != NIL; ++qq ){
				if( tshape( q, r->mform )) goto gotit;
				}
			}
		}

	/* we can't do it; die */
# ifdef DOASSERTS
	printf("RECLAIM ERROR:  recres entries:\n");
	for( qq=recres; *qq != NIL; ++qq ){
		fwalk(*qq,eprint,0);
		printf("~~~~~~~~~~~~~~~~~\n");
		}
# endif
        cerror("cant reclaim");

	gotit:
	if( p->in.op == STARG ) p = p->in.left;  /* STARGs are still STARGS */

	q->in.type = p->in.type;  /* to make multi-register allocations work */
		/* maybe there is a better way! */

	q = tcopy(q);

	tfree(p);

	p->in.op = q->in.op;
	p->tn.lval = q->tn.lval;
	p->tn.rval = q->tn.rval;
#ifdef FLEXNAMES
	p->in.name = q->in.name;
#ifdef ONEPASS
	p->in.stalign = q->in.stalign;
#endif
#else
	for( i=0; i<NCHNAM; ++i )
		p->in.name[i] = q->in.name[i];
#endif

	q->in.op = FREE;

	/* if the thing is in a register, adjust the type */

	switch( p->in.op ){

	case REG:
		/* The following check was added 8/6/82 (jima) */
		if( busy[p->tn.rval]>1 && istreg(p->tn.rval) ) 
			cerror( "potential reg overwrite");
		/* JZ register variables should not change type !?*/
		/* Floating point registers need to always be double sized */

		if( (!rtyflg|| p->in.type == FLOAT ) && istnode(p) ){
			/* the C language requires intermediate results to change type */
			/* this is inefficient or impossible on some machines */
			/* the "T" command in match supresses this type changing */
			if( p->in.type == CHAR || p->in.type == SHORT ) p->in.type = INT;
			else if( p->in.type == UCHAR || p->in.type == USHORT ) p->in.type = UNSIGNED;
			else if( p->in.type == FLOAT ) p->in.type = DOUBLE;
			}
		if( ! (p->in.rall & MUSTDO ) ) return;  /* unless necessary, ignore it */
		i = p->in.rall & ~MUSTDO;
		if( i & NOPREF ) return;
		if( i != p->tn.rval ){
			if( busy[i] || ( szty(p->in.type)==2 && busy[i+1] ) ){
				cerror( "faulty reg move" );
				}
			rbusy( i, p->in.type );
			rfree( p->tn.rval, p->in.type );
			rmove( i, p->tn.rval, p->in.type );
			p->tn.rval = i;
			}

	case OREG:
		if( p->in.op == REG || !R2TEST(p->tn.rval) ) {
			if( busy[p->tn.rval]>1 && istreg(p->tn.rval) ) cerror( "potential reg overwrite");
			}
		else
			if( (R2UPK1(p->tn.rval) != 100 && busy[R2UPK1(p->tn.rval)]>1 && istreg(R2UPK1(p->tn.rval)) )
				|| (busy[R2UPK2(p->tn.rval)]>1 && istreg(R2UPK2(p->tn.rval)) ) )
			   cerror( "potential reg overwrite");
		}

	}

ncopy( q, p ) NODE *p, *q; {
	/* copy the contents of p into q, without any feeling for
	   the contents */
	/* this code assume that copying rval and lval does the job;
	   in general, it might be necessary to special case the
	   operator types */
	register i;

	q->in.op = p->in.op;
	q->in.rall = p->in.rall;
	q->in.type = p->in.type;
	q->tn.lval = p->tn.lval;
	q->tn.rval = p->tn.rval;
#ifdef FLEXNAMES
	q->in.name = p->in.name;
#ifdef ONEPASS
	q->in.stalign = p->in.stalign;
#endif
#else
	for( i=0; i<NCHNAM; ++i ) q->in.name[i]  = p->in.name[i];
#endif

	}

NODE *
tcopy( p ) register NODE *p; {
	/* make a fresh copy of p */

	register NODE *q;
	register r;

	ncopy( q=talloc(), p );

	r = p->tn.rval;
	if( p->in.op == REG ) rbusy( r, p->in.type );
	else if( p->in.op == OREG ) {
		if( R2TEST(r) ){
			if( R2UPK1(r) != 100 ) rbusy( R2UPK1(r), PTR+INT );
			rbusy( R2UPK2(r), INT );
			}
		else {
			rbusy( r, PTR+INT );
			}
		}

	switch( optype(q->in.op) ){

	case BITYPE:
		q->in.right = tcopy(p->in.right);
	case UTYPE:
		q->in.left = tcopy(p->in.left);
		}

	return(q);
	}

allchk(){
	/* check to ensure that all register are free */

	register i;

	REGLOOP(i){
		if( istreg(i) && busy[i] ){
			cerror( "reg alloc error");
			}
		}

	}
