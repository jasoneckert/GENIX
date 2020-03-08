# ifndef NOSCCS
static char *sccsid = "@(#)c20.c	2.56	8/23/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif
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

/*
 *	 C object code improver
 */

extern char *version;  /* In module c2vers.c */

#include "c2.h"
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
char *index();

int gdebug=0;
int ckflag=0; /* -c option: Insert calls to __ck_{precall,postcall} */

static char _sibuf[BUFSIZ], _sobuf[BUFSIZ];
long	isn	= 2000000;
	 
struct optab *oplook();
struct optab *getline();
static cleanup();
static char *outname; /* Set to output file name if created, else left zero */

static long lgensym[10] =
  {100000L,200000L,300000L,400000L,500000L,600000L,700000L,800000L,900000L,1000000L};

static int bytesize[] = {0,1,2,4}; /* indexed by type */

/* Negated conditional branch ops - indexed by original subop: */
char negbr[] = {
/* []:  jeq  jle  jge  jlt   jgt  jne    <signed tests 0-5> */
        JNE, JGT, JLT, JGE,  JLE, JEQ,
/*     (jeq) jlos jhis jlo   jhi  (jne)  <unsigned tests 6-11> */
	JNE, JHI, JLO, JHIS, JLOS,JEQ,
/*      jbfc  jbfs  spare  spare  jbcc  jbcs  spare  spare  12-19*/
	JBFS, JBFC, NULBR, NULBR, JBCS, JBCC, NULBR, NULBR };

static int lineno = 0;  /* count asm lines for panic messages */
char enable_iterate=1, enable_comjump=1;

struct node first;
struct node zeronode; /* Remains zero initialized always */

/* Return ptr to "an" bytes, aligned.  Most space is reclaimed after each
   cycle, because main() resets "lasta", the availble-location pointer. */
struct node *
alloc(an)
{
	REGISTER int n;
	REGISTER char *p;

	n = an;
	n+=sizeof(char *)-1;
	n &= ~(sizeof(char *)-1);
	if (lasta+n >= lastr) {
		if (sbrk(2000) == -1) {
			p = "Optimizer: out of space\n";
			fprintf(stderr,p); printf(p);
			exit(1);
		}
		lastr += 2000;
	}
	p = lasta;
	lasta += n;
	return( (struct node *)p );
}

/* push-down of active .program & .static declarations */
#define SEGSTKSZ 30
short seg_stack[SEGSTKSZ] = {PROGRAM};
short *curr_seg = seg_stack;

push_segdecl(opc) short opc; { 
	if (curr_seg >= (seg_stack+SEGSTKSZ-1)) 
		panic("segments nested too deep");
	*++curr_seg = opc; 
}

short
pop_segdecl() { 
	if (curr_seg <= seg_stack) panic("pop_segdecl");
	return( *curr_seg-- );
}

