
/*
 * parse.h: version 1.3 of 11/29/82
 * 
 *
 * @(#)parse.h	1.3	(National Semiconductor)	11/29/82
 */

/* for looking at line have read and getting operands */

/* represent registers are negative numbers for dot and address variables */

/* return values for what was parsed */
#define NO_TYPE	 0 /* no type know yet */
#define PERROR   1
#define PUNRECOG 2
#define PNUMBER	 3
#define PREG 	 4

/* 16032 registers */

#define SR0	-1
#define SR1	-2
#define SR2	-3
#define SR3	-4
#define SR4	-5
#define SR5	-6
#define SR6	-7
#define SR7	-8

#define SFP	-9
#define SSP	-10
#define SPC	-11
#define SPSR	-12
#define SMOD	-13

/* 16082 registers */

#define SMSR	-14
#define SEIA	-15
#define SPF0	-16
#define SPF1	-17
#define SSC0	-18
#define SBPR0	-19
#define SBPR1	-20
#define SBCNT	-21
#define SPTB0	-22
#define SPTB1	-23
#define SINTB	-24

#define MAXOPS		14

struct aregtab {
	char *regname;
	int  regval;
};

extern struct aregtab reg2tab[];	/* 2 character register lookup table */
extern struct aregtab reg3tab[];	/* 3 character register lookup table */
extern struct aregtab reg4tab[];	/* 4 character register lookup table */
extern int	ops[MAXOPS];	/* operand values */
extern int	typeops[MAXOPS];/* type operands */
extern int	opcnt;		/* number of operands seen */
extern int	escopcnt;	/* number of operands seen in a $ command */
extern int	esctoo;		/* saw two esc command, a $$ command */
extern int	parseit();
