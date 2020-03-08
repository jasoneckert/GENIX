
/*
 * process.c: version 1.12 of 8/29/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)process.c	1.12 (NSC) 8/29/83";
# endif


/* process control and execution functions */

#include <setjmp.h>
#include <stdio.h>
#include <signal.h>
#include <sys/reg.h>
#include "main.h"
#include "symbol.h"
#include "display.h"
#include "parse.h"
#include "error.h"
#include "scan.h"
#include "bpt.h"
#include "machine.h"

extern char **environ;
extern int errno;

int	pid = -1;
int	lastsig = 0;
int	realsig = 0;
int	mmuactive = FALSE;
int	mmsr = 0;


processabort()
{
	mmuactive = FALSE;
	longjmp(resetbuf, PROC_ERROR);
}

run()
{
int wrst, wstatus;
	realsig = lastsig = 0;
	if (pid != -1) {
		machine(KILL,pid,0,0);
		pid = -1;
	}
	startup();
	if (pid != -1) {
	    for (wrst=0; (wrst != pid);) {
		wrst = waitfor(&wstatus);
		if (wrst == -1) {
			pid = -1;
			stty(0, &ddtstty);
			printf("\tprocess died\r\n");
			processabort();
		} 
	    }
	    stty(0, &ddtstty);
	    seesig(wstatus);
	    if (lastsig == SIGTRAP) lastsig = realsig;
	    if (pid == -1) processabort();
	} else {
		printf("\tfork failed\n\r");
		processabort();
	}
	if (!remotemachine) setreg(SBPR0,0);
}

runit()
{
    getexecargs();
    run();
    dogo();
}

dostep(inbpts) 
{
	if (pid == -1) run();
	proceed(STEP,inbpts);
	showspot();
	dostop(-1);
}

dostepover(inbpts) 
{
	int pcis,instr,instr1,instr2,instlen;
	mmuactive = FALSE;
	pcis = getreg(SPC);
	instr = getword(pcis);
	instr1 = instr & 0xff;
	instr2 = instr & 0x07fc;
	if ((instr1 == 0x22) || (instr1 == 0x02) ||
	     (instr2 == 0x67c) || (instr2 == 0x7c)) {
	     if (disasm(pcis,&instlen,FALSE)) {
		 maketemp(pcis+instlen);
		 putinbpts(TEMP);
		 proceed(GOGO,inbpts);
		 outbpts(TEMP);
		 if (lastsig == SIGBPT) lastsig = SIGTRAP;
	     } else proceed(STEP,inbpts);
	     showspot();
	     dostop(-1);
	} else {
	    if (remotemachine)
		if ((instr1 == 0x42) || (instr1 == 0x52)) {
			steprett();
		}
	    proceed(STEP,inbpts);
	    showspot();
	    dostop(-1);
	}
}

uplevel(perm)
{
int framepc;
	mmuactive  = FALSE;
	framepc = getdouble(getreg(SFP)+4);
	if (perm == TRUE)
	    makebpt(framepc);
	else {
	    maketemp(framepc);
	    putinbpts(TEMP);
	    proceed(GOGO,FALSE);
	    outbpts(TEMP);
	    if (lastsig == SIGBPT) lastsig = SIGTRAP;
	    showspot();
	    dostop(-1);
	}
}

showspot() 
{
	int pcis;
	if (pid == -1) {
		printf("\tprocess gone\r\n");
		return;
	}
	if (lastsig == SIGTRAP) {
		lastsig = realsig;
		pcis = getreg(SPC);
		dot = pcis;
		address = dot;
		printf("\r\n");
		typeout(address,"*/\t");
		lastdata = display(address);
		printf("\t");
		fflush(stdout);
	} else if (lastsig == SIGBPT) {
		lastsig = realsig;
		pcis = getreg(SPC);
		dot = pcis;
		address = dot;
		if (mmuactive) showbpt(-1);
		else showbpt(pcis);
		printf("\t");
		fflush(stdout);
	} else {
		pcis = getreg(SPC);
		dot = pcis;
		address = dot;
		printf("\r\n");
		typeout(address,"*/\t");
		lastdata = display(address);
		printf("\t");
		fflush(stdout);
	}
}

dogo() 
{
	mmuactive = FALSE;
	proceed(GOGO,TRUE);
	showspot();
	dostop(-1);
}

doproceed() 
{
	mmuactive = FALSE;
	if (lastsig) {
	    if (!remotemachine) {
		    printf("\r\nproceeding with ");
		    printsig(lastsig);
	    }
	}
	proceed(STEP,FALSE);
	if (lastsig == SIGTRAP) {
	    lastsig = realsig;
	    proceed(GOGO,TRUE);
	}
	showspot();
}

donproceed() 
{
	mmuactive = FALSE;
	if (lastsig) {
	    if (!remotemachine) {
		    printf("\r\nproceeding with ");
		    printsig(lastsig);
	    }
	}
	if (getmode(TRUE)) {
		setreg(SMSR,(getreg(SMSR) & 0xfeefffff));
	} else {
		setreg(SMSR,(getreg(SMSR) | 0x01000000));
	}
	proceed(STEP,FALSE);
	if (lastsig == SIGTRAP) {
	    lastsig = realsig;
	    mmuactive = TRUE;
	    proceed(GONT,FALSE);
	    setreg(SMSR,(getreg(SMSR) & 0xfdffffff));
	}
	showspot();
	dostop(-1);
	mmuactive = FALSE;
}

