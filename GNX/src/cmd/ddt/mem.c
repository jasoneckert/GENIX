
/*
 * mem.c: version 1.8 of 7/14/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)mem.c	1.8 (NSC) 7/14/83";
# endif

#include <setjmp.h>
#include <a.out.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/panic.h>
#include <sys/pte.h>
#include <sys/vm.h>
#include "symbol.h"
#include "main.h"
#include "parse.h"
#include "display.h"
#include "error.h"
#include "machine.h"

extern int pid;
extern int errno;
extern int unixcore;

struct user u;
char 	uspace[10+USIZE - sizeof(struct user)];

/* these functions will be for accessing the user's
   address space. An invalid address will cause longjmp back
   to main.
*/

memabort(addrat)
{
	longjmp(resetbuf, MEM_ERROR);
}

getbyte(ataddr)
{
int data = 0;
		getmemb(ataddr,1,&data);
		return(data);
}

getword(ataddr)
{
int data = 0;
		getmemb(ataddr,2,&data);
		return(data);
}

getdouble(ataddr)
{
int data = 0;
		getmemb(ataddr,4,&data);
		return(data);
}

getmem(ataddr)
{
int data = 0;
	switch (acontext) {
		case 1: getmemb(ataddr,1,&data);
			break;
		case 2: getmemb(ataddr,2,&data);
			break;
		case 4: getmemb(ataddr,4,&data);
			break;
		default: printf("\tcontext bug %d\r\n",acontext);
			 memabort(ataddr);
	}
	return(data);
}

getinimage(mstart,msize,mbuf)
int mstart, msize;
char *mbuf;
{
struct exec *hpr = &ddtheader;
	if (mstart >= 0) {
		if (lseek(imagefd,(1024+mstart)-(hpr->a_text_addr),0)<0) {
		} else
			if (read(imagefd, mbuf, msize) != msize) {
			} else return;
	}
	for (; msize > 0; msize--) {
		*mbuf = (char)0;
		mbuf++;
	}
	write(1,"\7",1);				

}

getmemb(mstart,msize,mbuf)
int mstart, msize;
char *mbuf;
{
int *pint;
char *cint;
int pstart;
register struct exec *hpr = &ddtheader;
	if (mstart < 0) {
		for (; msize > 0; msize--) {
			*mbuf = (char)0;
			mbuf++;
		}
		write(1,"\7",1);				
		return;
	}
	if (skipptrace)  {
		if ((hpr->a_text_addr <= mstart) &&
		(mstart < (hpr->a_text_addr + hpr->a_text))) {
			getinimage(mstart,msize,mbuf);
			return;
		}
	}
	if (usefile == TRUE) {
		if (memfd > 0) {
			if (lseek(memfd,mstart,0)<0) {
				printf("\tinvalid address %x\n",mstart);
				memabort(mstart);
			}
			if (read(memfd, mbuf, msize) != msize) {
				printf("\tbad read from %x\r\n",mstart);
				memabort(mstart+msize);
			}
			return;
		}
	}
	if (unixcore == TRUE) {
		if (memfd > 0) {
			cint = mbuf;
			for(; msize > 0; msize--) {
				pstart = physaddr(SYSSPTI,mstart);
				if (lseek(memfd,pstart,0)<0) {
					printf("\tinvalid address %x\n",mstart);
					memabort(mstart);
				}
				if (read(memfd, cint, 1) != 1) {
					printf("\tbad read from %x\r\n",mstart);
					memabort(mstart+msize);
				}
				mstart++; cint++;
			}
			return;
		}
	}
	if (corefd > 0) {
		if (mstart < (u.u_tsize+u.u_dsize)) {
			if (lseek(corefd,(USIZE+mstart),0)<0) {
				printf("\tinvalid address %x\n",mstart);
				memabort(mstart);
			}
		} else {
		    if (u.u_ssize <= 0) {
			printf("\tcore stack size zero\r\n");
			memabort(mstart);
		    }
		    if (mstart >= u.u_ar0[RSP]) {
			mstart -= u.u_ar0[RSP];
			if (lseek(corefd,(USIZE+u.u_tsize+u.u_dsize+mstart),0) 
				<0) {
		    printf("\tstack address not in core file %x\r\n",mstart);
				memabort(mstart);
			}

		    } else {
		printf("\tstack address not in core file %x\r\n",mstart);
			memabort(mstart);
		    }
		}
		if (read(corefd, mbuf, msize) != msize) {
		    printf("\t core read from %x fails %x\r\n",mstart);
			memabort(mstart+msize);
		}
	} else if ((imageonly) || (pid == -1)) {
		getinimage(mstart,msize,mbuf);
	} else {
		    pint = (int *) mbuf;
		    *pint = machine(RMEM,pid,mstart,0);
		    if (errno) {
			    printf("\tcan not read process data\r\n");
			    memabort(mstart);
		    }
		    switch (msize) {
		    case 1:
		    		*pint &= 0xff;
				break;
		    case 2:	*pint &= 0xffff;
				break;
		    case 4:
		    default: break;
		    }
	}
}

