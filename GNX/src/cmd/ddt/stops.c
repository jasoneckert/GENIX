
/*
 * stops.c: version 1.3 of 7/21/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)stops.c	1.3 (NSC) 7/21/83";
# endif

/* stop string functions */

#include <setjmp.h>
#include <stdio.h>
#include "main.h"
#include "symbol.h"
#include "scan.h"
#include "display.h"
#include "error.h"
#include "bpt.h"
#include "parse.h"
#include "machine.h"

#define	ANYSTOP -1

struct sbox {
	int	active;
	char	scmds[MAXSTRLEN];
};

struct sbox anystop;		/* do on any stop */
struct sbox bptstp[MAXBPTS];	/* for breakpoints */

initstops()
{
	anystop.active = FALSE;
	remallstops();
}

remallstops()
{
int i;
	for (i = 0; i < MAXBPTS; i++)
		bptstp[i].active = FALSE;
}

remastop(n)
{
	if (n == ANYSTOP) anystop.active = FALSE;
	else 
		bptstp[n].active = FALSE;
}

stopprint(xstr)
char *xstr;
{
	while (*xstr != '\0') {
		     if (*xstr == ESC)  printf("$");
		else if (*xstr == '\n') printf("\'");
		else if (*xstr == '\r') printf(" cr ");
		else if (*xstr == '\t') printf(" tab ");
		else printf("%c",*xstr);
		xstr++;
	}
	printf("\r\n");
}

liststops()
{
int i;
	printf("\r\n");
	if (anystop.active) {
		printf("T:\t");
		stopprint(anystop.scmds);
	}
	for (i = 0; i < MAXBPTS; i++)
		if (bptstp[i].active)  {
			printf("%d:\t",i);
			stopprint(bptstp[i].scmds);
		}
}

makeastop()
{
char *cptr = scanbuffer;
char tstr[MAXSTRLEN];
char *tptr = tstr;
struct sbox *sptr;
int i = 0;
	do {
		if (*cptr == 0) {
			printf("\tbad string\r\n");
			scanabort();
		}
		cptr++;
	} while (*cptr != 'e');
	cptr++;
	do {
		if (*cptr == 0) {
			printf("\tbad string\r\n");
			scanabort();
		}
		if (i >= MAXSTRLEN) {
			printf("\tstring too long\r\n");
			scanabort();
		}
		*tptr++ = *cptr++;
		i++;
	} while (*cptr != '"');
	if (i == MAXSTRLEN) {
		printf("\tstring too long\r\n");
		scanabort();
	}
	*tptr++ = '\0';
	cptr = scanbuffer;
	parseit(&cptr);
	if (opcnt) {
		i = ops[opcnt-escopcnt-1];
		if (i < 0) i = -1;
	} else i = -1;
	if (i >= MAXBPTS) {
		printf("\tbad bpt number\r\n");
		scanabort();
	}
	if (i == ANYSTOP) sptr = &anystop;
	else 
		sptr = &bptstp[i];
	tptr = tstr;
	for (i = 0; *tptr != '\0'; i++)
		sptr->scmds[i] = *tptr++;
	sptr->scmds[i] = '\0';
	sptr->active = TRUE;
}

jmp_buf	stopbuf;
extern char promptstr[];
extern int stillopen, nowopen;
extern char	sgetchr(), getchr();

passstops()
{
int errorflg;
    while (1) {
	errorflg = setjmp(stopbuf);
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
	case SCAN_EDIT:
	default:
		printf("\r\nstop string error\r\n");
	case STP_ERROR:
		nowopen = FALSE;
		stillopen = FALSE;
		return;
	case NO_ERRORS:
		stillopen = FALSE;
		scaninit(sgetchr, promptstr, stopbuf);
		break;
	}
	getcmd();
    }
}

struct sbox *dptr;
int	dchar;

char sgetchr()
{
    char ch;
    ch = dptr->scmds[dchar];
    if (ch == '\0') {
	    longjmp(stopbuf, STP_ERROR);
    } else {
	    dchar++;
	    return(ch);
    }
}

dostop(n)
{
int i = 0,errorflg;
	if (n == ANYSTOP) dptr = &anystop;
	else dptr = &bptstp[n];
	if (dptr->active) {
		printf(" ");
		fflush(stdout);
		dchar = 0;
		passstops();
		scaninit(getchr, promptstr, resetbuf);
	}
}

