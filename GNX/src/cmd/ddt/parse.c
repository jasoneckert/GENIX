
/*
 * parse.c: version 1.5 of 4/5/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)parse.c	1.5 (NSC) 4/5/83";
# endif

#include <setjmp.h>
#include <ctype.h>
#include "main.h"
#include "error.h"
#include "parse.h"
#include "scan.h"
#include "symbol.h"
#include <sys/fpu.h>

int	ops[MAXOPS];	/* operand values */
int	typeops[MAXOPS];/* type operands */
int	opcnt;		/* number of operands seen */
int	escopcnt;	/* number of operands seen in a $ command */
int	esctoo;		/* saw esc command, $ or $$ command */

struct aregtab reg2tab[] = {	/* 2 character register lookup table */
	"r0",	SR0,
	"r1",	SR1,
	"r2",	SR2,
	"r3",	SR3,
	"r4",	SR4,
	"r5",	SR5,
	"r6",	SR6,
	"r7",	SR7,
	"fp",	SFP,
	"sp",	SSP,
	"pc",	SPC,
	"f0",	F0,
	"f1",	F1,
	"f2",	F2,
	"f3",	F3,
	"f4",	F4,
	"f5",	F5,
	"f6",	F6,
	"f7",	F7,
	"\0",	0
};
struct aregtab reg3tab[] = {	/* 3 character register lookup table */
	"psr",	SPSR,
	"mod",	SMOD,
	"msr",	SMSR,
	"eia",	SEIA,
	"pf0",	SPF0,
	"pf1",	SPF1,
	"sc0",	SSC0,
	"fsr",	FSR,
	"\0",	0
};
struct aregtab reg4tab[] = {	/* 4 character register lookup table */
	"bpr0",	SBPR0,
	"bpr1",	SBPR1,
	"bcnt",	SBCNT,
	"ptb0",	SPTB0,
	"ptb1",	SPTB1,
	"intb",	SINTB,
	"\0",	0
};

parseabort()
{
	longjmp(resetbuf, PARSE_ERROR);
}

parseit(s)
char	**s;
{
char	*ps;
int	gotoken;

	ps = *s;
	esctoo = opcnt = escopcnt = 0;
	typeops[0] = PERROR;
	gotoken = PERROR;
	do {
		gotoken = parsetoken(&ps, &ops[opcnt], &typeops[opcnt]);
		if (gotoken == PUNRECOG) break;
		gotoken = parseexpr(&ps, &ops[opcnt], &typeops[opcnt]);
		opcnt++;
		if (opcnt > MAXOPS) {
			printf("\r\ntoo many operands(%d)\r\n",opcnt);
			parseabort();
		}
	} while (gotoken == ',');
	if (esctoo = (*ps == ESC)) {
		    ps++;
		    while (*ps == ' ') ps++;	
		    while (*ps == ESC) { esctoo++; ps++; }
		    do {
			while ((*ps==' ') || (*ps==',') || (*ps==ESC)) ps++;
			if ((*ps >= '0') && (*ps <= '9'))
			{
			    parsenumber(&ps,&ops[opcnt],&typeops[opcnt]);
			    opcnt++;
			    escopcnt++;
			    if (opcnt > MAXOPS)  {
				printf("\r\ntoo many operands(%d)\r\n",opcnt);
				parseabort();
			    }
			}
		    }
		    while ((*ps == ESC) || (*ps == ','));
		    if (isalpha(*ps) == 0) gotoken = PERROR;
	}
	if ((gotoken != PUNRECOG) || (gotoken == PERROR)) {
		printf("\r\nunrecognizable command character ->%c\r\n",*ps);
		parseabort();
	}
	*s = ps;

}