main(argc, argv)
char **argv;
{
	REGISTER int niter, maxiter, isend;
	int nflag,infound; char buffer_output=1;

	lasta = lastr = (char *)sbrk(2);  /* Initialize storage allocator */

	signal(SIGINT,cleanup); /* do cleanup when interrupted */

	nflag = 0; infound=0; argc--; argv++;
	while (argc>0) {/* get flags */
		if (**argv=='+') debug = gdebug = nflag = xdebug = tdebug = 1;
		else if (**argv=='-') { REGISTER char *cp = *argv; 
			while (*++cp) switch (*cp) {
			case 'g': gdebug=1; break; /* Getline debugging */
			case 'd': debug=1; break;  /* "General" debugging */
			case 'x': xdebug=1; break; /* "Detailed" debugging */
			case 't': tdebug=1; break; /* Do inTerpass dumps */
			case 'i': ioflag=1; break; /* Optimizing i/o program */ 
			case 'n': nflag=1; break;  /* Give statistics */
			case 'c': ckflag=1; break;  /* Insert calls to __ck_* */
			case 'u': buffer_output=0; break;
			case 'p': /* pass code characters follow */
				enable_rmove=0; enable_bmove=0;
				enable_iterate=0; enable_comjump=0;
				while(*++cp) switch(*cp) {
				case 'r': enable_rmove=1; break;
				case 'b': enable_bmove=1; break;
				case 'i': enable_iterate=1; break;
				case 'c': enable_comjump=1; break;
				}
				--cp;
				break;
					
			default:
				fprintf(stderr,"c2: Unknown option -%c\n", 
					*cp);
				fprintf(stderr," Options are -gdinxtu -p<passes>\n");
				exit(1);
			}
		} else if (infound==0) {
			if (freopen(*argv, "r", stdin) ==NULL) {
				fprintf(stderr,"c2: can't find %s\n", *argv);
				exit(1);
			}
			setbuf(stdin,_sibuf); ++infound;
		} else {
			outname = copy(*argv);  /* Save for cleanup() */
			if (freopen(outname, "w", stdout) ==NULL) {
				fprintf(stderr,"c2: can't create %s\n",outname);
				exit(1);
			}
		}
		if (buffer_output) setbuf(stdout,_sobuf);
		else {setbuf(stdout,NULL); setbuf(stderr,NULL);}
		argc--; argv++;
	}
	printf(";%s\n",version);
	opsetup();
	lasta = firstr = lastr = (char *)alloc(0);
	maxiter = 0;
	if (ckflag) {
		printf("	.importp	__ck_precall\n");
		printf("	.importp	__ck_postcall\n");
	}
	do {
		isend = input(); /* Stops at end of 1 or more routines */
		niter = 0;
		/* Expand literals so peep-hole stuff in bmove will work */
		refcount(); rmove( /*expand_leterals:*/ 1);
		/* Do peephole stuff, insert loads for multi-use reads */
		bmove( /*do_reads:*/ 1,  /*del_dead_stores:*/ 0 );

		{ REGISTER int bugtest=0;
		do {
			refcount();
			do {
			if (bugtest++ > 20) panic("no convergence");
				iterate();
				clearreg(0/*all,not just temps*/);
				niter++;
			} while (nchange);
			comjump();
		   	rmove(0); /* This undoes "expand_literals" above */
				  /* and uses registers loaded by bmove  */
		} while (nchange || jumpsw());

		bugtest=0;
		do { 	nchange=0;
			bmove( /*do_reads:*/1,  /*del_dead_stores:*/ 1 );
			refcount(); rmove(0);
			if (bugtest++ > 10) {
				REGISTER char *s = "warning - internal c2 \
problem (no convergence) - please notify C2 maintainer\n\
;\t(WARNING ONLY - no bad code generated)\n";
				printf(";Asm line %d:%s",lineno,s);
				fprintf(stderr,", ASM line %d:%s",lineno,s);
				break;
			}
		} while (nchange);
		}
		output();
		if (niter > maxiter)
			maxiter = niter;
		lasta = firstr;
	} while (isend);
	if (nflag) {
		fprintf(stderr,"%d iterations\n", maxiter);
		fprintf(stderr,"%d jumps to jumps\n", nbrbr);
		fprintf(stderr,"%d inst. after jumps\n", iaftbr);
		fprintf(stderr,"%d jumps to .+1\n", njp1);
		fprintf(stderr,"%d redundant labels\n", nrlab);
		fprintf(stderr,"%d cross-jumps\n", nxjump);
		fprintf(stderr,"%d code motions\n", ncmot);
		fprintf(stderr,"%d branches reversed\n", nnegbr);
		fprintf(stderr,"%d redundant moves\n", redunm);
		fprintf(stderr,"%d equivalent to nops\n", equivnop);
		fprintf(stderr,"%d simplified addresses\n", nsaddr);
		fprintf(stderr,"%d commuted operands to elim move\n", ncommut);
		fprintf(stderr,"%d register-register substitutions\n", nregsub);
		fprintf(stderr,"%d loops inverted\n", loopiv);
		fprintf(stderr,"%d redundant jumps\n", nredunj);
		fprintf(stderr,"%d common seqs before jmp's\n", ncomj);
		fprintf(stderr,"%d skips over jumps\n", nskip);
		fprintf(stderr,"%d sob's added\n", nsob);
		fprintf(stderr,"%d redundant tst's\n", nrtst);
		fprintf(stderr,"%d jump on bit\n", nbj);
		fprintf(stderr,"%d field operations\n", nfield);
		fprintf(stderr,"%dK core\n", ((unsigned)lastr+01777) >> 10);
	}
	putc('\n',stdout);
	fflush(stdout); exit(0);
}

