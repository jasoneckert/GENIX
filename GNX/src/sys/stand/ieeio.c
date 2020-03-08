
/*
 * ieeio.c: version 2.5 of 8/18/83
 * VAX file server for 16K workstation
 */
# ifdef SCCS
static char *sccsid = "@(#)ieeio.c	2.5 (NSC) 8/18/83";
# endif

#include <signal.h>
#include <stdio.h>
#include <setjmp.h>
#include <sys/ieeio.h>

/* VSPY IO PAGE to VM mappings */
#define  IE8291		0xffec00		/* 8291A */
#define  IE8292		0xffea00		/* 8292 */
#define  VM8291		0xa00000		/* vspy 8291A */
#define  VM8292		0xb00000		/* vspy 8292 */
/*
  8291A t/l register offsets
*/
#ifdef STANDALONE
#define  DI		0xffec00		/* r data in*/
#define  DO		0xffec00		/* w data out*/
#define  IS1		0xffec02		/* r interrupt status 1*/
#define  IM1		0xffec02		/* w interrupt mask 1*/
#define  IS2		0xffec04		/* r interrupt status 2*/
#define  IM2		0xffec04		/* w interrupt mask 2*/
#define  SPS		0xffec06		/* r serial poll mode*/
#define  SPM		0xffec06		/* w serial poll status*/
#define  AS		0xffec08		/* r address status*/
#define  AM		0xffec08		/* w address mode*/
#define  CPT		0xffec0a		/* r command pass thru*/
#define  AUXM		0xffec0a		/* w auxiliary mode*/
#define  A0		0xffec0c		/* r address 0*/
#define  A0_1		0xffec0c		/* w address 0/1*/
#define  A1		0xffec0e		/* r address 1*/
#define  EOS		0xffec0e		/* EOS reg*/
#else
#define  DI		0xa00000		/* r data in*/
#define  DO		0xa00000		/* w data out*/
#define  IS1		0xa00002		/* r interrupt status 1*/
#define  IM1		0xa00002		/* w interrupt mask 1*/
#define  IS2		0xa00004		/* r interrupt status 2*/
#define  IM2		0xa00004		/* w interrupt mask 2*/
#define  SPS		0xa00006		/* r serial poll mode*/
#define  SPM		0xa00006		/* w serial poll status*/
#define  AS		0xa00008		/* r address status*/
#define  AM		0xa00008		/* w address mode*/
#define  CPT		0xa0000a		/* r command pass thru*/
#define  AUXM		0xa0000a		/* w auxiliary mode*/
#define  A0		0xa0000c		/* r address 0*/
#define  A0_1		0xa0000c		/* w address 0/1*/
#define  A1		0xa0000e		/* r address 1*/
#define  EOS		0xa0000e		/* EOS reg*/
#endif

/* 8292 addresses	*/
#define  DD		0xb00000		/* r data bus */
#define  IM		0xb00000		/* w interrupt mask */
#define  IS		0xb00002		/* r interrupt status */
#define  CM		0xb00002		/* w command field */

/* 8291A bits */
#define	 RSV		0x40			/* srq bit enable*/
#define  RM		0x01			/* BI bit in IS1 */
#define  WM		0x02			/* BO bit in IS1 */
#define  GET		0x20			/* a group trigger msg */
#define  DEC		0x08			/* a device clear msg */
#define  ERR		0x04			/* interface error */
/* 8291A commands */
#define	 PON		0x00			/* Aux reg PON */
#define	 RST		0x02			/* Aux reg RESET*/
#define	 CLK		0x26			/* 6Mhz clk */
#define	 BAUX		0xa4	/* Aux B TRISTATE xfer */
#define	 DEAD		0xe0	/* diasble extended address in A0_1 */
/* 8292 IS bits */
#define	 OBF		0x01
#define	 IBF		0x02

/* 8292 commands */
#define	 RCST		0xe6
#define	 RBST		0xe7
#define	 ABORT		0xf9
#define	 SRST		0xf2

#define MAXTRYS		4		/* max times to restart request */

struct { char devchar; };

jmp_buf env; /* reset buffers */

/* reset hardware the t/l chip with passed address */
iesetwaddr(theaddr)
char theaddr;
{
	AUXM->devchar =  RST;		/* hardware reset */
	IM1->devchar = 0;		/* clear masks */
	IM2->devchar = 0;		/* clear masks */
	AM->devchar = 1;		/* set address mode to primary */
	A0_1->devchar = theaddr;	/* restore address */
	A0_1->devchar = DEAD;		/* disable address extended */
	AUXM->devchar = CLK;		/* set speed */
	AUXM->devchar = BAUX;		/* set other modes */
	SPM->devchar = 0;		/* clear poll register */
	AUXM->devchar =  PON;		/* release PON */
}

