/*
 * @(#)pte.h	3.11	9/2/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

typedef	long pte;	/* should be a structure, but that had problems */


/* Macros to access various parts of a virtual address */
#define	VA_OFFSET	0x000001ff	/* mask for offset portion */
#define	VA_INDEX2	0x0000fe00	/* second level index */
#define	VA_INDEX1	0x00ff0000	/* first level index */
#define	VA_UNUSED	0xff000000	/* unused, illegal */
#define	VA_USED		0x00ffffff	/* legal parts of virtual addresses */
#define	BUILDADDR(i1,i2,off)	(((i1)<<16)|((i2)<<9)|(off))	/* build addr */
#define	MAXPAGES	(128 * 256)	/* maximum number of virtual pages */
#define	MAXPTE2ADDRS	((VA_OFFSET | VA_INDEX2) + 1) /* maximum pte2 address */
#define	MAXADDRS	(MAXPAGES * PGSIZE)	/* maximum number of bytes */


/* Macros to extract offsets of various parts of an address while taking
 * into account the memory cluster size
 */
#define	GETVOFF(d)	((d)&MCOFSET)			/* extract offset */
#define	GETV2OFF(d)	((d)&((1<<(MCSHIFT+7))-1))	/* 2nd level offset */
#define	GETVI1(d)	(((d)>>16)&(-MCPAGES))		/* extract level 1 */
#define	GETVI2(d)	(((d)>>9)&(0x7f*MCPAGES))	/* extract level 2 */


/* bit definitions within a pte */
#define	PG_V		0x00000001	/* page is valid */
#define	PG_PROT		0x00000006	/* protection level (see below) */
#define	PG_REF		0x00000008	/* page has been referenced */
#define	PG_MOD		0x00000010	/* page has been modified */
#define	PG_UNUSED1	0x00000020	/* unused */
#define	PG_UNUSED2	0x00000040	/* unused */
#define	PG_RES1		0x00000080	/* reserved for software (see below) */
#define	PG_RES2		0x00000100	/* reserved for software (see below) */
#define	PG_PGNUM	0xfffffe00	/* page number (or other data ) */
#define	PG_TYPE		(PG_V|PG_RES1|PG_RES2)	/* software defined pte type */
#define	PG_FILE		0x00000200	/* in PT_DISK type, page is from file */


/* Kinds of protections */
#define	PG_RONA		0x00000000	/* supervisor read, user no access */
#define	PG_RWNA		0x00000002	/* sup read/write, user no access */
#define	PG_RWRO		0x00000004	/* sup read/write, user read only */
#define	PG_RWRW		0x00000006	/* sup read/write, user read/write */


/* Kinds of ptes.  Selected by the PG_TYPE field of a pte.  The PTEHASH macro
 * only works because of the location of the PG_V, PG_RES1 and PG_RES2 bits.
 * The PTEISMEM macro works only for specific memory pte type assignments.
 * So be wary of changing these definitions.
 */
#define	PT_NULL		0			/* null pte */
#define	PT_MEM		PG_V			/* normal memory */
#define	PT_SPY		(PG_V | PG_RES1)	/* spying memory */
#define	PT_LOCK		(PG_V | PG_RES2)	/* locked memory */
#define	PT_DISK		PG_RES1			/* disk page (file or swap) */
#define	PT_SPT		PG_RES2			/* indirect through spt */
#define	PT_IOP		(PG_RES1 | PG_RES2)	/* memory page doing I/O */


/* Levels of page tables */
#define PTLEVEL1	1			/* level 1 page table */
#define PTLEVEL2	2			/* level 2 page table */


/* Pte related macros */

#define	PTESIZE	4		/* number of bytes for a pte */
#define	NPTEPG	(NBPG/PTESIZE)	/* number of ptes per page */
#define	PTENULL			((pte) (0))
#define	PTEBUILD(typ,addr,prot)	((pte)(((int)(addr)&PG_PGNUM)|((prot)|(typ))))
#define	PTEMEM(addr,prot)	(PTEBUILD(PT_MEM,addr,prot))
#define	PTELOCK(addr, prot)	(PTEBUILD(PT_LOCK,addr,prot))
#define	PTEIOP(addr, prot)	(PTEBUILD(PT_IOP,addr,prot))
#define	PTESPY(addr,prot)	(PTEBUILD(PT_SPY,addr,prot))
#define	PTELOCK(addr, prot)	(PTEBUILD(PT_LOCK,addr,prot))
#define	PTESPT(idx,page,prot)	(PTEBUILD(PT_SPT,((page)<<9)|((idx)<<24),prot))
#define	PTEDISK(fbit,page,prot)	\
			(PTEBUILD(PT_DISK,((page)<<10)|((fbit)?PG_FILE:0),prot))
#define	PTESWAP(page,prot)	(PTEDISK(0,page,prot))
#define	PTEFILE(page,prot)	(PTEDISK(1,page,prot))

#define	PTETYPE(pte)		(((int)(pte)) & PG_TYPE)
#define	PTEHASH(pte)		(PTETYPE(pte) % 9)
#define	PTEHASH_NULL		(PTEHASH(PT_NULL))
#define	PTEHASH_MEM		(PTEHASH(PT_MEM))
#define	PTEHASH_SPY		(PTEHASH(PT_SPY))
#define	PTEHASH_LOCK		(PTEHASH(PT_LOCK))
#define	PTEHASH_DISK		(PTEHASH(PT_DISK))
#define	PTEHASH_SPT		(PTEHASH(PT_SPT))
#define	PTEHASH_IOP		(PTEHASH(PT_IOP))

#define	PTEPROT(pte)		(((int)(pte)) & PG_PROT)
#define	PTEVALID(pte)		((int)(pte) & PG_V)
#define	PTEINVALID(pte)		(((int)(pte) & PG_V) == 0)
#define	PTEISMEM(pte)		((((int)(pte)) & (PG_V | PG_RES1)) == PG_V)
#define	PTEISSWAP(pte)		(((int)(pte) & (PG_TYPE|PG_FILE)) == PT_DISK)
#define PTEISFILE(pte)	(((int)(pte) & (PG_TYPE|PG_FILE)) == (PT_DISK|PG_FILE))

#define	PTEMEM_ADDR(pte)	((int)(pte) & PG_PGNUM)
#define	PTELOCK_ADDR(pte)	((int)(pte) & PG_PGNUM)
#define	PTEIOP_ADDR(pte)	((int)(pte) & PG_PGNUM)
#define	PTESPY_ADDR(pte)	((int)(pte) & PG_PGNUM)
#define	PTESWAP_PAGE(pte)	((unsigned)(pte) >> 10)
#define	PTEFILE_PAGE(pte)	((unsigned)(pte) >> 10)
#define	PTESPT_INDEX(pte)	((unsigned)(pte) >> 24)
#define	PTESPT_PAGE(pte)	(((unsigned)(pte) >> 9) & 0x7fff)
