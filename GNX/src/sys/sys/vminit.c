/*
 * vminit.c: version 4.15 of 10/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)vminit.c	4.15 (NSC) 10/8/83";
# endif

/* Virtual memory initialization module for National DS/16000 system	*/
/*	David I. Bell	Laura Neff     6/28/82		*/

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/psr.h"
#include "../h/devpm.h"
#include "../h/mmu.h"
#include "../h/vm.h"
#include "../h/vmmeter.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/vlimit.h"

/*
	NOTES ON THE ALGORITHM FOR CHECKING LEGAL MEMORY REFERENCES

Standard Unix understands two separate sections of the user's address space.
The first is the text plus data segment, which starts at location zero and
grows upward.  The second is the stack segment, which starts at a high address
and grows downward.  The size of the text plus data segment is changed by
the sbreak system call.  The size of the stack segment changes as the user
stack pointer is changed.  A memory reference is legal only if it lies in the
text plus data segment, or in the stack segment.  If this is not the case, a
segment fault error should be  generated.

However, this is complicated by the fact that abort traps can legitimately
occur which are near the stack pointer, and yet below it.  As an example,
the instruction "movqd 0,tos" accesses beyond the current stack pointer, and
can cause an abort at that address.  In such a case, however, the stack pointer
is not decremented, and so a trivial check would make the reference fail.
In order to allow that kind of legatimate access, a small leeway (STACKGRACE)
is allowed below the stack pointer.  This amount is 32, which allows for the
worst case of the "save" instruction, which can push all eight registers
on the stack.

However, the instruction "enter" can adjust the stack by an arbitrary amount,
and then store the registers on the stack!!  So no amount of leeway is good
enough for this instruction.  Therefore, if the instruction causing the abort
is an "enter" instruction, any memory reference has to be allowed.  Whew!!
*/
/*         CST LAYOUT AND RELATED DATA STRUCTURES

The cst (core status table) contains one entry for every physical memory page.
It is allocated at runtime, in routine initvm, beginning at location topsys.
When the cst is allocated, topsys is incremented to include the cst area.

The code makes a few basic assumptions about how memory is laid out.  Physical
memory is assumed to be configured in 1/4 megabyte pieces, aligned on 1/4
megabyte boundaries.  It is also assumed that there is enough memory beginning
at physical location zero to handle the kernel plus the cst area, and that the
pte1 area, which is put at the top of physical memory, requires less than 1/4
megabyte.  The 1/4 megabyte is actually a constant defined in param.h.

The cst occupies a single contiguous chunk of memory, even if physical memory
contains "holes" where certain physical pages do not exist.  The cst is
logically divided into one or more pieces, each covering 1/4 megabyte of
physical memory.  Data structure mempgs describes the physical pages which
actually exist.  It is a list of the beginning page numbers of each existing
1/4 megabyte piece, with a -1 to end the table.  Cstptr is an array of pointers
into the cst, one for each possible 1/4 megabyte piece of physical memory.  If
a piece of physical memory does not exist, the pointer is NILL.

A picture will clarify this.  If the system is configured with 1/4 megabyte
of memory beginning at physical page 0, and 1/2 megabyte beginning at physical
page 2048, then mempgs, cstptr, and the cst would be laid out like this:

       mempgs            cstptr                              cst
     +--------+      +---------------+                    +-----------+
     |     0  |      | for page 0    |------------------> |  page 0   |
     +--------+      +---------------+                    +___________+
     |  2048  |      | for page 512  |->NILL              |  page 1   |
     +--------+      +---------------+                    +___________+
     |  2560  |      | for page 1024 |->NILL              /     .     /
     +--------+      +---------------+                    /     .     /
     |    -1  |      | for page 1536 |->NILL              +-----------+
     +--------+      +---------------+                    | page 511  |
   (Remainder of     | for page 2048 |-------------+      +-----------+
 mempgs is ignored)  +---------------+             +----> | page 2048 |
                     | for page 2560 |----------+         +___________+
                     +---------------+          |         | page 2049 |
                     | for page 3072 |->NILL    |         +___________+
                     +---------------+          |         /     .     /
                     | for page 3584 |->NILL    |         /     .     /
                     +---------------+          |         +-----------+
                     /       .       /          |         | page 2559 |
                     /       .       /          |         +-----------+
                     +---------------+          +-------> | page 2560 |
                     | for page 6656 |->NILL              +-----------+
                     +---------------+                    | page 2561 |
                                                          +-----------+
                                                          /     .     /
                                                          /     .     /
                                                          +-----------+
                                                          | page 3071 |
                                                          +-----------+ */