parsetoken(s,opval,optype)
char	**s;
int	*opval,*optype;
{
char	*ps;
int	rtncode = PERROR;

	ps = *s;
	while (*ps == ' ') ps++;	
	if  (*ps == '.') {
		if (dot < 0) {
		    *opval = getreg(dot);
		    *optype = dot;
		} else {
		    *opval = dot;
		    *optype = rtncode = PNUMBER;
		}
		ps++;
	} else if (*ps == '`') {
		*opval = lastdata;
		*optype = rtncode = PNUMBER;
		ps++;
	} else if (isdigit(*ps)) {
		rtncode = parsenumber(&ps,opval,optype);
	} else if (isalpha(*ps)) {
		rtncode = parsesymbol(&ps,opval,optype);
	} else {
		switch(*ps) {
		case '_':
		case '$':
				rtncode = parsesymbol(&ps,opval,optype);
				break;
		case '%':
				if (isalpha(*(ps+1))) {
				    rtncode = parsesymbol(&ps,opval,optype);
				    break;
				}
		case '+':
		case '-':
		case '*':
		case '&':
		case '|':
		case '(':
		case ')':
		case '^':
		case '~':
		case '#':
		case '<':
		case '>':
		case ',':
		case '@':
				*optype = rtncode = *ps;
				ps++;
				break;
		default:	*optype = rtncode = PUNRECOG;
				break;
		}
	}
	*s = ps;
	return(rtncode);
}

parsepthree(s,opval,optype)
char	**s;
int	*opval,*optype;
{
int	lastoken,pval,ptype;
	if (*optype == '(') {
		parsetoken(s, opval, optype);
		lastoken = parseexpr(s,opval,optype);
		if (lastoken != ')') {
			printf("\r\n  ) is missing\r\n");
			parseabort();
		}
	} else if ((*optype != PNUMBER) && (*optype > SR0)) {
		printf("\r\n bad expression\r\n");
		parseabort();
	}
	parsetoken(s, &pval, &ptype);
	while (ptype == '@') {
		*optype = PNUMBER;
		*opval = getdouble(*opval);
		parsetoken(s, &pval, &ptype);
	}
	return(ptype);
}

parseptwo(s,opval,optype)
char	**s;
int	*opval,*optype;
{
int	lastoken;
	if (*optype == '~') {
		parsetoken(s, opval, optype);
		lastoken = parsepthree(s,opval,optype);
		*opval = ~*opval;
		*optype = PNUMBER;
	} else if (*optype == '-') {
		parsetoken(s, opval, optype);
		lastoken = parsepthree(s,opval,optype);
		*opval = 0 - *opval;
		*optype = PNUMBER;
	} else lastoken = parsepthree(s,opval,optype);
	return(lastoken);
}

parsepone(s,opval,optype)
char	**s;
int	*opval,*optype;
{
int	doop,lastoken,rightval,righttype;

	lastoken = parseptwo(s,opval,optype);
	while ((lastoken == '*') || (lastoken == '&') || 
	(lastoken == '#') || (lastoken == '<') || (lastoken == '>') || 
	(lastoken == '%')) {
		doop = lastoken;
		parsetoken(s, &rightval, &righttype);
		lastoken = parseptwo(s,&rightval,&righttype);
		if (((doop == '#')||(doop == '%')) && (rightval == 0)) {
			printf("\r\ndivide by zero expression\r\n");
			parseabort();
		} else {
		    switch(doop) {
		    case '*':
			    *opval *= rightval;
			    break;
		    case '&':
			    *opval &= rightval;
			    break;
		    case '#':
			    *opval /= rightval;
			    break;
		    case '%':
			    *opval %= rightval;
			    break;
		    case '>':
			    *opval >>= rightval;
			    break;
		    case '<':
			    *opval <<= rightval;
			    break;

		    }
		}
	}
	return(lastoken);
}

parseexpr(s,opval,optype)
char	**s;
int	*opval,*optype;
{
int	doop,lastoken,rightval,righttype;

	lastoken = parsepone(s,opval,optype);
	while ((lastoken == '+') || (lastoken == '-') || 
	(lastoken == '|') || (lastoken == '^')) {
		doop = lastoken;
		parsetoken(s,&rightval,&righttype);
		lastoken = parsepone(s,&rightval,&righttype);
		switch(doop) {
		case '+':
			*opval += rightval;
			break;
		case '-':
			*opval -= rightval;
			break;
		case '|':
			*opval |= rightval;
			break;
		case '^':
			*opval ^= rightval;
			break;

		}
		*optype = PNUMBER;

	}
	return(lastoken);
}