setbyte(ataddr,val)
{
int tempval = 0;
	if (pid == -1) {
		printf("\tno process to write data in\r\n");
		memabort();
	}
	tempval = machine(RMEM,pid,ataddr,0);
	if (errno) {
		printf("\tcan not read data %x\r\n",ataddr);
		memabort();
	}
	val = (tempval & 0xffffff00) | (0xff & val);
	machine(WMEM,pid,ataddr,val);
	if (errno) {
		printf("\tcan not insert data here %x\r\n",ataddr);
		memabort();
	}
}

setword(ataddr,val)
{
int tempval = 0;
	if (pid == -1) {
		printf("\tno process to write data in\r\n");
		memabort();
	}
	tempval = machine(RMEM,pid,ataddr,0);
	if (errno) {
		printf("\tcan not read data %x\r\n",ataddr);
		memabort();
	}
	val = (tempval & 0xffff0000) | (0xffff & val);
	machine(WMEM,pid,ataddr,val);
	if (errno) {
		printf("\tcan not insert data here %x\r\n",ataddr);
		memabort();
	}
}

setdouble(ataddr,val)
{
int tempval = 0;
	if (pid == -1) {
		printf("\tno process to write data in\r\n");
		memabort();
	}
	machine(WMEM,pid,ataddr,val);
	if (errno) {
		printf("\tcan not insert data here %x\r\n",ataddr);
		memabort();
	}
}

setmem(ataddr,val)
{
int i;
struct exec *hpr = &ddtheader;
	if (imageonly == TRUE) {
		if (lseek(imagefd,(1024+ataddr)-(hpr->a_text_addr),0)<0) {
		    printf("\tinvalid address %x\r\n",ataddr);
			memabort((1024+ataddr)-hpr->a_text_addr);
		}
		if (write(imagefd, ((char *) &val), tempmodes.context) != 
		    tempmodes.context) {
			printf("\tbad write to %x\r\n",ataddr);
			memabort(ataddr);
		}
		return;
	}
	if (usefile == TRUE) {
		if (memfd > 0) {
			if (lseek(memfd,ataddr,0)<0) {
				printf("\tinvalid address %x\n",ataddr);
				memabort(ataddr);
			}
		if ((i = write(memfd, ((char *) &val), tempmodes.context)) != 
		    tempmodes.context) {
			printf("\twrite to file fails %x\r\n",ataddr);
			printf("\twrite result %d errno %d\r\n",i,errno);
    printf("\twrite(%d,%x -> %x, %d)\r\n",memfd,((char *) &val),
    (*((char *) &val)),tempmodes.context);
			memabort(ataddr);
		}
			return;
		}
	}
	switch (tempmodes.context) {
		case 1: setbyte(ataddr,val);
			break;
		case 2: setword(ataddr,val);
			break;
		case 4: setdouble(ataddr,val);
			break;
		default: printf("\tcontext bug %d\r\n",acontext);
			 memabort(ataddr);
	}
}