struct	cst	*cstptr[MEMCHKS];
				/* ptrs into cst at MEMCHUNK boundaries */
int	mempgs[MEMCHKS];		/* existent memory (MEMCHUNK pieces) */
struct	cst	*cstrover;		/* rover for free cst pages */
extern	char	*kalloctable;		/* bit table for memory allocation */
extern	int	nkalloc;		/* size of table */
extern	char	*swapbittable;		/* bit table for swap allocation */
extern	int	swapused;		/* number of swapping pages used */
extern	char	etext;			/* end of kernel text area */
extern	pte	*uptetable[];		/* pointers to pte entries for u. */
extern	struct	cst	*freepagelist;	/* header of list of free pages */
extern	struct	cst	*zeropagelist;	/* header of list of zeroed pages */
extern	int	pagesfree;		/* free pages (includes zerofree) */
extern	int	zerofree;		/* number of zeroed free pages */
int	kalloccount;			/* pages allocated by kernel */
int	topsys;				/* top of kernel */
int	totalpages;			/* total number of memory pages */
int	userpages;			/* memory pages available to users */
int	availpages;			/* free memory and swapping pages */
extern	int	topmem;			/* top of physical memory */
extern	int	botpte1;		/* beginning of level 1 page tables */
extern	int	toppte1;		/* top of level 1 page tables */
int	botalloc;			/* beginning of mem allocation area */
int	kdebug;				/* if zero, writelock kernel text */

/* Mappings between virtual and physical memory for device accesses */

struct devmap devmap[] =
{
/* Rom monitor */
	PR_Monitor,	PR_Monitor,	PR_Pagelen,

# if NSIO > 0
/* SIO board (hacked: assumes NSIO == 1) */
	SIO_BASE,	0xa00000,	15,
# endif

# if NBM > 0
/* Graphics board (screen and keyboard) */
	GB_VideoRAM,	0xc00000,	153,
	GB_Video2RAM,	0xc20000,	153,
	GB_WindowRAM,	0xc40000,	153,
   (int)GB_CRTC_Addr,	0xc80000,	1,
/*	GB_CRTC_Data,	0xc80002,	1,	*/
   (int)GB_IntMask,	0xc80200,	1,
   (int)GB_IntStatus,	0xc80400,	1,
   (int)GB_IntClear,	0xc80600,	1,
   (int)GB_Control,	0xc80c00,	1,
/*	GB_Status,	0xc80c00,	1,	*/
   (int)GB_Tone,	0xc80e00,	1,
   (int)GB_Keyboard,	0xc81000,	1,
   (int)GB_Joystick,	0xc81200,	1,
   (int)GB_CursDot,	0xc81a00,	1,
   (int)GB_CursRAM,	0xc81c00,	1,
# endif NBM

/* Disk and tape controllers */
   (int)DCUA_CHAN0,	0xd00000,	1,
   (int)TCUA_STATUS,	0xd00200,	1,
  
/* The memory device page.  Physical address is changed at runtime as needed */
	TEMPPAGE,	0,		1,

/* CPU board */
	CP_MasterICU,	CP_MasterICU,	1,
/*	CP_RemoteICU,	CP_RemoteICU,	1,	*/
	CP_Usart,	CP_Usart,	1,
#if NLP > 0
	CP_PIO,		CP_PIO,		1,
#endif
	CP_Time,	CP_Time,	1,
	CP_OptBuffer,	CP_OptBuffer,	1,
	CP_ErrorReg,	CP_ErrorReg,	1,
	CP_TestReg,	CP_TestReg,	1,
	CP_DiagReg,	CP_DiagReg,	1,
/*	CP_IBTLsel,	CP_IBTLsel,	1,	*/
/*	CP_IBcontrol,	CP_IBcontrol,	1,	*/
/*	CP_Aux1,	CP_Aux1,	1,	*/
/*	CP_Aux2,	CP_Aux2,	1,	*/
/*	CP_Aux3,	CP_Aux3,	1,	*/
/*	CP_Aux4,	CP_Aux4,	1,	*/

/* End of table */
	0,		0,		0
};

