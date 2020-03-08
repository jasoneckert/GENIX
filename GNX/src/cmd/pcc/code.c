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
 * code.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)code.c	5.1 (NSC) 9/8/83";
# endif

/* jima 5/19/82 Modified ejobcode to use SCOMM and SREF symtab flags */
/* jima 5/24/82 SA options for stand-alone execution. */
/* jima 6/2 /82 Put SREF use back in ejobcode (which somehow crept out) */
/* jima 6/3/82  Made bycode emit shorter .byte strings for asm limitation */
/* jima 7/17/82 removed SA options. */
/* jima 7/28/82 put in signal-catching hack for unix kernel bugs */
/* jima 7/29/82 Fixed .import + .export bug (TNULL check in ejobcode) */
/* jima 8/3/82 gen rxp0 for printf, etc.; stack adjustment checking code */
/* JZ 9/20/82 from Jima generate .endseg to match .prog  added in main at end. For MikeP */
/* JZ 9/22/82 In main switched order of .program and .export for %initfun to match rest of compiler for MikeP */
/* JZ 9/30/82: Changed the registers used to contain the values of the structure being returned in efcode so that
	      they will be in the correct registers for movsb instruction if that is necessary. This
	      code should be rewritten so that they are not placed in registers but rather are simply placed
	      in the tree so that if the normal moves are possible that can be done with out any registers
	      and if the movs instruction is needed it will be placed in the proper registers by rallo.*/
/*JZ 10/5/82: above changed to return the address of the original function in r0 where it will be expected */
/*JZ 10/12/82: Changed bfcode to generate name:: for global functions and
changed %%initfun in main to be defined the same way*/
/*JZ 11/24/82: Changed profiling code to pass address in r0 instead of tos*/
/*JZ 12/3/82: Changed the tmpfiles to generate the path from macro TEMPDIR
	      which is set in the main makefile.  Also eliminated some 
	      obsolete debugging code. All this was in main()*/
/*JZ 12/27/82: efcode was changed to generate the new return sequence for
	       structure returning functions.  It now just copies the structure
	       from the address in r0 to the address in 12(fp).*/
/*JZ 12/29/82: bfcode was changed to generate the correct location for
	       parameters when the function returns a structure.  This will
	       need to be changed later to allow small structures to be
	       returned in r0*/
/*JZ 12/30/82: bfcode was changed to change the offsets when a structure is
	       being returned.  Eventually the front end should be changed to
	       recognise this fact and either simply bump the offset or
	       actually place the pointer in the symtab. */
/*JZ 1/07/82:  efcode was changed to place the pointer to the function in
	       r0 so that return(fn()) where fn() and the present function
	       are structure returning functions will work correctly.
	       Since efcode expects a pointer to the structure to be
	       returned to allready be in r0. */
/* JZ 2/4/83:  In ejobcode added .importp for mcount if proflg is set, so
	       it will be imported for profiling code.*/
/* JZ 3/4/83:  Added code in genswitch and makeheap to check to see if a quick
	       comparison can be made and do it if it can. */
/*  JZ 3/8/83: Added to bycode my original code to check line length and 
	       guarantee that no sequence of escape characters can cause it to
	       overflow the assembler.  Also always puts 63 characters on a 
	       line instead of the previous 31. Also added .exportp and
	       changed .routine to .importp. */
/*JZ 3/31/82:  Fixed change in makeheap() and  makeswitch() to do the correct
	       comparisons.  All comparisons are now of a number against r0.
	       This means that the compq case is the same as the others.
	       Previously the order of compq was oposite from the others
	       so if a heapswitch was made where there was a cmpq then it
	       was doing the opposite logic.*/
/* JZ 4/14/83: Added a variable struc_skip_lab in efcode() that has as its
	       value the label to skip to if we are skipping the structure
	       return code in case we have called a function that is doing
	       that.  For ex return (fn());  This is for structure returning
   	       functions of course. */
/* JZ 4/23/83: Added bregvar to p2bbeg() as a parameter since DOUBLE register
	       variables are now allowed and must be kept track of*/
