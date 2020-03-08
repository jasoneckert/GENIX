
/*
 * main.c: version 1.22 of 8/29/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)main.c	1.22 (NSC) 8/29/83";
# endif

/* ddt,	duncan's debugging tool */
/* Duncan Gurley	with honors to Ross Harvey */

#include <setjmp.h>
#include <signal.h>
#include <sgtty.h>
#include <stdio.h>
#include <sys/fpu.h>
#include "main.h"
#include "error.h"
#include "scan.h"
#include "parse.h"
#include "display.h"
#include "symbol.h"
#include "machine.h"
#include "bpt.h"

#define MAXFILENAME 80	/* a standard terminal line */

int	ddtquit();	/* use this for exiting ddt with question */
int	ddtdied();	/* use this for crash exit from ddt */
int	ddtsigbus();	/* caught a sigbus, printf and die */
int	ddtsigsegv();	/* caught a sigsegv, printf and die */
int	ddtstart();	/* start, stop; job control, if remoteddt */
int	ddtstop();	/* then reset port */
int	ddtfloat();	/* handle FPE traps printf bombs on some numbers */
char	getchr();	/* basic character reader */
struct	sgttyb	ddtstty,userstty; /* ddt is in raw, noecho mode, save users */
jmp_buf	resetbuf;	/* jump back through here */
char	promptstr[PROMPTLEN] = "\0";	/* prompt string */
int	errorflg;			/* last error code */
char	imagename[MAXFILENAME] = "a.out";/* object filename */
char	corename[MAXFILENAME] = "core";	/* core filename */
int	imageonly = FALSE;		/* show only object file contents */	
int	coreonly = FALSE;		/* show only core file contents */	
int	usefile = FALSE;		/* use file for data */
int	patchfile = FALSE;		/* want to write on file */
int	skipptrace = FALSE;		/* use object, not ptrace for text */
int	printsyms = FALSE;		/* show symbols as found in a.out */
int	trustsyms = TRUE;		/* find module linked syms with STEs */
int	remotemachine = FALSE;		/* debug a remote box via serial port */
int	nommu = FALSE;			/* Is there a mmu in the system */
int	unixcore = FALSE;		/* A unix kernel core crash session */
int	imagefd = -1;			/* object file descriptor */
int	corefd = -1;			/* core file descriptor */
int	memfd = -1;			/* file file descriptor */
struct modetypes tempmodes, permmodes;	/* input/display mode state */
int	matchbase = 0; /* look for symbols if address greater than this */
int	matchequal = 1000;/* match symbols this near the address */
int	nowopen = FALSE;/* location open for store */
int	stillopen = FALSE;	/* location still open for store */
int	dot = 0,address = 0,lastdata = 0;	/* address values */
int	lasttypeout = 0; 	/* last typeout value */
int	help = FALSE;		/* junk help flag */

main(argc, argv)
int	argc;
char	**argv;
{
	gtty(0, &ddtstty);
	userstty = ddtstty;
	signal(SIGINT,ddtquit);
	signal(SIGQUIT,ddtdied);
	signal(SIGSEGV,ddtsigsegv);
	signal(SIGBUS,ddtsigbus);
	signal(SIGTSTP,ddtstop);
    	signal(SIGCONT,ddtstart);
    	signal(SIGFPE,ddtfloat);
	ddtstty.sg_flags |= CBREAK;
	ddtstty.sg_flags &= ~(ECHO | CRMOD);
	ddtstty.sg_erase = '\010'; /* datamedia control-h */
	ddtstty.sg_kill = '\025';  /* control-u */
	stty(0, &ddtstty);
	gtty(0, &ddtstty);
	options(argc,argv);	/* get command line options */
	initexec();	/* init exec arg points */
	if (imagefd > 0) getsymboltable(); 
	if (corefd > 0) 
	if (unixcore)
		getpanicblk();	/* get unix kernel panic block */
	else getudot();	/* get user.h structure from core file*/
	initstops();
	getcmds();	/* read commands */
}