getudot()
{
int i;
    if (lseek(corefd,0,0) <0) printf("\tio core failure\n\r");
    if (read(corefd,&u,USIZE)<0) printf("\tio core failure\n\r");
    i = (int) u.u_ar0;
    u.u_ar0 = (int *) (((int) &u + (PGSIZE * 3)) + (i & 511));
    if (u.u_tsize == 0) {
	printf("Warning u.u_tsize is zero\r\n");
	if (imagefd > 0) {
	    printf("Changing to object text size\r\n");
	    u.u_tsize = ddtheader.a_text;
	}
    }
    if (u.u_dsize == 0) {
	printf("Warning u.u_dsize is zero\r\n");
	if (imagefd > 0) {
	    printf("Changing to object data size\r\n");
	    u.u_dsize = ddtheader.a_data+ddtheader.a_bss;
	}
    }
    if (u.u_ssize == 0) {
	printf("Warning u.u_ssize is zero\r\n");
    }
}

fpcheck(fpat)
int fpat;
{
	if ((fpat >= getreg(SFP)) && (fpat < TOPUSER)) return(TRUE);
	return(FALSE);
}

struct panic upanic;

getpanicblk()
{
int i;
char pstr[130];
    memfd = corefd;
    if (lseek(corefd,PANICADDR,0) <0) printf("\tio core failure\n\r");
    if (read(corefd,&i,4)<0) printf("\tio core failure\n\r");
    if (lseek(corefd,i,0)<0) printf("\tio core failure\n\r");
    if (read(corefd,&upanic,sizeof(struct panic))<0) printf("\tio core failure\n\r");
    if (lseek(corefd,upanic.ps_version,0)<0) printf("\tio core failure\n\r");
    if (read(corefd,pstr,sizeof(pstr))<0) printf("\tio core failure\n\r");
    printf("%s\r\n",pstr);
    if (lseek(corefd,upanic.ps_string,0)<0) printf("\tio core failure\n\r");
    if (read(corefd,pstr,sizeof(pstr))<0) printf("\tio core failure\n\r");
    printf("%s\r\n",pstr);
}

/* Return the physical memory address corresponding to a given virtual
 * address.  Returns -1 if physical address does not exist.
 */
physaddr(spti, addr)
	register spt_t	spti;		/* spt index */
	register unsigned long addr;	/* virtual address */
{
	pte	*curpte;		/* current pte entry */
	pte	thepte;			/* the pte entry */
	struct	spt	*sptn;		/* spt[spti] entry */
	pte	*sptpte1;		/* spt_pte1 address */

	
	if (addr >= MAXADDRS) return(-1);
	/* if (spti >= NSPT) return(-1);  kernel vallocs now, just skip it */
	thepte = (int)upanic.ps_spt + (sizeof(struct spt) * spti);
	sptn = (struct spt *) thepte;
	usefile = TRUE;	/* A REAL CROCK */
	sptpte1 = (pte *) getdouble((int) &(sptn->spt_pte1));
	usefile = FALSE; /* A REAL CROCK */
	if (sptpte1 == NULL) return(-1); 
	curpte = sptpte1 + GETVI1(addr);
	usefile = TRUE;	/* A REAL CROCK */
	thepte = getdouble(curpte);
	usefile = FALSE; /* A REAL CROCK */
	if (PTEINVALID(thepte)) return(-1);
	curpte = (pte *) PTEMEM_ADDR(thepte);
	curpte += GETVI2(addr);
	usefile = TRUE;	/* A REAL CROCK */
	thepte = getdouble(curpte);
	usefile = FALSE; /* A REAL CROCK */
	if (PTEINVALID(thepte)) return (-1);
	return (PTEMEM_ADDR(thepte) | GETVOFF(addr));
}