# include "mfile1"
# include <sys/types.h>
# include <a.out.h>
# include <stab.h>
# include <signal.h>  #TEMP HACK IN MAIN TO CATCH QUITs

int proflg = 0;	/* are we generating profiling code? */
int strftn = 0;  /* is the current function one which returns a structure */
int gdebug;
FILE *tmpfile, *outfile, *initfile;
int fdefflag;  /* are we within a function definition ? */
char NULLNAME[8];
int labelno;

branch( n ){
	/* output a branch to label n */
	/* exception is an ordinary function branching to retlab:return */
	if( n == retlab ) printf( "	br	L%d\n", retlab );
	else	printf( "	br	L%d\n", n );
	}

int lastloc = { -1 };

short log2tab[] = {0, 0, 1, 2, 2, 3, 3, 3, 3};
#define LOG2SZ 9

defalign(n) {
	/* cause the alignment to become a multiple of n */
	n /= SZCHAR;
	if( lastloc != PROG && n > 1 ) printf( "; 	.align	%d\n", n >= 0 && n < LOG2SZ ? log2tab[n] : 0 );
	}

locctr( l ){
	register temp;
	/* l is PROG, ADATA, DATA, STRNG, ISTRNG, or STAB */

	if( l == lastloc ) return(l);
	temp = lastloc;
	lastloc = l;
	switch( l ){

	case PROG:
		if( outfile != stdout)
			outfile = stdout;
		else
		{
			printf( ".endseg\n.program\n" );
			psline();
		}
		break;

	case DATA:
	case ADATA:
		outfile = stdout;
		if(temp == PROG || temp < 0) 
			printf( ".endseg\n.static\n" );
		break;

	case STRNG:

	case ISTRNG:
		
		outfile = tmpfile;
		break;

	case STAB:
		outfile = stdout;
		printf( "	.stab\n" );
		break;

	default:
		cerror( "illegal location counter" );
		}

	return( temp );
	}

deflab( n ){
	/* output something to define the current position as label n */
	fprintf(outfile, "L%d:\n", n );
	}

int crslab = 10;

getlab(){
	/* return a number usable for a label */
	return( ++crslab );
	}

#define ALL_FREE 12
int reg_use = ALL_FREE;
int struc_skip_lab; /* label to branch to if the structure return code is  */
		    /* being skipped. Which is you called another function */
		    /* that did the actual returning */

efcode(){
	/* code for the end of a function */

 	deflab( retlab );
	if( strftn ){  /* copy output (in R0) to caller */
	       /* The address the output is to be copied to is the first arg*/
		register NODE *l, *r;
		register struct symtab *p;
		register TWORD t;
		register int j;
		int i;

		p = &stab[curftn];
		t = p->stype;
		t = DECREF(t);


/*		i = getlab();	/* label for return area */
/*		printf(".endseg\n.static\n" );
*		printf("; 	.align	2\n" );
*		printf("L%d:	.blkb	%d\n", i,
*			tsize(t, p->dimoff, p->sizoff)/SZCHAR );
*		printf(".endseg\n.program\n" );
*		psline();
*		printf("	addr	L%d,r2\n", i);*/

		reached = 1;
		l = block( OREG, NIL, NIL, PTR|t, p->dimoff, p->sizoff );
		l->tn.rval = FP;  /*  */
/*		l->tn.name="12(fp)";  /*location of first argument*/
		l->tn.lval = 12;  /* no offset */
		r = block( REG, NIL, NIL, PTR|t, p->dimoff, p->sizoff );
		r->tn.rval = 0;  /* R0 */ /* changed to r1 by zzzcode S if necessary */
		r->tn.lval = 0;
		l = buildtree( UNARY MUL, l, NIL );
		r = buildtree( UNARY MUL, r, NIL );
		l = buildtree( ASSIGN, l, r );
		l->in.op = FREE;
		ecomp( l->in.left );
/*		printf( "	addr	L%d,r0\n", i );*/
		/*  Return a pointer so that return (a=fn()); will work */
		printf( "	movd	12(fp),	r0	;efcode\n");

		if (struc_skip_lab) /* skip structure return stuff since it
					will be done in the called function */
		{
		    deflab(struc_skip_lab);
		    struc_skip_lab =0;/* reset for the next function */
		}
		/* turn off strftn flag, so return sequence will be generated */
		strftn = 0;
	}
	{
	extern int maxoff;
	int spoff;
	spoff = maxoff;
	if( spoff >= AUTOINIT ) spoff -= AUTOINIT;
	SETOFF(spoff,4);
	docontext("exit",spoff/SZCHAR);
	}
	p2bend();
	fdefflag = 0;
	reg_use = ALL_FREE;
}