options(pargc,pargv)
/* get ddt command line options */
int	pargc;
char	*pargv[];
{
register int	i,j;

	permmodes.inradix = 16;		/* default radix is hex */
	permmodes.automat = TRUE;	/* automatic instruction display */
	for(i=1; i<pargc; i++) {
		if (pargv[i][0] == '-') {
		for (j=1; pargv[i][j]!='\0'; j++) {
		    switch(pargv[i][j]) {
		    case 0:
			    break;
		    case 'i':
			    imageonly = TRUE;	/* look at object file only */
			    remotemachine = FALSE;
			    continue;
		    case 'c':
			    coreonly = TRUE;	/* only a core file exists */
			    remotemachine = FALSE;
			    continue;
		    case 'h':
			    permmodes.inradix = 16;	/* radix hex */
			    continue;
		    case 'd':
			    permmodes.inradix = 10; 	/* radix decimal */
			    continue;
		    case 'f':
			    usefile = TRUE; /* use file as mem file  */
			    remotemachine = FALSE;
			    continue;
		    case 'n':
			    nommu = TRUE; /* don't use remote mmu commands */
			    continue;
		    case 'q':
			    skipptrace = TRUE;	/* use object file for data */
			    continue;
		    case 'r':
			    remotemachine = TRUE; /* remote debug via port */
			    continue;
		    case 's':
			    printsyms = TRUE;	/* just show symbols in file */
			    remotemachine = FALSE;
			    continue;
		    case 't':
			    trustsyms = FALSE;/* use module table not symbols*/
			    continue;
		    case 'w':
			    patchfile = TRUE;	/* allow writes to file */
			    continue;
		    case 'u':
			    unixcore = TRUE;	/* unix core file debugging */
			    remotemachine = FALSE;
			    continue;
		    default:
			    printf("unknown option\r\n");
			    break;
		    }
		}
		} else {	/* grab core file and open */
		    if ((imagefd > 0) || coreonly) {
			for (j=0; pargv[i][j]!='\0'; j++)
			    corename[j] = pargv[i][j];
			corename[j] = '\0';
			if (patchfile)
				corefd = open(corename,2);
			else corefd = open(corename,0);
			if (corefd < 0) {
			    coreonly = FALSE;
			    printf("can't open core file %s\r\n",corename);
			    ddtdied();
			}
		    } else {	/* grab object file */
			for (j=0; pargv[i][j]!='\0'; j++)
			    imagename[j] = pargv[i][j];
			imagename[j] = '\0';
			if (patchfile)
				imagefd = open(imagename,2);
			else imagefd = open(imagename,0);
			if (imagefd < 0) {
			    printf("can't open object file %s\r\n",imagename);
			    if (!remotemachine) ddtdied();
			}
			}
		    }
		}
	if (imagefd == -1) {  /* check for file ddt option */
		if (patchfile)
			imagefd = open(imagename,2);
		else imagefd = open(imagename,0);
		if ((imagefd < 0) && (coreonly == FALSE)) {
		    printf("no object file\r\n");
		    if (!remotemachine) ddtdied();
		}
	}
	if (usefile == TRUE) { /* check for /dev/kmem or file ddt */
		if (corefd < 0) {
			if (imagefd < 0) {
			    printf("\r\nfile open failed\r\n");
			    ddtdied();
			} else {
			    memfd = imagefd;
			    imagefd = -1;
			    permmodes.automat = FALSE;
			}
		} else {
			memfd = corefd;
			corefd = -1;
			coreonly = FALSE;
		}
	}
}