/* init the t/l chip assume everything is ok */
ieinit()
{
	SPM->devchar = 0;		/* clear poll register */
	AUXM->devchar =  PON;		/* PON it */
}


/* reset ie interface and change address to theaddr */
resetaddr(theaddr)
char theaddr;
{
	theaddr &= 0x1f;	/* mask everything but address */
	iesetwaddr(theaddr);
	return(0);
}

/* reset ie interface and save address */
iereset()
{
char theaddr;
	theaddr = A0->devchar;
	iesetwaddr(theaddr);
	return(0);
}

ieexit()
{
	iereset();
	printf("\r\nrequest aborted\r\n");
#ifdef STANDALONE
	asm("bpt");
#else		
	exit(-1);
#endif
}

int ntrys; /* count number of re trys */

/* decide to give up or try again */
ieretry()
{
	if (ntrys > MAXTRYS) {
		ieexit();
	} else {
		iereset();
#ifndef STANDALONE
		printf("x");
		fflush(stdout);
#endif
		ntrys++;
	}
}

/* ie service request */
iesrq() 
{
int gpibstatus;
	while (1) {
	    ieinit();
	    SPM->devchar = RSV;
	    gpibstatus = IS1->devchar;	
	    while ((SPS->devchar) & RSV)
	    {
		gpibstatus = IS1->devchar;	
		if (gpibstatus & ERR) {
			printf("\r\ninterface error - retry\r\n");
			gpibstatus = -1;
			break;
		}
		if (gpibstatus & DEC) {
			gpibstatus = -1;
			break;
		}
		if (gpibstatus & GET) {
			if ((SPS->devchar) & RSV)
				continue;
			else
				return;
		}
	    }
	    if (gpibstatus != -1) {
		do {
		    gpibstatus = IS1->devchar;	
		    if (gpibstatus & ERR) {
			    printf("\r\ninterface error - retry\r\n");
			    break;
		    }
		    if (gpibstatus & DEC) {
			    break;
		    }
		    if (gpibstatus & GET) {
			    return;
		    }
		} while (1);
	    }
	}
}

ibwrite(xib,slen)
char *xib;
{
register char *rxib;
register int rlen;
register int gpibstatus;
/*
retry:
	gpibstatus = IS1->devchar;	
*/
	rxib = xib;
	for(rlen = slen; rlen > 0; rlen--)
	{
	    do {
		    gpibstatus = IS1->devchar;	
		    if (gpibstatus & ERR) {
			printf("\r\ninterface error\r\n");
			for (gpibstatus = 0; gpibstatus < 500; gpibstatus++) ;
		    }
		    /*
		    if (gpibstatus & DEC) {
			    goto retry;
		    }
		    */
		    if (gpibstatus & GET) {
			longjmp(env,1);
		    }
	    }
	    while (!(gpibstatus & WM));
	    (DO->devchar) = *rxib++;
	}
}

ibread(xib,slen)
char *xib;
{
register char *rxib;
register int rlen;
register int gpibstatus;
/*
retry:
	gpibstatus = IS1->devchar;	
*/
	rxib = xib;
	for(rlen = slen; rlen > 0; rlen--)
	{
	    do {
		    gpibstatus = IS1->devchar;	
		    /*
		    if (gpibstatus & DEC) {
			    goto retry;
		    }
		    */
		    if (gpibstatus & GET) {
			longjmp(env,1);
		    }
	    }
	    while (!(gpibstatus & RM));
	    *rxib++ = (DI->devchar);
	}
}


#ifdef STANDALONE
int errno;
#else
extern int errno;
#endif

int transbyte; /* number of bytes transferred on read or write */

iespy() {
#ifndef STANDALONE
	if (vspy(VM8291, IE8291, 1)) {
		printf("Cannot spy on ie 8291A registers\n");
		errno = 6;
		return(-1);
	}
	signal(SIGINT,ieexit);
#endif
	return(0);
}

rtransfer(ibt)
struct ibcommand *ibt;
{
register char *ip,checksum;
int sresult;
	checksum = 0;
	for (ip = (char *) ibt; ip < &(ibt->checkbyte); ip++) 
		checksum += *ip;
	ibt->checkbyte = checksum;
	do {
	    ibwrite((char *) ibt,sizeof *ibt);
	    ibread(&sresult,sizeof sresult);
	    if (ibt->checkbyte == (char)(sresult & 0xff)) break;
	    else sresult = BADSUM;
	} while (sresult == BADSUM);
	ibread(&sresult,sizeof sresult);
	if (sresult < 0) {
	    ibread(&errno,sizeof errno);
	} else errno = 0;
	return(sresult);
}