int ftlab1, ftlab2;
int ncntargs;

int do_rxpn; /* Use "rxp n", not rxp 0 (with adjsp at call site) */

bfcode( a, n ) int a[]; {
	/* code for the beginning of a function; a is an array of
		indices in stab for the arguments; n is the number */
	register i;
	register temp;
	register struct symtab *p;
	int off;
	char *toreg();
	extern force_rxp0();

	ncntargs = n;
	locctr( PROG );
	p = &stab[curftn];

	do_rxpn = !force_rxp0(exname(p->sname)); /* HACK for VARARGS */

	/*printf( "	.align	2\n"); */
	defnam( p );
	temp = p->stype;
	temp = DECREF(temp);
	strftn = (temp==STRTY) || (temp==UNIONTY);

	retlab = getlab();

	/* routine prolog */

	if (gdebug) {
#ifdef STABDOT
		pstabdot(N_SLINE, lineno);
#else
		pstab(NULLNAME, N_SLINE);
		printf("0,%d,LL%d\n", lineno, labelno);
		printf("LL%d:\n", labelno++);
#endif
	}
	ftlab1 = getlab();
	ftlab2 = getlab();
	printf( "	br 	L%d\t;bfcode\n", ftlab1);
	printf( "L%d:\n", ftlab2);
	if( proflg ) {	/* profile code *//*address is passed in r0*/
		i = getlab(); /* mcount is imported in ejobcode*/
		printf("	addr	L%d,r0\n", i);
		printf("	cxp 	mcount\n");
		printf("	.static\n");
		printf("L%d:	.double	0\n", i);
		printf("	.endseg\n");
		psline();
	}

	off = ARGINIT;
	if (strftn) off += SZPOINT;/* first argument is a pointer to  the*/
				   /* function return location */

	for( i=0; i<n; ++i ){
		p = &stab[a[i]];
		if( p->sclass == REGISTER ){
		    extern char *rnames[];
			temp = p->offset;  /* save register number */
			p->sclass = PARAM;  /* forget that it is a register */
			p->offset = NOOFFSET;
			oalloc( p, &off );
	/*tbl*/		printf( "	%s	%d(fp),%s\n", toreg(p->stype),
				p->offset/SZCHAR, rnames[temp] );
			p->offset = temp;  /* remember register number */
			p->sclass = REGISTER;   /* remember that it is a register */
		} else if( p->stype == STRTY || p->stype == UNIONTY ) {
			p->offset = NOOFFSET;
			if( oalloc( p, &off ) ) cerror( "bad argument" );
			SETOFF( off, ALSTACK );
		} else if (strftn) /* offsets must be changed to reflect */
				   /* first argument is now a pointer*/
		{
			p->offset = NOOFFSET;
			if(oalloc(p,&off)) cerror("bad argument");
		}
		else {
			if( oalloc( p, &off ) ) cerror( "bad argument" );
		}

		if (p->sclass == PARAM)
			printf(";%s:	.EQU	%d(fp)\n", exname(p->sname),
					(p->offset)/SZCHAR);

	}
	fdefflag = 1;
}

bccode(){ /* called just before the first executable statment */
		/* by now, the automatics and register variables are allocated */
	SETOFF( autooff, SZINT );
	/* set aside store area offset */
	p2bbeg( autooff, regvar, bregvar );
	reg_use = (reg_use > regvar ? regvar : reg_use);
	}