getcmds()
/* command top level, all abort long jump to here if they continue */
{
    permmodes.relative = TRUE;
    permmodes.outputmode = NUMERIC;
    permmodes.outradix = permmodes.inradix;
    permmodes.context = 4;
    cancel();
    while (1) {
	errorflg = setjmp(resetbuf);
	switch (errorflg) {
	case PROC_ERROR:
	case MAC_ERROR:
		stty(0, &ddtstty);
	case SCAN_ABORT:
	case SCAN_ERROR:
	case USER_INT:
	case PARSE_ERROR:
	case MEM_ERROR:
	case REG_ERROR:
	case BPT_ERROR:
		nowopen = FALSE;
	case NO_ERRORS:
		stillopen = FALSE;
		scaninit(getchr, promptstr, resetbuf);
		break;
	case SCAN_EDIT:
	default:
		break;
	}
	getcmd();
    }
}

#define CNTRLD	4
#define CNTRLF	6
#define CNTRLB	2

getcmd()
{
    int		i,newdot;
    char	ch, *pstr;
    pstr = scanbuffer;
    while (1) {
	    ch = scanchar();
	    switch (ch) {
	    case ESC:
		    stillopen = FALSE;
		    doesccmd(ch);
		    break;
	    case '\n':
		    /* increment dot and display */
		    incrdot(pstr);
		    break;
	    case CNTRLB:
		    for (i = 0; i < 10; i++) {
			dostep(FALSE);
		    }
		    break;
	    case CNTRLD:
		    for (i = 0; i < 10; i++) {
			printf("\r\n");
		    	incrdot(pstr);
		    }
		    break;
	    case CNTRLF:
		    for (i = 0; i < 10; i++) {
			dostepover(FALSE);
		    }
		    break;
	    case ']':
		    /* do a single step */
			*scanreadptr = '\0';
			parseit(&pstr);
			if (opcnt) {
				i = ops[opcnt-escopcnt-1];
				if (i < 0) i = 1;
			} else i = 1;
			for(; i > 0; i--) dostep(FALSE);
			break;
	    case '[':
		    /* try a big step */
			*scanreadptr = '\0';
			parseit(&pstr);
			if (opcnt) {
				i = ops[opcnt-escopcnt-1];
				if (i < 0) i = 1;
			} else i = 1;
			for(; i > 0; i--) dostepover(FALSE);
			break;
	    case '}':
		    if ((scanreadptr - 1) == scanbuffer)  {
		    /* do a single step with bpts in */
			dostep(TRUE);
			break;
		    } else continue;
	    case '{':
		    if ((scanreadptr - 1) == scanbuffer)  {
		    /* try a big step with bpts in */
			dostepover(TRUE);
			break;
		    } else continue;
	    case '\r':
		    *scanreadptr = '\0';
		    parseit(&pstr);
		    if (opcnt) {
			if (typeops[opcnt-escopcnt-1] < 0) {
				lastdata = typeops[opcnt-escopcnt-1];
				lastdata = getreg(lastdata);
			} else {
				lastdata = ops[opcnt-escopcnt-1];
			}
			if (nowopen) {
				if (address < 0) {
					setreg(address,lastdata);
				} else {
					setmem(address,lastdata);
				}
			}
		    }
		    cancel();
		    if (help) printf("\n\r do cancel, store\r\n");
		    break;
	    case '\t':
		    /* open, display and change dot to address */
		    if (help) printf("\n\r open,display change dot\r\n");
		    *scanreadptr = '\0';
		    parseit(&pstr);
		    printf("\r\n");
		    nowopen = stillopen = TRUE;
		    if (opcnt) {
			if (typeops[opcnt-escopcnt-1] < 0) {
			       address = typeops[opcnt-escopcnt-1];
			} else {
			    address = ops[opcnt-escopcnt-1];
			    if (address < 0) address &= 0xffffff;
			}
			lasttypeout = dot = address;
		    } 
		    if (lasttypeout < 0) {
			if ((lasttypeout < SINTB) &&
			!((lasttypeout <= F0) && (lasttypeout >= FSR)))
			    address  = dot = lasttypeout & 0xffffff;
			else {
			    address = getreg(lasttypeout);
			    lasttypeout = dot = address;
			}
		    } else address = dot = lasttypeout;
		    typeout(address,"*/\t");
		    lastdata = display(address);
		    printf("\t");
		    break;
	    case '?':
		    /* decrement dot and display */
		    if (help) printf("\n\r -dot display\r\n");
		    *scanreadptr = '\0';
		    parseit(&pstr);
		    if (opcnt) {
			if (typeops[opcnt-escopcnt-1] < 0) {
				lastdata = typeops[opcnt-escopcnt-1];
				lastdata = getreg(lastdata);
			} else {
				lastdata = ops[opcnt-escopcnt-1];
			}
			if (nowopen) {
				if (address < 0) {
					setreg(address,lastdata);
				} else {
					setmem(address,lastdata);
				}
			}
		    }
		    nowopen = stillopen = TRUE;
		    if (dot < 0) {
			if ((dot <= F0) && (dot >= FSR)) {
				if (dot == F0) dot = FSR;
				else dot++;
			} else {
				if (remotemachine) {
				    if (dot == SR0) dot = SINTB;
				    else dot++;
				} else if (dot == SR0) dot = SBCNT;
					else dot++;
			}
			address = dot;
			lastdata = getreg(dot);
			printf("%s/\t",regstr(dot));
			typeout(lastdata,"*\t*");
		    } else {
			if ((newdot=ibackup(dot))==dot) dot -= acontext;
			else dot = newdot;
			address = dot;
			typeout(address,"*/\t");
			lastdata = display(address);
			printf("\t");
		    }
		    break;
	    case '\\':
		    /* open, display and don't change dot to address */
		    if (help) printf("\n\r open,display don't change dot\r\n");
	    case '/':
		    /* open, display and change dot if an address is typed */
		    if (help && (ch == '/'))
				printf("\n\r open,display change dot\r\n");
		    *scanreadptr = '\0';
		    parseit(&pstr);
		    nowopen = stillopen = TRUE;
		    if (opcnt) {
			if (typeops[opcnt-escopcnt-1] < 0) {
			       address = typeops[opcnt-escopcnt-1];
			} else {
			    address = ops[opcnt-escopcnt-1];
			    if (address < 0) address &= 0xffffff;
			}
			if (ch == '/') dot = address;
		    } 
		    if (address < 0) {
			lastdata = getreg(address);
			printf("\t");
			typeout(lastdata,"***");
		    } else {
			address = dot;
			printf("\t");
			lastdata = display(address);
		    }
		    printf("\t");
		    break;
	    case '!':
		    /* open, change dot if an address is typed */
		    if (help) printf("\n\r open if change dot\r\n");
		    *scanreadptr = '\0';
		    parseit(&pstr);
		    if (opcnt) {
			if (typeops[opcnt-escopcnt-1] < 0) {
			       address = typeops[opcnt-escopcnt-1];
			} else {
			    address = ops[opcnt-escopcnt-1];
			    if (address < 0) address &= 0xffffff;
			}
		    } 
		    nowopen = stillopen = TRUE;
		    break;
	    case '=':
		    /* retype last expr as numeric */
		    if (help) printf("\n\r retype as numeric\r\n");
		    *scanreadptr = '\0';
		    stillopen = TRUE;
		    parseit(&pstr);
		    if (opcnt) {
			if (typeops[opcnt-escopcnt-1] < 0) {
			        lastdata = typeops[opcnt-escopcnt-1];
				if (lastdata < 0) lastdata = getreg(lastdata);
			} else lastdata = ops[opcnt-escopcnt-1];
		    } else ;
		    snprint(lastdata);
		    printf("\t");
		    break;
	    case ';':
		    /* retype last expr as symbol */
		    if (help) printf("\n\r retype as symbol\r\n");
		    *scanreadptr = '\0';
		    stillopen = TRUE;
		    parseit(&pstr);
		    if (opcnt) {
			if (typeops[opcnt-escopcnt-1] < 0) {
			       lastdata = typeops[opcnt-escopcnt-1];
			       if (lastdata < 0) lastdata = getreg(lastdata);
			} else lastdata = ops[opcnt-escopcnt-1];
		    } else ;
		    if (tempmodes.relative) {
			i = matchequal;
			matchequal = 10000;
			typeout(lastdata,"  \t");
			matchequal = i;
		    } else {
			tempmodes.relative = TRUE;
			i = matchequal;
			matchequal = 10000;
			typeout(lastdata,"  \t");
			matchequal = i;
			tempmodes.relative = FALSE;
		    }
		    break;
	    default:
		    continue;
	    }
	    fflush(stdout);
	    if (!stillopen) nowopen = FALSE;
	    return;
    }
}