proceed(prop,dobpts)
{
int wrst, wstatus;
	if (pid != -1) {
	    if (dobpts) {
		putinbpts(0);
	    }
	    if (prop == GOGO) {
		if (!remotemachine) stty(0, &userstty);
		printf("\n");
	    }
	    machine(prop,pid,1,lastsig);
	    if (errno) {
		    printf("\r\nproceed failed\r\n");
		    if (prop == GOGO) {
			stty(0, &ddtstty);
		    }
		    processabort();
	    }
	    for (wrst=0; (wrst != pid);) {
		wrst = waitfor(&wstatus);
		if (wrst == -1) {
			machine(KILL,pid,0,0);
			pid = -1;
			if (prop == GOGO) {
			    stty(0, &ddtstty);
			}
			printf("\r\nproceed failed\r\n");
			processabort();
		}
	    }
	    if (prop == GOGO) {
		stty(0, &ddtstty);
	    }
	    seesig(wstatus);
	    if (dobpts && (pid != -1)) outbpts(0);
	    if (pid == -1) processabort();
	} else {
		printf("\n\rno process to proceed\n\r");
		processabort();
	}
}

seesig(thesig)
{
	if (!remotemachine) {
	    if ((thesig & 0x7f) != 0x7f) {
		machine(RREG,pid,R0,0);
		if (errno) pid = -1;
		printsig(thesig & 0x7f);
		if (thesig & 0x8000) {
			printf("\r\ncore dump\r\n");
			close(corefd);
			corefd = open("core",0);
			if (corefd > 0) getudot();
		}
		printf("\r\nprocess exits status %x\r\n",((thesig & 0x7fff) >> 8));
		return;
	    }
	}
	lastsig = (thesig & 0x7fff) >> 8;
	if (remotemachine) {
		if (lastsig == SIGNMI) {
			mmsr = getreg(SMSR);
			if (mmsr & 4) {
				mmuactive = TRUE;
				lastsig = SIGBPT;
			} else if (mmuactive) lastsig = SIGTRAP;
				else mmuactive = FALSE;
		} else mmuactive = FALSE;
	} else {
		if (getreg(SMSR) & 0x4)
			if (lastsig == SIGBPT)
				mmuactive = TRUE;
		else mmuactive = FALSE;
	}
	if ((lastsig != SIGBPT) && (lastsig != SIGTRAP))
		realsig = lastsig;
	printsig(lastsig);

}

printsig(thesig)
{
    if (remotemachine) {
	switch (thesig) {
	case 0:
	case SIGBPT:
	case SIGTRAP:
		return;
		break;
	case SIGINT:
		printf("\r\nTrap(INT)\r\n");
		break;
	case SIGILL:
		printf("\r\nTrap(ILL)\r\n");
		break;
	case SIGIOT:
		printf("\r\nTrap(FLG)\r\n");
		break;
	case SIGEMT:
		printf("\r\nTrap(UND)\r\n");
		break;
	case SIGFPE:
		printf("\r\nTrap(FPU)\r\n");
		break;
	case SIGBUS:
		printf("\r\nTrap(ABT)\r\n");
		break;
	case SIGSYS:
		printf("\r\nTrap(SVC)\r\n");
		break;
	case SIGDVZ:
		printf("\r\nTrap(DVZ)\r\n");
		break;
	case SIGNMI:
		printf("\r\nTrap(NMI)\r\n");
		break;
	case SIGKILL:
	default:
		printf("\r\nUnknown Trap\r\n");
		break;
	}
    } else {
	switch (thesig) {
	case 0:
	case SIGBPT:
	case SIGTRAP:
		return;
		break;
	case SIGHUP:
		printf("\r\nSIGHUP \r\n");
		break;
	case SIGINT:
		printf("\r\nSIGINT \r\n");
		break;
	case SIGQUIT:
		printf("\r\nSIGQUIT\r\n");
		break;
	case SIGILL:
		printf("\r\nSIGILL \r\n");
		break;
	case SIGIOT:
		printf("\r\nSIGIOT \r\n");
		break;
	case SIGEMT:
		printf("\r\nSIGEMT \r\n");
		break;
	case SIGFPE:
		printf("\r\nSIGFPE \r\n");
		break;
	case SIGKILL:
		printf("\r\nSIGKILL\r\n");
		break;
	case SIGBUS:
		printf("\r\nSIGBUS \r\n");
		break;
	case SIGSEGV:
		printf("\r\nSIGSEGV\r\n");
		break;
	case SIGSYS:
		printf("\r\nSIGSYS \r\n");
		break;
	case SIGPIPE:
		printf("\r\nSIGPIPE\r\n");
		break;
	case SIGALRM:
		printf("\r\nSIGALRM\r\n");
		break;
	case SIGTERM:
		printf("\r\nSIGTERM\r\n");
		break;
	case SIGSTOP:
		printf("\r\nSIGSTOP\r\n");
		break;
	case SIGTSTP:
		printf("\r\nSIGTSTP\r\n");
		break;
	case SIGCONT:
		printf("\r\nSIGCONT\r\n");
		break;
	case SIGCHLD:
		printf("\r\nSIGCHLD\r\n");
		break;
	case SIGTTIN:
		printf("\r\nSIGTTIN\r\n");
		break;
	case SIGTTOU:
		printf("\r\nSIGTTOU\r\n");
		break;
	case SIGTINT:
		printf("\r\nSIGTINT\r\n");
		break;
	case SIGXCPU:
		printf("\r\nSIGXCPU\r\n");
		break;
	case SIGXFSZ:
		printf("\r\nSIGXFSZ\r\n");
		break;
	case SIGDVZ:
		printf("\r\nSIGDVZ \r\n");
		break;
	case SIGNMI:
		printf("\r\nSIGNMI \r\n");
		break;
	case SIGPROF:
		printf("\r\nSIGPROF \r\n");
		break;
	default: printf("\r\nSIG #%x\r\n",thesig);
		break;
	}
    }
	fflush(stdout);
}

