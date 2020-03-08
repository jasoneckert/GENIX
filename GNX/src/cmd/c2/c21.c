/* TODO: 
 	Test indirction test in dest().
 */
# ifndef NOSCCS
static char *sccsid = "@(#)c21.c	2.56	8/23/83";
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
 * C object code improver -- second part
 */

#include "c2.h"
#include <stdio.h>
#include <ctype.h>
char *index();

/* Re-define vars mentioned in c2.h to make them exported from this module */
int debug=0, xdebug=0;
int ioflag=0; /* -i option: Don't change access pattern to likely i/o regs */

/* #define NUSE 3  /* No. of temp (not register-variable) registers for uses[]*/
#define NUSE NREG	/* Do usage analysis on ALL registers now... (jima) */
int tdebug=0; /* -t option: Dump inTer-pass results */
static unsigned biti[NUSE+1] = { 1,2,4,8,16,32,64,128,256,512,1024,2048,
			         4096,8192,16384,32768, 65536 }; /* 32 bits */

/* The following are indexed by (-,BYTE,WORD,DOUBLE,FLOAT,LONG). */
static int bitsize[6] = {0,8,16,32,32,64};
static long unsigned signmask[6] = {0,0x80,0x8000,0x80000000,0,0};  
static long unsigned maxunsval[6] = {0,0xfF,0xFFFF,0xFFFFFFFF,0,0};
static long          maxval[6] = {0,0x7F,0x7FFF,0x7FFFFFFF,0,0};

/* Maximum value for (-,BYTE,WORD,DOUBLE) displacements: */
static long unsigned maxdispl[4] = {0,0x7F,0x3FFF,0x3FFFFFFF};

static int pos,siz; long f; /* for bit field communication */

static struct node *uses[NUSE];	/* for backwards flow analysis */
			        /* valid only when usect[x] != 0 */
static int usect[NUSE];  	
static struct node *curr_cmpuse;/* bflow: last CBR or CSETB/W/D (use of cmp) */
static struct node *cmploc;	/* most-recently seen cmp */
static struct node *cmpuse;	/* saved curr_cmpuse when cmploc set */

/* Storage for read-only operands examined by saveread() via bflow().
   This allows repeatedly used items to be put into available temp regs. */
# define NREADS 10
static struct readsrec {
	char r_type;	/* (BYTE,WORD,LONG,FLOAT,DOUBLE); Zero if unused. */
	long r_cost;	/* No. of bits consumed by all uses of the value */
	char r_anycon;	/* 1 iff a scalar or FLOAT/DOUBLE immediate */
	char r_string[C2_ASIZE]; /* The operand */
} reads[NREADS];
/* next entry to blast when there are no free entries to use: */
static struct readsrec *nextreads_toreuse = reads; 
# define LASTTEMP 2       /* temp registers last allocated by ccom, and so */
# define LASTFTEMP 11	  /* most likely to be free.  Used by saveread().  */
			  /* Note: "r11" is really f3                      */
/* rnums[] is used by clearreg & clearuse to select all or only
   temporary register numbers: */
char rnums[] = {0,1,2,8,9,10,11,12,13,14,15,3,4,5,6,7};
# define RNUMS_LASTTEMP 6 /* Index of last temp reg number in rnums[] */
# define RNUMS_LAST    15 /* Index of last entry in rnums[] */

char *lastrand=0; /* last operand of instruction */
static struct node *bflow();
static struct node *bicopt();
static char *findcon();
static void squishout_regs_field(); 
static void clearuse();
static void setspecuse();
static void upd_uses();
static void clearreads();
static void ins_readreg();
static void swap_nodes();


int nregsub=0;  /* No. of alternate register substitutions */
int nsaddr=0;   /* No. of simplified addresses */
int ncommut=0;  /* No. of commuted operands to eliminate moves */

/* Used to remember previous comparison which set current condition codes */
char cc_left_data[], cc_right_data[];  /* defined below */
static char *cc_left = cc_left_data;
static char *cc_right = cc_right_data;
static char cc_type;

/* Reversed-side conditional branch ops - indexed by original subop: */
static char revbr[] = {
/* []:  jeq  jle  jge  jlt   jgt  jne    <signed tests 0-5> */
	JNE, JGE, JLE, JGT,  JLT, JEQ,
/*     (jeq) jlos jhis jlo  jhi  (jne)  <unsigned tests 6-11> */
	JNE, JHIS,JLOS,JHI, JLO,  JEQ,
/*      jbfc  jbfs  spare  spare  jbcc  jbcs  spare  spare  12-19*/
	JBFS, JBFC, NULBR, NULBR, JBCS, JBCC, NULBR, NULBR };

char enable_bmove=1;
char enable_rmove=1;

static char cc_left_data[C2_ASIZE];  /* these are at end... */
static char cc_right_data[C2_ASIZE]; /* to shorten SB-offsets of preceeding */

/* /* Return location of substring or zero.  The substring must not be null */
/* static char *
/* substr( s, subst) REGISTER char *s, *subst; {
/* 	REGISTER int firstc = *subst;
/* 	REGISTER char *cp, *asubst = subst;
/* 	while (*s) {
/* 		if (*s == firstc) {
/* 			cp = s;
/* 			while (*++subst==*++s && *subst!=0);	
/* 			if (*subst==0) return(cp); 
/* 			else { s = cp; subst = asubst; }
/* 		}
/* 		++s;
/* 	}
/* 	return(0);
/* }
 not used presently... */

/* Return location of an operand part or zero.  The substring to be matched
   must start with an alphanumeric, and will only match if bounded by
   special characters (or null).
   If the substring is null, the location of the null at the end of the 
   string is returned. (jima) */
static char *
part( s, subst) REGISTER char *s, *subst; {
	REGISTER int firstc = *subst;
	REGISTER char *cp, *asubst = subst;
	while (*s) {
		if (*s == firstc) {
			cp = s;
			while (*++subst==*++s && *subst!=0);	
			if (*subst==0 && !isalnum(*s)) return(cp); 
			else { s = cp; subst = asubst; }
		} else {
			/* skip to next possible "word" start */
			while (isalnum(*s)) ++s;
			if (*s==0) --s; /* ...but not if the last one */
		}
		++s;
	}
	return(0);
}

/* Return node prior to p which mentions s in an operand.
   Returns zero if there is no previous reference. regs[] are not modified.
   (jima) */
struct node *
prev_use(p,s) REGISTER struct node *p; REGISTER char *s; {
	while (p=p->back) if (p->code && part(p->code,s)) break;
	return(p);
}

/* Return node following to p which mentions s in an operand.
   Returns zero if there is no following reference. regs[] are not modified.
   (jima) */
struct node *
next_use(p,s) REGISTER struct node *p; REGISTER char *s; {
	while (p=p->forw) if (p->code && part(p->code,s)) break;
	return(p);
}