cancel()
/* cancel temp modes */
{
    tempmodes.relative = permmodes.relative;
    tempmodes.automat = permmodes.automat;
    tempmodes.outputmode = permmodes.outputmode;
    tempmodes.inradix = permmodes.inradix;
    tempmodes.outradix = permmodes.outradix;
    tempmodes.context = permmodes.context;
}

doesccmd(ch)
/* process a $X command */
char ch;
{
    int		i;
    char	*pstr;
    pstr = scanbuffer;
    do {
	ch = scanchar(); 
    }
    while ((ch == ',') || (ch == ESC) ||
	    ((ch >= '0') && (ch <= '9')));
    switch(ch) {
    case 'B':
		remallbpts();
		break;
    case 'b':
		*scanreadptr = '\0';
		parseit(&pstr);
		if (opcnt-escopcnt) {
		    if (typeops[opcnt-escopcnt-1] < 0) {
			   address = typeops[opcnt-escopcnt-1];
			   address = getreg(address);
		    } else {
			address = ops[opcnt-escopcnt-1];
			if (address < 0) address &= 0xffffff;
		    }
		} else if (dot < 0) {
		    address = getreg(dot);
		} else address = dot;
		if (opcnt-escopcnt) {
			makebpt(address);
		} else {
			if (escopcnt == 1) {
				rembpt(ops[opcnt-escopcnt]);
			} else makebpt(address);
		}
		break;
    case 'c':
		*scanreadptr = '\0';
		parseit(&pstr);
		if (opcnt-escopcnt) {
			docall();
		} 
		break;
    case 'G':
		while (ch != '\r') ch = scanchar();
    case 'g':	*scanreadptr = '\0';
		runit();
		break;
    case 'k':
		skipinst();
		break;
    case 'l':
		listbpt();
		break;
    case 'q':
		printf("\r\n");
	 	ddtdied();	
		break;
    case 'p':
		*scanreadptr = '\0';
		parseit(&pstr);
		if (opcnt-escopcnt) {
			i = ops[opcnt-escopcnt-1];
			while (i > 0) {
				doproceed();
				i--;
			}
		} else doproceed();
		break;
    case 'P':
		*scanreadptr = '\0';
		parseit(&pstr);
		if (opcnt-escopcnt) {
			i = ops[opcnt-escopcnt-1];
			while (i > 0) {
				if (remotemachine) donproceed();
				else doproceed();
				i--;
			}
		} else if (remotemachine) donproceed();
			else doproceed();
		break;
    case 'm':
		ch = scanchar();
		switch (ch) {
		case 'b': i = ABSNUMERIC;
			break;
		case 'c': i = CHARACTER;
			break;
		case 'f': i = FLOATING;
			break;
		case 'i': i = INSTRUCT;
			break;
		default:
		case 'n': i = NUMERIC;
			break;
		case 's': i = STRING;
			break;
		case 'm':
			*scanreadptr = '\0';
			parseit(&pstr);
			if (opcnt-escopcnt) {
			    matchequal = ops[opcnt-escopcnt-1];
			    printf("\t");
			    fflush(stdout);
			}
			return;	
			break;
		case 'r':
		case 'a':
			if (ch == 'r')
			    tempmodes.relative = TRUE;
			else tempmodes.relative = FALSE;
			*scanreadptr = '\0';
			parseit(&pstr);
			if (esctoo > 1) permmodes.relative = tempmodes.relative;
			printf("\t");
			fflush(stdout);
			return;	
			break;
		case 'A':
		case 'N':
			if (ch == 'N') tempmodes.automat = FALSE;	
			else tempmodes.automat = TRUE;
			*scanreadptr = '\0';
			parseit(&pstr);
			if (esctoo > 1) permmodes.automat = tempmodes.automat;
			printf("\t");
			fflush(stdout);
			return;	
			break;
		case 'q': skipptrace = TRUE;
			printf("\t");
			fflush(stdout);
			return;	
			break;
		case 'p': skipptrace = FALSE;
			printf("\t");
			fflush(stdout);
			return;	
			break;
		}
		tempmodes.outputmode = i;
		*scanreadptr = '\0';
		parseit(&pstr);
		if (esctoo > 1) permmodes.outputmode = i;
		printf("\t");
		display(address);
		break;
    case 'r':
		ch = scanchar();
		switch (ch) {
		case 'b': i = 2;
			break;
		case 'o': i = 8;
			break;
		case 'd': i = 10;
			break;
		default:
		case 'x': i = 16;
			break;
		}
		if (i != 2) tempmodes.inradix = i;
		tempmodes.outradix = i;
		*scanreadptr = '\0';
		parseit(&pstr);
		if (esctoo > 1) {
		    if (i != 2) permmodes.inradix = i;
		    permmodes.outradix = i;
		}
		break;
    case 's':
		printf("\r\n");
		tracestack(getreg(SFP),getreg(SPC),getreg(SMOD),TRUE);
		break;
    case 'S':
		printf("\r\n");
		tracestack(getreg(SFP),getreg(SPC),getreg(SMOD),FALSE);
		break;
    case 't':
		ch = scanchar();
		switch (ch) {
		case 'b': i = 1;
			break;
		case 'w': i = 2;
			break;
		default:
		case 'd': i = 4;
			break;
		}
		tempmodes.context = i;
		*scanreadptr = '\0';
		parseit(&pstr);
		if (esctoo > 1) permmodes.context = i;
		break;
    case 'u':
		uplevel(FALSE);
		break;
    case 'U':
		uplevel(TRUE);
		break;
    case 'n':
		/* retype last expr as  unsigned numeric */
		*scanreadptr = '\0';
		stillopen = TRUE;
		parseit(&pstr);
		if (opcnt) {
		    if (typeops[opcnt-escopcnt-1] < 0) {
			    lastdata = typeops[opcnt-escopcnt-1];
			    if (lastdata < 0) lastdata = getreg(lastdata);
		    } else lastdata = ops[opcnt-escopcnt-1];
		} else ;
		firsthex = TRUE;
		printf("\t");
		nprint(lastdata,tempmodes.outradix);
		break;
    case 'v':
		if (remotemachine) 
			switchps(TRUE);
		else printf("\tThis is a remote command\r\n");
		break;
    case 'V':
		if (remotemachine) 
			switchps(FALSE);
		else printf("\tThis is a remote command\r\n");
		break;
    case 'd':
		if (remotemachine) {
			*scanreadptr = '\0';
			parseit(&pstr);
			if (opcnt-escopcnt) {
				address = ops[opcnt-escopcnt-1];
				if (address < 0) address &= 0xffffff;
			} else address = -1;
			serialdown(address);
			address = 0;
			setreg(SR0,8);	/* mesa unix kernel checks r0 */
			/* if r0 is 8 allows remote debugging (kdebug) */
		} else printf("\tThis is a remote command\r\n");
		break;
    case 'f':
		if (remotemachine) {	
			gpibload();
			setreg(SR0,8);	/* mesa unix kernel checks r0 */
			/* if r0 is 8 allows remote debugging (kdebug) */
		}
		else printf("\tThis is a remote command\r\n");
		break;
    case 'X':
		ch = scanchar();
    case 'x':
		*scanreadptr = '\0';
		parseit(&pstr);
		if (opcnt-escopcnt) {
		    if (typeops[opcnt-escopcnt-1] < 0) {
			   address = typeops[opcnt-escopcnt-1];
			   address = getreg(address);
		    } else {
			address = ops[opcnt-escopcnt-1];
			if (address < 0) address &= 0xffffff;
		    }
		} else if (dot < 0) {
		    address = getreg(dot);
		} else address = dot;
		switch (ch) {
		case 'r':
			i = 0x10000000;
			break;
		case 'w':
			i = 0x08000000;
			break;
		case 'R':
			i = 0x18000000;
			break;
		case 'X':
			i = 0x20000000;
			break;
		case 'x':
		default: i = 0x38000000;
			break;
		}
		makemmubpt(address,i);
		break;
    case 'e':
		while (ch != '"') ch = scanchar();
		*scanreadptr = '\0';
		makeastop();
		break;
    case 'E':
		*scanreadptr = '\0';
		parseit(&pstr);
		if (opcnt) {
			i = ops[opcnt-escopcnt-1];
			if (i < 0) i = -1;
		} else i = -1;
		if (i >= MAXBPTS) {
			printf("\tbad bpt number\r\n");
		} else {
			remastop(i);
		}
		break;
    case 'L':
		liststops();
		break;
    case 'h':
printf("\r\n");
printf("\t A Quick DDT help guide                                        \r\n");
printf(" ddt has two types of commands: data display and debug functions \r\n");
printf(" the display commands are  TAB RETURN / \\ ; = ? \' 	         \r\n");
printf(" the debug functions are        ($ is ESC) ([..] is options list \r\n");
printf(" $t[bwd] sets access size to byte, word, or double               \r\n");
printf("\r\n");
printf(" $r[bodx] set input and output radix to Binary,Octal,Decimal,heX \r\n");
printf("\r\n");
printf(" $m[bcins] set output mode to aBsolute, Character, Instruction,  \r\n");
printf("\r\n");
printf(
"Numeric, and String; $mb is numeric regardless of symbolic or automatic modes\r\n");
printf(" $mr turns symbolic mode on $ma turns symbolic mode off\r\n");
printf(" $mA is Automatic mode on $mN Automatic off.                   \r\n");
printf(" Automatic is forced instruction display based on object format  \r\n");
printf(" Symbolic is symbolic matching on values for display             \r\n");
printf(" double $$ make a display mode permanent			 \r\n");
printf("\r\n");
printf(" $s is C stack display with arguments				 \r\n");
printf(" for the display commands there is either a typed address or	 \r\n");
printf(" dot(.) the last known address. so typing main/ will display	 \r\n");
printf(" the data at address main and set dot to that value.             \r\n");
printf(" so NEWLINE or \' increments dot and displays contents           \r\n");
printf(" ? decrements dot and displays contents                          \r\n");
printf(" TAB always shows address and displays if an address is typed    \r\n");
printf(" / shows address and displays if an address is typed             \r\n");
printf(" \\ is like / but doesn't change dot to address                  \r\n");
printf(" = shows last displayed thing as number in current radix         \r\n");
printf(" ; shows last displayed thing as symbol with offset if any       \r\n");
printf(" RETURN resets modes to permanent settings   	                 \r\n");
printf(" expresions can have >,< shifts; # div, % mod, @ indirection     \r\n");
printf(" + - & | ^ ~ as in c language  () as forced ordering.            \r\n");
printf(" expresions can use '.' for dots value and ` for last display    \r\n");
printf(" value. further information see DDT(1)                           \r\n");
printf("                                                                 \r\n");
printf(" debugging commands                                              \r\n");
printf(" $g starts your program with no command args     		 \r\n");
printf(" $G command args<cr> starts your program with the command args   \r\n");
printf(" $b sets bpt at current address $#b removes bpt #                \r\n");
printf(" $B removes all breakpoints                                      \r\n");
printf(" $l lists existing bpts $g runs expecting to hit a bpt           \r\n");
printf(" $k advances the pc as if the pc instruction was executed        \r\n");
printf(" this is useful for stepping over bpts.                          \r\n");
printf(" $p proceeds a program from any spot, ] single steps             \r\n");
printf(" $s is stack display with arguments from stack			 \r\n");
printf(" ] steps over cxp, bsr and jsr instructions                      \r\n");
printf(" TO QUIT use del which then asks you if you really want to       \r\n");
printf("  y or <cr> both will exit, only n will not.                     \r\n");
printf(" TO QUIT without questions use $q                                \r\n");
		break;
    default:    printf("\tno such command\r\n");
		scanabort();
		break;
    }
    printf("\t");
}

