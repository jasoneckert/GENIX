/*
 * @(#)vm.h	3.29	9/1/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/* Virtual memory definitions */

typedef	long	spt_t;			/* spt index type */


/* The possible kinds of physical pages */
#define	PTY_ZERO 	0	/* unused and zeroed pages */
#define	PTY_FREE	1	/* unused and dirty pages */
#define	PTY_SYSTEM	2	/* kernel page */
#define	PTY_PTE1	3	/* page table level 1 */
#define	PTY_PTE2	4	/* page table level 2 */
#define	PTY_DATA	5	/* swappable pages -- user and u. area */


/* Structure of the special pages table.  This table is used to remember
 * all of the page tables in use by the system.  The first few entries
 * in the table are the page maps for the processes.  Following those is
 * a page map for the system.  Following that are the page maps for files.
 */

struct	spt {
	pte	*spt_pte1;		/* pointer to page table */
	struct	inode	*spt_inode;	/* pointer to inode if a file */
	long	spt_share;		/* share count */
	long	spt_age;		/* age counter for text segments */
	short	spt_proccount;		/* number of processes using text */
	short	spt_usedpages;		/* count of non-empty pte entries */
	short	spt_mempages;		/* count of real memory pages */
	short	spt_tmempages;		/* like mempages, but text only */
	short	spt_tablepages;		/* number of page table pages in use */
	short	spt_reserved;		/* pages reserved to address space */
	short	spt_flags;		/* flag bits */
};

#define	SPT_INIT	0x1		/* text being inited */
#define	SPT_WANT	0x2		/* someone is waiting for it */
#define	SPT_DEAD	0x4		/* no further access allowed */
#define SPT_XSRSS	0x8		/* resident set size is too big */
#define	SPT_NOEIA	0x10		/* address invalidation not needed */

/* Structure of the core status table.  This table contains the status of
 * each physical memory page. The routines addrtocst and csttopg convert
 * between physical memory address and cst entries.  Definitions of fields:
 *
 *  cst_type:	The type of the page.
 *
 *  cst_spti:	The spt index which owns the page.
 *
 *  cst_sptp:	The page number within the owner's virtual address space.
 *
 *  cst_intrans: Nonzero if this page is being read/written to the disk.
 *
 *  cst_wanted:	While the page was in transit, a process wanted it and had
 *		to sleep.  Wakeup is needed when IO finishes.
 *
 *  cst_file:	If zero, then the backup address is on the swapping device.
 *		Otherwise, the backup address is within the file system
 *		indicated by the owning spt index's inode pointer.
 *
 *  cst_lock:	Number of reasons why this page cannot be swapped.
 *		For page maps, an increment for every owned page which is
 *		in core.  Therefore, page maps cannot swap if any of the
 *		owned pages are in core.  Pages are also locked temporarily
 *		for swapping operations or to prevent races.
 *
 *  cst_share:	The share count of the page.  This is the number of page
 *		map entries which point to this physical page.   If the
 *		physical page must be moved, all the relevant page map
 *		entries must be tracked down and changed.  Good news:
 *		All of the required page maps will be in memory.
 *
 *  cst_backup:	The page number on the swap (or file) device where this page is
 *		swapped to/from.  If nonpositive, a location is not assigned.
 *		If the page is from a text file, cst_file is set.
 *
 *  cst_next:	Forward pointer in linked list of pages.  Various lists are
 *		for free pages, zeroed, pages, and pages being swapped.
 */

struct	cst {
	unsigned int cst_wanted:1;	/* someone is waiting for this page */
	unsigned int cst_intrans:1;	/* page is in transit */
	unsigned int cst_file:1;	/* backup address is from a file */
	char	cst_type;		/* type of page */
	short	cst_spti;		/* spt index which owns the page */
	short	cst_sptp;		/* page number in spt address space */
	short	cst_lock;		/* lock count (reasons not to swap) */
	short	cst_share;		/* share count (number of users) */
	long	cst_backup;		/* backup location (swapping block) */
	struct	cst	*cst_next;	/* next page in list (NULL if none) */
};


/* Useful macros */

#define	SYSSPTI		(0)		/* spt index of the system */
#define	SPTISPROC(n)	((n)<=lastprocspti)	/* spt index is for a process */
#define	SPTISSYS(n)	((n)==SYSSPTI)	/* spt belongs to system */
#define	SPTISFILE(n)	((n)>=filespti)	/* spt index is for a file */
#define	BADSPT		((struct spt *) -1)	/* a known bad spt pointer */
#define	BADCST		((struct cst *) -1)	/* a known bad cst pointer */
#define	NOCST(pg)	(((int)pg) <= 0)	/* bad or null cst pointer */
#define	NOSPT(fs)	(((int)fs) <= 0)	/* bad or null spt pointer */
#define	allocpage(s,a,t) alloclistpage(s,a,t,0)		/* allocate free page */
#define	alloczeropage(s,a,t) alloclistpage(s,a,t,1)	/* allocate zero page */


/* The following macros are used to manipulate MCPAGES number of ptes at once.
 * This makes the clustering of memory pages easier to manage.  In all of the
 * following macros, addr is the address of the first pte of a cluster.
 */

#if MCPAGES == 1
#define	getpte(addr)		((addr)[0])
#define	setpte(addr,data)	(addr)[0] = (data)
#define	orpte(addr,data)	(addr)[0] |= (data)
#define	andpte(addr,data)	(addr)[0] &= (data)
#endif

#if MCPAGES == 2
#define	getpte(addr)\
	((addr)[0] | ((addr)[1] & (PG_REF | PG_MOD)))
#define	setpte(addr,data)\
	(addr)[0] = (data);\
	(addr)[1] = (addr)[0];\
	if ((addr)[0] & PG_V) (addr)[1] += PGSIZE
#define	orpte(addr,data)\
	(addr)[0] |= (data);\
	(addr)[1] |= (data)
#define	andpte(addr,data)\
	(addr)[0] &= (data);\
	(addr)[1] &= (data)
#endif


/* Useful external definitions for some data and procedures */

#ifdef KERNEL
extern	struct	cst	*cst;		/* the cst table */
extern	struct	cst	*cstNCST;	/* end of table */
extern	int	ncst;			/* number of cst entries */
extern	struct	spt	*spt;		/* the spt table */
extern	struct	spt	*sptNSPT;	/* end of table */
extern	struct	spt	*sptFILESPTI;	/* first file spt */
extern	int	nspt;			/* number of spt entries */
extern	int	lastprocspti;		/* spt index of last process */
extern	int	filespti;		/* spt index of first text file */
extern	int	topsys;			/* top address of kernel */
extern	int	topmem;			/* last address of real memory */
extern	int	availpages;		/* total memory and swap space */
extern	struct	cst	*addrtocst();	/* convert address to cst pointer */
extern	pte	*pageinpt();		/* get pointer to map entry */
extern	pte	getpgmap();		/* return map entry */
extern	pte	*skippte();		/* skip pte entries */
extern	setpgmap();
#define setmap(spti,addr,data)\
	setpgmap(spti,addr,data,PTLEVEL2)
#define getmap(spti,addr)\
	getpgmap(spti,addr,PTLEVEL2)
#define	lockpage(ptr)	(ptr)->cst_lock++
#define	unlockpage(ptr)	if (--((ptr)->cst_lock) < 0) unlockpanic()
#endif