/* Do backward flow analysis [via bflow()], and misc. peephole optimizations.

   When do_reads is selected, a move of a datum to an available register is 
   inserted before a sufficiently long series of references to that datum.

   If del_dead_stores is non-zero, loads to dead registers are eliminated.  
   This is not selected until after an rmove(0) call, which replaces (possibly
   expanded) literals with register references.  Otherwise most loads of
   literals would be eliminated as "useless" by the effect 
   rmove(expand_literals=1), even though they are needed again later.
(jima) */
bmove(do_reads,del_dead_stores) {
   REGISTER struct node *p; struct node *lastp;
   REGISTER int r;
   static char first_bmove = 1;  /* reset to zero after first call */
   char look_again=0;
   refcount();
   if (!enable_bmove) return;
   for (p=lastp= &first; 0!=(p=p->forw); lastp=p);
   clearreg(0); clearuse(0); if (do_reads) clearreads();
   for (p=lastp; p!= &first; p=(look_again? p:p->back)) {
	if (debug) {
		printf((look_again ? "-again-" : "---"));
		dump(p);
		for (r=NUSE;--r>=0;) if (usect[r]) { 
			printf("Uses[%d]#%d t=%d:",r,usect[r],regs[r][0]); 
			dump(uses[r]);
			if (regs[r][1]) 
				printf("    regs:%s\n",&regs[r][1]);
		}
	}
	if (xdebug) {
		REGISTER struct readsrec *rp;
		for (rp=reads; rp < (reads+NREADS); rp++)
			if (rp->r_type) printf("   READS[%d](@x%X): type=%d cost=%d anycon=%d  %s\n",
				        	(rp-reads),rp,rp->r_type,
						rp->r_cost,rp->r_anycon,
						rp->r_string);
	}
	r=((p->subop>>4)&0xf);
	if (r==OP2) {
	   REGISTER int p_op = p->op;
	   if( *p->code=='0' && p->code[1]==',' &&
	       (splitrand(p),!source(regs[RT2])) && okio(regs[RT2]) ) {
	      	/* Operation with zero, no side-effects */
		switch(p_op) {
		case MUL: case AND: /* Becomes MOVQi 0,dst */
			p->op=MOV; p->subop &= 0xf; p->pop=0; break;
		case DIV: case QUO: case REM: case MOD:
			if (first_bmove) fprintf(stderr,"c2: warning - zero divide\n"); break;
		case OR:
		case XOR: 
		case ADD: case SUB: /* ASSUMPTION: Carry not needed */
		case ASH: case LSH: case ROT:
		     /* It's a nop - get rid of it */
		     delnode(p); equivnop++; 
		     continue;
		} /* switch p_op with zero operand */
	   }
	   if( (p_op==DIV || p_op==QUO) && *(p->code)=='1' && p->code[1]==',' &&
	       (splitrand(p),!source(regs[RT2])) ) {
	     	/* No-op */
		delnode(p); equivnop++; 
		continue;
	   }
	}
	look_again=0; /* set to 1 to enable repeated changes */
	switch (p->op) {
	case LABEL: case DLABEL:
	   {	REGISTER int r;
		/* use the ref field to record which registers are in 
		   use at this point (as a bit array).  The bit for 
		   "register NUSE" (not otherwise valid) is used to 
		   indicate that this label has been seen.  BR & CBR
		   processing which uses this info to tell usage after
		   branching.  refcount() clears all ref fields in labels.
		   (jima)
		*/
		p->ref = (struct node *) biti[NUSE];
		for (r=NUSE; --r>=0;) 
			if (usect[r]) 
				p->ref=(struct node *) (((int)p->ref)|biti[r]);
		/* Put in register loads and erase reads[]. */
		if (do_reads) {
			ins_readreg(LASTTEMP,p); 
			ins_readreg(LASTFTEMP-1,p);
		}
		break; 
	   }
	case CXPD:
	case JSR:
	case BSR:
	case CXP:
		clearuse(1/*temps only*/);
		if (do_reads) {
			ins_readreg(LASTTEMP,p); 
			ins_readreg(LASTFTEMP-1,p);
		}
		/* If profiling call, r0 has argument */
		if (equstr(p->code,"mcount")) setspecuse(0,p);
		goto std;
	case EXTS:
	{
		/* bit tests:
		**	extsi	srcbase,r,offset,1  \
		**	cmpi	0,r {only use of r}  >	tbitB	offset,srcbase
		**	...		            /	...
		**	beq/bne/seqi/snei...       /	bfc/bfs/sfci/sfsi...
		**
		** also field extracts which can be moves:
		**	extb	srcb,dest,n*8,(>=8)  -->  movb n+srcb,dest
		**	extw	srcb,dest,n*8,(>=16) -->  movw n+srcb,dest
		**	extw	srcb,dest,n*8,8      -->  movzbw n+srcb,dest
		**	extd	srcb,dest,n*8,32     -->  movd n+srcb,dest
		**	extd	srcb,dest,n*8,16     -->  movzwd n+srcb,dest
		**	extd	srcb,dest,n*8,8      -->  movzbd n+srcb,dest
		*/
		int	flen;			/* field length */
		int	destlen;		/* destination length in bits */

		splitrand(p);
		if ((flen = getnum(regs[RT4])) <= 0) goto std;
		if (flen == 1) {
			REGISTER struct	node	*pcmp;	/* cmp node */
			REGISTER struct	node	*pcbr;	/* the beq/bne node */
			REGISTER int	extreg;		/* reg extracted to */
			extreg = isreg(regs[RT2]);
			if (extreg < 0 || extreg >= NUSE) goto std;
			if (usect[extreg]!=1) goto std;
			/* Be sure ccodes not used inbetween... */
			if (curr_cmpuse != 0) goto std;
			if ((pcmp=uses[extreg]) != cmploc) goto std;
			if (p->subop != pcmp->subop) goto std;
			splitrand(pcmp);
			if (extreg != isreg(regs[RT2])) goto std;
			if (!isimm(regs[RT1]) || getnum(regs[RT1])!=0) goto std;
			if (cmpuse==0) panic("Ecmpuse");

			splitrand(p);
			strcpy(regs[RT2],regs[RT1]);  /* base   */
			strcpy(regs[RT1],regs[RT3]);  /* offset */
			regs[RT3][0] = '\0';  regs[RT4][0] = '\0'; 
			COMBOP(p) = T(TBIT,BYTE); p->pop = 0;
			newcode(p);
			delnode(cmploc); cmploc=0;
		        usect[extreg] = 0;

			for (pcbr=pcmp->forw; ;pcbr=pcbr->forw) {
			   	if ((r=pcbr->op)==CBR ||(r>=CSETB && r<=CSETD)){
				   pcbr->subop=HASEQ(pcbr->subop) ? JBFC:JBFS;
				   pcbr->pop = 0;
				   nchange++;
				}
				if (pcbr==cmpuse) break;
			}	
			goto std;
		}
		destlen = bitsize[p->subop&0xf];
		if ( (destlen==8  && flen>=8) ||
		     (destlen==16 && (flen==8 || flen>=16)) ||
		     (destlen==32 && (flen==8 || flen==16 || flen==32)) ) {
			REGISTER int	boff;	/* bit offset */
			REGISTER int	ftype;  /* type for flen (e.g., BYTE) */

			if (!isimm(regs[RT3])) goto std;
			if ((boff = getnum(regs[RT3])) < 0) goto std;
			if ((boff % 8) != 0) goto std;
			if (boff>0 && (isreg(regs[RT1]) >= 0)) goto std;

			if (destlen<=flen) {
				p->op = MOV;
				p->subop &= 0xf;
			} else {
				ftype = BYTE+(flen/8)-1;
				p->subop = U(ftype,(p->subop&0xf));
				p->op = MOVZ;
			}
			p->pop = 0; 
			strcpy(regs[RT4],regs[RT1]);  /* temp copy */
			sprintf(regs[RT1], "%d+%s", (boff/8), regs[RT4] );
			regs[RT3][0] = '\0'; regs[RT4][0] = '\0';
			newcode(p);
			nchange++;
		}
		goto std;
	}

	case AND:
	   {
		REGISTER int r, power;
		REGISTER struct	node	*pcbr, *pcmp;
		/*
		**	movi	<power-of-2>,r	>	movi  <power-of-2>,r
		**	andi	XXX,r        	>	tbitB <power>,XXX
		**      ...			>	...	
		**	cmpi	0,r (only use) 	>
		**	...			>	...
		** 	beq/bne/seqx/snex ...	>	bfc/bfs/sfcx/sfsx ...
		**
		** ---or---
		**
		**	andi	<power-of-2>,r	>	tbitB <power>,r
		**      ...			>	...	
		**	cmpi	0,r (only use) 	>
		**      ...                     >       ...
		** 	beq/bne/seqx/snex ...	>	bfc/bfs/sfcx/sfsx ...
		*/
		splitrand(p);
		r = isreg(regs[RT2]);
		if (r < 0 || r >= NUSE) goto std;
		if (usect[r]!=1) goto std;
		if (uses[r] != (pcmp=cmploc)) goto std;
		if (curr_cmpuse != 0) goto std; /* cc used between */
		if ((pcmp->subop&0xf) != (p->subop&0xf)) goto std;
		power = ispow2(getnum(regs[RT1]));

		splitrand(pcmp);
		if (r != isreg(regs[RT2])) goto std;
		if (!isimm(regs[RT1]) || getnum(regs[RT1])!=0) goto std;
			
		if (power<0) { /* see if a power-of-2 is in r */
			if (p->back->op==MOV) {
				REGISTER char *t;
				splitrand(p->back);
				power = ispow2(getnum(regs[RT1]));
				if (power<0 || r!=isreg(regs[RT2]) ||
				    pcmp->subop!=p->back->subop)goto std;
				splitrand(p);
				t=regs[RT2]; regs[RT2]=regs[RT1]; regs[RT1]=t;
			} else goto std;
		} else 
			splitrand(p);
		/* Note: regs[RT2] contains a proper bit offset for TBIT */
		if (cmpuse==0) panic("Acmpuse");
		COMBOP(p) = T(TBIT,BYTE); p->pop = 0;
		p->subop &= 0xf;  /* get rid of OP2 */
		sprintf(regs[RT1],"%d",power);
		newcode(p);
		delnode(pcmp); cmploc=0;
		usect[r] = 0; /* bflow() will notice new use in tbit */

		for (pcbr=pcmp->forw; ;pcbr=pcbr->forw) {
		   	if ((r=pcbr->op)==CBR || (r>=CSETB && r<=CSETD)){
			   pcbr->subop=HASEQ(pcbr->subop) ? JBFC:JBFS;
			   pcbr->pop = 0;
			   nchange++;
			}
			if (pcbr==cmpuse) break;
		}	
	   	goto std;
	   } /* end case AND: */
	case ADD:  /* sub->addq, or sub<-->add for smaller displacement */
	case SUB:
	   { REGISTER int val; REGISTER char *delim; REGISTER int tmpc; 
		if (delim=isimm(p->code)) {
			tmpc = *delim; 
			*delim='\0'; val=getnum(p->code); *delim=tmpc;
			if ((val >= -7 && val <= 8) || val==64 || val==8192) {
				if (p->op==SUB || val >= 64) {
					p->op = (p->op==ADD ? SUB:ADD);
					p->pop=0; /* add-->addq in output() */
					if (val<0) ++(p->code);  /*zap minus*/
					else p->code = copy2("-",p->code);
				}
			}
		}
		goto std;
	   }
	case STATIC: case BSS: case ALIGN: 
	case BGEN: case WGEN: case LGEN: case FGEN: case DGEN: case JSW:
	case COMMENT:
	case EROU: 
	case PROGRAM: 
	case END: 
		break;
	case BR: case JMP: 
		clearuse(0);
		if (do_reads) {
			ins_readreg(LASTTEMP,p); 
			ins_readreg(LASTFTEMP-1,p);
		}
		if (p->subop==RXP || p->subop==RET) {
			setspecuse(0,p);
			setspecuse(FIRSTFREG,p); /* f0 */
			break;
		}
		if (p->ref==0) goto std;	/* jmp (r0) */
		/* fall through */
	case CBR:
	   {
	     /* Note: Register use was recorded at the label when it was
		seen previously, if below (i.e., this is a forward branch).
		Even if no regs were in use, the bit for NUSE was set to
		show that the data is valid.  If this is a backwards branch,
		the label hasn't yet been processed, and all bits are zero;
		since there is no valid use info, all regs are assumed to be
		in use.  usect[] and regs[] are set to bogus values which
		won't match comparisons elsewhere.  (jima)
	     */
	   	REGISTER int usemask = (int)p->ref->ref;
		for(r=NUSE;--r>=0;)
			if (0==usemask || (biti[r] & usemask)) setspecuse(r,p);
		if (curr_cmpuse==0) curr_cmpuse=p;
	   break;
	   }
	case CSETB: case CSETW: case CSETD:
		if (curr_cmpuse==0) curr_cmpuse=p;
		goto std;
	case ENTER: case EXIT: case SAVE: case RESTORE:
	case 0: /* Unrecognized entry - assume all registers used */
	   { REGISTER int r;
		for (r=0; r<NUSE; ++r) {
			if (++usect[r]==1){
				uses[r]=p;
				regs[r][1] = 0;
			}
			regs[r][0] = (r<FIRSTFREG ? LONG : FLOAT);
		}
		if (do_reads) {
			ins_readreg(LASTTEMP,p); 
			ins_readreg(LASTFTEMP-1,p);
		}
		break;
	   }
	case CASE:
		/* Reads[] empties when WGEN/DGEN was encountered */
	default: std:
		p=bflow(p,do_reads,del_dead_stores);
		break;
	} /* switch */
	} /* for */
	for (p= &first; p!=0; p=p->forw)
		if (p->op==LABEL || p->op==DLABEL) p->ref=0; /* erase tracks */

	first_bmove=0;

if (tdebug) { 
	printf("after bmove(do_reads=%d del_dead_stores=%d):",
	       do_reads,del_dead_stores);
	dump_all(); 
}
} /* bmove */

/*
 OPERAND CLASS WORD - specifies mode of operands for a given opcode

 opclasses() returns an integer containing packed fields describing
 the mode of each operand for the specified opcode.  The fields
 contain bits defined by these masks:
 
	CLREAD....operand is somehow read. With CLWRITE, rmw.  If CLREAD is 
		  the only flag set, then it can be a register or not 
		  without restrictions.
	CLCON.....operand is a constant which can't be changed in any way.
	CLWRITE...operand is somehow written.  If CLWRITE is the only flat
		  set, then it is a pure write (not rmw, etc.)
	CLADDR....operand is 16K "addr" class or, with CLREG, "regaddr"
	CLREG.....operand is 16K "reg" class or, with CLADDR, "regaddr"

 These masks specify bits for the "current" operand, initially the first
 operand.  ADVANCE_CL(classword) shifts the classword in place so that
 the next operand's bits will be accessable.

 Operands which don't exist (for a given instruction) have arbitrary mode
 descriptions.

 CLSPECIAL is returned for instructions which require special processing.
 In this case the class bits are undefined.
*/
static CLASSWORD
opclasses(p) REGISTER struct node *p; {
	/* NOTE: FFS, coded as an OP2, always has a BYTE offset (=dest) */
	if (OP2==(p->subop>>4)) return( CL_DEF2(CLREAD,CLWRITE|CLREAD) );
	switch (p->op) { 
	case MOV: case NEG: case ABS: case COM: case NOT:
	case CVT: case MOVZ:
		return( CL_DEF2(CLREAD,CLWRITE) );
	case CSETB: case CSETW: case CSETD:
		return( CLWRITE );
	case CMP:
		return( CL_DEF2(CLREAD,CLREAD) );
	case ADDR: case LXPD:
		return( (p->labno ? CLWRITE : CL_DEF2(CLADDR,CLWRITE)) );
	case MEI: case DEI: /* dummy operand added by input() 
			       for 2nd dest cell */
		return( CL_DEF3(CLREAD,CLREAD|CLWRITE,CLREAD|CLWRITE) );
	case TBIT: /* offset<read.i>,base<regaddr> */
		return( CL_DEF2(CLREAD,CLREAD|CLREG|CLADDR) );
	case SBIT: case CBIT: case IBIT: /* offset<read.i>,base<regaddr> */
		return( CL_DEF2(CLREAD,CLWRITE|CLREG|CLADDR) );
	case CVTP: /* NOTE: always has LONG size dest */
		return( CL_DEF3(CLREG|CLREAD,CLADDR,CLWRITE) );
	case EXT: /* offset<reg>,base<regaddr>,dest<write.i>,length<disp> */
		return(CL_DEF4(CLREG|CLREAD,CLREG|CLADDR|CLREAD,CLWRITE,CLCON));
	case EXTS: /* base<regaddr>,dest<write.i>,offset<imm>,length<imm> */
		return( CL_DEF4(CLREG|CLADDR|CLREAD,CLWRITE,CLCON,CLCON) );
	case INS:  /* offset<reg>,src<read.i>,base<regaddr>,length<disp> */
	        return(CL_DEF4(CLREG|CLREAD,CLREAD,CLWRITE|CLREG|CLADDR,CLCON)); 
	case INSS: /* src<read.i>,base<regaddr>,offset<imm>,length<imm> */
		return( CL_DEF4(CLREAD,CLWRITE|CLREG|CLADDR,CLCON,CLCON) );
	case MOVM: case CMPM: /* block1<addr>,block2<addr>,length<disp> */
		return( CL_DEF3(CLADDR,CLADDR,CLCON) );
        case INDEX: /* accum<reg>,length<read.i>,index<read.i> */
		return( CL_DEF3(CLWRITE|CLREAD|CLREG,CLREAD,CLREAD) );
	case JSR: case CXPD: case JMP: /* jmptarget<addr> */
		return( CLREAD|CLADDR );
	case RDVAL: case WRVAL: /* src/dest<addr> */
	case CASE: case ADJSP: case BICPSR: case BISPSR:
		return( CLREAD );
	case ACB: /* inc<quick>,index<rmw.i>,jumpdest<disp> */
		return( CL_DEF3(CLCON,CLREAD|CLWRITE,CLCON) );
	case LPR: /* procreg<short>,src<read.i> */
	case LMR: /* mmureg<short>,src<read.D>   NOTE: src always LONG */
		return( CL_DEF2(CLCON,CLREAD) );
	case SPR: /* procreg<short>,dest<write.i> */
	case SMR: /* mmureg<short>,dest<write.D>   NOTE: dest always LONG */
		return( CL_DEF2(CLCON,CLWRITE) );
	case COMMENT:
		return( 0 );
	case MOVSU: case MOVUS: /* src<addr>,dest<addr> */
		return( CL_DEF2(CLADDR|CLREAD,CLADDR|CLWRITE) );
	case CBR: case BSR: case RET: case RXP: case RETT: case RETI:
	case CXP: /* <disp> */
		return( CLCON );
	case MOVS: case CMPS: case SKPS:    /* r0,r1,r2,r3,r4 operands added */
	case MOVST: case CMPST: case SKPST: /* by input(), removed by output()*/
		 /* Must all include CLADDR to prevent substitution */
		return( CL_DEF5(CLADDR|CLREG|CLREAD|CLWRITE, CLADDR|CLREAD|CLWRITE, CLADDR|CLREAD|CLWRITE, CLADDR|CLREAD, CLADDR|CLREAD) );
	case SAVE: case RESTORE: case ENTER: case EXIT: case SETCFG:
	default: 
		return( CLSPECIAL );
	} /* switch */
}

/* Returns non-zero iff instruction modifies condition codes */
static
changes_cc( p ) REGISTER struct node *p; {
	switch( p->op ) {
	/* Note: cmpq and addq are removed by input() */
	case CMP: case SUBC: case NEG: case ADDC: 
	case ABS: case TBIT: case SBIT: case CBIT: case IBIT: case FFS:
	case MOVS: case CMPS: case SKPS: case CMPM: case ADDP: case SUBP:
	case CHECK: case RDVAL: case WRVAL:
		return(1);
	case ADD: case SUB: 
		return((p->op&0xf) < FLOAT); /* Carry set on scalar ops */
	default: 
		return(0);
	}
}