linecmd()
/* process a line command */
{
	*scanreadptr = '\0';
	printf("\n\r'l' %s\r\n",scanbuffer);
}

incrdot(pstr)
char *pstr;
{
	/* increment dot and display */
	*scanreadptr = '\0';
	parseit(&pstr);
	if (opcnt) {
	    if (typeops[opcnt-escopcnt-1] < 0) {
		    lastdata = typeops[opcnt-escopcnt-1];
		    lastdata = getreg(lastdata);
	    } else {
		    lastdata = ops[opcnt-escopcnt-1];
	    }
	    if (nowopen) {
		    if (address < 0) {
			    setreg(address,lastdata);
		    } else {
			    setmem(address,lastdata);
		    }
	    }
	}
	nowopen = stillopen = TRUE;
	if (dot < 0) {
	    if ((dot <= F0) && (dot >= FSR)) {
		    if (dot == FSR) dot = SR0;
		    else dot--;
	    } else {
		    if (remotemachine) {
			if (dot == SINTB) dot = SR0;
			else dot--;
		    } else if (dot == SBCNT) dot = SR0;
			    else dot--;
	    }
	    address = dot;
	    lastdata = getreg(dot);
	    printf("%s/\t",regstr(dot));
	    typeout(lastdata,"*\t*");
	} else {
	    dot += acontext;
	    address = dot;
	    typeout(address,"*/\t");
	    lastdata = display(address);
	    printf("\t");
	}
}