ieopen(name,mode)
char *name;
{
struct ibcommand ibopen;
	if (iespy() < 0) return(-1);
	ntrys = 0;
	if (setjmp(env)) ieretry();
	iesrq();
	strcpy(ibopen.fname,name);
	ibopen.fcommand = 'o';
	ibopen.i1st = mode;
	return(rtransfer(&ibopen));
}

iecreat(name,mode)
char *name;
{
struct ibcommand ibcreat;
	if (iespy() < 0) return(-1);
	ntrys = 0;
	if (setjmp(env)) ieretry();
	iesrq();
	strcpy(ibcreat.fname,name);
	ibcreat.fcommand = 'm';
	ibcreat.i1st = mode;
	return(rtransfer(&ibcreat));
}

ieread(fildes,buffer,nbytes)
char *buffer;
{
struct ibcommand ibread;
int sresult;
	ntrys = 0;
	if (setjmp(env)) ieretry();
	iesrq();
	ibread.fcommand = 'r';
	ibread.i1st = fildes;
	ibread.i2nd = nbytes;
	sresult = rtransfer(&ibread);
	if (sresult < 0) return(sresult);
	transbyte = 0;
	inblock(buffer,nbytes);
	return(transbyte);
}

iewrite(fildes,buffer,nbytes)
char *buffer;
{
struct ibcommand ibwrite;
int sresult;
	ntrys = 0;
	if (setjmp(env)) ieretry();
	iesrq();
	ibwrite.fcommand = 'w';
	ibwrite.i1st = fildes;
	ibwrite.i2nd = nbytes;
	sresult = rtransfer(&ibwrite);
	if (sresult < 0) return(sresult);
	transbyte = 0;
	outblock(buffer,nbytes);
	return(transbyte);
}

ielseek(fildes,offset,whence)
{
struct ibcommand iblseek;
	ntrys = 0;
	if (setjmp(env)) ieretry();
	iesrq();
	iblseek.fcommand = 's';
	iblseek.i1st = fildes;
	iblseek.i2nd = offset;
	iblseek.i3rd = whence;
	return(rtransfer(&iblseek));
}

ieclose(fildes)
{
struct ibcommand ibclose;
	ntrys = 0;
	if (setjmp(env)) ieretry();
	iesrq();
	ibclose.fcommand = 'c';
	ibclose.i1st = fildes;
	return(rtransfer(&ibclose));
}

inblock(buffer,nbytes)
char *buffer;
{
struct ibblock inblock;
int sresult;
register int j;
register char *ip, checksum;
	ip = (char *) &inblock;
	for (j=0; j < (sizeof inblock); j+=512)
		ip[j] = 0;
	do {
	    do {
		    ip = (char *) &inblock;
		    ibread(ip,sizeof inblock);
		    checksum = 0;
		    for (ip=(char *)&inblock; ip < &(inblock.checkbyte); ip++) 
			    checksum += *ip;
		    if (checksum != inblock.checkbyte)
			sresult = BADSUM;
		    else sresult = checksum;
		    ibwrite(&sresult,sizeof sresult); 
	    } while (sresult != (inblock.checkbyte));
	    j = 0;
	    for (ip = inblock.iopage; j < inblock.nbytes; j++)
		    *buffer++ = *ip++;
	    transbyte += inblock.nbytes;
	    if (inblock.nbytes < BUFSIZE)
		    nbytes = 0;
	    else
		    nbytes -= inblock.nbytes;
	} while (nbytes > 0);
}

outblock(buffer,nbytes)
char *buffer;
{
struct ibblock outblock;
int sresult;
register int j;
register char *ip, checksum;
	ip = (char *) &outblock;
	for (j=0; j < (sizeof outblock); j+=512)
		ip[j] = 0;
	do {
	    if (nbytes < BUFSIZE)
		    outblock.nbytes = nbytes;
	    else
		    outblock.nbytes = BUFSIZE;
	    j = 0;
	    for (ip = outblock.iopage; j < outblock.nbytes; j++)
		    *ip++ = *buffer++;
	    checksum = 0;
	    for (ip=(char *)&outblock; ip < &(outblock.checkbyte); ip++) 
		    checksum += *ip;
	    outblock.checkbyte = checksum;
	    do {
		    ip = (char *) &outblock;
		    ibwrite(ip,sizeof outblock);
		    ibread(&sresult,sizeof sresult); 
		    if (sresult == outblock.checkbyte)
			    break;
	    } while (sresult != (outblock.checkbyte));
	    ibread(&sresult,sizeof sresult); 
	    if (sresult < 0) {
		    ibread(&errno,sizeof errno); 
		    transbyte = sresult;
		    return;
	    }
	    transbyte += outblock.nbytes;
	    if (outblock.nbytes < BUFSIZE)
		    nbytes = 0;
	    else
		    nbytes -= outblock.nbytes;
	} while (nbytes > 0);
}
