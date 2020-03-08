
/*
 * reg.c: version 1.8 of 8/29/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)reg.c	1.8 (NSC) 8/29/83";
# endif

#include <setjmp.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/reg.h>
#include "main.h"
#include "parse.h"
#include "display.h"
#include "error.h"
#include "machine.h"
#include <sys/fpu.h>
#include <sys/mmu.h>
#include <sys/panic.h>

extern int unixcore;
extern struct panic upanic;
extern struct user u;
extern char 	uspace[10+USIZE - sizeof(struct user)];

extern int pid;
extern int errno;

/* these functions will be for accessing the user's
   registers. An invalid register will cause longjmp back
   to main.
*/

regabort(regnum)
{
	longjmp(resetbuf, REG_ERROR);
}

getcpu(regn)
/* get register contents 16032 */
{
}

getmmu(regn)
/* get register contents 16082 */
{
}

setcpu(regn,val)
/* set register contents 16032 */
{
}

setmmu(regn,val)
/* set register contents 16082 */
{
}

setreg(regn,val)
/* set register contents */
{
int regvalue;
	if (regn >= 0) {
	    printf("\r\nillegal reg %s\r\n",regstr(regn));
	    regabort(regn);
	}
	if (remotemachine) {
		remoteregw(regn,val);
		return;
	}
	if (unixcore) {
		printf("\r\nNo registers writable with unix core\r\n");
		regabort(regn);
	}
	if (imageonly) {
		printf("\r\nNo registers accessable with imageonly\r\n");
		regabort(regn);
	} else {
		switch (regn) {
		case SR0:
		    regvalue = R0;
		    break;
		case SR1:
		    regvalue = R1;
		    break;
		case SR2:
		    regvalue = R2;
		    break;
		case SR3:
		    regvalue = R3;
		    break;
		case SR4:
		    regvalue = R4;
		    break;
		case SR5:
		    regvalue = R5;
		    break;
		case SR6:
		    regvalue = R6;
		    break;
		case SR7:
		    regvalue = R7;
		    break;
		case SFP:
		    regvalue = RFP;
		    break;
		case SSP:
		    regvalue = RSP;
		    break;
		case SPC:
		    regvalue = RPC;
		    break;
		case SMOD:
		    regvalue = RPSRMOD;
		    break;
		case SPSR:
		    regvalue = RPSRMOD;
		    break;
		case SBPR0:
		    if (pid < 0) return;
		    ptrace(10,pid,val,0);
		    if (errno) {
			printf("\twrite to bpr0 failed\r\n");
		    	regabort(regn);
		    }
		    return;
		    break;
		case SBPR1:
		    if (pid < 0) return;
		    ptrace(10,pid,val,1);
		    if (errno) {
			printf("\twrite to bpr1 failed\r\n");
		    	regabort(regn);
		    }
		    return;
		    break;
		case SBCNT:
		    enablebcnt(val);
		    return;
		    break;
		case SSC0:
		case SPF0:
		case SPF1:
		case SEIA:
		case SMSR:
		default:
		    if ((regn <= F0) && (regn >= FSR))
			regvalue = regn;
		    else {
			printf("\tillegal reg %s\r\n",regstr(regn));
			regabort(regn);
		    }
		    break;
		}
		if (pid == -1) {
		    printf("\r\nno process running\r\n");
		    regabort(regn);
		}
		if (regn == SPSR) {
		    val = (val << 16) | getreg(SMOD);
		} else if (regn == SMOD) {
		    val = (getreg(SPSR) << 16) | (val & 0x0000ffff);
		}
		machine(WREG,pid,regvalue,val);
		if (errno) {
		    printf("\r\nwrite of register failed\r\n");
		    regabort(regn);
		}
	}
}