/* -------------------------------------------------------------------------
 * Called just before final exit of the compiler.
 *
 * The symbol table is traversed and appropriate external directives are
 *  emitted for external variables and functions.  This is done here rather
 *  than when the declarations were encountered to avoid duplicate directives
 *  and directives for items declared but not actually used.  Also, functions
 *  may be referenced before (or without) declaraion, so it is not always 
 *  known until the end whether or not functions are external.
 *
 * Duplicate delcarations are legal in C, not in assembler.
 *  
 * This scheme works only in 1-pass configuration, where stab remains.
 */
ejobcode( flag ){  /* flag is 1 if errors, 0 if none */
     register struct symtab *p;
     extern char *release;
     if (proflg) printf("\t.importp	mcount\n");
     for ( p = stab; p < &stab[SYMTSZ]; p++) {
	if (p->stype == TNULL) continue; /* unused entry */
	if (p->sclass == EXTERN) {
		/* When .comms were emitted in commdef, a check
		 * was made for tsize(...) < 0, in which case
		 * nothing was done.  It is not known why this
		 * was done; if the need arises in the future,
		 * a similar check may be necessary here.
		 */
		if (p->sflags & SCOMM) {
			/* emit common specifications even if unreferenced -
			   they may be the only non-extern definition */
                       	printf("\t.comm\t%s,", exname( p->sname ) );
                      	printf( CONFMT, 
		      	        tsize(p->stype, p->dimoff,p->sizoff)/SZCHAR );
			printf( "\t\t;ejobcode\n" );
			} /* SCOMM */
		else
		if (p->sflags & SREF) 
			if ( ISFTN(p->stype))
    				printf("\t.importp %s\t\t;ejobcode\n",
				       exname(p->sname));
			else   
				/* explicit "extern" variable */
				printf("\t.import\t%s\t\t;ejobcode\n", 
				       exname( p->sname ) );
		} /* EXTRN */
	} /* loop */
printf(";%s\n",release);
}

aobeg(){
	/* called before removing automatics from stab */
}

aocode(p) struct symtab *p; {
	/* called when automatic p removed from stab */
}

aoend(){
	/* called after removing all automatics from stab */
}

defnam( p ) register struct symtab *p; {
	/* define the current location as the name p->sname */

	if( p->sclass == EXTDEF&& !ISFTN(p->stype)){
		printf( "	.export	%s\n", exname( p->sname ) );
		}
	if( p->sclass == STATIC && p->slevel>1 ) deflab( p->offset );
	else if (!ISFTN(p->stype)||p->sclass ==STATIC)
		printf( "%s:\n", exname( p->sname ) );
		/*Global functions */
	else	printf( ".exportp\t%s\n%s::\n", exname( p->sname ), exname(p->sname) );

}
#define OCT_LINE_LEN 077	/*  maximum characters on a line */
#define ASSTRINGS 
bycode( t, i ){
#ifdef ASSTRINGS
static	int	lastoctal = 0;
static	int	stringlen = 0;
#endif

	/* put byte i+1 in a string */

#ifdef ASSTRINGS

	i &= OCT_LINE_LEN;/* force new group every so often to please assembler*/
	if ( t < 0 ){  /* the end */
	    if (stringlen !=0) /* non-null sequence */
	    {
		
			if( !lastoctal )
			{
			    fprintf( outfile, "\"\n" );
			    stringlen += 2;
			}
			    
			    
			else	fprintf( outfile, "\n");
			stringlen =0;
			
		}
		    
		else;	
	} else {
		if ( stringlen == 0 )
		{ /* starting new group */
			fprintf( outfile, "\t.byte\t");
			if ( t == '"' || t < 040 || t >= 0177 )
				lastoctal++;
			else
			{
				lastoctal = 0;
				fprintf( outfile, "\"");
				stringlen++;
			}
		} else if ( lastoctal ) fprintf( outfile, ",");
		
		if ( t == '"' || t < 040 || t >= 0177 ){
			if(!lastoctal)
			{   /* terminate previous string */
				lastoctal++;
				fprintf( outfile , "\",");
				stringlen++;
			}
			fprintf(outfile,"x'%x", t );
			stringlen += 5;			
		}
		else if ( lastoctal )
		{
			lastoctal = 0;
			fprintf( outfile, "\"%c", t );
			stringlen += 2;
		}
		else
		{
		    
		    fprintf( outfile, "%c", t);
		    stringlen++;
		}
		
		if ( stringlen >= OCT_LINE_LEN )
		{
		    
		    if ( lastoctal ) fprintf( outfile, "\n");
		    else	fprintf( outfile, "\"\n");
		    stringlen = 0;
		}
		
	}
#else
	i &= 07;
	if(t < 0) {		/* end of the string */
		if(i != 0)
			fprintf(outfile,"\n");
	} else {		/* stash byte t into string */
		if(i==0)
			fprintf(outfile,"	.byte	" );
		else
			fprintf(outfile, "," );
		fprintf(outfile,"x'%x", t );
		if(i == 07)
			fprintf(outfile, "\n" );
	}
#endif
}

