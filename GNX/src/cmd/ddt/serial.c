
/*
 * serial.c: version 1.9 of 8/26/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)serial.c	1.9 (NSC) 8/26/83";
# endif

#include <stdio.h>
#include <a.out.h>
#include <stab.h>
#include <setjmp.h>
#include <sgtty.h>
#include <signal.h>
#include "main.h"
#include "parse.h"
#include "machine.h"

#define BUFSIZE 1024

int ldcmdsize = 3;
char *loadcmd = "@i\r";
int serialtime = 5;

serialabort()
{
	cookconfig();
	machineabort();
}

serialtimeout()
{
	printf("\r\nserial line timeout on download\r\n");
	serialabort();
}

serialalarm(n)
{
	signal(SIGALRM,serialtimeout);
	alarm(n);
}

rawconfig() {
struct sgttyb portconfig;
		portconfig.sg_ispeed = portconfig.sg_ospeed = B9600;
		portconfig.sg_erase = portconfig.sg_kill = 0;
		portconfig.sg_flags = (EVENP|ODDP|RAW);
		stty(port,&portconfig);
}

cookconfig() {
struct sgttyb portconfig;
		portconfig.sg_ispeed = portconfig.sg_ospeed = B9600;
		portconfig.sg_erase = portconfig.sg_kill = 0;
		portconfig.sg_flags = (EVENP|ODDP);
		stty(port,&portconfig);
}


serialdown(startat)
{
int filetype, fileopen, readrst;
	fileopen = open(imagename,0);
	if (fileopen < 0) {
		printf("\r\nopen of %s fails\r\n",imagename);
		serialabort();
	}
	readrst = read(fileopen, &filetype, sizeof filetype);
	if (readrst < sizeof filetype) {
		printf("Bad object format type read\n");
		close(fileopen);
		serialabort();
	}
	if (filetype == ZMAGIC) {
		zload(fileopen,startat);
	} else {
		printf("Do not know this a.out format %x\n",filetype);
		close(fileopen);
		serialabort();
	}
}

zload(fileload,startat)
{
struct exec bhead;
char ioblock[BUFSIZE], checkbyte;
int i, start, len, rlen;
register char checksum;
register char *checkptr;
	lseek(fileload,0,0);
	i = read(fileload, (char *)&bhead, sizeof bhead);
	if (i != sizeof bhead || (bhead.a_magic != ZMAGIC)) {
		if (bhead.a_magic != ZMAGIC)
			printf("Bad ZMAGIC %x\n",bhead.a_magic);
		else printf("Bad file read\n");
		close(fileload);
		serialabort();
		return;
	}
	if (lseek(fileload,1024,0) < 0) {
		printf("lseek on bootimage fails\n");
		close(fileload);
		serialabort();
		return;
	}
	setreg(SPC,bhead.a_entry);
	setreg(SMOD,bhead.a_entry_mod); /* the module number for a.out */
	if (bhead.a_text_addr != bhead.a_mod_addr) {
		printf("\tddt will not load this file\r\n");
		serialabort();
	} else {
		if (startat < 0) {
		    start = bhead.a_text_addr;	/* load text at */
		} else { start = startat; }
	}
	if ((bhead.a_text + bhead.a_text_addr) != bhead.a_dat_addr) {
		printf("\tddt will not load this file\r\n");
		serialabort();
	}
	len = bhead.a_text + bhead.a_data;
	/* clear memory out for bss segment */
	clearmem((start + len),(start + len + bhead.a_bss));
	printf("\t%d",len);
	fflush(stdout);
	while (len > 0) {
	    rawconfig();
	    rlen = read(fileload,ioblock,BUFSIZE);
	    if (rlen <= 0) {
		    printf("read of %s failed\n",imagename);
		    close(fileload);
		    serialabort();
	    }
	    if (rlen > len) rlen = len;
	    checksum = 0;
	    checkptr = ioblock;
	    for (i = 0; i < rlen; i++)
		checksum += *checkptr++;
	    checkbyte = checksum;
	    while (1) {
		    serialalarm(serialtime);
		    lwrite(loadcmd,ldcmdsize);
		    lwrite(&start,sizeof start);
		    lwrite(&rlen, sizeof rlen);
		    lwrite(ioblock, rlen);
		    lwrite(&checkbyte, sizeof checkbyte);
		    flushline(FALSE);
		    if (getstar(TRUE) == STAR) break;
		    else {
		    	printf(" checksum error- retry");
			fflush(stdout);
			rawconfig();
		    }
	    }
	    len -= rlen;
	    start += rlen;
	    printf(" %d",len);
	    fflush(stdout);
	}
	alarm(0);
	cookconfig();
	close(fileload);
	printf("\r\n");
}

lwrite(cptr,csize)
char *cptr;
int csize;
{
	if (write(port,cptr,csize) <= 0) {
		printf("\r\nserial write fails\r\n");
		serialabort();
	}
}