/*
 * Determine amount of memory and its layout.
 */
initmem()
{
	register int	*testaddr;	/* Addr to test for memory existence */
	register int	n,memidx,addr;

	for (memidx = 0; memidx < MEMCHKS; memidx++) mempgs[memidx] = -1;
	memidx = 0;
	totalpages = 0;
	for (addr = 0 ; addr < TOPPHYMEM ; addr += MEMCHUNK ) {
		if (addr > topsys) {	/* don't try to write on kernel */
			testaddr = (int *)addr;
			*testaddr = 0;
			n = *testaddr;
			if (n != 0) continue;
			*testaddr = -1;
			n = *testaddr;
			if (n != -1) continue;
		}
		mempgs[memidx++] = addr >> MCSHIFT;
		topmem = addr;
		totalpages += MEMCHUNK/MCSIZE;
	}
	topmem += MEMCHUNK;
}

/* routine to initialize the vm tables */
initvm()
{
	register struct	cst *pg;	/* current cst entry */
	register struct	spt *pp;	/* current spt entry */
	register unsigned long addr;	/* address */
	register pte	ptedata;	/* current pte */
	register int	i;		/* loop variable */
	pte	*upte;			/* address of pte for u. */
 	struct devmap *dm;		/* virtual to physical mapping */
  	int pageadr;			/* another page address */
	struct	cst	*topmemcst;

	/* initialize cstptr table */
	for (i = 0; i < MEMCHKS; i++) cstptr[i] = NULL;
	for (i = 0; i < MEMCHKS; i++) {
		if (mempgs[i] >= 0) {
			cstptr[mempgs[i] >> (MEMSHIFT - MCSHIFT)] =
			   (MEMCHUNK / MCSIZE) * i + cst;
		}
		else break;
	}
	cstrover = addrtocst(topsys);

	/* initialize the spt table */
	for (pp = spt; pp < sptNSPT; pp++) {
		pp->spt_pte1 = (pte *) (botpte1 + ((pp - spt) * P1SIZE));
		pp->spt_inode = NULL;
		pp->spt_share = 0;
		pp->spt_proccount = 0;
		pp->spt_tablepages = 0;
		pp->spt_usedpages = 0;
		pp->spt_mempages = 0;
		pp->spt_tmempages = 0;
		pp->spt_reserved = 0;
		pp->spt_flags = 0;
	}

	/* set all of memory to be free */
	for (pg = cst; pg < cstNCST; pg++) {
		pg->cst_type = PTY_FREE;
		pg->cst_lock = 0;
		pg->cst_share = 0;
		pg->cst_intrans = 0;
		pg->cst_backup = 0;
		pg->cst_next = NULL;
		pg->cst_spti = SYSSPTI;
		pg->cst_sptp = csttopg(pg);
	}

	/* set all of the kernel to system pages */
	topmemcst = addrtocst(topsys);
	for (pg = cst; pg < topmemcst; pg++) {
		pg->cst_type = PTY_SYSTEM;
	}

	/* set the pte level 1 pages */
	topmemcst = addrtocst(toppte1 - 1);
	for (pg = addrtocst(botpte1); pg <= topmemcst; pg++) {
		pg->cst_type = PTY_PTE1;
		pg->cst_share++;
		lockpage(pg);
	}

	/* generate the list of free pages */
	pagesfree = 0;
	zerofree = 0;
	freepagelist = NULL;
	zeropagelist = NULL;
	for (pg = cst; pg < cstNCST; pg++) {
		if (pg->cst_type != PTY_FREE) continue;
		pg->cst_next = freepagelist;
		freepagelist = pg;
		pagesfree++;
	}

	/* build the kernel map */
	for (addr = 0; addr < topmem; addr += MCSIZE) {
		if (addr < (int)&etext)		/* kernel text */
			ptedata = PTELOCK(addr, PG_RONA|PG_REF);
		else if (addr < topsys)			/* kernel data */
			ptedata = PTELOCK(addr, PG_RWNA|PG_REF);
		else					/* rest of memory */
			ptedata = PTESPY(addr, PG_RWNA|PG_REF);
		setmap(SYSSPTI, addr, ptedata);
	}
 	for (dm = devmap; dm->pagelen; dm++) {		/* device mappings */
		addr = dm->virtaddr & ~MCOFSET;
		pageadr = dm->physaddr & ~MCOFSET;
 		for (i = dm->pagelen; i > 0; i -= MCPAGES) {
 			setmap(SYSSPTI, addr, PTESPY(pageadr, PG_RWNA|PG_REF));
			addr += MCSIZE;
			pageadr += MCSIZE;
 		}
 	}
	userpages = pagesfree;			/* avail memory pages */
	availpages = userpages + nswap/MCPAGES;	/* avail swap, memory pages */
	spt[SYSSPTI].spt_reserved = ((USIZE/MCSIZE) * nproc) + 10;
	availpages -= spt[SYSSPTI].spt_reserved; /* reserve some for kernel */

 	/* map u. area to first u. slot (for init) */
 	for (i = 0; i < USIZE; i += MCSIZE) {
 		pageadr = alloczeropage(SYSSPTI, BOTXU + i, PTY_DATA);
		if (pageadr <= 0) panic("initmemalloc");
 		setmap(SYSSPTI, BOTXU + i, PTEMEM(pageadr, PG_RWNA));
 		setmap(SYSSPTI, BOTU + i, PTESPY(pageadr, PG_RWNA));
		upte = pageinpt(SYSSPTI, BOTU + i, 0, PTLEVEL2);
		if (upte == NULL) panic("initmemu");
		lockaddr(upte);		/* lock level pte2 page for safety */
		uptetable[i>>MCSHIFT] = upte;	/* remember pte address */
 	}

	bitinit(swapbittable, nswap/MCPAGES);	/* clear swapping table */
	swapalloc();			/* allocate block zero forever */
	bitinit(kalloctable, nkalloc);	/* clear memory allocation table */
	botalloc = topmem;
	kalloccount = 0;
	swapused = 0;
	sum.v_pgin = 0;
	cnt.v_pgin = 0;
	sum.v_pgout = 0;
	cnt.v_pgin = 0;
 	mtpr(ptb0, spt[SYSSPTI].spt_pte1);	/* set page table address */
	addr = mfpr(msr);		/* read mmu status register */
	addr &= (MSR_BRKERR | MSR_BPT1 | MSR_READBPT | MSR_BPTSTAT |
		MSR_BRKENABLE | MSR_BRKUSER | MSR_SEQENABLE | MSR_FLOWUSER );
					/* only keep bits used for debugging */
 	addr |= (MSR_TRANSUSER | MSR_TRANSSUP | MSR_USEPTB1 |
  		MSR_FLOWENABLE );
					/* set translation and tracing */
	if (!kdebug) addr |= (MSR_FLOWUSER | MSR_BRKUSER | MSR_AI ); 
			/* if not kdebug allow user flow and user breakpoints */
	mtpr(msr, addr);		/* enable the mmu */
}