zecode( n ){
	/* n integer words of zeros */
	OFFSZ temp;
	if( n <= 0 ) return;
	printf( "	.blkb	%d\n", (SZINT/SZCHAR)*n );
	temp = n;
	inoff += temp*SZINT;
	}

fldal( t ) unsigned t; { /* return the alignment of field of type t */
	uerror( "illegal field type" );
	return( ALINT );
	}

fldty( p ) struct symtab *p; { /* fix up type of field p */
	;
	}

where(c){ /* print location of error  */
	/* c is either 'u', 'c', or 'w' */
	/* GCOS version */
	fprintf( stderr, "%s, line %d: ", ftitle, lineno );
}


/* tbl - toreg() returns a pointer to a char string
		  which is the correct  "register move" for the passed type 
 */
struct type_move {TWORD fromtype; char tostrng[8];} toreg_strs[] =
	{
	CHAR, "movxbd",
	SHORT, "movxwd",
	INT, "movd",
	LONG, "movd",
	FLOAT, "movf",
	DOUBLE, "movl",
	UCHAR,	"movzbd",
	USHORT,	"movzwd",
	UNSIGNED,	"movd",
	ULONG,	"movd",
	-1, ""
	};

char
*toreg(type)
	TWORD type;
{
	struct type_move *p;

	for ( p=toreg_strs; p->fromtype > 0; p++)
		if (p->fromtype == type) return(p->tostrng);

	/* type not found, must be a pointer type */
	return("movd");
}
/* tbl */


char tmpname[30] = TEMPDIR;
char initname[30] = TEMPDIR;
static char *tmpfilename="pcXXXXXX";
static char *initfilename="pciXXXXXX";
extern ninitdef;

main( argc, argv ) char *argv[]; {

	int r;
	char c;
        extern char *release;


#ifdef BUFSTDERR
	char errbuf[BUFSIZ];
	setbuf(stderr, errbuf);
#endif
/* The following are to allow Macro defined temporary directory path */
/* The temp directory path is set in the makefile as the value of TEMP*/
strcat(initname,initfilename);
strcat(tmpname,tmpfilename);
	if (argc>1 && strcmp(argv[1],"-B")==0) {
		setbuf(stdout,NULL);
		setbuf(stderr,NULL);
		}
	mktemp(tmpname);
	mktemp(initname);
	tmpfile = fopen( tmpname, "w" );
	if(tmpfile == NULL) cerror( "Cannot open temp file" );
	initfile = fopen( initname, "w" );
	if(initfile == NULL) cerror( "Cannot open initcode temp file" );
	r = mainp1( argc, argv );
	tmpfile = freopen( tmpname, "r", tmpfile );
	if( tmpfile != NULL )
		while((c=getc(tmpfile)) != EOF )
			putchar(c);
	else cerror( "Lost temp file" );
	unlink(tmpname);
	initfile = freopen( initname, "r", initfile );
	printf("\t.endseg\n");   /* Jima */
	if( initfile != NULL )
		if( ninitdef != 0)
		{
			
			printf("\n; Initcode \n\t.program\n%%initfun::\n");

			while((c=getc(initfile)) != EOF )
				putchar(c);
			printf("\trxp 0\n\t.endseg\n");
		}
		else ;
	else cerror( "Lost initcode temp file" );
	unlink(initname);
	return(r);
	}