parsenumber(s,opval,optype)
char	**s;
int	*opval,*optype;
{
	int cval, radix, sawhex = FALSE;
	char numstr[MAXNUM], *nps, *ps;
	ps = *s;
	nps = numstr;
	radix = tempmodes.inradix;
	do {
		if (isdigit(*ps)) *nps++ = *ps++;
		else if (((*ps >= 'a') && (*ps <= 'f')) ||
			 ((*ps >= 'A') && (*ps <= 'F'))) {
			 if (esctoo) break;
			 sawhex = TRUE;
			 radix = 16;
			 *nps++ = *ps++;
		     } else break;
		     if (nps > (numstr + MAXNUM)) {
			printf("\r\nnumber too large\r\n");
			parseabort();
		     }

	} while (TRUE);
	*nps = '\0';
	if (esctoo) {
	    radix = 10;
	} else
	    if (*ps == 'x') {
		    radix = 16;
		    ps++;
	    } else if (*ps == '.') {
		    radix = 10;
		    ps++;
	    } else if (*ps == 'o') {
		    radix = 8;
		    ps++;
	    }
	if (sawhex) radix = 16;
	nps = numstr;
	*opval = 0;
	do {
		if (*nps >= 'a') cval = 10 + (*nps - 'a');
		else if (*nps >= 'A') cval = 10 + (*nps - 'A');
		else cval = *nps - '0';
		*opval = (*opval * radix) + cval;
		nps++;
	} while (*nps != '\0');
	*optype = PNUMBER;
	*s = ps;
	return(*optype);
}

regtablookup(tabptr,sptr,opval,optype)
struct aregtab *tabptr[];
char	*sptr;
int	*opval,*optype;
{
	int i = 0;
	*optype = NO_TYPE;
	while (*(tabptr[i]->regname) != '\0') {
		if (strcmp(tabptr[i]->regname,sptr) == 0) {
			*optype = tabptr[i]->regval;
			*opval = getreg(*optype);
		} else i++;
	}
}

parsesymbol(s,opval,optype)
char	**s;
int	*opval,*optype;
{
char *strps,sym[MAXSTRLEN], *ps;
int strlen, regnum, themod;
int i = 0;
	ps = *s;
	strps = sym;
	strlen = 0;
	while (isalnum(*ps) || (*ps == '_') || (*ps == '$') ||
		(*ps == '.') || (*ps == '%'))
	    {
		if (*ps == '$') {
			*strps++ = '.';
			*ps++;
		} else *strps++ = *ps++;
		if (strlen < MAXSTRLEN) strlen++; else break;
	    }
	*strps = '\0';
	*optype = NO_TYPE;
	if (strlen == 2) {
	    *optype = NO_TYPE;
	    while (*(reg2tab[i].regname) != '\0') {
		    if (strcmp(reg2tab[i].regname,sym) == 0) {
			    *optype = reg2tab[i].regval;
			    *opval = getreg(*optype);
			    break;
		    } else i++;
	    }
	} else if (strlen == 3) {
	    *optype = NO_TYPE;
	    while (*(reg3tab[i].regname) != '\0') {
		    if (strcmp(reg3tab[i].regname,sym) == 0) {
			    *optype = reg3tab[i].regval;
			    *opval = getreg(*optype);
			    break;
		    } else i++;
	    }
	} else if (strlen == 4) {
	    *optype = NO_TYPE;
	    while (*(reg4tab[i].regname) != '\0') {
		    if (strcmp(reg4tab[i].regname,sym) == 0) {
			    *optype = reg4tab[i].regval;
			    *opval = getreg(*optype);
			    break;
		    } else i++;
	    }
	}
	if (*optype == NO_TYPE) {
		if (lookbysym(opval,&themod,sym) == FALSE) parseabort();
		*optype = PNUMBER;
	}
	*s = ps;
	return(*optype);

}

parselinecmd(s)
char	**s;
{
}