/* Read in some code until the "end of a routine" (as determined by getline)
   is encountered.  Called from main().

   Some canonicalization of unusual instructions is done to simplify
   processing elsewhere.  These changes are generally undone by output().

   Floating point regs f0..f7 are changed to r8..r15 (reversed by output()).
(jima) */
input()
{
	REGISTER struct node *p, *lastp;
	struct optab *op;
	REGISTER char *cp1;
	static struct optab F77JSW = {".double", T(JSW,1)};

	lastp = &first;
	for (;;) {
	  top:
		op = getline(); /* read label or instruction */

	  dispatch:
		switch (op->opcode&0377) {
	
		case LABEL:
			p = alloc(sizeof first);
			if (isdigit(line[0]) && (p->labno=locdef(line)) ||
			  (line[0] == 'L') && (p->labno=getnum(line+1))) {
				COMBOP(p) = LABEL;
				if (p->labno<100000L && isn<=p->labno) isn=1+p->labno;
				p->code = 0;
			} else { /* "Data" (not local) label */
				COMBOP(p) = DLABEL;
				p->labno = 0;
				p->code = copy(line);  /* The label name */
			}
			break;
	
		case LGEN:
			if (*curlp!='L' && !locuse(curlp)) goto std;
			op= &F77JSW;
		case BR:
			if (op->opcode==T(BR,RXP) || op->opcode==T(BR,RET)) goto std;
		case CBR:
		case JMP:
		case JSW:
		case ACB:
			p = alloc(sizeof first);
			COMBOP(p) = op->opcode; p->code=0; cp1=curlp;
			if ((!isdigit(*cp1) || 0==(p->labno=locuse(cp1))) &&
			  (*cp1!='L' || 0==(p->labno = getnum(cp1+1)))) {/* jbs, etc.? */
				while (*cp1++); while (*--cp1!=',' && cp1!=curlp);
				if (cp1==curlp ||
				  (!isdigit(*++cp1) || 0==(p->labno=locuse(cp1))) &&
				  (*cp1!='L' || 0==(p->labno=getnum(cp1+1))))
					p->labno = 0;
				else *--cp1=0;
				p->code = copy(curlp);
			}
			if (isn<=p->labno) isn=1+p->labno;
			break;

		case CHECK: { /* possibly prepatory to CASE */
			char *lp, *endlp;
			p = alloc(sizeof first);
			COMBOP(p) = op->opcode;
			p->code = copy(curlp);
			lp=curlp; while (*lp++!=','); /* find hi_low label */
			/* terminate label string for getnum... */
			endlp=lp; while(*endlp++!=','); *--endlp = 0;
			p->labno = getnum(lp+1);
			if (isn<=p->labno) isn=1+p->labno;
 if (xdebug) printf("input CHECK: labno=%d code=%s\n",p->labno,p->code);
			break;
			}

		case ADDR: /* If LABEL reference, labno=lab no. and
		              code=operand(s) after "<label>,".  Otherwise,
			      labno=0 and code=all operands.

			      Convert absolute-constant mode to MOV,LONG 
			      (will be converted back in output()).  (jima) */
			p=alloc(sizeof first);
			COMBOP(p)=op->opcode; p->code=0; cp1=curlp+1;
			if (cp1[-1]=='L' || isdigit(cp1[-1])) {
				while (*cp1++!=','); *--cp1=0;
				if (0!=(p->labno=locuse(curlp)) ||
					0!=(p->labno=getnum(curlp+1))) p->code=copy(cp1+1);
				else {*cp1=','; p->code=copy(curlp);}
			} else 
			if (cp1[-1] == '@' && isimm(cp1)) {
				COMBOP(p) = T(MOV,LONG);
				p->code=copy(cp1); 
				p->labno=0;
				op = 0;
			} else {
				p->code=copy(--cp1); 
				p->labno=0;
			}
			break;


		case MOVQ:
		case CMPQ:
		case ADDQ:
			p = alloc(sizeof first);
			COMBOP(p) = op->opcode-1;  /* non-quick form */
			p->labno = 0;
			p->code = copy(curlp);
			op = 0;	/* ...will become quick again in output */
			break;

		case MODULE:
		case SET:
		case COMM:
		case LCOMM:
			printf("%s\n",line); goto top;

		/* Segments: All data declarations are copied through
		   immediately.  All segment-control declaratives
		   are copied through (except initially empty pairs),
		   although .program segments will appear empty, since their 
		   contents are saved and regenerated all together at the 
		   end of the procedure.  .program is assumed to initially 
		   obtain.  
		   
		   Note that this prevents use of any future lexical
		   assembler features outside of .program segments. (jima) */

		case ENDSEG:
			printf(".endseg\n");
			pop_segdecl();
			op = getline(); /* read next item... */
			if (*curr_seg == STATIC ) goto copy_data_decls;
			if (*curr_seg == PROGRAM) goto dispatch;
			panic("endseg");
		case PROGRAM:
			op = getline(); /* read next item... */
			if (ENDSEG==(op->opcode&0377)) goto top; /*ignore them*/
			printf(".program\n");
			push_segdecl(PROGRAM);
			goto dispatch; /* go process succeeding item */
		case BSS:
			panic("bss"); /* not in 16K (but future? - jima) */
		case STATIC:
			op = getline(); /* read next item... */
			if (ENDSEG==(op->opcode&0377)) goto top; /*ignore them*/
			printf(".static\n");
			push_segdecl(STATIC);

		     copy_data_decls: /* come here after reading next op */
			/* pass all data decls through unprocessed */
			while( op->opcode != END) {
				if (op->opcode==STATIC || 
				    op->opcode==PROGRAM ||
				    op->opcode==ENDSEG) goto dispatch;
				printf( (op->opcode==LABEL ? "%s:\n":
					 op->opcode==COMMENT ? "; %s\n":"%s\n"),
					        line );
				op=getline();
			}
			/* end reached during data - force return to code seg */
			printf(".program\n");
			goto std; /* process the END */

		default:
		std:
			p = alloc(sizeof first);
			COMBOP(p) = op->opcode;
			p->labno = 0;
			if ((p->subop & 0xF) >= FLOAT ||
			    (p->subop <= (DOUBLE<<4) && p->subop >= (FLOAT<<4)))
				f07_to_r815(curlp); /*Map FPU regs to r8..r15 */
			p->code = copy(curlp); /* rest of line (operands) */
			break;

		}
		p->forw = 0;
		p->back = lastp;
		p->pop = op;
		lastp->forw = p;
		lastp = p;
		p->ref = 0;
		if (p->op==CASE) {
			char *lp;
			/* Read the case index words and make JSW nodes.
			   They are all ".word Lxxx-Lcasebase"; the last  
			   is immediately followed by the hi,low constants 
			   for the preceeding CHECK.  output() reconstructs
			   all this using a brand-new casebase label.(jima) */
			while ( ((op=getline())->opcode&0377)==WGEN && *curlp=='L') {
				p = alloc(sizeof first); COMBOP(p) = JSW; p->code = 0;
				lp=curlp; while(*lp++!='-'); *--lp=0; p->labno=getnum(curlp+1);
				if (isn<=p->labno) isn=1+p->labno;
				p->forw = 0; p->back = lastp; lastp->forw = p; lastp = p;
				p->ref = 0; p->pop=0;
			}
			goto dispatch; /* just read something else */
				       /* This is ugly code, but I'm 
					  trying to make only minimal
					  changes in the first cut... */
		}
		if (p->op==DEI || p->op==MEI) {
			/* Add a bogus third operand which explicitly mentions
			   the implied adjacent location (or register)
			   referenced by dest (rmw.2i).  This is deleted
			   by output. (jima) */
			REGISTER int r;
			splitrand(p);
			if ( 0<=(r=isreg(regs[RT2])) )
				sprintf(regs[RT3],"r%d",r+1);
			else
				sprintf(regs[RT3],
					"%d+%s", bytesize[p->subop],regs[RT2]);
			newcode(p);
		}
		if (p->op==MOVS || p->op==MOVST || 
		    p->op==CMPS || p->op==CMPST ||
		    p->op==SKPS || p->op==SKPST ) {
			/* Add bogus operands to mention implied register */
			/* operands.  Removed by output().		  */
			if (p->code[0]) {
				panic("Can't handle movs/skps/cmps B/U/W\n");
			}
			strcpy(regs[RT1],"r0");
			strcpy(regs[RT2],"r1");
			strcpy(regs[RT3],"r2");
			strcpy(regs[RT4],"r3");
			strcpy(regs[RT5],"r4");
			newcode(p);
		}


		if (op->opcode==EROU) /* Item which denotes End Routine */
			return(1);
		if (op->opcode==END)
			return(0);
	}
}