getreg(regn)
/* get register contents */
{
int regvalue;
	if (regn >= 0) {
	    printf("\r\nillegal reg %s\r\n",regstr(regn));
	    regabort(regn);
	}
	if (remotemachine) {
		return(remoteregr(regn));
	}
	if (unixcore) {
		return(unixreg(regn));
	}
	if (corefd > 0) {
			switch (regn) {
			case SR0:
			    regvalue = u.u_ar0[R0];
			    break;
			case SR1:
			    regvalue = u.u_ar0[R1];
			    break;
			case SR2:
			    regvalue = u.u_ar0[R2];
			    break;
			case SR3:
			    regvalue = u.u_ar0[R3];
			    break;
			case SR4:
			    regvalue = u.u_ar0[R4];
			    break;
			case SR5:
			    regvalue = u.u_ar0[R5];
			    break;
			case SR6:
			    regvalue = u.u_ar0[R6];
			    break;
			case SR7:
			    regvalue = u.u_ar0[R7];
			    break;
			case SFP:
			    regvalue = u.u_ar0[RFP];
			    break;
			case SSP:
			    regvalue = u.u_ar0[RSP];
			    break;
			case SPC:
			    regvalue = u.u_ar0[RPC];
			    break;
			case SMOD:
			    regvalue = (u.u_ar0[RPSRMOD]) & 0x0000ffff;
			    break;
			case SPSR:
			    regvalue = (u.u_ar0[RPSRMOD] >> 16) & 0x0000ffff;
			    break;
			case SEIA:
			    regvalue = u.u_eiareg & 0xffffff;
			    break;
			case SMSR:
			    regvalue = u.u_msrreg;
			    break;
			case SBPR0:
			    regvalue = u.u_bpr0;
			    break;
			case SBPR1:
			    regvalue = u.u_bpr1;
			    break;
			case SBCNT:
			    regvalue = u.u_bcnt;
			    break;
			case SPF0:
			    regvalue = u.u_pf0;
			    break;
			case SPF1:
			    regvalue = u.u_pf1;
			    break;
			case SSC0:
			    regvalue = u.u_sc;
			    break;
			default:
			    if ((regn <= F0) && (regn >= FSR)) {
				if (regn == FSR) regvalue = u.u_fsr;
				else regvalue = u.u_fregs[F0 - regn];
			    } else {
				printf("\r\nthis reg is not in core file\r\n");
				regabort(regn);
			    }
			    break;
			}
	} else if (imageonly) {
		printf("\r\nNo registers accessable with imageonly\r\n");
		regabort(regn);
	} else {
		switch (regn) {
		case SR0:
		    regvalue = R0;
		    break;
		case SR1:
		    regvalue = R1;
		    break;
		case SR2:
		    regvalue = R2;
		    break;
		case SR3:
		    regvalue = R3;
		    break;
		case SR4:
		    regvalue = R4;
		    break;
		case SR5:
		    regvalue = R5;
		    break;
		case SR6:
		    regvalue = R6;
		    break;
		case SR7:
		    regvalue = R7;
		    break;
		case SFP:
		    regvalue = RFP;
		    break;
		case SSP:
		    regvalue = RSP;
		    break;
		case SPC:
		    regvalue = RPC;
		    break;
		case SMOD:
		    regvalue = RPSRMOD;
		    break;
		case SPSR:
		    regvalue = RPSRMOD;
		    break;
		case SEIA:
		    regvalue = UEIA;
		    break;
		case SMSR:
		    regvalue = UMSR;
		    break;
		case SBPR0:
		    regvalue = (int)&u.u_bpr0 - (int)&u;
		    break;
		case SBPR1:
		    regvalue = (int)&u.u_bpr1 - (int)&u;
		    break;
		case SPF0:
		    regvalue = (int)&u.u_pf0 - (int)&u;
		    break;
		case SPF1:
		    regvalue = (int)&u.u_pf1 - (int)&u;
		    break;
		case SSC0:
		    regvalue = (int)&u.u_sc - (int)&u;
		    break;
		case SBCNT:
		    if (pid < 0) return(0);
		    regvalue = (int)&u.u_bcnt - (int)&u;
		    break;
		default:
		    if ((regn <= F0) && (regn >= FSR))
			regvalue = regn;
		    else {
			printf("\r\nillegal reg %s\r\n",regstr(regn));
			regabort(regn);
		    }
		    break;
		}
		if (pid == -1) {
		    printf("\r\nno process running\r\n");
		    regabort(regn);
		}
		regvalue = machine(RREG,pid,regvalue,0);
		if (errno) {
		    printf("\r\nread of register failed\r\n");
		    regabort(regn);
		}
		if (regn == SPSR) {
		    regvalue = (regvalue >> 16) & 0x0000ffff;
		} else if (regn == SMOD) {
		    regvalue = (regvalue) & 0x0000ffff;
		} else if (regn == SEIA) {
		    regvalue = regvalue & 0xffffff;
		}
	}
	return(regvalue);
}

unixreg(regn)
{
int regvalue;
	switch (regn) {
	case SR0:
	    regvalue = upanic.ps_gen[0];
	    break;
	case SR1:
	    regvalue = upanic.ps_gen[1];
	    break;
	case SR2:
	    regvalue = upanic.ps_gen[2];
	    break;
	case SR3:
	    regvalue = upanic.ps_gen[3];
	    break;
	case SR4:
	    regvalue = upanic.ps_gen[4];
	    break;
	case SR5:
	    regvalue = upanic.ps_gen[5];
	    break;
	case SR6:
	    regvalue = upanic.ps_gen[6];
	    break;
	case SR7:
	    regvalue = upanic.ps_gen[7];
	    break;
	case SFP:
	    regvalue = upanic.ps_fp;
	    break;
	case SSP:
	    regvalue = upanic.ps_sp;
	    break;
	case SPC:
	    regvalue = upanic.ps_pc;
	    break;
	case SMOD:
	    regvalue = upanic.ps_psrmod & 0xffff;
	    break;
	case SPSR:
	    regvalue = upanic.ps_psrmod >> 16;
	    break;
	case SEIA:
	    regvalue = upanic.ps_eia;
	    break;
	case SMSR:
	    regvalue = upanic.ps_msr;
	    break;
	case SPTB0:
	    regvalue = upanic.ps_ptb0;
	    break;
	case SPTB1:
	    regvalue = upanic.ps_ptb1;
	    break;
	case SINTB:
	    regvalue = upanic.ps_intbase;
	    break;
	default:
		printf("\r\nillegal reg %s\r\n",regstr(regn));
		regabort(regn);
	    break;
	}
	return(regvalue);
}


setbcnt(val)
{
    if (pid < 0) return;
    ptrace(11,pid,val,0);
    if (errno) {
	printf("\twrite to bcnt failed\r\n");
	regabort(SBCNT);
    }
}
