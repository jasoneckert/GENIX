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
 * xdefs.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)xdefs.c	5.1 (NSC) 9/8/83";
# endif
/* JZ 4/23/83:  Added bregvar, minbvar for implementing floating point register
		variables.  They are used in cgram.y, mfile1, code.c and pftn.c
		*/
/*  JZ 5/6/83:  change dcon to quad_wrd if ns16000 is not defined*/
# include "mfile1"

/*	communication between lexical routines	*/

char	ftitle[100] = "";   	/* title of the file */
char	ititle[100] = "";   	/* title of initial file */
int	lineno;		/* line number of the input file */

CONSZ lastcon;  /* the last constant read by the lexical analyzer */
#if ns16000
double dcon;   /* the last double read by the lexical analyzer */
#else
quad_wrd dcon;
#endif


/*	symbol table maintainence */

struct symtab stab[SYMTSZ+1];  /* one extra slot for scratch */

int	curftn;  /* "current" function */
int	ftnno;  /* "current" function number */

int	curclass,	  /* current storage class */
	instruct,	/* "in structure" flag */
	stwart,		/* for accessing names which are structure members or names */
	blevel,		/* block level: 0 for extern, 1 for ftn args, >=2 inside function */
	curdim;		/* current offset into the dimension table */
	
int	dimtab[ DIMTABSZ ];

int	paramstk[ PARAMSZ ];  /* used in the definition of function parameters */
int	paramno;	  /* the number of parameters */
int	autooff,	/* the next unused automatic offset */
	argoff,	/* the next unused argument offset */
	strucoff;	/*  the next structure offset position */
int	regvar;		/* the next free register for register variables */
int	bregvar;	/* The next free B (floating) register variable */
int	minbvar;	/* The smallest that bregvar gets within a function*/
int	minrvar;	/* the smallest that regvar gets within a function */
OFFSZ	inoff;		/* offset of external element being initialized */
int	brkflag = 0;	/* complain about break statements not reached */

struct sw swtab[SWITSZ];  /* table for cases within a switch */
struct sw *swp;  /* pointer to next free entry in swtab */
int swx;  /* index of beginning of cases for current switch */

/* debugging flag */
int xdebug = 0;

int strflg;  /* if on, strings are to be treated as lists */

int reached;	/* true if statement can be reached... */

int idname;	/* tunnel to buildtree for name id's */


NODE node[TREESZ];

int cflag = 0;  /* do we check for funny casts */
int hflag = 0;  /* do we check for various heuristics which may indicate errors */
int pflag = 0;  /* do we check for portable constructions */

int brklab;
int contlab;
int flostat;
int retlab = NOLAB;
int retstat;

/* save array for break, continue labels, and flostat */

int asavbc[BCSZ];
int *psavbc = asavbc ;

# ifndef BUG1
static char *
ccnames[] = { /* names of storage classes */
	"SNULL",
	"AUTO",
	"EXTERN",
	"STATIC",
	"REGISTER",
	"EXTDEF",
	"LABEL",
	"ULABEL",
	"MOS",
	"PARAM",
	"STNAME",
	"MOU",
	"UNAME",
	"TYPEDEF",
	"FORTRAN",
	"ENAME",
	"MOE",
	"UFORTRAN",
	"USTATIC",
	};

char * scnames( c ) register c; {
	/* return the name for storage class c */
	static char buf[12];
	if( c&FIELD ){
		sprintf( buf, "FIELD[%d]", c&FLDSIZ );
		return( buf );
		}
	return( ccnames[c] );
	}
# endif