static char lookahead_label[LINESZ] = {0};  /* saved EROU-triggering label */
struct optab *
getline()
{
	REGISTER char *lp;
	REGISTER int c;
	REGISTER struct optab *result;
	static struct optab OPLABEL={"",LABEL};
	static struct optab OPEROU={"",EROU};
	static struct optab OPEND={"",END};
	char *startlp;

	curlp = line; /* in case oplook not called */

	lp = line;
	if (lookahead_label[0] != 0) {
		/* previously encountered procedure-start labal was saved */
		strcpy(line,lookahead_label);
		lookahead_label[0] = 0;
		result = &OPLABEL;
	}
	else
	while(lp==line) { /* find non-comment line */
	   /*skip initial space */
	   while (EOF!=(c=getchar()) && isspace(c)) if (c=='\n') ++lineno;

	   /* read interesting characters - <label> or <op + operands> */
	   /* Stop on end, _label:, or newline (after eating comments) */
	   
	   for(;;) {
	        if (c==';')
	 		while((c=getchar())!='\n' && c!=EOF); /* eat comment */
		if (c=='\"') { /* read quoted string */
			*lp++ = c;
			while( (c=getchar()) != '\"') {
				if( c=='\n' ) { /* eat escaped newlines */
					++lineno;
					if( *--lp!='\\') 
						panic("unclosed \"quote");
				} else
					*lp++ = c;
			}
			/* c is now the closing quote, stored below */
		}
		if (c==':') {
			REGISTER char *lp1 = line;
			while(!isspace(*lp1) && ++lp1<lp);
			if (lp1==lp) {  /* It's a label... */
				if( (c=getchar())==':' ) {
					/* An exported label ala "_foo::" */
					*lp++ = c; /* 1st colon is in name */
				} else {
					if (ungetc(c,stdin)<0) 
						panic("can't ungetc");
		        	}
				*lp++ = 0; /* label sans 2nd colon */
				if ((line[0]=='_' || line[0]=='%') && 
				    !isdigit(line[1])) {
					/* Non-local label - assume this
					   introduces a new procedure.  Save
					   it for now, and end previous proc */
					strcpy(lookahead_label,line);
					line[0] = 0;
					result = &OPEROU;
				} else {
					result = &OPLABEL;/*may become DLABEL*/
				}
				break;
			} /* else imbedded space - can't be a label */
		}
		if (c==EOF) {
			if (lp!=line) panic("partial last line");
			*lp++ = 0;
			result = &OPEND;
			break;
		}
		if (c=='\n') {
			++lineno;  /* for panic() */
			if (lp!=line) {
				/* delete trailing space and exit */
				while(isspace(*(lp-1))) lp--; 
				*lp++ = 0;
				result = oplook(); 
				/* Returns optab[] entry or {"",0} */
			}
			break;
		}
		*lp++ = c;
		c = getchar();
	   }/* main character forloop */
	}/* while haven't got a line yet */
   
	/* result has been set to an optab record */

	if (gdebug) printf("getline: op=\"%s\"(code=%d)\tline=%s\n",
			    result->opstring, result->opcode, line );
	if (lp > line+MAX_STRLEN) panic("input line too long");
	return(result);
}

/* convert null-terminated numeric string to binary.  Return zero if not
   properly terminated. */
long
getnum(p)
REGISTER char *p;
{
	REGISTER int c; int neg; REGISTER long n;

	n = 0; neg=0; if (*p=='-') {++neg; ++p;}
	while (isdigit(c = *p++)) {
		 c -= '0'; n *= 10; if (neg) n -= c; else n += c;
	}
	if (*--p != 0)
		return(0);
	return(n);
}

/* local labels (e.g, 3f ... 3:) are replaced by new unique labels.
   lgensym[] gives the (next) unique label for each of the ten possible 
   local labels which may be in use at any time.  locuse() detects local 
   label references, assigning unique labels when found, and lgensym 
   detects local label definitions, and returns their unique equivalents 
   and increments the appropriate lgensym[] entry.  This is presnetly only 
   a VAX Unix assembler phenomona, but may be a Mesa feature eventually, so 
   it is left in (comment:jima) */

locuse(p)
REGISTER char *p;
{
	REGISTER int c; int neg; REGISTER long n;

	if (!isdigit(p[0]) || p[1] != 'f' && p[1] != 'b' || p[2]) return(0);
	return (lgensym[p[0] - '0'] - (p[1] == 'b'));
	/* lgensym[i] incremented by locdef when the label is encountered */
}

locdef(p)
REGISTER char *p;
{

	if (!isdigit(p[0]) || p[1]) return(0);
	return (lgensym[p[0] - '0']++);
}

