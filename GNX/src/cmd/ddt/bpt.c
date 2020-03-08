
/*
 * bpt.c: version 1.7 of 8/29/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)bpt.c	1.7 (NSC) 8/29/83";
# endif

/* breakpoint functions */

#include <setjmp.h>
#include <stdio.h>
#include "main.h"
#include "symbol.h"
#include "display.h"
#include "error.h"
#include "bpt.h"
#include "parse.h"
#include "machine.h"

#define	INVALID -1
#define	BPR0	1
#define	BPR1	2
#define	BPT	3

#define	BPTCODE	0xf2

extern int pid;
extern int errno;
extern int mmsr;
extern int mmuactive;

struct bptentry {
int	bptat;
int	bpttype;
int	realcode;
int	bptcode;
int	sumode;
};

int firsttime = TRUE;
struct bptentry bptab[MAXBPTS];
struct bptentry tempbpt;

bptabort() 
{
	if (remotemachine) switchps(getmode(TRUE));
	longjmp(resetbuf, BPT_ERROR);
}

initbpttable()
{
int i;
	firsttime = FALSE;
	for (i=0; i < MAXBPTS; i++)
		bptab[i].bpttype = INVALID;
}

findentry()
{
int i;
	if (firsttime) initbpttable();
	for (i=0; i < MAXBPTS; i++) {
		if (bptab[i].bpttype == INVALID) return(i);
	}
	printf("\r\nall bpts used\r\n");
	bptabort();
}

findmatch(addrat,pswmode)
{
int i;
	for (i=0; i < MAXBPTS; i++) {
		if (bptab[i].bpttype == INVALID) continue;
		if (bptab[i].bptat == addrat) {
		    if (remotemachine)
			if (getmode(pswmode) != bptab[i].sumode) continue;
		    return(TRUE);
		}
	}
	return(FALSE);
}

usedmmubpt()
{
int i, x0 = 1, x1 = 1;
	for (i=0; i < MAXBPTS; i++) {
		if (bptab[i].bpttype == INVALID) continue;
		if (bptab[i].bpttype == BPR0) x0--;
		if (bptab[i].bpttype == BPR1) x1--;
	}
	if (x0) return(BPR0);
	if (x1) return(BPR1);
	printf("\tboth mmu bpts used\r\n");
	bptabort();
}

maketemp(addrat)
{
	tempbpt.bptat = addrat;
	tempbpt.bpttype = BPT;
	if (remotemachine) tempbpt.sumode = getmode(TRUE);
}

makebpt(addrat)
{
int i;
register struct bptentry *pbpt;
	i = findentry();
	if (findmatch(addrat,FALSE)) {
		    printf("\r\nbreakpoint exists already\r\n");
		    bptabort();
	}
	pbpt = &bptab[i];
	pbpt->bptat = addrat;
	pbpt->bpttype = BPT;
	if (remotemachine) pbpt->sumode = getmode(FALSE);
}

makemmubpt(addrat,btype)
{
int i, mmsr, bprn;
register struct bptentry *pbpt;
	i = findentry();
	bprn = usedmmubpt();
	pbpt = &bptab[i];
	pbpt->bptat = addrat;
	pbpt->bpttype = bprn;
	if (remotemachine) {
		pbpt->sumode = getmode(FALSE);
		mmsr = getreg(SMSR);
		if (pbpt->sumode) {
			if (!(mmsr & 0x20000))
				btype |= 0x40000000;
		} else {
			if (mmsr & 0x10000) {
				if (mmsr & 0x40000) btype |= 0x80000000;
			} else btype |= 0x40000000;
		}
		pbpt->realcode = btype;
	} else {
		pbpt->realcode = btype;
		if (bprn - 1) setreg(SBPR1,bptab[i].bptat);
		else setreg(SBPR0,bptab[i].bptat);
		pbpt->sumode = FALSE;

	}
}

rembpt(indexat)
{
	bptab[indexat].bpttype = INVALID;
	remastop(indexat);
}

remallbpts()
{
int i;
	for (i=0; i < MAXBPTS; i++)
		bptab[i].bpttype = INVALID;
	remallstops();
}