struct sw heapsw[SWITSZ];	/* heap for switches */

genswitch(p,n) register struct sw *p;{
/*
 *	p points to an array of structures, each consisting
 *	of a constant value and a label.
 *	The first is >=0 if there is a default label;
 *	its value is the label number
 *	The entries p[1] to p[n] are the nontrivial cases
 */
	register i;
	register CONSZ j, range;
	register dlab, swlab;
	int	 bound_lab;

	range = p[n].sval-p[1].sval;
/*
 * someday...
 *
 * use check[bw] if the selector is in range.
 *
 * use an fsa instead of the heap switch
 */
	if(0<range && range <= 3*n
	&& n>=4 ) { /* implement a direct switch */

		swlab = getlab();
		dlab = p->slab >= 0 ? p->slab : getlab();
		/*
		 * selector is in r0
		 */
		printf("\tcheckd\tr0,L%d,r0\n\tbfs\tL%d\n",
			bound_lab = getlab(), dlab);
		printf("L%d:\tcasew	L%d+4[r0:w]\n", swlab, swlab);
		for(i=1,j=p[1].sval; i<=n; j++) {
			printf("\t.word\tL%d-L%d\n",
				(j == p[i].sval ?
					((j=p[i++].sval), p[i-1].slab)
				      : dlab),
				swlab);
		}
		printf("L%d:\t.double	%d,%d\n",
			bound_lab, p[n].sval, p[1].sval);
		if( p->slab < 0 )
			printf("L%d:\n", dlab);
		return;

	}
/*
 * Actually, for a large & sparse switch, the smallest code might use
 * a table and a simple fsa.
 */
	if(n>8) {	/* heap switch */

		heapsw[0].slab = dlab = p->slab >= 0 ? p->slab : getlab();
		makeheap(p, n, 1);	/* build heap */

		walkheap(1, n);	/* produce code */

		if( p->slab >= 0 )
			branch( dlab );
		else
			printf("L%d:\n", dlab);
		return;
	}

	/* debugging code */

	/* out for the moment
	if( n >= 4 ) werror( "inefficient switch: %d, %d", n, (int) (range/n) );
	*/

	/* simple switch code */

	for( i=1; i<=n; ++i ){
		/* already in r0 */
		if (p[i].sval <= 7 && p[i].sval >= -8)
		    printf("	cmpqd 	%ld,r0",p[i].sval);
		else
		{
		    printf( "	cmpd	%ld,r0",p[i].sval );
		}
		
		printf( "\n	beq	L%d\n", p[i].slab );
	}

	if(p->slab>=0)
		branch( p->slab );
}
	    

makeheap(p, m, n)
register struct sw *p;
{
	register int q;

	q = select(m);
	heapsw[n] = p[q];
	if( q>1 ) makeheap(p, q-1, 2*n);
	if( q<m ) makeheap(p+q, m-q, 2*n+1);
}

select(m) {
	register int l,i,k;

	for(i=1; ; i*=2)
		if( (i-1) > m ) break;
	l = ((k = i/2 - 1) + 1)/2;
	return( l + (m-k < l ? m-k : l));
}

walkheap(start, limit)
{
	int label;


	if( start > limit ) return;
	if (heapsw[start].sval <7 && heapsw[start].sval > -8)
	    printf("	cmpqd	%d,r0\n",  heapsw[start].sval);
	else
	    printf("	cmpd	%d,r0\n",  heapsw[start].sval);

	printf("	beq	L%d\n", heapsw[start].slab);
	if( (2*start) > limit ) {
		printf("	br 	L%d\n", heapsw[0].slab);
		return;
	}
	if( (2*start+1) <= limit ) {
		label = getlab();
		printf("	blt	L%d\n", label);
	} else
		printf("	blt	L%d\n", heapsw[0].slab);
	walkheap( 2*start, limit);
	if( (2*start+1) <= limit ) {
		printf("L%d:\n", label);
		walkheap( 2*start+1, limit);
	}
}