output()
{
	REGISTER struct node *p;
	int casebas;

	p = &first;
	while (p = p->forw) switch (p->op) {

	case END:
		fflush(stdout);
		return;

	case LABEL:
		printf("L%d:\n", p->labno);
		continue;

	case DLABEL:
		printf("%s:\n", p->code);
		continue;

	case COMMENT: /* special comment which is passed through.*/
	      	/* (used in c2 test and demo programs)	 */
		printf("; %s\n", p->code);
		continue;	

	case CXP: case CXPD: case JSR: case BSR:
		if (ckflag && strcmpn(p->code,"__ck_",5) != 0){
			/* Insert calls to checking routines around user call */
			printf("	cxp	__ck_precall\n");
			if (p->pop==0 || p->pop->opstring[0]==0)
				panic("-c call, no pop");
			printf("	%s	%s\n",p->pop->opstring,p->code);
			printf("	cxp	__ck_postcall\n");
			continue;
		}
		goto std;

	case MOVQ: 
	case CMPQ:
	case ADDQ:
		panic("outquick"); /* should not exist during processing */
	case MOV: 
	case CMP:
	case ADD:
	{	REGISTER int val;
		REGISTER char *delim;
		if ( (delim=isimm(p->code)) ) {
			*delim = '\0'; val = getnum(p->code); *delim = ',';
			if (val>=-8 && val<=7) {
				p->op += 1;  /* convert to quick form */
				p->pop=0;
			}
			else 
			if (COMBOP(p)==T(MOV,LONG) && val>=-8192 && val<=8191){
				/* 4 or 5-byte instruction */
				COMBOP(p) = T(ADDR,LONG); 
				p->pop=0;
				p->code = copy2("@",p->code);
			} 
		} /* fall through to std... */ 
	}

	{ REGISTER int p_subop;
	default: std:
		p_subop = p->subop;
		if ((p_subop&0xF) >= FLOAT || 
		    (p_subop < (OP2<<4) && p_subop >= (FLOAT<<4)))
			/* Possible FPU instruction, restore proper reg names */
			r815_to_f07(p->code); /* map r8..15 to f0..f7 */
		if (p->pop==0) {/* must find it */
			REGISTER struct optab *op;
			for (op=optab; op->opstring[0]; ++op)
				if (op->opcode==COMBOP(p)) {p->pop=op; break;}
			if (p->pop==0) {dump(p);panic("bad output opcode");}
		}
		if (p->pop->opstring[0]!=0 ||
		    (p->code && p->code[0])) {
			printf("\t%s", p->pop->opstring);
			if (p->code) printf("\t%s", p->code);
			if (p->labno!=0) printf("%cL%d\n",
				  	    (p->code ? ',' : '\t'), p->labno);
			else printf("\n");
		}
		continue;
	}

	case ADDR:
	case LXPD:
		if (p->labno==0) goto std;
		printf("\taddr	L%d,%s\n", p->labno, p->code);
		continue;

	case CASE:   /* new for 16K - baselabel is on CASE, not after */
		{ REGISTER char *cp;
		casebas = isn++; /* New label for the case instruction */
		cp=p->code; while(*cp++!='+');  /* skip old "Lxxx+4" */ 
		if (*cp++ != '4') panic("CASEout");
		/* Can't support case[bd] now because of the F77JSW 
		   strangeness, which preempts the type field (jima) */
		if(p->subop != WORD) panic("case[^w]");
		printf("L%d: case%c L%d+4%s\n",
		       casebas,"bwd"[p->subop-BYTE],casebas,cp );
		continue;
		}

	case JSW:
		if (p->subop!=0) {/* F77JSW */
			printf(".long\tL%d\n",p->labno); continue;
		}
		printf(".word\tL%d-L%d\n", p->labno, casebas);
		continue;

	case CHECK: 
		{ REGISTER char *cp;
		/* L<labno> is the middle of 3 operands */
		cp=p->code; 
		while(*cp++!=','); *cp=0;   /* terminate "op1," */
		while(*++cp!=',');          /* find start of ",op3" */
		printf("\tcheck%c\t%sL%d%s\n",
		       "bwdq"[p->subop-BYTE], p->code, p->labno, cp );
		continue;
		}
	case DEI: case MEI:	
		{ REGISTER char *cp;
		/* Remove bogus third operand added by input() */
		cp=p->code; 
		while(*cp++!=',');  while (*cp++!=','); /* find 3rd op'rand */
		cp[-1]=0;
		goto std;
		}
	case MOVS: case CMPS:
		/* Remove bogus operands added by input() */
		p->code[0] = '\0'; 
		goto std;
	}
}

/* Copy *ap to new storage.  copy2 concatenates two strings into new storage.
#ifdef MULTIVAL
   In both cases, an extra null is appended, to accomodate multiple-string
   regs[] entries and similar (jima) 
#endif
*/
/* Copy used to be variable-argument, to handle both cases; but that
    doesn't work on the 16K because the vax nargs() feature is not
    supported (jima) */
char * 
copy(ap) char *ap; {
	REGISTER char *p, *np, *onp;

	p = ap;
	while (*p++);
#ifdef MULTIVAL
	onp = np = (char *)alloc( 1+p-ap );
	while (*np++ = *ap++);  /* copy the string */
	*np = '\0';  		/* Add the extra null */
#else
	onp = np = alloc( p-ap );
	while (*np++ = *ap++);  /* copy the string */
#endif
	return(onp);
}
char *
copy2(ap1,ap2) char *ap1,*ap2; {
	REGISTER char *p, *np;
	REGISTER int nbytes;
	REGISTER char *onp;

	nbytes = 0;
	p = ap1; while (*p++) ++nbytes;
	p = ap2; while (*p++) ++nbytes;
#ifdef MULTIVAL
	onp = np = (char *) alloc( nbytes+2 ); /* two for the final 2 nulls */
	while (*np++ = *ap1++) --nbytes;   /* copy the first string */
	--np;
	while (*np++ = *ap2++) --nbytes;   /* ...then the second.   */
	*np = '\0';  			   /* Add the extra null */
#else
	onp = np = (char *) alloc( nbytes+1 ); /* one for the final null */
	while (*np++ = *ap1++) --nbytes;   /* copy the first string */
	--np;
	while (*np++ = *ap2++) --nbytes;   /* ...then the second.   */
#endif

	if (nbytes!=0) panic("copy2");
	return(onp);
}