listbpt()
{
int i,themode;
	if (remotemachine) themode = getmode(FALSE);
	for (i=0; i < MAXBPTS; i++) {
		if ((bptab[i].bpttype == BPT) || (bptab[i].bpttype == BPR0) ||
			(bptab[i].bpttype == BPR1)) {
		    printf("\r\n");
		    printf("%d:\t",i);
		    typeout(bptab[i].bptat,"*/\t");
		    if (remotemachine) switchps(bptab[i].sumode);
		    display(bptab[i].bptat);
		    if (remotemachine)
			if (bptab[i].sumode) printf("\tsupr mode");
			else printf("\tuser mode");
		    if ((bptab[i].bpttype == BPR0) ||
		    	(bptab[i].bpttype == BPR1)) {
			printf("\tbpr%d",bptab[i].bpttype - 1);
			if (bptab[i].realcode & 0x20000000)
				printf(" execute");
			if (bptab[i].realcode & 0x10000000)
				printf(" read");
			if (bptab[i].realcode & 0x08000000)
				printf(" write");
			if (bptab[i].realcode & 0x04000000)
				printf(" cnt");
		    }
		}
	}
	printf("\r\n");
	if (remotemachine) switchps(themode);
}

showmsrbrk()
{
int i;
int rmsr;
	if (remotemachine) rmsr = mmsr;
	else rmsr = getreg(SMSR);
	if (remotemachine) {
	if (rmsr & 0x200) printf("\t< read");
	else printf("\t< write");
	switch ((rmsr >> 13) & 0x7) {
	case 0: printf("seq fetch");
		break;
	case 1: printf("nonseq fetch");
		break;
	case 2: printf("data transfer");
		break;
	case 3: printf("rmw operand");
		break;
	case 4: printf("effective addr");
	default: break;
	}
	} else {
	switch ((rmsr >> 13) & 0x7) {
	case 0: printf("< fetch");
		break;
	case 1: printf("< nseq fetch");
		break;
	case 2:
		if (rmsr & 0x200) printf("\t< read");
		else printf("\t< write");
		break;
	case 3: printf("< rmw");
		break;
	case 4: printf("< effective addr");
	default: break;
	}
	}
}

showbpt(addrat)
{
int i;
	for (i=0; i < MAXBPTS; i++) {
		if ((bptab[i].bpttype == BPT)||(bptab[i].bpttype == BPR0)||
			(bptab[i].bpttype == BPR1)) {
		    if (bptab[i].bptat == addrat) {
			if (remotemachine)
				if (getmode(TRUE) != bptab[i].sumode) continue;
			printf("\r\n");
			typeout(bptab[i].bptat,"*/\t");
			display(bptab[i].bptat);
			if ((bptab[i].bpttype == BPR0)
				|| (bptab[i].bpttype == BPR1)) {
				if (remotemachine)
					showmsrbrk();
				else if (getreg(SMSR) & 0x4)
					showmsrbrk();
				else printf("msr shows no trigger");
			} else printf("\t<<bpt %d",i);
			fflush(stdout);
			dostop(i);
			return;
		    }
		}
	}
	if ((getreg(SMSR) & 0x4) && !remotemachine && mmuactive) {
		for (i=0; i < MAXBPTS; i++) {
		    if (((bptab[i].bpttype == BPR0)&& !(getreg(SMSR) & 0x40)) ||
		    ((bptab[i].bpttype == BPR1)&& (getreg(SMSR) & 0x40)))
		    {
				printf("\r\n");
				typeout(getreg(SPC),"*/\t");
				display(getreg(SPC));
				showmsrbrk();
				printf(" > ");
				typeout(bptab[i].bptat,"*/\t");
				display(bptab[i].bptat);
				dostop(i);
				printf("\t");
				fflush(stdout);
				return;
			}
		}
	}
	typeout(addrat,"*/\t");
	display(addrat);
	printf("\t<<bpt");
	fflush(stdout);
}

