/*
 * dcheck.c: version 1.2 of 3/11/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)dcheck.c	1.2 (NSC) 3/11/83";
# endif

/*
 * dcheck - check directory consistency
 */
#define	NI	16
#define	NB	10
#define	NDIR	(BSIZE/sizeof(struct direct))
#define	PAGESIZE 512	/* Used to page align buffers for raw io. */
#define	PAGEOFFSET 511
#define ITABSIZE (NI * BSIZE)
#define min(x,y) (((x)<(y))?(x):(y))

#include <stdio.h>
#include <assert.h>
#include <sys/param.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/dir.h>
#include <sys/filsys.h>
#include <sys/fblk.h>


struct	filsys	sblock;
char	itbuf[ITABSIZE + PAGEOFFSET];	/* itab will point into this
						 * buffer on a page boundary.
						 */
struct	dinode	*itab;	/* Originally struct dinode itab[INOPB*NI] */
char	iobuf[BSIZE + PAGEOFFSET]; /* Page aligned buffer for rest of raw io. */
char	*iobufptr;
daddr_t	iaddr[NADDR];
ino_t	ilist[NB];

int	fi;
ino_t	ino;
char	*ecount;
int	headpr;
unsigned	nfiles;

int	nerror;
daddr_t	bmap();
long	atol();
char	*malloc();

main(argc, argv)
char *argv[];
{
	register i;
	long n;

	itab = (struct dinode *)(((int)itbuf + PAGEOFFSET) & (~PAGEOFFSET));
	iobufptr = (char *)(((int)iobuf + PAGEOFFSET) & (~PAGEOFFSET));
	while (--argc) {
		argv++;
		if (**argv=='-')
		switch ((*argv)[1]) {

		case 'i':
			for(i=0; i<NB; i++) {
				n = atol(argv[1]);
				if(n == 0)
					break;
				ilist[i] = n;
				argv++;
				argc--;
			}
			ilist[i] = 0;
			continue;

		default:
			printf("Bad flag %c\n", (*argv)[1]);
			nerror++;
		}
		check(*argv);
	}
	return(nerror);
}

check(file)
char *file;
{
	register i;
	register j;

	fi = open(file, 0);
	if(fi < 0) {
		printf("cannot open %s\n", file);
		nerror++;
		return;
	}
	headpr = 0;
	printf("%s:\n", file);
	sync();
	bread((daddr_t)1, (char *)&sblock, sizeof(sblock));
	nfiles = (sblock.s_isize-2)*INOPB;
	if (nfiles > 250000) {
		printf("Only doing 250000 files\n");
		nfiles = 250000;
	}
	ecount = malloc(nfiles+1);
	if (ecount==NULL) {
		printf("Not enough core\n");
		exit(04);
	}
	for (i=0; i<=nfiles; i++)
		ecount[i] = 0;
	ino = 0;
	for(i=2;; i+=NI) {
		if(ino >= nfiles)
			break;
		bread((daddr_t)i, (char *)itab, ITABSIZE);
		for(j=0; j<INOPB*NI; j++) {
			if(ino >= nfiles)
				break;
			ino++;
			pass1(&itab[j]);
		}
	}
	ino = 0;
	for(i=2;; i+=NI) {
		if(ino >= nfiles)
			break;
		bread((daddr_t)i, (char *)itab, ITABSIZE);
		for(j=0; j<INOPB*NI; j++) {
			if(ino >= nfiles)
				break;
			ino++;
			pass2(&itab[j]);
		}
	}
	free(ecount);
}

pass1(ip)
register struct dinode *ip;
{
	struct direct dbuf[NDIR];
	long doff;
	struct direct *dp;
	register i, j;
	int k;
	daddr_t d;
	ino_t kno;

	if((ip->di_mode&IFMT) != IFDIR)
		return;
	l3tol(iaddr, ip->di_addr, NADDR);
	doff = 0;
	for(i=0;; i++) {
		if(doff >= ip->di_size)
			break;
		d = bmap(i);
		if(d == 0)
			break;
		bread(d, (char *)dbuf, BSIZE);
		for(j=0; j<NDIR; j++) {
			if(doff >= ip->di_size)
				break;
			doff += sizeof(struct direct);
			dp = &dbuf[j];
			kno = dp->d_ino;
			if(kno == 0)
				continue;
			if(kno > nfiles || kno <= 1) {
				printf("%5u bad; %u/%.14s\n", kno, ino, dp->d_name);
				nerror++;
				continue;
			}
			for (k=0; ilist[k] != 0; k++)
				if (ilist[k]==kno) {
					printf("%5u arg; %u/%.14s\n", kno, ino, dp->d_name);
					nerror++;
				}
			ecount[kno]++;
			if (ecount[kno] == 0)
				ecount[kno] = 0377;
		}
	}
}

pass2(ip)
register struct dinode *ip;
{
	register i;

	i = ino;
	if ((ip->di_mode&IFMT)==0 && ecount[i]==0)
		return;
	if (ip->di_nlink==((ecount[i])&0377) && ip->di_nlink!=0)
		return;
	if (headpr==0) {
		printf("     entries  link cnt\n");
		headpr++;
	}
	printf("%u	%d	%d\n", ino,
	    ecount[i]&0377, ip->di_nlink);
}

/* */
bread(bno, buf, cnt)
daddr_t bno;
char *buf;
{
	char *tmpba;
	int i, size;

	if (!((long)buf % PAGESIZE) && !(cnt % PAGESIZE)) {
		if (diskread(bno, buf, cnt) == 0)
			for(i=0; i<(min(cnt, BSIZE)); i++)
				buf[i] = 0;
	} else	{
		tmpba = buf;
		size = cnt;
		while (size > 0)	{
			if (diskread(bno, iobufptr, BSIZE) == 0) {
				for(i=0; i< (min(cnt, BSIZE)); i++)
					buf[i] = 0;
				size = 0;
			} else {
				if (size >= BSIZE) 	{
					movebuf(tmpba, iobufptr, BSIZE);
					tmpba += BSIZE;
				} else
					movebuf(tmpba, iobufptr, size);
				size -= BSIZE;
			}
		}
	}
}

diskread(bno, buf, cnt)
daddr_t bno;
char *buf;
{

	register i;

	lseek(fi, (long)bno*BSIZE, 0);
	if (read(fi, buf, cnt) != cnt) {
		printf("read error %d\n", bno);
		return(0);
	} else return(1);
}


/* Move data between two buffers quickly */
movebuf(dest, src, len)
char	*dest, *src;
{
	/*
	while (len-- > 0) *dest++ = *src++;
	*/

	assert((len % 4) == 0);
	len /= 4;
	asm("	movd	20(fp),r0	");	/* doubleword count */
	asm("	movd	16(fp),r1	");	/* source string */
	asm("	movd	12(fp),r2	");	/* destination address */
	asm("	movsd			");	/* do the copy */
}

daddr_t
bmap(i)
{
	daddr_t ibuf[NINDIR];

	if(i < NADDR-3)
		return(iaddr[i]);
	i -= NADDR-3;
	if(i > NINDIR) {
		printf("%u - huge directory\n", ino);
		return((daddr_t)0);
	}
	bread(iaddr[NADDR-3], (char *)ibuf, sizeof(ibuf));
	return(ibuf[i]);
}
