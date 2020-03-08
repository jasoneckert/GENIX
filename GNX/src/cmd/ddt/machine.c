
/*
 * machine.c: version 1.25 of 10/6/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)machine.c	1.25 (NSC) 10/6/83";
# endif

/* machine interface: local(use ptrace syscall) or remote(use remote monitor) */

#include <setjmp.h>
#include <sgtty.h>
#include <stdio.h>
#include <signal.h>
#include "main.h"
#include "symbol.h"
#include "display.h"
#include "parse.h"
#include "error.h"
#include "scan.h"
#include "bpt.h"
#include "machine.h"

#define MAXARGS 15
#define MAXLINE 120

extern char **environ;
extern int errno;
extern int pid;

/* rdt dependent stuff for commands */
#define PRINTSIZE 13
#define CHANGESIZE 22
#define CPUPSIZE 5
#define CPUCSIZE 14
#define MMUPSIZE 4
#define MMUCSIZE 13
#define gocmdsize 3
#define ntcmdsize 3
#define stepcmdsize 3
char gocmd[] = "@g\r";
char ntcmd[] = "@q\r";
char stepcmd[] = "@s\r";
char changecmd[] = "@cvd%08x=%08x\r";
char printcmd[] = "@pvd%08x\r";
char nommuchangecmd[] = "@cmd%08x=%08x\r";
char nommuprintcmd[] = "@pmd%08x\r";
struct rdtregs {
	char *printreg;
	char *changereg;
} rdtreg[] = {
	"@pr0\r",	"@cr0=%08x\r",
	"@pr1\r",	"@cr1=%08x\r",
	"@pr2\r",	"@cr2=%08x\r",
	"@pr3\r",	"@cr3=%08x\r",
	"@pr4\r",	"@cr4=%08x\r",
	"@pr5\r",	"@cr5=%08x\r",
	"@pr6\r",	"@cr6=%08x\r",
	"@pr7\r",	"@cr7=%08x\r",
	"@pfp\r",	"@cfp=%08x\r",
	"@pis\r",	"@cis=%08x\r",
	"@ppc\r",	"@cpc=%08x\r",
	"@pps\r",	"@cps=%08x\r",
	"@pmo\r",	"@cmo=%08x\r",
	"@ra\r",	"@wa=%08x\r",
	"@rf\r",	"@wf=%08x\r",
	"@r4\r",	"@w4=%08x\r",
	"@r5\r",	"@w5=%08x\r",
	"@r8\r",	"@w8=%08x\r",
	"@r0\r",	"@w0=%08x\r",
	"@r1\r",	"@w1=%08x\r",
	"@rb\r",	"@wb=%08x\r",
	"@rc\r",	"@wc=%08x\r",
	"@rd\r",	"@wd=%08x\r",
	"@pin\r",	"@cin=%08x\r"
};

struct {
	char *printreg;
	char *changereg;
} spreg = {
	"@psp\r",	"@csp=%08x\r"
};

int fswsize = 10;
char *sfsw = "@cfs=0000\r";
char *ufsw = "@cfs=ffff\r";
int gfswsize = 5;
char *gfsw = "@pfs\r";
char clrcmd[] = "@f %08x %08x 00\r";


char *envname = "DDTPORT";
int port = -1;  /* file to remote machine monitor */
int initopen = TRUE;
int linewait = 4; /* wait this many seconds before timeout */