char getchr()
{
    char	ch;
    read(0, &ch, 1);
    ch = ch & 0x7f;
    if (ch == '\'') ch = '\n';
    return (ch);
}

ddtsigbus()
{
	printf("\r\nSIGBUS to ddt\r\n");
	ddtdied();
}

ddtsigsegv()
{
	printf("\r\nSIGSEGV to ddt\r\n");
	ddtdied();
}

ddtdied()
{
	stty(0, &userstty);
	exit(0);
}

ddtquit()
{
	char	ch;
	signal(SIGINT,ddtquit);
	printf("\n\rQuit (y\\n) ?");
	fflush(stdout);
	read(0, &ch, 1);
	write(1, &ch, 1);
	ch = ch & 0x7f;
	if ((ch == 'y') || (ch == '\r') || (ch == '\n')) {
		if (ch != '\r') write(1, "\r", 1);
		write(1, "\n", 1);
		stty(0, &userstty);
		exit(0);
	} else { /* reset what we can */
	    write(1, "\n", 1);
	    stty(0, &ddtstty);
	    scanreadptr = scanbuffer;
	    scanwriteptr = scanbuffer;
	    longjmp(resetbuf, USER_INT);
	}
}

ddtfloat()
{
    signal(SIGFPE,ddtfloat);
    stty(0, &ddtstty);
    scanreadptr = scanbuffer;
    scanwriteptr = scanbuffer;
    printf(" Bad float\r\n");
    longjmp(resetbuf, USER_INT);
}


ddtstop()
{
    signal(SIGTSTP,ddtstop);
    if (remotemachine) ioctl(port,TIOCNXCL,0);
    stty(0, &userstty);
    kill(0,SIGSTOP);
    stty(0, &ddtstty);
}

ddtstart()
{
    signal(SIGCONT,ddtstart);
    if (remotemachine) {
	ioctl(port,TIOCFLUSH);
	setport();
	ioctl(port,TIOCFLUSH);
    }
}