/* Change FPU register references (f0..f7) to bogus registers r8..r15.  There
   must be sufficient storage to allow for string expansion.

   Called from input().  This allows all regs (including FPU regs) to be
   processed together, e.g., in regs[], uses[], etc.	(jima)
*/

f07_to_r815(s) REGISTER char *s; {
	REGISTER char *last, *newlast;
	while (*s) {
		if (*s=='f' && (s[2]==',' || s[2]==0) ){
			*s++ = 'r';
			if (*s <= '1') *s += ('8' - '0');
			else {
				last = index(s,'\0'); newlast = last+1;
				while (last>=s) (*newlast-- = *last--);
				*s = '1'; s[1] -= ('2' - '0');
			}
		}
		++s;
		while (*s && *s++ != ',') {};
	}
}

/* Change r8..r15 back to f0..f7.  Called from output().   (jima) */

r815_to_f07(s) REGISTER char *s; {
	REGISTER char *src, *dst; REGISTER int c;
	while (*s) {
		if (*s=='r' && (((c=s[1])=='1' && isdigit(s[2])) 
				|| c=='8' || c=='9')) {
			*s++ = 'f';
			if (c >= '8') *s = c - ('8' - '0');
			else {
				strcpy( s, s+1 );
				*s += ('2' - '0');
			}
			if (s[1] && s[1] != ',') panic("r815_to_f01");
		}
		++s;
		while (*s && *s++ != ',') {};
	}
} 

#define	OPHS	560
struct optab *ophash[OPHS];

opsetup() /* called from main().  Builds hashtable for optab[]. (jima) */
{
	REGISTER struct optab *optp, **ophp;
	REGISTER int i,t;

	for(i=NREG+5;--i>=0;) regs[i] = (char *)alloc(C2_ASIZE);
	for (optp = optab; optp->opstring[0]; optp++) {
		t=OPNLEN; i=0; while (--t>=0) i+= i+optp->opstring[t];
		ophp = &ophash[i % OPHS];
		while (*ophp++) {
/*			printf("\ncollision: %d %s %s",
/*				ophp-1-ophash,optp->opstring,(*(ophp-1))->opstring);
*/
			if (ophp > &ophash[OPHS])
				ophp = ophash;
		}
		*--ophp = optp;
	}
}

/* Recognize operation in "line", returning optab[] entry. {"",0} is returned
   if the opcode is unrecognizable.  Sets curlp to point to operands after
   the recognized op.  If unrecognized, curlp is line origin. (jima) */
struct optab *
oplook()
{
	REGISTER struct optab *optp,**ophp;
	REGISTER char *p,*p2;
	REGISTER int t;
	char tempop[20];
	static struct optab OPNULL={"",0};

	/* get opcode string */
	for (p=line,p2=tempop; 
	     *p && !isspace(*p) && p2<tempop+18; *p2++ = *p++); *p2=0; 
	/* set curlp to first operand */
	p2=p; while (isspace(*p2)) ++p2; curlp=p2;
	/* look up opcode and return token codes */
	t=0; while(--p>=line) t += t+*p; ophp = &ophash[t % OPHS];
	while (optp = *ophp) {
		if (equstr(tempop,optp->opstring)) return(optp);
		if ((++ophp) >= &ophash[OPHS]) ophp = ophash;
	}
	/* Unrecognizable opcode */
	curlp = line;
	return(&OPNULL);
}

/* Recompute refc (ref count) and ref (referenced node) fields. (jima) */
refcount()
{
	REGISTER struct node *p, *lp;
		 struct node *labhash[LABHS];
	REGISTER struct node **hp;

	/* Rebuild label hash table */
	for (hp = labhash; hp < &labhash[LABHS];)
		*hp++ = 0;
	for (p = first.forw; p!=0; p = p->forw)
		if (p->op==LABEL) {
			labhash[p->labno % LABHS] = p;
			p->refc = 0;
		}
	for (p = first.forw; p!=0; p = p->forw) {
		if (COMBOP(p)==BR || p->op==CBR || p->op==JSW || p->op==JMP
		  || p->op==CHECK
		  || p->op==ACB || (p->op==ADDR && p->labno!=0)) {
			/* This node references a local label (jima) */
			p->ref = 0;
			lp = labhash[p->labno % LABHS];
			if (lp==0 || p->labno!=lp->labno)
			for (lp = first.forw; lp!=0; lp = lp->forw) {
				if (lp->op==LABEL && p->labno==lp->labno)
					break;
			}
			if (lp) {
				/* Use last of several labels */
				lp = nonlab(lp)->back; p->labno = lp->labno;
				p->ref = lp;
				if (xdebug) { printf("setting ref:"); dump(p); }
				lp->refc++;
			}
		}
	}
	/* delete initially unreferenced labels, but leaving any which
	   might be involved in a non-standard usage, 
	   e.g., CASE (comment:jima) */
	for (p = first.forw; p!=0; p = p->forw)
		if (p->op==LABEL && p->refc==0
		 && (lp = nonlab(p))->op && lp->op!=JSW)
			decref(p);
}