/* Returns non-zero iff instruction references condition codes */
static
uses_cc( p ) REGISTER struct node *p; {
	switch( p->op ) {
	case ADDC: case SUBC: case ADDP: case SUBP:
	case CSETB: case CSETW: case CSETD:
	case FLAG: 	
		return(1);
	default: 
		return(0);
	}
}

static
commutative(o) {
	return(o==ADD || o==AND || o==MUL || o==XOR || o==OR || o==ADDP); }

/* Replace operands with registers known to contain the same value.  Part
   of rmove().

   Replacement may occur only for ordinary or selected REGADDR "read" 
   class operands.  classword is obtained by the caller from opclasses().

   If expand_literals is specified, references to registers which contain
   constants are replaced by the constants (see rmove commentary).  (jima) */

static
repladdr(p,classword,expand_literals) 
	struct node *p; 
	REGISTER CLASSWORD classword;
{
	REGISTER char *p1, *p2;
	REGISTER int r, nrepl;
	REGISTER char **preg; 

	if (classword == CLSPECIAL) return; /* can't do any operands */

	preg=regs+RT1; nrepl=0;
	while (*(p1 = *preg)) {
		if ((CL(classword)==CLREAD ||
		     (CL(classword)==(CLREAD|CLREG|CLADDR) && substregaddr(p)))
		    && !source(p1)) {
			if (expand_literals && 0<=(r=isreg(p1)) && 
			    regs[r][0] && isimm(&regs[r][1]) &&
			    equtype(regs[r][0],p->subop)) {
				strcpy(p1,&regs[r][1]);	nrepl++;
				if (debug) 
					printf("### expanding r%d to %s\n",r,p1);
				++nchange;
			}
			else
			if ((!expand_literals || !isimm(p1)) &&
			    0<=(r=findrand(p1,p->subop))) {
				if (debug) printf("### repladdr: %s becomming r%d\n",p1,r);
				/* If it was a register already, then must be
			   	   alternate-register assignment */
				if (0<=isreg(p1)) ++nregsub; else ++nsaddr;
				++nrepl; ++nchange;
				*p1++='r'; 
				if (r>9) {*p1++='1'; r -= 10;}
				*p1++ = '0'+r; *p1=0;
			}
		}
		ADVANCE_CL(classword);
		++preg;
	}
			
	if (nrepl) newcode(p);
}

/* Do forward flow analysis, replacing operands by register references
   where possible, based on known register contents.  Refcount() must have
   been called.

   If expand_literals is specified, references to regs containing constants
   are replaced by the constants themselves.  Only non-literal register
   values result in register insertions.  This facilitates subsequent
   bmove() processing, which wants all constant operands to appear as
   literals in each instruction.  Bmove tries to place
   "frequently" used constants (and other things) into registers.  
   The expand_literals option is selected only on the initial call (see
   main).
(jima) */

rmove(expand_literals)
{
   REGISTER struct node *p;
   REGISTER char *cp;
   char look_again;

   clearreg(0);
   if (!enable_rmove) return;
   for (p=first.forw; p!=0; p=(look_again? p:p->forw)) {
	if (debug) {
		REGISTER int r, type;
		printf("..."); dump(p);
		for (r=0; r<NREG; r++)
			if (regs[r][0]) {
				type=regs[r][0];
#ifdef MULTIVAL
				printf("Regs[r%d]: type=%d|%d\tvals=",
					r, type>>4, type&0xf);
				cp = &regs[r][1];
				while (*cp) {printf("%s ",cp);
					     while (*cp++); }
				printf("\n");
				if (*++cp) panic("no double null in regs");
#else
				printf("Regs[r%d]: type=%d|%d\tval=%s\n",
					r, type>>4, type&0xf, regs[r]+1);
#endif
			}
	}
	look_again=0; /* set to 1 to enable repeated changes */
	switch (p->op) {

	case AND:	
	   {    REGISTER struct node *psh; REGISTER char *regstr; 
		REGISTER long n;     /* must be signed! */
		REGISTER long mask;  /* must be signed! */
		REGISTER int i;
		/*
		**	ANDx <any bits, then n 0's>,r   \   LSH/LSH/ASHi -n,r
		**	...			         >  ...
		**	ROT/LSH/ASHi -n,r	        /   ANDx <the bits*>,r
		**
		**      (* <the bits> are sign-extended. ROT becomes LSH.)
		**
		**   Purpose: If negative mask, may allow x in ANDx to be made 
		**            smaller.  Prepares for conversion to EXTS.
		*/
		if ((regstr=isimm(p->code))>0 && 0<=isreg(++regstr) &&
		    (psh=next_use(p,regstr)) && 
		    (psh->op==LSH || psh->op==ASH || psh->op==ROT)) {
			regstr[-1] = '\0';    /* terminate mask str. */
			mask = getnum(p->code) | (~maxunsval[p->subop&0xf]);
			regstr[-1] = ',';     /* restore mask string */
			splitrand(psh);
			if ( equstr(regs[RT2],regstr) &&
			     (n= -getnum(regs[RT1])) > 0 &&
			     mask == ((mask>>n)<<n) ) {
				/* Ok. Swap the AND and shift... */
				swap_nodes(p,psh);
				/* Modify the mask... */
				i = psh->subop & 0xf; /* delete OP2 */
				sprintf(line,"%d,%s",
					mask>>=n /*sign extending*/, regstr);
				p->code = copy(line);
				if (p->op==ROT) p->op=LSH;
				if (xdebug) 
					printf("### AND/SHIFT swap & mask adj\n");
				look_again=1; /* shift is now first */
				++nchange;
				break;
			}
			     
		}		
	   }
	/* case AND: falls through... */
	case OR:
	case XOR:
	   {
		REGISTER long unsigned val;
		REGISTER int type = p->subop&0xf; /* sans OP2 */
		splitrand(p);
		if (0 != (val=getnum(regs[RT1]))){
			if (val==-1) {
				if (p->op==XOR && type>BYTE) {
				   REGISTER char *par=index(regs[RT2],'(');
				   REGISTER long unsigned offset;
				   if (par==0 ||
				       (*par=0,
					offset=getnum(regs[RT2]),*par='(',
				        isalpha(par[1]) && 
					offset<=maxdispl[type-1]) ){
					/* Don't do this if the cost of the
					   addressing displacements exceeds
					   the cost of the immediate... */
					COMBOP(p) = T(COM,type); p->pop=0;
					strcpy(regs[RT1],regs[RT2]);
					newcode(p);  /* now COM foo,foo */
					if (debug) printf("### %d => COM",p);
					++nchange;
					/* ++??????? */
					look_again=1;
					break;
				   }
				} else
				if (p->op==AND && okio(regs[RT2])) {
					delnode(p);	/* effective NOP */
					++nchange; ++equivnop;
					continue; /* look at next node */
				} else
				if (p->op==OR && okio(regs[RT2])) {
					/* OR -1,x --> MOVQ -1,x */
					COMBOP(p) = T(MOV,type); p->pop=0;
					++nchange;
					/* ++??????? */
					look_again=1;
					break;
				}
			}
			if (p->op==AND) val = ~val;
			while (val <= maxunsval[type-1] && okio(regs[RT2])) {
				/* shorter immediate suffices */
				--(p->subop); --type; p->pop=0;
				/* Adjust value to fit in new size if 
				   necessary.  Example: 
				   ANDD -65535,X (ffff0001) becomes ANDW 1,X */
				trunc_strcpy(regs[RT1],regs[RT1],type);
				newcode(p);
			}
		}
		goto split_std;
	   }

	case MOV:
		splitrand(p);
	    {   REGISTER struct node *pand; REGISTER int i; REGISTER char *cp;
		/* 	MOVi	<const>,r    \		MOVi	X,r
		**	...		      >		...
		**	ANDi	X,r	     /		ANDi	<const>,r
		**      (if <const> allows i to be reduced, or shift follows) 
		**      (Also provided r isn't referenced in X)
		**
		**	Purpose: May allow i in ANDi to be made smaller, or
		**            prepares for AND <con>,r\shift change, possibly
		**            leading to EXTS.
		*/
		if (isimm(regs[RT1]) && 0<=isreg(regs[RT2]) &&
		    (pand=next_use(p,regs[RT2])) && pand->op==AND &&
		    p->subop==(pand->subop&0xF) &&
		    ( (~getnum(regs[RT1])) <= maxunsval[(p->subop&0xF)-1] ||
		      (i=pand->forw->op)==LSH || i==ASH || i==ROT )) {
			cp = pand->code;		
			pand->code = p->code;	/* swap code strings */
			p->code = cp;
			splitrand(p);
		}
	   }
		goto CVT_split; /* Fall through, but skip splitrand */
	case CVT:
	   {	REGISTER int src_r,dest_r;
		splitrand(p);
	CVT_split:
		repladdr(p,CL_DEF2(CLREAD,CLWRITE),expand_literals);
		src_r = isreg(regs[RT1]);
		dest_r = isreg(regs[RT2]);

		if (p->op==MOV) {
			if (equstr(regs[RT1],regs[RT2]) && okio(regs[RT1]))
				goto del_redunm;
		        if (dest_r>=0) {
				if (reg_hasit(dest_r,regs[RT1],p->subop) &&
				    okio(regs[RT1])) 
					goto del_redunm;
			} else
		        if (src_r>=0) {
				if (reg_hasit(src_r,regs[RT2],p->subop) &&
				    okio(regs[RT2])) {
			    	       del_redunm: 
					/* *** Delete a redundant move *** */
					if (debug) printf("### REDUNM...");
					delnode(p); redunm++; 
					break;
						
				}
			}
		}

		dest(regs[RT2],p->subop); /* purge regs[] as appropriate */
		if (dest_r>=0) addreg(dest_r, regs[RT1], p->subop);
		if (src_r>=0) {
			if (dest_r>=0) {
#ifdef MULTIVAL
				/* Reg-reg xfer - put in known values of src */
				cp = regs[src_r];
				while(*++cp!=0) {
					addreg(dest_r, cp, p->subop);
					cp = index(cp,'\0'); /* next entry */
					if (cp==0) panic("no null1");
				}
#else
				savereg(dest_r, regs[src_r]+1, p->subop);
#endif
			} 
			if (p->op==MOV) addreg(src_r,regs[RT2], p->subop);
		} else 
		if (dest_r<0 && p->op==MOV) 
			setcon(regs[RT1], regs[RT2], p->subop);
		break;
	   }

	case CXPD:
	case JSR:
		splitrand(p);
		repladdr(p,CLADDR,expand_literals);
	case CXP:
	case BSR:
		clearreg(/* for_call: */ 1);
		break;

	case BR: 
		redunbr(p);
	case 0:
		 clearreg(0);
		 break;

	case MUL:
	{
		/*
		** Change multiplication by constant powers of 2 to
		**	add or shifts.  Delete mul by one.
		*/
		REGISTER int shcnt; REGISTER char *shcnt_str;
		splitrand(p);
		shcnt_str = findcon(RT1,p->subop);
		if( (shcnt=getnum(shcnt_str)) <= 0 ) goto split_std; 
		if( (shcnt=ispow2(shcnt)) < 0 ) goto split_std;
		switch (shcnt)
		{
		case 0:  /* multiplication by one is a nop */
		     delnode(p); equivnop++; 
		     continue;
		case 1:	/* "muli 2,x" => "addi x,x"  */
			if (!source(regs[RT2])) { /* no side-effects in x */
				strcpy(regs[RT1], regs[RT2]);
				p->op = ADD; p->pop = 0; newcode(p); nchange++;
			}
			break;
		default:
			sprintf(regs[RT1], "%d", shcnt);
			p->op = ASH; p->pop = 0; 
			newcode(p); nchange++;
		}
		look_again=1; break;  /* shift changes? */
	}
	case LSH:
	case ASH:
	   {	REGISTER int n,r;
		REGISTER struct node *pand;
	    	REGISTER int x;
		/*
		**	L/ASHi -n,r		 >   EXTSi r,r,x,n
		**	... (no use of r)	 >   ...
		**	ANDi	<x 1's>,r	 >   
		**      (with (x+n) <= 32 and x < 8)
		** or
		**	MOVi/CVTji/MOVZji XXX,r  
		**	...			
		**	L/ASHi -n,r		 >   EXTSi XXX+(n/8),r,n%8,x
		**	.... (no use of r)	 >   ....
		**	ANDi	<x 1's>,r	 >
		**      (with (x+n) <= 32 and (x+n) <= size of j)
		**      Note: Bmove will probably eliminate the register load.
		*/
		if (p->code[0] != '-') goto no_exts;
		splitrand(p);
		if ( (n= -getnum(regs[RT1])) <= 0 ) goto no_exts;
		if ( (r=isreg(regs[RT2])) < 0 ) goto no_exts;
		pand = next_use(p,regs[RT2]);
		if (pand==0 || pand->op!=AND || pand->subop!=p->subop) goto no_exts;
		splitrand(pand);
		if (r!=isreg(regs[RT2])) goto no_exts;
		x = ispow2( 1+getnum(regs[RT1]) );
		if (x<=0) goto no_exts;
		/* Verify that can do EXTS and memory operand is large enough*/
		/* Note: If r was loaded with CVTji, regs[r][0]==(i<<4 | j)  */
		if ((x+n)>32 || (x+n)>bitsize[ (regs[r][0])&0xf ]) goto no_exts;

		{ REGISTER char *base = &regs[r][1];
		if (n>7 && compat(regs[r][0],p->subop) && *base!=0 && 
		    isreg(base)<0 && isimm(base)==0) {
			/* Adjust base address */
		  	if ( (cp=index(base,'(')) ) *cp = '\0';
			if (isimm(base))
				sprintf(line,"%d(%s,%s,%d,%d",
				        getnum(base)+(n/8),
					cp+1, regs[RT2], n%8, x);
			else
				sprintf(line,"%s+%d,%s,%d,%d",
					base, n/8, regs[RT2], n%8, x);
		} else {
			if (n>7) goto no_exts; /* Offset too big for reg */
			/* Note: EXTSi r,r,n,x always generated here, even
			   if the real source is known, because bmove will
			   eliminate the register anyway and no adjustment
			   of the source address is needed, as is done above.*/
			sprintf(line,"%s,%s,%d,%d",
				regs[RT2], regs[RT2], n, x);
		}
		}
		/* line now contains the EXTS operands */

		p->code = copy(line);
		p->op = EXTS;
		p->subop &= 0xf;  /* remove OP2, leaving i */
		p->pop=0;
		delnode(pand);
		++nchange;
		look_again=1; 
		break;  /* go process the exts */
	   no_exts:
		3; /* suppresses c compiler bug */
	   }
	   {
		REGISTER int	shreg;
		REGISTER char *shcnt_str;
		long	shcnt;
		/* address comp:
		**	l/ashd	1|2|3,r0      \	addr	_foo[r0:w|d|q],r0
		**	addr	_foo[r0:b],r0 /
		*/
		splitrand(p);
		shcnt_str = findcon(RT1,p->subop);
		if ((shcnt = getnum(shcnt_str)) < 1 || shcnt>3) goto split_std;
		if ((shreg = isreg(regs[RT2])) < 0) goto split_std;
		if (p->op==ROT) goto split_std;
		if ((p->subop&0xf) != LONG) goto ashadd;
		if (shreg < NUSE)
		{
			REGISTER int xreg;
			REGISTER char *cp1;
			REGISTER struct	node *pf = p->forw;

			if (pf->op != ADDR) goto ashadd;
			if (pf->subop != LONG) panic("ADDR");
			splitrand(pf);
			if( (xreg=index_reg(regs[RT1]))<0 ) goto std;
			if (xreg != shreg) goto std;
			if (isreg(regs[RT2]) != xreg) goto std;
			if (index_bwdq(regs[RT1]) != 'b') goto std;
			delnode(p);
			cp1 = regs[RT1];
			while(*++cp1!=':');      /* locate the :bwdq */
			*++cp1 = "?wdq"[shcnt];  /* ...and change it */
			newcode(pf);
			redunm++; nsaddr++; nchange++;
		}
		goto std;
ashadd:
		if (shcnt == 1 && 0<=isreg(regs[RT2])) {
			/*
			** quickie:
			**	l/ashi	1,r0	>	addi	r0,r0
			*/
			p->op = ADD; p->pop = 0;
			strcpy(regs[RT1], regs[RT2]); 
			if (*regs[RT3] != '\0') panic("ashadd");;
			newcode(p); nchange++;
			look_again=1; break;
		}
		goto split_std;
	}

	case CMP:
	   { REGISTER char *cp_l, *cp_r;
		/* Record compare, setting cc_... (used by redunbr) */
		splitrand(p);
		cp_l = findcon(RT1,p->subop);
		cp_r = findcon(RT2,p->subop);
		if (okio(cp_l) && okio(cp_r)) {
			if (cc_left[0] && 
		    	    equstr(cp_l,cc_left) && equstr(cp_r,cc_right) &&
		    	    cc_type==p->subop) {
				delnode(p); /* redundant compare */
				break; /* advance to next node */
			}
			cc_type = p->subop;
			strcpy(cc_left, cp_l);
			strcpy(cc_right, cp_r);
		} else 
			cc_left[0] = 0;	/* Can't optimize out IOreg refs */
		if (xdebug) 
			printf("\t--cmp-- %s,%s\n",cc_left,cc_right);
		goto split_std;
	   }	
	case LABEL: case DLABEL:
		clearreg(0);

	case COMMENT: case EROU: case JSW:
	case PROGRAM: case STATIC: case BSS: case ALIGN: case WGEN: case END: ;
		break;
	case FFS:
		splitrand(p);
		repladdr(p,CL_DEF2(CLREAD,CLREAD|CLWRITE),expand_literals);
		dest(regs[RT2],BYTE);
		break;

	default:
	   {
		REGISTER char **preg;
   		REGISTER CLASSWORD classword;

	std: 	splitrand(p); 
	split_std: 
		classword = opclasses(p);

		/* Modify read-only source operands, if appropriate */
		repladdr(p,classword,expand_literals); 

		/* Now call dest() for all write or rmw operands */
		for( preg= &regs[RT1]; **preg; ADVANCE_CL(classword),++preg){
			if (CLWRITE&classword) dest(*preg,p->subop);
		}
	   }
	} /* switch */
   }
if (tdebug) { 
	printf("after rmove(expand_literals=%d):",expand_literals);
	dump_all(); 
}
} /* rmove */