char *inlineptr;
char inline[MAXLINE];
int lastvalue;
int lasterr;
char *fileptr[MAXARGS] = { imagename, 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
char *inefile;	/* input stdin file */	
char *outefile;	/* output stdout file */	

jmp_buf	retrybuf;	/* jump back through here for retry */
int	portrun = FALSE;

machineabort()
{
	alarm(0);
	portrun = FALSE;
	flushline(TRUE);
	longjmp(resetbuf, MAC_ERROR);
}

retry()
{
	alarm(0);
	longjmp(retrybuf, 1);
}

lineout()
{
	printf("\ttimeout\r\n");
	if (initopen) ddtdied();
	else machineabort();
}

setalarm(n)
{
	signal(SIGALRM,lineout);
	alarm(n);
}

setport() {
struct sgttyb portconfig;
	portconfig.sg_ispeed = portconfig.sg_ospeed = B9600;
	portconfig.sg_erase = portconfig.sg_kill = 0;
	portconfig.sg_flags = (EVENP|ODDP);
	stty(port,&portconfig);
	ioctl(port,TIOCEXCL,0);
}

attach()
{
int nchars;
char *portname;

	if (port == -1) {
		portname = (char *) getenv(envname);
		if (portname == 0) {
			printf("no %s defined\r\n",envname);
			ddtdied();
		}
		printf("opening %s\r\n",portname);
		port = open(portname,2);
		if (port < 0) {
			printf("open of %s failed\r\n",portname);
			ddtdied();
		}
		setport();
	}
	do {
		ioctl(port,FIONREAD,&nchars);
		if (nchars > MAXLINE) nchars = MAXLINE;
		if (nchars) read(port,inline,nchars);
	} while (nchars);
	inline[0] = '\r';
	write(port,inline,1);
	setalarm(linewait);
	while (1) {
		ioctl(port,FIONREAD,&nchars);
		if (nchars) {
			if (nchars > MAXLINE) nchars = MAXLINE;
			read(port,inline,nchars);
			alarm(0);
			break;
		}
		
	}

}

initexec()
{
int i = 0;
	if (remotemachine) {
		attach();
		pid = port;
		initopen = FALSE;
		flushline(TRUE);
	} else {
		fileptr[i] = imagename;
		for (i = 1; i < MAXARGS; i++) {
			if (fileptr[i] != 0) {
			    free(fileptr[i]);
			    fileptr[i] = 0;
			}
		}
	}
}

getexecargs() 
{
char *cptr = scanbuffer;
char *argbegin;
char inoutfile = 0;
int i = 1, xsize = 0;
	inefile = 0;
	outefile = 0;
	if (*cptr == 0) {
		printf("\tbad arg scan\r\n");
		processabort();
	}
	do {
		cptr++;
		if (*cptr == 0) {
			printf("\tbad arg scan\r\n");
			processabort();
		}
		if (*cptr == 'g') return;
	} while (*cptr != 'G');
	cptr++;
	if ((*cptr == '\r') || (*cptr == '\n')) return;
	do {
	    if (*cptr == 0) break;
	    while (*cptr == ' ') cptr++;
	    if ((*cptr == '\r') || (*cptr == '\n')) break;
	    if ((*cptr == '<') || (*cptr == '>')) {
		inoutfile = *cptr;
		cptr++;
		if (*cptr == 0) break;
		while (*cptr == ' ') cptr++;
		if ((*cptr == '\r') || (*cptr == '\n')) break;
	    }
	    argbegin = cptr;
	    while ((*cptr != ' ') && (*cptr != '\r') && (*cptr != 0)
		   && (*cptr != '\n')) cptr++;
	    if ((*cptr == '\r') || (*cptr == 0) || (*cptr == '\n'))
	   			*(cptr+1) = '\r';
	    *cptr = '\0';
	    cptr++;
	    xsize = strlen(argbegin);
	    if (xsize <= 0) break;
	    if (inoutfile != 0) {
		if (inoutfile == '<') {
		    if (inefile != 0) free(inefile);
		    inefile = (char *) malloc((xsize + 1));
		    strcpy(inefile,argbegin);
		} else {
		    if (outefile != 0) free(outefile);
		    outefile = (char *) malloc((xsize + 1));
		    strcpy(outefile,argbegin);
		}
		inoutfile = 0;
	    } else {
		if (fileptr[i] != 0) free(fileptr[i]);
		fileptr[i] = (char *) malloc((xsize + 1));
		strcpy(fileptr[i],argbegin);
		i++;
	    }
	} while ((*cptr != '\r') && (*cptr != '\n'));
	for (; i < MAXARGS; i++) {
		if (fileptr[i] != 0) {
		    free(fileptr[i]);
		    fileptr[i] = 0;
		}
	}
}

printargs()
{
int i;
    printf("\r\n");
    for (i=0; i < MAXARGS; i++) {
	if (fileptr[i] == 0) continue;
	printf("%s ",fileptr[i]);
    }
    printf("\r\n");
}

startup()
{
int i;
	if (remotemachine) {
		pid = port;
		lasterr = SIGTRAP;
		attach();
	} else {
		printargs();
		if ((pid = fork()) == 0) {
			close(imagefd);
			stty(0, &userstty);
			if (inefile != 0) {
				close(0);
				i = open(inefile,0);
				if (i < 0) {
				    printf("can't open %s\r\n",inefile);
				    exit(i);
				}
			}
			if (outefile != 0) {
				close(1);
				i = creat(outefile,0666);
				if (i < 0) {
				    printf("can't creat %s\r\n",outefile);
				    exit(i);
				}
			}
			machine(INIT,0,0,0);
			execvp(imagename,fileptr);
			perror("exec fails");
			exit(0);
		}
	}
}

waitfor(waitrst)
int *waitrst;
{
	if (remotemachine) {
		*waitrst = ((lasterr << 8) | 0x7f);
		return(pid);
	} else {
		return(wait(waitrst));
	}
}

machine(operation, processid, location, value)
{
int dotwo = TRUE;
	setjmp(retrybuf);
overagain:
	portrun = FALSE;
	if (remotemachine) {
		switch(operation) {
		case INIT:
			/* maybe download someday */
			lasterr = 0;
			errno = 0;
			break;
		case RMEM:
			if (remoteget(location)) {
				if (dotwo) {
					dotwo = FALSE;
					goto overagain;
				}
				printf("\tremoteget fails\r\n");
				machineabort();
			} else errno = 0;
			return(lastvalue);
			break;
		case WMEM:
			if (remoteput(location,value)) {
				if (dotwo) {
					dotwo = FALSE;
					goto overagain;
				}
				printf("\tremoteput fails\r\n");
				machineabort();
			} else errno = 0;
			break;
		case STEP:
			portrun = TRUE;
			if (remotestep()) {
				printf("\tremotestep fails\r\n");
				machineabort();
			} else errno = 0;
			break;
		case GOGO:
			printf("\r"); /* fix missing cr in process */
			/* process.c checked out so put here ARGHHH!! */
			portrun = TRUE;
			if (remotego()) {
				printf("\tremotego fails\r\n");
				machineabort();
			} else errno = 0;
			break;
		case GONT:
			portrun = TRUE;
			if (remotent()) {
				printf("\tremotenstep fails\r\n");
				machineabort();
			} else errno = 0;
			break;
		case KILL:
			errno = 0;
			break;
		default:
			printf("\tremote function not here\r\n");
			machineabort();
		}
	} else {
		return(ptrace(operation,processid,location,value));
	}
}

flushline(realflush)
{
	if (realflush) {
		ioctl(port,TIOCFLUSH);
		do {
			emptyline(TRUE);
		} while (*inlineptr != '\0');
	} else {
		inline[0] = '\0';
		inlineptr = &inline[0];
	}
}

emptyline(ignoreoverflow)
{
int nchars = 0;
	inline[0] = '\0';
	inlineptr = &inline[0];
	ioctl(port,FIONREAD,&nchars);
	if (nchars == 0) return;
	else if (nchars > MAXLINE) {
		nchars = MAXLINE;
	}
	nchars = read(port,inline,nchars);
	if (nchars < 0) {
		printf("\tread from port fails\r\n");
		machineabort();
	} else if (nchars) inline[nchars] = '\0';
	else inline[0] = '\0';
}

waiton(setbomb)
{
char inchar;
int startinput,userinput = 0;
	if (setbomb) setalarm(linewait);
	else ioctl(0,FIONREAD,&startinput);
	do {
		emptyline(FALSE);
		if (!setbomb) {
			ioctl(0,FIONREAD,&userinput);
			while (userinput > startinput) {
				if (read(0,&inchar,1) <= 0) machineabort();
				if (write(port,&inchar,1) <= 0) machineabort();
				ioctl(0,FIONREAD,&userinput);
			}
		}
	} while (*inlineptr == '\0');
	if (setbomb) alarm(0);
}

getstar(setbomb)
{
	lookagain:
	do {
		if (*inlineptr == '\0') waiton(setbomb);
		while ((*inlineptr == '\r')||(*inlineptr == '\n')) inlineptr++;
		if ((*inlineptr == '\0') && portrun) printf("%s",inline);
	} while (*inlineptr == '\0');
	switch (*inlineptr) {
	case '?':
		inlineptr++;
		retry();
	case '*':
		inlineptr++;
		while ((*inlineptr == '\r')||(*inlineptr == '\n')) inlineptr++;
		return(STAR);
	case 'E':
		if (*(inlineptr+1) != ' ') {
			goto dumpstr;
		}
		if ((*(inlineptr+2) < 'A')||(*(inlineptr+2) > 'U')) {
			goto dumpstr;
		}
		inlineptr += 2;
		switch (*inlineptr) {
		case 'A':
			lasterr = SIGBUS;
			break;
		case 'B':
			lasterr = SIGBPT;
			break;
		case 'D':
			lasterr = SIGDVZ;
			break;
		case 'F':
			if (*(inlineptr+1) == 'P')
				lasterr = SIGFPE;
			else
				lasterr = SIGIOT;
			break;
		case 'I':
			lasterr = SIGILL;
			break;
		case 'N':
			if (*(inlineptr+1) == 'M')
				lasterr = SIGNMI;
			else
				if (*(inlineptr+1) == 'V')
					lasterr = SIGINT;
				else
				    if (*(inlineptr+1) == 'E') {
					    printf("\tExternal board NMI\r\n");
					    lasterr = SIGNMI;
				    } else
				      if (*(inlineptr+1) == 'P')
				    	if (*(inlineptr+2) == 'A') {
					    printf("\tParity Error NMI\r\n");
					    lasterr = SIGNMI;
					} else {
					    printf("\tPower Fail NMI\r\n");
					    lasterr = SIGNMI;
					}
			break;
		case 'S':
			lasterr = SIGSYS;
			break;
		case 'T':
			lasterr = SIGTRAP;
			break;
		case 'U':
			lasterr = SIGEMT;
			break;
		default:
			goto dumpstr;
			break;
		}
		while ((*inlineptr != '\n')&&(*inlineptr != '\0')) inlineptr++;
		while ((*inlineptr == '\r')||(*inlineptr == '\n')) inlineptr++;
		if (!portrun) {
			printsig(lasterr);
			flushline(TRUE);
			showspot();
			machineabort();
		}
		return(ENTRYMSG);
	case '=':
		inlineptr++;
		sscanf(inlineptr,"%x",&lastvalue);
		while ((*inlineptr != '\n')&&(*inlineptr != '\0')) inlineptr++;
		while ((*inlineptr == '\r')||(*inlineptr == '\n')) inlineptr++;
		return(EQDATA);
	default:
	dumpstr:
		if ((*inlineptr == '\0') && portrun) {
			printf("%s",inline);
			fflush(stdout);
			goto lookagain;
		}
		while (1) {
			if (*inlineptr == '\0') break;
			if (*inlineptr == '=') break;
			if (*inlineptr == '?') break;
			if (*inlineptr == '*') break;
			if (*inlineptr == 'E') break;
			printf("%c",*inlineptr);
			fflush(stdout);
			inlineptr++;
		}
		goto lookagain;
	}

}

remotego()
{
int saw;
	flushline(TRUE);
	write(port,gocmd,gocmdsize);
	saw = getstar(FALSE);
	if (saw == STAR) {
		write(port,gocmd,gocmdsize);
		saw = getstar(FALSE);
	}
	if (saw != ENTRYMSG)
		return(-1);
	if (getstar(FALSE) != STAR)
		return(-1);
	return(0);
}

remotent()
{
int saw;
	flushline(TRUE);
	write(port,ntcmd,ntcmdsize);
	saw = getstar(FALSE);
	if (saw == STAR) {
		write(port,gocmd,gocmdsize);
		saw = getstar(FALSE);
	}
	if (saw != ENTRYMSG)
		return(-1);
	if (getstar(FALSE) != STAR)
		return(-1);
	return(0);
}

remotestep()
{
int saw;
	flushline(TRUE);
	write(port,stepcmd,stepcmdsize);
	saw = getstar(FALSE);
	if (saw == STAR) {
		write(port,gocmd,gocmdsize);
		saw = getstar(FALSE);
	}
	if (saw != ENTRYMSG)
		return(-1);
	if (getstar(FALSE) != STAR)
		return(-1);
	return(0);
}

remoteput(addrat,value)
{
char tmpstr[CHANGESIZE+1];
	flushline(FALSE);
	if (nommu) sprintf(tmpstr,nommuchangecmd,addrat,value);
	else sprintf(tmpstr,changecmd,addrat,value);
	write(port,tmpstr,CHANGESIZE);
	if (getstar(TRUE) != STAR)
		return(-1);
	return(0);

}

remoteget(addrat)
{
int x;
char tmpstr[PRINTSIZE+1];
	flushline(FALSE);
	if (nommu) sprintf(tmpstr,nommuprintcmd,addrat);
	else sprintf(tmpstr,printcmd,addrat);
	write(port,tmpstr,PRINTSIZE);
	if (getstar(TRUE) != EQDATA)
		return(-1);
	if (getstar(TRUE) != STAR)
		return(-1);
	return(0);

}


remoteregr(regn)
{
int pstrlen;
	setjmp(retrybuf);
	flushline(FALSE);
	if (regn < SINTB) {
		printf("\tillegal reg %s\r\n",regstr(regn));
		regabort(regn);
	} else {
		if (regn < SMOD) {
			if (nommu && (regn != SINTB)) {
				lastvalue = 0;
				return(lastvalue);
			}
			pstrlen = MMUPSIZE;
			if (regn == SINTB) pstrlen++;
		}
		else pstrlen = CPUPSIZE;
		if (regn == SSP) {
			if (!getmode(FALSE))
				write(port,spreg.printreg,pstrlen);
			else write(port,rdtreg[(-1-regn)].printreg,pstrlen);
		} else write(port,rdtreg[(-1-regn)].printreg,pstrlen);
	}
	if (getstar(TRUE) != EQDATA) {
		printf("\tremote access fails\r\n");
		machineabort();
	}
	if (getstar(TRUE) != STAR) {
		printf("\tremote access fails\r\n");
		machineabort();
	}
	return(lastvalue);
}

remoteregw(regn,val)
{
char tmpstr[CPUCSIZE+1];
int pstrlen;
	setjmp(retrybuf);
	flushline(FALSE);
	if (regn < SINTB) {
		printf("\tillegal reg %s\r\n",regstr(regn));
		regabort(regn);
	} else {
		if (regn < SMOD) {
			if (nommu && (regn != SINTB)) {
				return;
			}
			pstrlen = MMUCSIZE;
			if (regn == SINTB) pstrlen++;
		}
		else pstrlen = CPUCSIZE;
		if (regn == SSP) {
			if (!getmode(FALSE))
				sprintf(tmpstr,spreg.changereg,val);
			else sprintf(tmpstr,rdtreg[(-1-regn)].changereg,val);
		} else sprintf(tmpstr,rdtreg[(-1-regn)].changereg,val);
		write(port,tmpstr,pstrlen);
	}
	if (getstar(TRUE) != STAR) {
		printf("\tremote access fails\r\n");
		machineabort();
	}
	return;
}

switchps(fsmode)
{
	setjmp(retrybuf);
	flushline(FALSE);
	if (fsmode) write(port,sfsw,fswsize);
	else write(port,ufsw,fswsize);
	if (getstar(TRUE) != STAR) {
		printf("\tremote access fails\r\n");
		machineabort();
	}
}

getmode(realpsw)
{
	setjmp(retrybuf);
	if (realpsw) {
		if (getreg(SPSR) & 0x100) return(FALSE);
		else return(TRUE);
	} else {
		flushline(FALSE);
		write(port,gfsw,gfswsize);
		if (getstar(TRUE) != EQDATA) {
			printf("\tremote access fails\r\n");
			machineabort();
		}
		if (getstar(TRUE) != STAR) {
			printf("\tremote access fails\r\n");
			machineabort();
		}
		if (lastvalue & 0x100) return(FALSE);
		else return(TRUE);
	}
}

steprett()
{
int stackpsw, pswaddr;
	pswaddr = getreg(SSP) + 4;
	stackpsw = getdouble(pswaddr);
	stackpsw |= 0x04000000;
	setdouble(pswaddr,stackpsw);
}

clearmem(startaddr,endaddr)
{
char tmpstr[26];
	setjmp(retrybuf);
	setreg(SMSR,0);
	flushline(TRUE);
	sprintf(tmpstr,clrcmd,startaddr,endaddr);
	write(port,tmpstr,strlen(tmpstr));
	setalarm(10);
	if (getstar(FALSE) != STAR) {
		printf("\tclear memory fails\r\n");
		machineabort();
	}
	alarm(0);
}