putinbpts(istemp)
{
int i, mmsr;
register struct bptentry *pbpt;
	if (pid == -1) {
		printf("\r\nno process to put bpts in\r\n");
		bptabort();
	}
	if (istemp <= TEMP) {
		pbpt = &tempbpt;
		if ((!findmatch(pbpt->bptat,TRUE)) || (istemp != PTEMP)) {
		    if (remotemachine) switchps(pbpt->sumode);
		    pbpt->realcode = machine(RMEM,pid,pbpt->bptat,0);
		    if (errno) {
			    printf("\r\ncan not read for bpt temp\r\n");
			    bptabort();
		    }
		    pbpt->bptcode = (pbpt->realcode & 0xffffff00) | BPTCODE;
		    machine(WMEM,pid,pbpt->bptat,pbpt->bptcode);
		    if (errno) {
			    printf("\r\ncan not insert bpt temp\r\n");
			    bptabort();
		    }
		}
		if (istemp == TEMP)
			return;
	}
	for (i=0; i < MAXBPTS; i++) {
		if (bptab[i].bpttype == BPT) {
		    pbpt = &bptab[i];
		    if (remotemachine) switchps(pbpt->sumode);
		    pbpt->realcode = machine(RMEM,pid,pbpt->bptat,0);
		    if (errno) {
			    printf("\r\ncan not read for bpt %d\r\n",i);
			    bptabort();
		    }
		    pbpt->bptcode = (pbpt->realcode & 0xffffff00) | BPTCODE;
		    machine(WMEM,pid,pbpt->bptat,pbpt->bptcode);
		    if (errno) {
			    printf("\r\ncan not insert bpt %d\r\n",i);
			    bptabort();
		    }
		}
		if (bptab[i].bpttype == BPR0) {
			pbpt = &bptab[i];
			setreg(SBPR0,(pbpt->realcode | pbpt->bptat));
			if (remotemachine) {
				mmsr = getreg(SMSR);
				if (pbpt->sumode) mmsr &= 0xffdfffff;
				else mmsr |= 0x00200000;
				mmsr |= 0x00100000;
				setreg(SMSR,mmsr);
			} else {
				if (pbpt->realcode & 0x04000000) {
					setbcnt(pbpt->sumode);
				}
			}
		}
		if (bptab[i].bpttype == BPR1) {
			pbpt = &bptab[i];
			setreg(SBPR1,(pbpt->realcode | pbpt->bptat));
			if (remotemachine) {
				mmsr = getreg(SMSR);
				if (pbpt->sumode) mmsr &= 0xffdfffff;
				else mmsr |= 0x00200000;
				mmsr |= 0x00100000;
				setreg(SMSR,mmsr);
			}
		}

	}
}

outbpts(istemp)
{
int i;
register struct bptentry *pbpt;
	if (pid == -1) {
	    return;	
	}
	if (istemp <= TEMP) {
		pbpt = &tempbpt;
		if ((!findmatch(pbpt->bptat,TRUE)) || (istemp != PTEMP)) {
		    if (remotemachine) switchps(pbpt->sumode);
		    machine(WMEM,pid,pbpt->bptat,pbpt->realcode);
		    if (errno) {
			    printf("\r\ncan not remove bpt temp\r\n",i);
			    bptabort();
		    }
		}
		if (istemp == TEMP) {
		    if (remotemachine) switchps(getmode(TRUE));
		    return;
		}
	}
	for (i=0; i < MAXBPTS; i++) {
		if (bptab[i].bpttype == BPT) {
		    pbpt = &bptab[i];
		    if (remotemachine) switchps(pbpt->sumode);
		    machine(WMEM,pid,pbpt->bptat,pbpt->realcode);
		    if (errno) {
			    printf("\r\ncan not remove bpt %d\r\n",i);
			    bptabort();
		    }
		}
		if (bptab[i].bpttype == BPR0) {
			if (remotemachine)
			setreg(SMSR,(getreg(SMSR) & 0xffefffff));
			else setreg(SBPR0,bptab[i].bptat);
		}
		if (bptab[i].bpttype == BPR1) {
			if (remotemachine)
			setreg(SMSR,(getreg(SMSR) & 0xffefffff));
			else setreg(SBPR1,bptab[i].bptat);
		}
	}
	if (remotemachine) switchps(getmode(TRUE));
}

enablebcnt(val)
{
int xbpr1 = -1, i;

	for (i=0; i < MAXBPTS; i++) {
		if (bptab[i].bpttype == BPR0) {
			bptab[i].realcode |= 0x04000000;
			bptab[i].sumode = val;
			setbcnt(val);
			return;
		}
		if (bptab[i].bpttype == BPR1) xbpr1 = i;
	}
	if (xbpr1 >= 0) {
		bptab[xbpr1].bpttype = BPR0;
		bptab[xbpr1].realcode |= 0x04000000;
		bptab[xbpr1].sumode = val;
		setbcnt(val);
	}
}
