
/*
 * main.h: version 1.6 of 3/25/83
 * 
 *
 * @(#)main.h	1.6	(National Semiconductor)	3/25/83
 */

/* main ddt definitions */

#define TRUE		1
#define FALSE		0
#define PROMPTLEN	10	
#define MAXSTRLEN	20
#define MAXNUM	       	14

struct modetypes {
    int	relative,	/* relative symbol display mode(TRUE,FALSE) */
	automat,	/* automatic display, try to show as text/data, based
			   on object file info */
	outputmode,	/* output display type (floating,char,etc) */
	context,	/* data size */
	inradix,	/* output radix */
	outradix;	/* input radix */
};

extern int	ddtquit();	/* exit question function */
extern int	ddtdied();	/* exit function */
extern int	imageonly;	/* show only object file contents */	
extern int	imagefd;	/* object file descriptor */
extern int	coreonly;	/* show only object file contents */	
extern int	corefd;		/* core file descriptor */
extern int	usefile;	/* use file for data */
extern int	skipptrace;	/* use object, not ptrace for text */
extern int	printsyms;	/* show symbols as entered to table */
extern int	trustsyms;	/* find module linked syms with STEs */
extern int	remotemachine;	/* debug a remote box via serial port */
extern int	nommu;		/* Is there a mmu in the system */
extern int	memfd;		/* file file descriptor */
extern char	imagename[];    /* object filename */
extern char	corename[];	/* core filename */
extern int	address,dot,lastdata; /* last display, data and addresses */
extern int	lasttypeout; 	/* last typeout value */
extern struct modetypes tempmodes, permmodes;
extern int	matchbase;   /* look for symbols if address greater than this */
extern int	matchequal;	/* match symbols this near the address */
extern struct	sgttyb	ddtstty,userstty;