/* called from bmove() to kill all reads[] entries.  */
static void
clearreads() {
	REGISTER struct readsrec *rp;
	for (rp=reads; rp < (reads+NREADS); rp++ ) rp->r_type = 0;
}

/* Remember a non-register natural read-only operand, and insert a register 
   load if appropriate.  Called from bflow() with the type (low nibble) of 
   the operand.  Splitrand(p) must have been called on the node. */
struct node *
saveread(s,p,type) char *s; struct node * p; REGISTER int type; {
	REGISTER struct readsrec *rp, *availrp=0; int isscon, isfcon; 
	REGISTER int cost;
	REGISTER int val, opc=p->op;
	/* For simplicity, only use the last temp register, which is
	   assumed to be most likely to be available.  Maybe later all
	   the registers can be used, and the enter/exit instructions
	   modified, etc.  If This ever is done, beware of FPU regs
	   which must be saved & restored, and that DOUBLE register pairs
	   are involved. */

	/* Moves are actually inserted by ins_readreg(), called when
	   the register is about to be used (the move is inserted
	   after the last use of the temp register). */

	type &= 0xf;  /* Get rid of OP2, dest type of CVT/MOVZ, etc. */

	/* Forget it if register not available... */
	if (type >= FLOAT) 
		{ if (usect[LASTFTEMP-1]!=0 || usect[LASTFTEMP]!=0) return; }
	else
		{ if (usect[LASTTEMP]!=0) return; }
	if (!okio(s)) return;
	isscon = (isimm(s) != 0);   /* Get 0/1.  Scalar constant. */
	isfcon = (isfimm(s) != 0);  /* 	       FPU constant       */	
	if (isscon) {
	    	val=getnum(s);
		if ((MOV==opc || CMP==opc || ADD==opc) && -8 <= val && val <= 7)
			return;  /* will be a quick instruction */
	}
	for (rp=reads; rp < (reads+NREADS); rp++)
		if (rp->r_type==0) availrp=rp;
		else { 
			if (rp->r_type==type && equstr(rp->r_string,s)) 
				goto update_reads_entry; 
		}
	/* No match - write in a new entry */
	if (availrp==0) { 
		rp = nextreads_toreuse; 
		if ((nextreads_toreuse += 1)==(reads+NREADS))
			nextreads_toreuse=reads;
	} else rp = availrp;

	strcpy(rp->r_string,s);	 /* The thing referenced. */
	rp->r_type = type;
	rp->r_cost = 0; /* Zero cumulative size of references; */
	                 /* Fall thru to add in effect of this reference...*/
	rp->r_anycon = isscon | isfcon;
update_reads_entry:
	if (type > rp->r_type) {
		if (type == DOUBLE) return; /* *** Not compat with float! */
		rp->r_type = type;  /* Remember largest desired type */
	}

	/* If MOV will become addr instr, adjust type for size of displ.*/
	if (MOV==opc) {
		if (-64<= val && val <= 63 && type>=WORD) type=BYTE;
		else
		if (type==LONG && -8192<= val && val <= 8191) type=WORD;
	}
#ifdef SPACE
	if ((!isscon) && (!isfcon)) {
		/* If optimizing for minimum code size, charge one byte only
		   for fetching the operand in memory, + displacement sz. */
		rp->r_cost += 16; /* Probably 1 byte displacement */
	else
		rp->r_cost += bitsize[type];  /* size of immediate */
#else
	rp->r_cost += bitsize[type];	/* Charge for fetching operand itself */
	if ((!isscon) && (!isfcon)) {
		/* Non-constant item: Besides the operand in memory,
		   the addressing displacement is at least one byte... */
		rp->r_cost += 8; /* 8 bits */
	}
#endif
	return;
}

/* Insert a move of an operand into a register using information in reads[].  
   Part of bmove(), called just before processing an instruction which
   will use a register which might be used for a reads[] operand.
   
   All reads[] entries pertaining to register r are cleared.

   reg is the register about to become used in the preceeding instruction *p.
*/
static void
ins_readreg(reg,p) struct node *p; {
	REGISTER struct readsrec *bestrp = 0; /* Best entry to use */
	int best_type; /* type from *bestrp before it was cleared. */
	if (reg!=LASTTEMP && reg!=(LASTFTEMP-1)) panic("ins_readreg");
	{  REGISTER struct readsrec *rp;
	   REGISTER int newcost;
	   REGISTER int max_improvement = 0; /* Improvement for *bestrp */
	   for (rp=reads; rp < (reads+NREADS); rp++)
		if (rp->r_type==0 ||
		    (rp->r_type >= FLOAT && reg < FIRSTFREG) ||
		    (rp->r_type < FLOAT && reg >= FIRSTFREG))
			continue;  /* Register reg can't hold this */
		else {
	   		if (rp->r_anycon) { /* Scalar or FPU constant */
				REGISTER int val;
				/* First assume MOVi <con>,r */
				newcost = (2*8) + bitsize[rp->r_type];
#ifdef SPACE
				/* Empherical measurements indicate that it is 
				   slower on the 16032 to MOVQ or ADDR @ a 
				   small constant into a register and use it 
				   once than to just have a 4-byte immediate 
				   (this is contrary to what the published 
				   timing figures imply!).  Thus the smaller
				   size of these instructions are taken into
				   account only if optimizing for space.
				   (jima) */
				if (rp->r_type < FLOAT) {
					val = getnum(rp->r_string);
					if (-8 <= val && val <= 7) 
						/* MOVQi <con>,r */
						newcost = 2*8; 
					else
					if (rp->r_type==LONG && 
					    -8192<=val && val<=8191) 
						/*ADDR @<2byte displ>,r*/
						newcost = 4*8; 
				}
#endif
			}
			else
	   			/* Not an immediate: Cost is 2 bytes for MOV 
			   	   opcode + 1 byte (presumed) address displ. 
			   	   + memory operand size */
	   			newcost = ((2+1)*8) + bitsize[rp->r_type];
			if ((rp->r_cost - newcost) > max_improvement) {
				max_improvement = rp->r_cost - newcost;
				bestrp = rp;
				best_type = rp->r_type;
				if (xdebug) 
					printf("ins_readreg: max_improvment=%d t=%d c=%d %s\n", max_improvement, best_type, rp->r_cost, rp->r_string);
			}
			rp->r_type = 0;	/* Zap all entries considered */
		}
	} /* temp variable scope */
	
	if (bestrp != 0) {
        	REGISTER struct node *newp; REGISTER int r = reg;

		r = (best_type < FLOAT ? LASTTEMP : (LASTFTEMP-1)); 
		newp = alloc(sizeof first);
		p->forw->back = newp;	/* Insert just after p */
		newp->forw = p->forw;
		newp->back = p;
		p->forw = newp;
		COMBOP(newp) = T(MOV,best_type);
		newp->labno = 0;
		newp->pop = 0;
		sprintf(line,"%s,r%d\0", bestrp->r_string, r);
		newp->code = copy(line);
		if (debug) { printf("### ins_readreg(%d) - ",r); dump(newp); }
		++nchange;
		/* Fool bflow into not deleting the as-yet useless register 
		   load (rmove will replace the constants with register
		   references).  */
		uses[r]=p; usect[r]=1;
		regs[r][0] = -1; regs[r][1] = '\0'; 
#ifdef MULTIVAL
		regs[r][2] = '\0';
#endif
   	} 
}

/* ********************************************************************** */
/* Backwards-flow-analysis:  Bflow() is called by bmove() for most nodes,
   generally in last-to-first order.  
   
   During the traversal, usect[reg#] gives the number of references which
   might depend on that register's value (i.e., as a read or rmw operand),
   and uses[reg#] gives the most-distant such referencing instruction.
   Below that location, the register's "present" value (during the
   end-to-first scan) is not used.  Sometimes refererences deleted nodes.
   
   If usect[reg#]==0, then the register's value is not "presently" 
   relevant, and stores into it are eliminated in the absence of side-effects.
   Del_dead_stores enables this operation.

   regs[reg#] contains the known type (in byte [0]) and contents of
   a register.  "Contents", here, means the name of a location to which the
   value will be MOVed (null string if MOV is not last use of the value).
   If different types are used in scalar references (bwd), then regs[r][0]
   contains the largest of the types.

   If do_reads!=0, read-only operands are remembered (in reads[], 
   by saveread()), and when sufficiently repeated use of the same value 
   is detected, a move to an available temp register is inserted.  
   Subsequently, rmove() will replace references to the constants 
   with register references.

   The same node (p) is returned, or another node if appropriate...
   bmove steps back one to find the next item to look at.  Bflow may
   swap or delete instructions, but always returns something that
   insures that all instructions are processed at least once, and that
   usect[] & uses[] are correct.

   Most of bflow() was rewritten from scratch for the 16K (jima) */
static struct node *
bflow(p,do_reads,del_dead_stores) REGISTER struct node *p; {
	REGISTER char *cp1; char **preg; int r;
	REGISTER CLASSWORD classword;
	struct node *back;
	char p_op;
	char type, back_type; /* Low 4 bits only of subop */

	/* NOTE: Code here may return() at random places PROVIDED every
	   instruction eventually falls to the bottom (of bflow).  A
	   random return() is done, for example, when a node is deleted. */
  top: splitrand(p);
	p_op = p->op;
	type = p->subop&0xf;	/* this is a type only if proper op ! */
 	back=p->back; 
	back_type = back->subop&0xf;
	/* Rewrite this more efficiently someday... (jima) */
	if (OP2==p->subop>>4 &&
	    ((r=back->op)==MOV || r==MOVZ || (r==CVT && type<FLOAT)) &&
	    0<=(r=isreg(lastrand)) && r<NUSE &&
	    /* Ensure that for MOVi/CVTij/MOVZij;OPk, k>=i... */
	    type >= back_type) {
	   	
		REGISTER struct node *forw = p->forw; 
		         char srcreg = isreg(regs[RT1]);

		splitrand(back);
		if (r==isreg(lastrand) && uses[r]==forw && 
		    equstr(regs[RT1],&regs[r][1]) /* MOVi r,src follows */ &&
		    back_type==forw->subop && okio(regs[RT1])) {
			/*
			 *  MOVi/MOVZij/CVTij  src,r   \
			 *  OP2k  XXX,r                 > OP2i XXX,src
			 *  MOVi  r,src                /
			 *  (only use of reg r, sizes k>=i or just MOV if f.p.)
			 */
			if (usect[r]!=1) panic("usectOP2");
			delnode(back); delnode(forw);
			splitrand(p);
			strcpy(regs[RT2],&regs[r][1]); /* put in src operand */
			newcode(p); ++redunm;
			p->subop = U(forw->subop,OP2); p->pop=0;
			usect[r]=0;  /* register now unused! */
			goto top;    /* ...but may be used in src. Look again.*/
		}
		/* splitrand(back) is in effect here... */

		if (0<=srcreg && srcreg<NUSE && usect[srcreg]==0 && 
		    srcreg==isreg(regs[RT1]) && r==isreg(lastrand) &&
		    back->op==MOV && back_type==type) {
			/* Compute result in original register, and
			   THEN move it to the new register.  Maybe
			   the move will be eliminated later:
			   
			   movi rB,rA; op2i rB,rA --> op2i rB,rB; movi rB,rA
			*/
			swap_nodes(p->back,p); /* put the op2 first */
			splitrand(p);
			strcpy(regs[RT2],regs[RT1]);
			newcode(p);
			/* ++????? for statistics */
			++nchange;
			if (debug) {
				printf("### mov/op2 swapped, now:\n");
				dump(p); dump(p->forw);
			}
			return(p->forw->forw); /* process the move next */
		}
		splitrand(p);
	}
 	if (type && 0<=(r=isreg(lastrand)) && r<NUSE && usect[r]==1) {
	   char src[C2_ASIZE];
	   strcpy(src,regs[RT1]); 
	   /* r = possible destination, used only once (presumably readonly)*/
	   if (  (p_op==MOV || p_op==CVT || p_op==MOVZ)
		 && compat(p->subop,regs[r][0])
# ifdef SPACE
		 && !isimm(src) /* but leave movq/addr@ to save space */
# endif
		 ){
		REGISTER struct node *q;
		REGISTER CLASSWORD classword;
		q=uses[r];  /* Verify that src is not changed before place of 
			       use of r.  If (base) or [index] appear in src,
			       then we can't be sure intervening instructions
			       don't change them, so use must be next instr.
		             */
		if (q==p->forw || natural(src))
		   while((q=q->back) != p) {
			if (q->op==LABEL) break;
			splitrand(q);
			if ( CLSPECIAL==(classword = opclasses(q)) ) break;
			for( preg = &regs[RT1]; **preg; 
			     ADVANCE_CL(classword),++preg ) {
				if ((CLWRITE&classword) &&
				    (!natural(*preg)||equstr(*preg,src))) break;
			}
			if (**preg) break; /* Found a write to it... */
		}
		if (q==p) {
			/* Delete fetch to reg and replace use with value. */
			/* ...but not if an [index] or (offset) usage...   */
			/* (note: usect==1, so there's <= 1 replacable use)*/
			REGISTER int nmatch=0; CLASSWORD repl_classword;
			q = uses[r];
			splitrand(q); 
			if (CLSPECIAL!=(classword = opclasses(q)))
				for( preg = &regs[RT1]; **preg; 
			     	ADVANCE_CL(classword),++preg )
					if ((CL(classword)==CLREAD || 
				     	CL(classword)==(CLREAD|CLREG|CLADDR)) &&
				    	r==isreg(*preg)) { 
						/* Copy and truncate literal
						   (if present) to fit type. */
						trunc_strcpy(*preg,src,regs[r][0]);
						repl_classword=classword;
						++nmatch;
				}
			if (nmatch==1) { REGISTER int srcreg;
				if (debug) printf("### avoiding reg use...");
				usect[r]=0; ++redunm; ++nchange;
				newcode(q);
				/* Update uses[] (& usect[], etc.) & reads[]
				   to reflect whatever is in the src
				   operand moved to the instruction already
				   processed. */
				read_upduses(q,src,repl_classword,do_reads);
				baseindx_upduses(q,src);
				delnode(p);
				return(p);
			} 
			else 
				/* Must have been [index] or (offset) use */
				if (nmatch>1) panic("bad usect");
		} 
		splitrand(p); /* undo the damage */
     	   } /* end possibly avoidable MOV,CVT,MOVZ */
	   if (regs[r][1] && 
	       OP2==(p->subop>>4) && commutative(p->op) &&
	       equstr(&regs[r][1],src) && equtype(p->subop,regs[r][0]) ) {
		if (p->forw->op != MOV) panic("bflow comop2");

		splitrand(p->forw);
		if (r==isreg(regs[RT1]) && equstr(src,regs[RT2])) {
			/* Swap sides of commutative op to eliminate
			   the intermediate register:
			     op2 foo,r; mov r,foo --> op2 r,foo
			*/
			REGISTER char *tmp;
			splitrand(p);
			tmp=regs[RT1]; regs[RT1]=regs[RT2]; regs[RT2]=tmp;
			if (debug) printf("### commuting operands...");
			newcode(p);
			delnode(p->forw);
			usect[r]=0;
			++ncommut; ++nchange;
			return(p->forw);	/* Look again at the OP2 */
		} else
			splitrand(p);
	   }	
	} /* lastrand reg used only once */


		/* these two are here, rather than in bmove(),
		/* because I decided that it was better to go for 3-address code
		/* (save time) rather than fancy jbxx (save 1 byte)
		/* on sequences like  bisl2 $64,r0; movl r0,foo
		*/
/*	if (p->op==BIC) {p=bicopt(p); splitrand(p); lastrand=byond_anyrd(p);}
/*	if (p->op==OR) {bixprep(p,JBSS);           lastrand=byond_anyrd(p);}
 disabled for now - possibly obsolete or superceded elsewhere - jima */


	/* --------------------------------------------------------------
	   Now set uses[], usect[], and regs[] for the instruction, or delete 
	   the instruction if it uselessly manipulates a dead register.

	   Write-only destinations get cleared usect[], since the register's
	   previous value is irrelevant, unless the reg is read also in instr.

	   Write and rmw destinations get cleared regs[][1], since they no
	   longer contain the value to be MOVed to the indicated destination.

	   Read-only and possible-rmw operands increment usect[].  If this is
	   a new last-reference (usect was 0), regs[][0] is set to the type
	   and regs[][1] to the destination if a MOV, else set to null.
	   On subsequent refereces, regs[][0] is increased if a larger type 
	   is used.  

	   Read-only operands are remembered and possibly placed into an
	   available temp register if used repeatedly.
	*/

	/* Do write-only reg operands first, to detect stores
	   to dead regs and clear usect[].  Also eliminate register
	   use when result is just stored.  Then do all others... */

	{ REGISTER int dead_store  = -1; /*reg# if a store to dead reg found*/
	  REGISTER int cant_delete = 0;
	/* Note: mei/dei have two rmw operands incl. dummy added by input()*/

	if (CLSPECIAL==(classword = opclasses(p))) 
		return(p); /* Assume this can be treated like a nop */
	for (preg = &regs[RT1]; *(cp1= *preg); ADVANCE_CL(classword),++preg) {
		if ((CLWRITE&classword)) {
			++cant_delete;  /* tentative... */
		    	if (0<=(r=isreg(cp1)) && r<NUSE) {
				/* Delete write/rmw to truly dead register */
				if (del_dead_stores && !(CLADDR&classword) &&
				    (usect[r]==0 || uses[r]==p)) {
					--cant_delete; /* undo above ++ */
				    	dead_store=r;
				}
				if (!(CLREAD&classword)) { /* pure write... */
					REGISTER int type = p->subop;
					/* Make type be real destination type */
					if (p->op >= CSETB && p->op<=CSETD)
						type = BYTE + (p->op - CSETB);
					else
					if (type >= (1<<4) && 
					    type < (OP2<<4)) 
						type >>= 4; else type &= 0xf;
					if (type==0) panic("type=0");
					if (regs[r][1] && uses[r]==p->forw &&
					    usect[r]==1 &&
					    regs[r][0]==type) {
						/* Store to reg which is just
						   moved somewhere - eliminate
						   the register usage... */
						if (uses[r]->op != MOV)
							panic("regs!=MOV");
						if (debug) printf("### direct to dest...\n");
						strcpy(cp1,&regs[r][1]);
						newcode(p);
						delnode(uses[r]);
					} 
					if (regs[r][0] <= type || type>=FLOAT) {
						/* Old value not needed */
						usect[r]=0;  
					        /* Check for register pairs: */
						if (type==DOUBLE ||
					     	    (r>=FIRSTFREG && (r&1) && 
						     regs[r-1][0]==DOUBLE))
							usect[r^1]=0;
					} else {
						/* CLWRITE, but smaller type:
						   Partial modification.
						   Treat as rmw use, retaining
						   previous (larger) type. */
						upd_uses(r,p,regs[r][0]);
					}
				}
				regs[r][1]=0; /* move target no longer valid */
			}	
		} /* write or rmw to register */
		else 
		if (source(cp1) || !okio(cp1) ) ++cant_delete;

	} /* for loop */
	if (dead_store>=0 && (!cant_delete) && (!changes_cc(p))) {
		if(debug) 
		   printf("### deleting store to dead r%d.\n",dead_store);
		redunm++; ++nchange;
		delnode(p); 
		return(p); /* <<******* */
	}
	} /*(variable scope boundary)*/

	/* splitrand(p) still in effect here... */

	classword = opclasses(p);
	for (preg = &regs[RT1]; *(cp1= *preg); ADVANCE_CL(classword),++preg) {
		if (CLREAD&classword) 
				read_upduses(p,cp1,classword,do_reads);
		if (do_reads && (CLWRITE&classword)) {
			REGISTER struct readsrec *rp; 
			REGISTER int nat;
			/* Zap reads[] entries invalidated by write */
			nat = natural(cp1);
			for (rp=reads; rp<(reads+NREADS); rp += 1) 
			   if (rp->r_type != 0) {
				if (nat) 
					{if (equstr(rp->r_string,cp1))
						rp->r_type=0;}
				else     
					{if (!rp->r_anycon)rp->r_type=0;}
			   }
		} /* purge reads */
	
		/* check for (r) and [r:x] - usect/uses[r] set because reg 
		   is not dead, but type (SUBOP) is made funny to inhibit
		   incorrect optimizations -- normally SUBOP records the
		   operand types, not the type of base or index registers
		   involved in addressing the operands */
		baseindx_upduses(p,cp1);

	} /* other than write-only for loop */

	if (CMP==p->op) {
		cmpuse = curr_cmpuse;	/* save for use in bmove */
		cmploc=p;
		if (debug)printf("\t--cmpuse=%d;curr_cmpuse->zero\n",curr_cmpuse);
		if (cmpuse==0) { printf("; ?? cmpuse==0 at %d\n",p);
				 if (debug) dump_all(); }
		curr_cmpuse = 0;
	}
	/* Note: curr_cmpuse=p in bmove for CBR,CSETB,CSETW,CSETD */
	return(p);
}

/* Update uses[], etc., to reflect use of registers for (base) or [index]. */
baseindx_upduses(p,s) REGISTER struct node *p; REGISTER char *s; {
	for ( ; *s; ++s)
	   if (*s=='(' || *s=='[') {
		/* get register number */
		REGISTER char *rNstring, t;  REGISTER int r;
		rNstring= ++s; /* start of "rN" */
		while (*++s!=')' && *s!=':' && *s); t= *s; 
		if (*s) { /* it really is (r) or [r:x] */
			*s=0; /* Temporarily terminate "rN" */
			if ( 0<=(r=isreg(rNstring)) && r<NUSE ) {
				upd_uses(r,p,LONG);  /*Base or Index*/
				if (uses[r]==p) regs[r][1]=0;
			}
			*s=t; /* restore char after "rN" */
		}
	   }
}

/* Update uses[], etc., for a read operand (read,rmw,reg,regaddr).  Also
   update reads[] for read-only operands (if do_reads!=0).  */
read_upduses(p,s,classword,do_reads) 
   struct node *p; char *s; CLASSWORD classword; {
	REGISTER int r,n;
	if (0<=(r=isreg(s))) {
		if (r<NUSE) {
			if (p->op==TBIT && (CLADDR&classword)){
				/* 2nd operand's "type" is determined
			   	by the offset value, not the size of
			   	the offset operand (subop). */
				upd_uses(r,p,
				 	(isimm(regs[RT1])==0 ||
				  	(n=getnum(regs[RT1])) > 15 ?
				  	LONG : (n > 7? WORD:BYTE)));
			} else
				upd_uses(r,p,p->subop);
		} 
	} else {
		/* Not a register.  Update reads[] for read-only operands */
		if (do_reads /* && p->op != MOV */ ) {
			if (CL(classword)==CLREAD && natural(s))
		    		saveread(s,p,p->subop);
			else
		    	if (CL(classword)==(CLREAD|CLREG|CLADDR)
			    && substregaddr(p) && natural(s))
				/*It's substitutable with long*/
				saveread(s,p,LONG);
		}
	}
}

/* Update uses[r], usect[r], regs[r] for node p (part of bflow).  r is a
   read operand (or rmw, etc.).  splitrand(p) must have been called.
   Also inserts moves as indicated by reads[] when appropriate. 
   
   type should be p->subop (or low 4 bits) except in special cases where
   another type is to be forced.
*/
static void
upd_uses(r,p,type) REGISTER int r; struct node *p; REGISTER int type; {
	type &= 0xf; /* Delete OP2, dest type of CVT, etc. */ 
	if ((p->op >= CSETB && p->op<=CSETD && type != LONG) || type==0) 
		panic("upd_uses"); /*If CSETx can only be (base) or [index] */
	if (++usect[r]==1){
		uses[r]=p;
		regs[r][0]=type;
		if (p->op==MOV)   /* MOV r,somewhere */
			strcpy(&regs[r][1],regs[RT2]); 
		else
			regs[r][1]=0;
		if (r >= FIRSTFREG) {
			if (type==DOUBLE) setspecuse(r+1,p);
			if (r==(LASTFTEMP-1) || r==LASTFTEMP)
				ins_readreg(LASTFTEMP-1,p);
		} else 
			{ if (r==LASTTEMP) ins_readreg(LASTTEMP,p); }
	} else {
		if (regs[r][0] < type) /* Should only occur if type < FLOAT */
			regs[r][0] = type; /* larger size used */
	}

}

/* Check to see if the regaddr operand in an instruction can be substituted.
   Return the minimum-sized type if so, else zero.  Splitrand(p) must have
   been called. */
substregaddr(p) REGISTER struct node *p; {
	REGISTER int minsize;
     	switch (p->op) {
	 case EXT: case INS: /* forget it - reg offset */
	 case MOVS: case CMPS: case SKPS:    /* r0,r1,r2,r3,r4 bogus operands */
	 case MOVST: case CMPST: case SKPST: /* ...are never changable.  */
		return(0);
	 case TBIT: case SBIT: case CBIT: case IBIT:
		/* Size must be at least equal to 1+offset */
		minsize = 1+getnum(regs[RT1]);
		if (minsize>32 || (minsize==1 && !isimm(regs[RT1]))) return(0);
		break;
	 case INSS: case EXTS:
		/* Size must be at least offset+length */
		minsize = getnum(regs[RT3]) + getnum(regs[RT4]);
		break;
	 default: panic("substregaddr");
	}

	/* Return the type which is at least minsize bits */
	return( (minsize<=8 ? BYTE : (minsize<=16 ? WORD : LONG)) );
}

/* Copy a string, modifying literals to fit in a specified type (&0xf) */
trunc_strcpy(dst,src,type) char *dst, *src; REGISTER int type; {
	REGISTER unsigned long unsval;
	type &= 0xf;
	if (type<LONG && 0!=(unsval=getnum(src)) && unsval>0xfF) {
		sprintf( dst,"%d",
		         (type==BYTE ? (char)unsval : (short)unsval) );
	} else {
		while(*dst++ = *src++); /* copy the string literally */
	}
}

ispow2(n) REGISTER long n; {/* -1 -> no; else -> log to base 2 */
	REGISTER int log;
	if (n==0 || (n&(n-1))) return(-1); log=0;
	for (;;) {n >>= 1; if (n==0) return(log); ++log; if (n== -1) return(log);}

}

isfield(n) REGISTER long n; {/* -1 -> no; else -> position of low bit */
	REGISTER int pos; REGISTER long t;
	t= ((n-1)|n) +1;
	if (n!=0 && (0==t || 0==n || 0<=ispow2(t))) {
		pos=0; while(!(n&1)) {n >>= 1; ++pos;} return(pos);
	} else return(-1);
}

/* LEFT OVER FROM VAX - NEVER CONVERTED - always returns 0. SUPERCEDED? ... 
/*  
/* bixprep(p,bix) REGISTER struct node *p; {
/* /* initial setup, single-bit checking for bisopt, bicopt.
/* /* return: 0->don't bother any more; 1->worthwhile trying
/* */
/* 	REGISTER char *cp1,*cp2;
/* 	splitrand(p); cp1=regs[RT1]; cp2=regs[RT2];
/* 	if (*cp1++!='$' || 0>(pos=isfield(f=getnum(cp1)))
/* 	  || !okio(cp2) || indexa(cp2) || source(cp2) || !okio(lastrand)) return(0);
/* 	f |= f-1; if (++f==0) siz=32-pos; else siz=ispow2(f)-pos;
/* 	if (siz==1 && pos>5 && (p->subop>>4)==OP2 && (p->subop&0xf)!=BYTE
/* 	  && pos<bitsize[p->subop&0xf]) {
/* 		p->ref = insertl(p->forw); COMBOP(p) = CBR | (bix<<8);
/* 		p->pop=0;
/* 		p->labno = p->ref->labno;
/* 		if (pos>9) {*cp1++= pos/10 +'0'; pos %= 10;}
/* 		*cp1++=pos+'0'; *cp1=0; newcode(p); nbj++; return(0);
/* 	}
/* 	return(1);
/* }
/* 
/* 
/* static struct node *
/* bicopt(p) REGISTER struct node *p; {
/* /* use field operations or MOVZ if possible.  done as part of 'bflow'.
/* */
/* 	REGISTER char *cp1,*cp2; int r;
/* 	char src[C2_ASIZE];
/* 	if (!bixprep(p,JBCC)) return(p);
/* 	if (f==0) {/* the BIC isolates low order bits */
/* 		siz=pos; pos=0;
/* 		if ((p->subop&0xf)==LONG && *(regs[RT2])!='$') {/* result of EXTS is long */
/* 			/* save source of BICL in 'src' */
/* 			cp1=regs[RT2]; cp2=src; while (*cp2++= *cp1++);
/* 			if (p->back->op==ASH) {/* try for more */
/* 				splitrand(p->back); cp1=regs[RT1]; cp2=regs[RT3];
/* 				if (*cp1++=='$' && *(regs[RT2])!='$' && !indexa(regs[RT2])
/* 				  && 0>(f=getnum(cp1)) && equstr(src,cp2)
/* 				  && 0<=(r=isreg(cp2)) && r<NUSE) {/* a good ASH */
/* 					pos -= f; cp1=regs[RT2]; cp2=src; while (*cp2++= *cp1++);
/* 					delnode(p->back);
/* 				}
/* 			}
/* 			if (p->back->op==CVT || p->back->op==MOVZ) {/* greedy, aren't we? */
/* 				splitrand(p->back); cp1=regs[RT1]; cp2=regs[RT2];
/* 				if (equstr(src,cp2) && okio(cp1) && !indexa(cp1)
/* 				  && 0<=(r=isreg(cp2)) && r<NUSE
/* 				  && bitsize[p->back->subop&0xf]>=(pos+siz)
/* 				  && bitsize[p->back->subop>>4]>=(pos+siz)) {/* good CVT */
/* 					cp1=regs[RT1]; cp2=src; while (*cp2++= *cp1++);
/* 					delnode(p->back);
/* 				}
/* 			}
/* 			/* 'pos', 'siz' known; source of field is in 'src' */
/* 			splitrand(p); /* retrieve destination of BICL */
/* 			if (siz==8 && pos==0) {
/* 				COMBOP(p) = T(MOVZ,U(BYTE,LONG));
/* 				sprintf(line,"%s,%s",src,lastrand);
/* 			} else {
/* 				COMBOP(p) = T(EXTS,LONG);
/* 				sprintf(line,"$%d,$%d,%s,%s",pos,siz,src,lastrand);
/* 			}
/* 			p->pop=0;
/* 			p->code = copy(line); nfield++; return(p);
/* 		}/* end EXTS possibility */
/* 	}/* end low order bits */
/* /* unfortunately, INSV clears the condition codes, thus cannot be used */
/* /*	else {/* see if BICL2 of positive field should be INSV $0 */
/* /*		if (p->subop==(LONG | (OP2<<4)) && 6<=(pos+siz)) {
/* /*			COMBOP(p) = INSV;
/* /*			sprintf(line,"$0,$%d,$%d,%s",pos,siz,lastrand);
/* /*			p->code = copy(line); nfield++; return(p);
/* /*		}
/* /*	}
/* */
/* 	return(p);
/* }
 */

/* Modify adjacent CBR & BR instructions so that the Conditional branch
   references the shorter distance (comment:jima) */
jumpsw()
{
	REGISTER struct node *p, *p1;
	int nj;

	nj = 0;
	{ REGISTER int seqn=0; 
	  for (p=first.forw; p!=0; p = p->forw)
		p->seq = ++seqn;
	}
	for (p=first.forw; p!=0; p = p1) {
		p1 = p->forw;
		if (p->op == CBR && p1->op==BR && p->ref && p1->ref
		 && abs(p->seq - p->ref->seq) > abs(p1->seq - p1->ref->seq)) {
			if (p->ref==p1->ref)
				continue;
			p->subop = negbr[p->subop];
			p->pop=0;
			{ REGISTER struct node *t;
			  t = p1->ref;
			  p1->ref = p->ref;
			  p->ref = t;  }
			{ REGISTER int t;
			  t = p1->labno;
			  p1->labno = p->labno;
			  p->labno = t; }
#ifdef COPYCODE
			if (p->labno == 0) {
				REGISTER char *t;
				t = p1->code; p1->code = p->code; p->code = t;
			}
#endif
			nnegbr++;
			nj++;
		}
	}
	return(nj);
}


/* This was for the vax (SOB instructions...) (jima)
/*addsob()
/*{
/*	REGISTER struct node *p, *p1, *p2, *p3;
/*
/*	for (p = &first; (p1 = p->forw)!=0; p = p1) {
/*	if (COMBOP(p)==T(DEC,LONG) && p1->op==CBR) {
/*		if (abs(p->seq - p1->ref->seq) > 8) continue;
/*		if (p1->subop==JGE || p1->subop==JGT) {
/*			if (p1->subop==JGE) COMBOP(p)=SOBGEQ; else COMBOP(p)=SOBGTR;
/*			p->pop=0;
/*			p->labno = p1->labno; delnode(p1); nsob++;
/*		}
/*	} else if (COMBOP(p)==T(INC,LONG)) {
/*		if (p1->op==LABEL && p1->refc==1
/*		    && COMBOP(p1->forw)==T(CMP,LONG)
/*		    && COMBOP((p2=p1->forw->forw))==T(CBR,JLE)
/*		    && COMBOP((p3=p2->ref->back))==BR && p3->ref==p1
/*		    && p3->forw->op==LABEL && p3->forw==p2->ref) {
/*			/* change	INC LAB: CMP	to	LAB: INC CMP */
/*			p->back->forw=p1; p1->back=p->back;
/*			p->forw=p1->forw; p1->forw->back=p;
/*			p->back=p1; p1->forw=p;
/*			p1=p->forw;
/*			/* adjust beginning value by 1 */
/*				p2=alloc(sizeof first); COMBOP(p2)=T(DEC,LONG);
/*				p2->pop=0;
/*				p2->forw=p3; p2->back=p3->back; p3->back->forw=p2;
/*				p3->back=p2; p2->code=p->code; p2->labno=0;
/*		}
/*		if (COMBOP(p1)==T(CMP,LONG) && (p2=p1->forw)->op==CBR) {
/*			REGISTER char *cp1,*cp2;
/*			splitrand(p1); if (!equstr(p->code,regs[RT1])) continue;
/*			if (abs(p->seq - p2->ref->seq)>8) {/* outside byte displ range */
/*				if (p2->subop!=JLE) continue;
/*				COMBOP(p)=T(ACB,LONG);
/*				cp2=regs[RT1]; cp1=regs[RT2]; 
/*				while (*cp2++= *cp1++); /* limit */
/*				cp2=regs[RT2]; cp1="$1"; 
/*				while (*cp2++= *cp1++); /* increment */
/*				cp2=regs[RT3]; cp1=p->code; 
/*				while (*cp2++= *cp1++); /* index */
/*				p->pop=0; newcode(p);
/*				p->labno = p2->labno; delnode(p2); delnode(p1);
/*				nsob++;
/*			} else if (p2->subop==JLE || p2->subop==JLT) {
/*				if (p2->subop==JLE) COMBOP(p)=AOBLEQ; 
/*				else COMBOP(p)=AOBLSS;
/*				cp2=regs[RT1]; cp1=regs[RT2]; 
/*				while (*cp2++= *cp1++); /* limit */
/*				cp2=regs[RT2]; cp1=p->code; 
/*				while (*cp2++= *cp1++); /* index */
/*				p->pop=0; newcode(p);
/*				p->labno = p2->labno; 
/*				delnode(p2); delnode(p1); nsob++;
/*			}
/*		}
/*	}
/*	}
/*}
 */

abs(x)
{
	return(x<0? -x: x);
}

equop(p1, p2) /* Returns 1 iff two instructions are definiately identical */
REGISTER struct node *p1;
struct node *p2;
{
	REGISTER char *cp1, *cp2;

	if (COMBOP(p1) != COMBOP(p2))
		return(0);
	if (p1->op>0 && p1->op<FNBRPS)  /* branch or pseudo-op */
		return(0);
	if ((p1->op==ADDR || p1->op==LXPD) && p1->labno!=p2->labno) return(0);
	cp1 = p1->code;
	cp2 = p2->code;
	if (cp1==cp2)
		return(1);
	if (cp1==0 || cp2==0)
		return(0);
	while (*cp1 == *cp2++)
		if (*cp1++ == 0)
			return(1);
	return(0);
}

delnode(p) REGISTER struct node *p; {
	if (xdebug) { printf("delnode: "); dump(p);}
	p->back->forw = p->forw;
	p->forw->back = p->back;
}

decref(p)
REGISTER struct node *p;
{
	if (p && --p->refc <= 0) {
		nrlab++;
		delnode(p);
	}
}

struct node *
nonlab(ap)
struct node *ap;
{
	REGISTER struct node *p;

	p = ap;
	while (p && p->op==LABEL)
		p = p->forw;
	return(p);
}
static void
swap_nodes(p1,p2) REGISTER struct node *p1, *p2; {
	REGISTER struct node *p1back, *p2forw;
	p1back = p1->back; if (p1back==p2) p1back=p2->back;
	delnode(p1); 
	p2forw = p2->forw;
	delnode(p2);

	p2->forw = p1back->forw;	/* re-insert p2 */
	p1back->forw->back = p2;
	p2->back = p1back;
	p1back->forw = p2;

	p1->back = p2forw->back;	/* re-insert p1 */
	p2forw->back->forw = p1;
	p1->forw = p2forw;
	p2forw->back = p1;
}

/* Set uses[r] to p for a special use.  Put full-width type information and
   usect>1 (99), to suppress single-use optimizations.
   This is used when (p) "uses" the register in a strange way */
static void
setspecuse(r,p) REGISTER int r; REGISTER struct node *p; {
	uses[r]=p; usect[r]=99; 
	regs[r][0] = (r<FIRSTFREG ? LONG : FLOAT);
	regs[r][1]=0;
}

static  void
clearuse(temps_only) {
	REGISTER int r,i;
	for (i=(temps_only?RNUMS_LASTTEMP:RNUMS_LAST);i>=0;--i) {
		r = rnums[i];
		uses[r] = 0;
		usect[r] = 0;
	}
	cmploc=0;
	cmpuse=0;
	curr_cmpuse=0;
}

#ifdef MULTIVAL
/* Squish out a sub-field of a regs[] entry.  See clearreg(), dest(). */
static void
squishout_regs_field(cp) REGISTER char *cp;{
	REGISTER char *src = 1+index(cp,'\0');
	do { while (*cp++ = *src++) {};
	} while (*src);
	*cp = 0; /* Plant 2nd null */
}
#endif

/* Clear regs[] entries.  If for_call is non-zero, preserve
   non-temp reg entries containing a constant. */
   
clearreg(for_call) {
	REGISTER int i = RNUMS_LAST;
	if (for_call) {
		/* Preserve non-temp references to constants */
		REGISTER char *cp; REGISTER int r; REGISTER char *startcp;
		do {
			cp = startcp = regs[rnums[i]];
			if (*cp) {
#ifdef MULTIVAL
				/* Scan and copy over, removing sub-fields
				   invalidated by a procedure call */
				REGISTER char *src, *dst;
				++cp; /* Start of 1st field */
				do {
					if (call_invalidates(cp))
						squishout_regs_field(cp);
					else
						cp = 1+index(cp,'\0');
				} while (*cp);
				if (cp==startcp+1) *startcp=0; /* none left */
#else
				if (call_invalidates(cp+1)) *(short *)(cp) = 0; 
#endif
			}
		} while (--i > RNUMS_LASTTEMP);
	}

	/* Delete entire entries for temp regs, or all if for_call==0 */
	do { *(long *)(regs[rnums[i]]) = 0; } while (--i >= 0);
	
	conloc[0] = 0; 
	cc_left[0] = 0;
}

/* Return non-zero iff regs[] entry might be invalidated by a call, 
   i.e., is not a constant or simple reference to a non-temp register. 
   Part of clearreg(for_call). */
call_invalidates(s) REGISTER char *s; {
	REGISTER int r,c;
	if (*s=='r') {
		r = *++s - '0';
		c = *++s; 
		if (c!=0) r = (r*10) + (c - '0');
		return ( (r<FIRSTFREG ? (r<=LASTTEMP) : 
				        (r<=LASTFTEMP || r>=NREG)) );
	} else
		return (!isimm(s) && !isfimm(s));
}


#ifdef MULTIVAL
/* Store source *s with type in regs[ai], unless indexing with other
   registers, in which case regs[ai] is cleared.  If *s has side-effects,
   e.g., tos, nothing is done.

   If entries already exist in regs[ai], and the type is the same, then
   *s is added to them, pushing out existing entries if necessary.  If the
   type is not the same, the old entries are deleted.	 (jima)
*/
addreg(ai, s, type)
#else
/* Store source *s with type in regs[ai], unless indexing with other
   registers, in which case regs[ai] is cleared.  If *s has side-effects,
   e.g., tos, nothing is done.
*/
savereg(ai, s, type)
#endif
REGISTER char *s;
/* Note: "type" may be a CVT subop, i.e., dest & src types in uppper & lower
   4 bits, respectively. */
{
	REGISTER char *p, *sp; /* for undoing entry */ 

	if (source(s)) /* side effects in addressing */
		return;
	sp = p = regs[ai];
#ifdef MULTIVAL
	if (*p==type) { /* existing compatible entries */
		REGISTER int slen = 2+strlen(s); /*2 for final 2 nulls*/
		REGISTER char *deadentry=0;
		++p;
		/* Set p to the location for the new entry (erasing the end
		   entries -the newest,sadly- if necessary)... */
		while(*p) {
			deadentry=p; 
			while(*p++);
			if ( (slen+(p-sp)) >= C2_ASIZE ) {p=deadentry; break;}
		}
	} else
#endif
		/* Lay down a singular entry */
		*p++ = type;
	/* if any offsets, must be parameter, local, or sb-relative. */
	/* Indirection (as in "0(-4(fp))") is ok, however */
	/* (why is [indexing] excluded?  I'm dis-allowing non-zero offsets,
	   e.g., 3(-4(fp)), here, since I don't understand this fully
	   yet.  The vax allowed "*-n(fp)" but not indexing. (jima)) */
	while (*p++ = *s) s++;  /* leave p pointing after copied null */ 
	--s; /* s now last non-nul char; look first for `0(n(xx))'... */
	if (*(s-1)==')' && *(sp+1)=='0') --s; /* ...and step over last `) */

	if (*s==']' || 
	    (*s==')' && *(s-2)!='f' && *(s-1)!='b'))
		p = sp; /* set to erase the new entry - unacceptable form */

	/* Add  2 more nulls (or erase entry if it was unacceptable) */
	* (short *) p = 0;
}

/* Zap regs[] entries as appropriate when *s is a destination (wo or rmw). */
static
dest(s,type)
REGISTER char *s;
{
	REGISTER int i;
	REGISTER char *cp;

	source(s); /* handle addressing side effects */
	if ((i = isreg(s)) >= 0) {
		/* if register destination, that reg is a goner */
#ifdef MULTIVAL
		*(long *)(regs[i]) = 0; 
#else
		*(short *)(regs[i]) = 0; 
#endif
		if (DOUBLE==(type&0xf) || DOUBLE==((type>>4)&0xf))
#ifdef MULTIVAL
			*(long *)(regs[i+1]) = 0; /* clobber two at once */
#else
			*(short *)(regs[i+1]) = 0;
#endif
		else
		if (i>FIRSTFREG && (i%2) && 
		    (DOUBLE==((regs[--i][0])&0xf) || DOUBLE==((regs[i][0]>>4)&0xf)) )
			/* This was 2nd half of DOUBLE, so clobber 1st half */
#ifdef MULTIVAL
			*(long *)(regs[i]) = 0; 
#else
			*(short *)(regs[i]) = 0;
#endif
	}
	
	if (!natural(s)) {/* wild store, everything except constants vanish */
		for (i=FIRSTFREG; --i>=0;) 
#ifdef MULTIVAL
			if (!(cp=isimm(&regs[i][1]))) *(long *)(regs[i]) = 0;
			else cp[1]=0; /* zap anything after the constant */
#else
			if (!isimm(&regs[i][1])) *(short *)(regs[i]) = 0;
#endif
		for (i=NREG; --i>=FIRSTFREG;) 
#ifdef MULTIVAL
			if (!(cp=isfimm(&regs[i][1]))) *(long *)(regs[i]) = 0;
			else cp[1]=0; /* zap anything after the constant */
#else
			if (!isfimm(&regs[i][1])) *(short *)(regs[i]) = 0;
#endif
		conloc[0] = 0; cc_left[0] = 0;
	}
	else 
	/* Prev. indirection through dest or values of dest are dead. */
	for (i=NREG; --i>=0;) {
#ifdef MULTIVAL
		REGISTER char *startcp;
		cp = startcp = regs[i]+1;
		while(*cp!=0) {
			if (part(cp,s)) 
				/* remove the dead entry */
				squishout_regs_field(cp);
			else
				cp = 1+index(cp,'\0'); /* find next entry */
			if ((int)(*cp)==1) panic("no null");
		}
		if (cp==startcp+1) *startcp=0; /* blast type - none left */
#else
		if (part(regs[i]+1,s)) {*(short *)(regs[i]) = 0; }
#endif
	}
	if (part(cc_left,s) || part(cc_right,s)) cc_left[0]=0;
}

/* separate operands at commas, copying them separately (null-termianted)
   to regs[RT1..RT5].  If there are more operands, they are glommed together
   in regs[RT5] (this should only happen for unusual instructions such as
   enter and exit which are not manipulated anyway). 
   
   lastrand is set to the last operand, or regs[RT1] if there are none. */
splitrand(p) struct node *p; {
	REGISTER char *p1, *p2; REGISTER char **preg;

/* Check for bogus address - SHOULD REMOVE THIS LATER */
if ( (char *)p < firstr || (char *)p > lasta ) panic("splitrand");

	lastrand = regs[RT1];
	preg=regs+RT1;
	if (p1=p->code) 
		while (*p1) {
			lastrand=p2= *preg++;
			while (*p1) 
				if (','==(*p2++= *p1++) && preg!=regs+RT5+1) {
					--p2;
					break;
				}
			*p2=0;
		}

	while (preg<(regs+RT5+1)) *(*preg++)=0;
}

compat(have, want) {
	REGISTER int hsrc, hdst;
	if (0==(want &= 0xf)) return(1); /* anything satisfies wildcard want */
	hsrc=have&0xf; if (0==(hdst=((have>>4)&0xf)) || hdst>=OP2) hdst=hsrc;
	if (want>=FLOAT) return(hdst==want && hsrc==want);
		/* FLOAT, DOUBLE not compat: rounding */
	return(hsrc>=want && hdst>=want && hdst<FLOAT);
}

/* Was just: {return(compat(t1,t2) && compat(t2,t1));}  (jima) */
equtype(t1,t2) {
	REGISTER int t1dest, t2dest; /* dest types in conversions */
	t1dest = (t1>>4)&0xf; t2dest = (t2>>4)&0xf;
	return(compat(t1,t2) && compat(t2,t1) && 
	       (t2dest>=OP2 || compat(t1,t2dest)) &&
	       (t1dest>=OP2 || compat(t2,t1dest)) );
}

/* Find a register which already contains the specified value.  Non-temp
   registers are preferred, followed by lower-numbered temp registers.
   This is to maximize the chance of a temp register becomming completely
   unused, with it's load eliminated by a subsequent bflow call. 
   
   If the specified value is already a register, then only a "better" 
   register is selected (which has the same or compatible value).
   
   The register number is returned (possibly the same as the original).
   -1 is returned if there is no match, or the value is the null string. */
findrand(as, type) char *as; {
	REGISTER char **j; REGISTER int r,newreg;
	char original_reg = -1; /* Original source reg if source not in mem */
	REGISTER char *cp; 
	REGISTER char *pregno; /* cursor for scal_regs[] or fpu_regs[] */
	/* Order of register preference: */
	static char scal_regs[] = { 7,6,5,4,3,0,1,2,99 };  /* 99 is teminator */
	static char fpu_regs[] = { 15,14,13,12,8,9,10,11,99 };

	pregno = ((type&0xf) < FLOAT ? scal_regs : fpu_regs);

	if (0<=(r=isreg(as))) { /* ...used to include "&& r<NUSE" */
		/* If the "value" is a register, look for a "better" 
	   	   register containing the same value. */
		as = &regs[r][1];
		/* The first field of regs[r] is the source of the data moved
		   into r, and will be the original memory data source unless
		   the first loaded register was loaded from an unknown
		   source, such as a function value in r0.  In that case,
		   the first-loaded register appears as the first field.
		   This register is returned if it is "better" than r. */
		if (*as=='r') original_reg = isreg(as);
	}
	if (*as==0) return(-1);

	/* Look for high non-temp regs first, then low temp regs */
	while( (newreg = *pregno) != 99) {
		if (newreg==r) break; /* No better reg. than what we've got*/
		cp = regs[newreg];
#ifdef MULTIVAL
		if (*cp && compat(*cp,type)) {
			++cp;
			while (*cp) {
				if (equstr(cp,as) || newreg==original_reg) {
					if (*cp==0) panic("null findrand");
					return(newreg);
				}
				cp = 1+index(cp,'\0');
				/* if ((int)cp==1)panic("no null3"); */
			}
		}
		if ((cp-regs[newreg]) >= C2_ASIZE) 
			panic("regs term"); /* no double null*/
#else
		if (*cp && equstr(cp+1, as) && compat(*cp,type)) {
			REGISTER int return_reg;
			if (cp[1]==0) panic("null findrand");
			if (*cp==0) panic("null findrand");
			return(newreg);
		}
#endif
		++pregno;  /* Bump scal_regs/fpu_regs index */
	}
	return(-1);  /* Nothing found */
}

/* Return non-zero iff register r already contains the datum in string s */
reg_hasit(r,s,type) char *s; {
	REGISTER char *cp;

	cp = regs[r];
	if (*cp && compat(*cp,type)) {
#ifdef MULTIVAL
		while (*++cp) {
			if (equstr(cp,s)) return(1); /* found it */
			cp = index(cp,'\0');
		}
#else
		if (equstr(cp+1,s) return(1);
#endif
	}
	return(0); /* the reg doesn't have the datum in it */
}

/* Return reg# if "s" is a null-terminated register name, else -1. (jima) */
isreg(s)
REGISTER char *s;
{
	if (*s++!='r' || !isdigit(*s++)) return(-1);
	if (*s==0) return(*--s-'0');
	if (*(s-1)=='1' && isdigit(*s++) && *s==0) return(10+*--s-'0');
	return(-1);
}

/* If an operand is an immediate scalar constant termianted by a null or comma,
   return pointer to the terminator character, otherwise zero.  (jima) */
char *
isimm(s) REGISTER char *s; {
	if (*s == '-') ++s;  /* possible negative */
	if (isdigit(*s)) {
		while (isdigit(*++s)); 
		if ( *s == ',' || *s == '\0') return(s);
	}
	return(0);

}

/* If an operand is an immediate floating or double floating hex constant,
   return pointer to the following comma or null.  Otherwise, return zero. 
   Intervening characters are not verified. (jima) */
char *
isfimm(s) REGISTER char *s; {
	if (*s=='l' && s[1]=='\'') {
		s += 2;
		while (*s != ',' && *s != '\0') ++s;
		return(s);
	}
	else return(0);
}
		
/* Return 'b', 'w', 'd', or 'q' from [index mode] if present, else zero */
index_bwdq(s) REGISTER char *s; {
	while(*s != '[') { if (*s++ == '\0') return(0); }
	if (*++s != 'r' || !isdigit(*++s) || *++s != ':') panic("indx_bwdq");
	return(*++s);
}
		
/* Return [index mode] register number if present, else -1 */
index_reg(s) REGISTER char *s; {
	while(*s != '[') { if (*s++ == '\0') return(-1); }
	if (*++s != 'r' || !isdigit(*++s) || s[1] != ':') panic("index_reg");
	return(*s - '0');
}


check()
{
	REGISTER struct node *p, *lp;

	lp = &first;
	for (p=first.forw; p!=0; p = p->forw) {
		if (p->back != lp)
			abort(-1);
		lp = p;
	}
}

/* Zap *regs[] entries for registers modified by side-effects of source
    operand.  
    
    1 is returned if there are side-effects, zero otherwise.
 
    On the 16032, only "tos" has side-effects.  regs[] entries with SP-relative
    addressing are invalidated.  (This is intended to help future use of 
    explicit SP-relative addressing, to eliminate enter/exit in 
    small procedures.  SP-rel references are not yet fully supported as 
    of 8/83 - jima).  Note that regs[] should never contain "tos", as
    "tos" is not a unique location.
 
    Source is also called for destination operands.
*/

source(ap)
char *ap;
{
	/* In the vax version, this did "*(short *)(regs[<regno>])=0"
	   for registers used with auto-incr/decr modes (jima) */
	if (equstr("tos",ap)) {
		/* Invalidate sp-relative references */
		REGISTER int i;
		REGISTER char *cp,*regs_string;
		for (i=NREG; --i>=0;) {
			regs_string = regs[i];
			if (*regs_string == 0) continue;
#ifdef MULTIVAL
			cp = regs_string+1;  /* Skip over type byte */
			do {
				if (*cp && part(cp,"sp"))
					/*Delete this field and all following.*/
					*(short *)(cp) = 0; /* 2 nulls */
				while (*cp++); /* Skip to next field, if any */
			} while (*cp); /* Double null terminates last field */
#else
			if (part(regs_string+1,"sp"))
			*(short *)(regs_string) = 0; /* type=0 & null string */
#endif
		} /* for each register */
		conloc[0] = 0; cc_left[0] = 0;
		return(1);
	}
	return(0);
}

newcode(p) struct node *p; {
	REGISTER char *p1,*p2,**preg;
	preg=regs+RT1; p2=line;
	while (*(p1= *preg++)) {while (*p2++= *p1++); *(p2-1)=',';}
	*--p2=0;
	p->code=copy(line);
}

/* movedat()
/* {
/* 	register struct node *p1, *p2;
/* 	struct node *p3;
/* 	REGISTER seg;
/* 	struct node data;
/* 	struct node *datp;
/* 
/* 	if (first.forw == 0)
/* 		return;
/* 	datp = &data;
/* 	for (p1 = first.forw; p1!=0; p1 = p1->forw) {
/* 		if (p1->op == STATIC) {
/* 			p2 = p1->forw;
/* 			while (p2 && p2->op!=PROGRAM)
/* 				p2 = p2->forw;
/* 			if (p2==0)
/* 				break;
/* 			p3 = p1->back;
/* 			p1->back->forw = p2->forw;
/* 			p2->forw->back = p3;
/* 			p2->forw = 0;
/* 			datp->forw = p1;
/* 			p1->back = datp;
/* 			p1 = p3;
/* 			datp = p2;
/* 		}
/* 	}
/* 	if (data.forw) {
/* 		datp->forw = first.forw;
/* 		first.forw->back = datp;
/* 		data.forw->back = &first;
/* 		first.forw = data.forw;
/* 	}
/* 	seg = -1;
/* 	for (p1 = first.forw; p1!=0; p1 = p1->forw) {
/* 		if (p1->op==PROGRAM||p1->op==STATIC||p1->op==BSS) {
/* 			if (p1->op == seg || p1->forw&&p1->forw->op==seg) {
/* 				p1->back->forw = p1->forw;
/* 				p1->forw->back = p1->back;
/* 				p1 = p1->back;
/* 				continue;
/* 			}
/* 			seg = p1->op;
/* 		}
/* 	}
/* }
*/

/* Short-circuit a br to a conditional br which will always be taken (when 
   reached via the br).  Part of rmove.			(comment:jima) */
   
redunbr(p)
REGISTER struct node *p;
{
	REGISTER struct node *p1;
	REGISTER char *ap1;
	REGISTER char *ap2, cmp_subop;

	if ((p1 = p->ref) == 0)
		return;
	p1 = nonlab(p1);
	if (p1->op != CMP) 
		return;
	cmp_subop = p1->subop;
	splitrand(p1);
	if (cc_left[0]!=0) { /* never!=0 if CMP operands are not okio() */
		ap1 = findcon(RT1, cmp_subop);
		ap2 = findcon(RT2, cmp_subop);
		/* ap1,ap2 are the operands or known equivalent constants */
		if ( equstr(ap1,cc_left) && equstr(ap2,cc_right) &&
		     equtype(cc_type,cmp_subop)) {
			/* Bypass redundant compare */
			if (debug) {
				printf("### redunbr: Bypassing redun cmp:");
				dump(p1);
			}
			p1=insertl(p1->forw); decref(p->ref);
			p->ref=p1; p->labno=p1->labno;
			nrtst++; nchange++;
		}
	}
	if (p1->forw->op==CBR) {
		ap1 = findcon(RT1, cmp_subop);
		ap2 = findcon(RT2, cmp_subop);
		p1 = p1->forw; /* Now the CBR */
		if (compare(p1->subop, ap1, ap2)) {
			nredunj++;
			nchange++;
			decref(p->ref);
			p->ref = p1->ref;
			p->labno = p1->labno;
#ifdef COPYCODE
			if (p->labno == 0)
				p->code = p1->code;
			if (p->ref)
#endif
				p->ref->refc++;
		}
		/* SOMEDAY: Put in check for reverse case, and branch
		   around the CMP/CBR if appropriate (jima) */
	}
}

/* Return a string which is either operand regs[i], or a constant 
   (i.e., digit string) which is the value of the operand if it is a
   register.  
   
   If the resulting string is a constant, it is the value of the operand.
   (comment:jima)*/
static char *
findcon(i, type)
{
	REGISTER char *cp;
	REGISTER int r;

	cp = regs[i];
	if (isimm(cp))
		return(cp);
	if (0<=(r=isreg(cp)) && compat(regs[r][0],type))
		return(regs[r]+1);
	if (equstr(cp, conloc))
		return(conval+1);
	return(cp);
}

/* Return true if two strings are constants and the specified comparison
   op relation holds.  (comment:jima) */
compare(op, acp1, acp2)
char *acp1, *acp2;
{
	REGISTER char *cp1, *cp2;
	REGISTER int n1;
	int n2;	int sign;

	cp1 = acp1;
	cp2 = acp2;
	if (!isimm(cp1) || !isimm(cp2))
		return(0);
	n1 = 0; sign=1; if (*cp2=='-') {++cp2; sign= -1;}
	while (isdigit(*cp2)) {n1 *= 10; n1 += (*cp2++ - '0')*sign;}
	n2 = n1;
	n1 = 0; sign=1; if (*cp1=='-') {++cp1; sign= -1;}
	while (isdigit(*cp1)) {n1 *= 10; n1 += (*cp1++ - '0')*sign;}
	if (*cp1=='+')
		cp1++;
	if (*cp2=='+')
		cp2++;
	do {
		if (*cp1++ != *cp2)
			return(0);
	} while (*cp2++);

	/* The following used to store n1 & n2 into cp1 & cp2, and test
	   the pointers, apparently to get unsigned comparisons.  Besides 
	   being obscure, the results were wrong (vax c2, at least, treats
	   pointers as signed integers), and incorrect results were obtained
	   when redunbr() eliminated certain unsigned comparisons. (jima) */
	switch(op) {

	case JEQ:
		return(n1 == n2);
	case JNE:
		return(n1 != n2);
	case JLE:
		return(n1 <= n2);
	case JGE:
		return(n1 >= n2);
	case JLT:
		return(n1 < n2);
	case JGT:
		return(n1 > n2);
	case JLO:
		return(((unsigned int)n1) < ((unsigned int)n2));
	case JHI:
		return(((unsigned int)n1) > ((unsigned int)n2));
	case JLOS:
		return(((unsigned int)n1) <= ((unsigned int)n2));
	case JHIS:
		return(((unsigned int)n1) >= ((unsigned int)n2));
	}
	return(0);
}

/* Record that constant *cv is being stored in location *cl (with type type)
   provided that *cv is in fact a constant and *cl is a natural location
   (i.e., does not alias unknown things).		(commnet:jima) */

setcon(cv, cl, type)
REGISTER char *cv, *cl;
{
	REGISTER char *p;

	if (!isimm(cv))
		return;
	if (!natural(cl))
		return;
	p = conloc;
	while (*p++ = *cl++);
	p = conval;
	*p++ = type;
	while (*p++ = *cv++);
}

/* return 1 if null-terminated strings are identical, else 0 */
equstr(p1, p2)
REGISTER char *p1, *p2;
{
	do {
		if (*p1++ != *p2)
			return(0);
	} while (*p2++);
	return(1);
}

okio(s) REGISTER char *s; {/* 0->probable I/O space address; 1->not */
	if (ioflag && (!natural(s) || *s=='@')) return(0);
	return(1);
}

indexa(p) REGISTER char *p; {/* 1-> uses [r] addressing mode; 0->doesn't */
	while (*p) if (*p++=='[') return(1);
	return(0);
}

/* Return 1 iff operand can't alias other names */
natural(cp) REGISTER char *cp;
{/* 1->null, simple local, parameter, global (name or explicit n(sb)), tos,
    or register; 0->otherwise.  BEWARE TOS, which doesn't reference a unique
    location... */
	while (*cp++);
	cp -= 2;  /* last character */
	if (*cp==']' || (*cp==')' && *(cp-2)!='f' && *(cp-1)!='b'))
		return(0);
	return(1);
}

/*
** Tell if an argument is most likely static.
*/

isstatic(cp)
REGISTER char	*cp;
{
	if (*cp == '_' || *cp == 'L' || (*cp++ == 'v' && *cp == '.'))
		return (1);
	return (0);
}