iterate()
{
	REGISTER struct node *p, *rp, *p1;

	if (!enable_iterate) return;
	if (xdebug) printf("iterate...\n");
	nchange = 0;
	/* Change "CBR -> BR -> somewhere" to "CBR -> somewhere"     (jima) */
	for (p = first.forw; p!=0; p = p->forw) {
		if (xdebug) dump(p);
		if ((p->op==BR||p->op==CBR||p->op==JSW) && p->ref) {
			rp = nonlab(p->ref);
			if (rp->op==BR && rp->labno && p->labno!=rp->labno) {
				if (xdebug) {
					printf("brbr:bypassing ");
					dump(rp);
				}
				nbrbr++;
				p->labno = rp->labno;
				decref(p->ref);
				rp->ref->refc++;
				p->ref = rp->ref;
				nchange++;
			}
		}
/* Change "CBR c -> foo; BR bar; foo: ..." to "CBR ~c -> bar" 		(jima)*/
#ifndef COPYCODE
		if (p->op==CBR && COMBOP((p1 = p->forw))==BR) {/* combop: RXP problems */
#else
		if (p->op==CBR && COMBOP((p1 = p->forw))==BR &&
		    p->ref) {/* combop: RXP problems */
#endif
			rp = p->ref;
			do
				rp = rp->back;
			while (rp->op==LABEL);
			if (rp==p1) {
				decref(p->ref);
				p->ref = p1->ref;
				p->labno = p1->labno;
#ifdef COPYCODE
				if (p->labno == 0)
					p->code = p1->code;
#endif
				p1->forw->back = p;
				p->forw = p1->forw;
				p->subop = negbr[p->subop];
				p->pop=0;
				nchange++;
				nskip++;
			} else
				if (xdebug) printf("CBR;BR not skip...\n");
		}
		if (p->op==BR || p->op==JMP) {
			/* delete unreachable code (comment:jima) */
			while (p->forw && p->forw->op!=LABEL && p->forw->op!=DLABEL
				&& p->forw->op!=EROU && p->forw->op!=END
				&& p->forw->op!=ALIGN
				&& p->forw->op!=JSW /* jima */
				&& p->forw->op!=COMMENT
				&& p->forw->op!=0 && p->forw->op!=STATIC) {
				nchange++;
				iaftbr++;
				if (p->forw->ref)
					decref(p->forw->ref);
				p->forw = p->forw->forw;
				p->forw->back = p;
			}
			/* Delete the branch if destination is (now)
			   the next instruction (comment:jima) */
			rp = p->forw;
			while (rp && rp->op==LABEL) {
				if (p->ref == rp) {
					p->back->forw = p->forw;
					p->forw->back = p->back;
					p = p->back;
					decref(rp);
					nchange++;
					njp1++;
					break;
				}
				rp = rp->forw;
			}
			xjump(p);
			p = codemove(p);
		}
	}
if (tdebug) { printf("After iterate:"); dump_all(); }

}

xjump(p1) /* p1-> BR or JMP.  Combine identical tails of two merging 
	     control paths.  (comment:jima) */
REGISTER struct node *p1;
{
	REGISTER struct node *p2, *p3;
	int nxj;

	nxj = 0;
	if( ( {
			rp = nonlab(p->ref);
			if (rp->op==BR && rp->labno && p->labno!=rp->labno) {
				if (xdebug) {
					printf("brbr:bypassing ");
					dump(rp);
				}
				nbrbr++;
				p->labno = rp->labno;
				decref(p->ref);
				rp->ref->refc++;
				p->ref = rp->ref;
				nchange++;
			}
		}
/* Change "CBR c -> foo; BR bar; foo: ..." to "CBR ~c -> bar" 		(jima)*/
#ifndef COPYCODE
		if (p->op==CBR && COMBOP((p1 = p->fn the new node's address. */
struct node *
insert_znode(p) REGISTER struct node *p; {
	REGISTER struct node *lp;
	lp = alloc(sizeof first);
	*lp = zeronode;
	lp->back = p->back;
	lp->forw = p;
	p->back->forw = lp;
	p->back = lp;
	return(lp);
}

struct node *
insertl(p) /* return a (new or existing) label for node "p" (comment:jima) */
REGISTER struct node *p;
{
	REGISTER struct node *lp;

	if (p->op == LABEL) {
		p->refc++;
		return(p);
	}
	if (p->back->op == LABEL) {
		p = p->back;
		p->refc++;
		return(p);
	}
	lp = insert_znode(p);
	COMBOP(lp) = LABEL;
	lp->labno = isn++;
	lp->refc = 1;
	return(lp);
}

struct node *
codemove(ap)
struct node *ap;
{
	REGISTER struct node *p1, *p2, *p3;
	struct node *t, *tl;
	int n;

	p1 = ap;
/* last clause to avoid infinite loop on partial compiler droppings:
	L183:	jbr L179
	L191:	jbr L179
			casel r0,$0,$1
	L193:	.word L183-L193
			.word L191-L193
	L179:	ret
*/
	if (p1->op!=BR || (p2 = p1->ref)==0 || p2==p1->forw)
		return(p1);
	while (p2->op == LABEL)
		if ((p2 = p2->back) == 0)
			return(p1);
	if (p2->op!=BR && p2->op!=JMP)
		goto ivloop;
	/* p1 is a branch to code which has no non-branch entry (no 
	   fall-thru from above).  Move the referenced code block to 
	   eliminate the p1 branch.  (comments:jima) */
	p2 = p2->forw; /* The first label of p1-referenced code block */
	p3 = p1->ref;
	while (p3) {
		if (p3->op==BR || p3->op==JMP) {
			if (p1==p3)
				return(p1); /* can't unfold loops! */
			ncmot++;
			nchange++;
			p1->back->forw = p2;
			p1->forw->back = p3;
			p2->back->forw = p3->forw;
			p3->forw->back = p2->back;
			p2->back = p1->back;
			p3->forw = p1->forw;
			decref(p1->ref);
			return(p2);
		} else
			p3 = p3->forw;
	}
	return(p1);


/* Invert loop to put conditional at end, avoiding uncond. branch each time.

      loop:	            --->   	goto first;
	S1;			      again:
	if (c) break;			S2;	
	S2;			      first:				
	goto loop;			S1;
					if (!c) goto again;
(comments:jima)*/
ivloop:
	if (p1->forw->op!=LABEL)
		return(p1);
	p3 = p2 = p2->forw;
	n = 16;
	do {
		if ((p3 = p3->forw) == 0 || p3==p1 || --n==0)
			return(p1);
	} while (p3->op!=CBR || p3->labno!=p1->forw->labno);
	do 
		if ((p1 = p1->back) == 0)
			return(ap);
	while (p1!=p3);
	p1 = ap;
	tl = insertl(p1);
	p3->subop = negbr[p3->subop];
	p3->pop=0;
	decref(p3->ref);
	p2->back->forw = p1;
	p3->forw->back = p1;
	p1->back->forw = p2;
	p1->forw->back = p3;
	t = p1->back;
	p1->back = p2->back;
	p2->back = t;
	t = p1->forw;
	p1->forw = p3->forw;
	p3->forw = t;
	p2 = insertl(p1->forw);
	p3->labno = p2->labno;
#ifdef COPYCODE
	if (p3->labno == 0)
		p3->code = p2->code;
#endif
	p3->ref = p2;
	decref(tl);
	if (tl->refc<=0)
		nrlab--;
	loopiv++;
	nchange++;
	return(p3);
}

comjump() /* called from main */
{
	REGISTER struct node *p1, *p2, *p3;
	if (!enable_comjump) return;

	for (p1 = first.forw; p1!=0; p1 = p1->forw)
		if (p1->op==BR && ((p2 = p1->ref) && p2->refc > 1
				|| p1->subop==RXP || p1->subop==RET))
			for (p3 = p1->forw; p3!=0; p3 = p3->forw)
				if (p3->op==BR && p3->ref == p2)
					backjmp(p1, p3);
}

/* Combine two paths which end in a branch to the same third location, and
   have identical final instructions.  The second path is truncated, and 
   made to branch into the first where the common code starts.  (jima) */
backjmp(ap1, ap2)
struct node *ap1, *ap2;
{
	REGISTER struct node *p1, *p2, *p3;

	p1 = ap1;
	p2 = ap2;
	for(;;) {
		while ((p1 = p1->back) && p1->op==LABEL);
		p2 = p2->back;
		if (equop(p1, p2)) {
			p3 = insertl(p1);
			p2->back->forw = p2->forw;
			p2->forw->back = p2->back;
			p2 = p2->forw;
			decref(p2->ref);
			COMBOP(p2) = BR; /* to handle RXP */
			p2->pop=0;
			p2->labno = p3->labno;
#ifdef COPYCODE
			p2->code = 0;
#endif
			p2->ref = p3;
			nchange++;
			ncomj++;
		} else
			return;
	}
}

static struct opntag { char op, *opn } opnames[] =
	{BR,"BR",	CBR,"CBR",	JMP,"JMP",	LABEL,"LABEL",
	 DLABEL,"DLABEL",		EROU,"EROU",	JSW,"JSW",
	 JSW,"JSW",	CXP,"CXP",	RXP,"RXP",
	 PROGRAM,"PROGRAM",		STATIC,"STATIC",BSS,"BSS",
	 END,"END",	ADDR,"ADDR",
	 BSR,"BSR",	RET,"RET",	LCOMM,"LCOMM",	COMM,"COMM",
	 CVT,"CVT",	MOV,"MOV",	CHECK,"CHECK",
	 ADD,"ADD",	SUB,"SUB", 	LSH,"LSH",
	 MUL,"MUL",	QUO,"QUO",	REM,"REM", 	MOD,"MOD",
	 CMP,"CMP",	AND,"AND",	TBIT,"TBIT",	SBIT,"SBIT",
	 INSS,"INSS",	EXTS,"EXTS",
	 0,0};

dump(p) struct node *p; {/* debugging routine */ 
	REGISTER struct opntag *popn;
	REGISTER char *popname, *pcode; char codebuf[20], opnamebuf[20];
	REGISTER int i;
	for (popn=opnames; popn->op && popn->op!=p->op; popn++);
	if (COMBOP(p)==T(BR,RXP))
		popname = "BR(RXP)";
	else
	if (COMBOP(p)==T(BR,RET))
		popname = "BR(RET)";
	else
	if (popn->op) 
		popname = popn->opn;
	else {
		sprintf(opnamebuf, "(op=%d)\0", p->op);
		popname = opnamebuf;
	}
	if (p->op==LABEL && p->labno) {
		sprintf(codebuf,"L<%d>",p->labno);
		pcode = codebuf;
	} else
		pcode = p->code;
	
	printf("%d: %s %s\tsub=x%x refc=%d f=%d b=%d ref=%d labno=%d\n",
		p, popname, pcode, p->subop, p->refc, p->forw, p->back, 
		p->ref, p->labno );
}	
dump_all() {
	REGISTER struct node *p;
	printf("...nchange=%d...\n",nchange);
	for (p=first.forw; p!=0; p=p->forw) dump(p);
}

panic(str) char *str; {
	if (str==0) str="";
	printf("panic (%s).\n",str);
	fflush(stdout);
	fprintf(stderr,"c2 panic, asm line # %d (%s).\n",lineno,str);
	fflush(stderr);
	exit(1);
}

static 
cleanup()
{
  
/* unlink output file if it was created */

	if(outname) {
	  fflush(stdout);	
	  fclose(stdout);
	  fprintf(stderr,"c2: Interrupt! Removing %s\n",outname);
	  unlink(outname);
	  fflush(stderr);	
        }
	exit(13);
}
