/*
 * ncheck.c: version 1.2 of 3/17/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)ncheck.c	1.2 (NSC) 3/17/83";
# endif

/*
 * ncheck -- obtain file names from reading filesystem
 */

#define	NI	16
#define	NB	500
#define	HSIZE	2503
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
struct	htab
{
	ino_t	h_ino;
	ino_t	h_pino;
	char	h_name[DIRSIZ];
} htab[HSIZE];

int	aflg;
int	sflg;
int	fi;
ino_t	ino;
int	nhent;
int	nxfile;

int	nerror;
daddr_t	bmap();
long	atol();
struct htab *lookup();

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

		case 'a':
			aflg++;
			continue;

		case 'i':
			for(i=0; i<NB; i++) {
				n = atol(argv[1]);
				if(n == 0)
					break;
				ilist[i] = n;
				nxfile = i;
				argv++;
				argc--;
			}
			continue;

		case 's':
			sflg++;
			continue;

		default:
			fprintf(stderr, "ncheck: bad flag %c\n", (*argv)[1]);
			nerror++;
		}
		check(*argv);
	}
	return(nerror);
}

check(file)
char *file;
{
	register i, j;
	ino_t mino;

	fi = open(file, 0);
	if(fi < 0) {
		fprintf(stderr, "ncheck: cannot open %s\n", file);
		nerror++;
		return;
	}
	nhent = 0;
	printf("%s:\n", file);
	sync();
	bread((daddr_t)1, (char *)&sblock, sizeof(sblock));
	mino = (sblock.s_isize-2) * INOPB;
	ino = 0;
	for(i=2;; i+=NI) {
		if(ino >= mino)
			break;
		bread((daddr_t)i, (char *)itab, ITABSIZE);
		for(j=0; j<INOPB*NI; j++) {
			if(ino >= mino)
				break;
			ino++;
			pass1(&itab[j]);
		}
	}
	ilist[nxfile+1] = 0;
	ino = 0;
	for(i=2;; i+=NI) {
		if(ino >= mino)
			break;
		bread((daddr_t)i, (char *)itab, ITABSIZE);
		for(j=0; j<INOPB*NI; j++) {
			if(ino >= mino)
				break;
			ino++;
			pass2(&itab[j]);
		}
	}
	ino = 0;
	for(i=2;; i+=NI) {
		if(ino >= mino)
			break;
		bread((daddr_t)i, (char *)itab, ITABSIZE);
		for(j=0; j<INOPB*NI; j++) {
			if(ino >= mino)
				break;
			ino++;
			pass3(&itab[j]);
		}
	}
}

pass1(ip)
register struct dinode *ip;
{
	if((ip->di_mode & IFMT) != IFDIR) {
		if (sflg==0 || nxfile>=NB)
			return;
		if ((ip->di_mode&IFMT)==IFBLK || (ip->di_mode&IFMT)==IFCHR
		  || ip->di_mode&(ISUID|ISGID))
			ilist[nxfile++] = ino;
			return;
	}
	lookup(ino, 1);
}

pass2(ip)
register struct dinode *ip;
{
	struct direct dbuf[NDIR];
	long doff;
	struct direct *dp;
	register i, j;
	int k;
	struct htab *hp;
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
		bread(d, (char *)dbuf, sizeof(dbuf));
		for(j=0; j<NDIR; j++) {
			if(doff >= ip->di_size)
				break;
			doff += sizeof(struct direct);
			dp = dbuf+j;
			kno = dp->d_ino;
			if(kno == 0)
				continue;
			hp = lookup(kno, 0);
			if(hp == 0)
				continue;
			if(dotname(dp))
				continue;
			hp->h_pino = ino;
			for(k=0; k<DIRSIZ; k++)
				hp->h_name[k] = dp->d_name[k];
		}
	}
}

pass3(ip)
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
		bread(d, (char *)dbuf, sizeof(dbuf));
		for(j=0; j<NDIR; j++) {
			if(doff >= ip->di_size)
				break;
			doff += sizeof(struct direct);
			dp = dbuf+j;
			kno = dp->d_ino;
			if(kno == 0)
				continue;
			if(aflg==0 && dotname(dp))
				continue;
			if(ilist[0] == 0)
				goto pr;
			for(k=0; ilist[k] != 0; k++)
				if(ilist[k] == kno)
					goto pr;
			continue;
		pr:
			printf("%u	", kno);
			pname(ino, 0);
			printf("/%.14s", dp->d_name);
			if (lookup(kno, 0))
				printf("/.");
			printf("\n");
		}
	}
}

dotname(dp)
register struct direct *dp;
{

	if (dp->d_name[0]=='.')
		if (dp->d_name[1]==0 || (dp->d_name[1]=='.' && dp->d_name[2]==0))
			return(1);
	return(0);
}

pname(i, lev)
ino_t i;
{
	register struct htab *hp;

	if (i==ROOTINO)
		return;
	if ((hp = lookup(i, 0)) == 0) {
		printf("???");
		return;
	}
	if (lev > 10) {
		printf("...");
		return;
	}
	pname(hp->h_pino, ++lev);
	printf("/%.14s", hp->h_name);
}

struct htab *
lookup(i, ef)
ino_t i;
{
	register struct htab *hp;

	for (hp = &htab[i%HSIZE]; hp->h_ino;) {
		if (hp->h_ino==i)
			return(hp);
		if (++hp >= &htab[HSIZE])
			hp = htab;
	}
	if (ef==0)
		return(0);
	if (++nhent >= HSIZE) {
		fprintf(stderr, "ncheck: out of core-- increase HSIZE\n");
		exit(1);
	}
	hp->h_ino = i;
	return(hp);
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
		fprintf(stderr, "ncheck: read error %d\n", bno);
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
		fprintf(stderr, "ncheck: %u - huge directory\n", ino);
		return((daddr_t)0);
	}
	bread(iaddr[NADDR-3], (char *)ibuf, sizeof(ibuf));
	return(ibuf[i]);
}
