/*
 * icheck.c: version 1.7 of 6/13/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)icheck.c	1.7 (NSC) 6/13/83";
# endif

#define	NI	16
#define	NB	500
#define	BITS	8
#define	MAXFN	500
#define	PAGESIZE 512	/* Used to page align buffers for raw io. */
#define	PAGEOFFSET 511
#define	ITABSIZE (NI * BSIZE)
#define min(x,y) (((x)<(y))?(x):(y))

#ifndef STANDALONE
#include <stdio.h>
#endif
#include <assert.h>
#include <sys/param.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/fblk.h>
#include <sys/filsys.h>

struct	filsys	sblock;
char	itbuf[ITABSIZE + PAGEOFFSET];	/* itab will point into this
						 * buffer on a page boundary.
						 */
struct	dinode	*itab;	/* Originally struct dinode itab[INOPB*NI] */
char	iobuf[BSIZE + PAGEOFFSET]; /* Page aligned buffer for rest of raw io. */
char	*iobufptr;
daddr_t	iaddr[NADDR];
daddr_t	blist[NB];
char	*bmap;

int	sflg;
int	mflg;
int	dflg;
int	fi;
ino_t	ino;

ino_t	nrfile;
ino_t	ndfile;
ino_t	nbfile;
ino_t	ncfile;

daddr_t	ndirect;
daddr_t	nindir;
daddr_t	niindir;
daddr_t	niiindir;
daddr_t	nfree;
daddr_t	ndup;

int	nerror;

long	atol();
daddr_t	alloc();
#ifndef STANDALONE
char	*malloc();
#endif

#ifndef STANDALONE
main(argc, argv)
char *argv[];
#else
main()
#endif
{
	register i;
	long n;

	itab = (struct dinode *)(((int)itbuf + PAGEOFFSET) & (~PAGEOFFSET));
	iobufptr = (char *)(((int)iobuf + PAGEOFFSET) & (~PAGEOFFSET));
	blist[0] = -1;
#ifndef STANDALONE
	while (--argc) {
		argv++;
		if (**argv=='-')
		switch ((*argv)[1]) {
		case 'd':
			dflg++;
			continue;


		case 'm':
			mflg++;
			continue;

		case 's':
			sflg++;
			continue;

		case 'b':
			for(i=0; i<NB; i++) {
				n = atol(argv[1]);
				if(n == 0)
					break;
				blist[i] = n;
				argv++;
				argc--;
			}
			blist[i] = -1;
			continue;

		default:
			printf("Bad flag\n");
		}
		check(*argv);
	}
#else
	{
		static char fname[128];

		printf("File: ");
		gets(fname);
		check(fname);
	}
#endif
#ifndef STANDALONE
	return(nerror);
#endif
}

check(file)
char *file;
{
	register i, j;
	ino_t mino;
	daddr_t d;
	long n;

	fi = open(file, sflg?2:0);
	if (fi < 0) {
		printf("cannot open %s\n", file);
		nerror |= 04;
		return;
	}
	printf("%s:\n", file);
	nrfile = 0;
	ndfile = 0;
	ncfile = 0;
	nbfile = 0;

	ndirect = 0;
	nindir = 0;
	niindir = 0;
	niiindir = 0;

	ndup = 0;
#ifndef STANDALONE
	sync();
#endif
	bread((daddr_t)1, (char *)&sblock, sizeof(sblock));
	sblock.s_tinode = 0;
	mino = ((int)sblock.s_isize-2) * INOPB;
	ino = 0;
	n = (sblock.s_fsize - (int)sblock.s_isize + BITS-1) / BITS;
	if (n != (unsigned)n) {
		printf("Check fsize and isize: %d, %u\n",
		   sblock.s_fsize, (int)sblock.s_isize);
	}
#ifdef STANDALONE
	bmap = NULL;
#else
	bmap = malloc((unsigned)n);
#endif
	if (bmap==NULL) {
#ifndef STANDALONE
		printf("Not enough core; duplicates unchecked\n");
#endif
		dflg++;
		sflg = 0;
	}
	if(!dflg)
	for(i=0; i<(unsigned)n; i++)
		bmap[i] = 0;
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
	ino = 0;
#ifndef STANDALONE
	sync();
#endif
	i = sblock.s_tinode;
	bread((daddr_t)1, (char *)&sblock, sizeof(sblock));
	sblock.s_tinode = i;
	if (sflg) {
		makefree();
		close(fi);
#ifndef STANDALONE
		if (bmap)
			free(bmap);
#endif
		return;
	}
	nfree = 0;
	while(n = alloc()) {
		if (chk(n, "free"))
			break;
		nfree++;
	}
	close(fi);
#ifndef STANDALONE
	if (bmap)
		free(bmap);
#endif

	i = nrfile + ndfile + ncfile + nbfile;
#ifndef STANDALONE
	printf("files %u (r=%u,d=%u,b=%u,c=%u)\n",
		i, nrfile, ndfile, nbfile, ncfile);
#else
	printf("files %u (r=%u,d=%u,b=%u,c=%u)\n",
		i, nrfile, ndfile, nbfile, ncfile);
#endif
	n = ndirect + nindir + niindir + niiindir;
#ifdef STANDALONE
	printf("used %d (i=%d,ii=%d,iii=%d,d=%d)\n",
		n, nindir, niindir, niiindir, ndirect);
	printf("free %d\n", nfree);
#else
	printf("used %d (i=%d,ii=%d,iii=%d,d=%d)\n",
		n, nindir, niindir, niiindir, ndirect);
	printf("free %d\n", nfree);
#endif
	if(!dflg) {
		n = 0;
		for(d=(int)sblock.s_isize; d<sblock.s_fsize; d++)
			if(!duped(d)) {
				if(mflg)
					printf("%d missing\n", d);
				n++;
			}
		printf("missing %d\n", n);
	}
}

pass1(ip)
register struct dinode *ip;
{
	daddr_t ind1[NINDIR];
	daddr_t ind2[NINDIR];
	daddr_t ind3[NINDIR];
	register i, j;
	int k, l;

	i = ip->di_mode & IFMT;
	if(i == 0) {
		sblock.s_tinode++;
		return;
	}
	if(i == IFCHR) {
		ncfile++;
		return;
	}
	if(i == IFBLK) {
		nbfile++;
		return;
	}
	if(i == IFDIR)
		ndfile++; else
	if(i == IFREG)
		nrfile++;
	else {
		printf("bad mode    ino = %u  mode = %o\n", ino,i);
		return;
	}
	l3tol(iaddr, ip->di_addr, NADDR);
	for(i=0; i<NADDR; i++) {
		if(iaddr[i] == 0)
			continue;
		if(i < NADDR-3) {
			ndirect++;
			chk(iaddr[i], "data (small)");
			continue;
		}
		nindir++;
		if (chk(iaddr[i], "1st indirect"))
				continue;
		bread(iaddr[i], (char *)ind1, BSIZE);
		for(j=0; j<NINDIR; j++) {
			if(ind1[j] == 0)
				continue;
			if(i == NADDR-3) {
				ndirect++;
				chk(ind1[j], "data (large)");
				continue;
			}
			niindir++;
			if(chk(ind1[j], "2nd indirect"))
				continue;
			bread(ind1[j], (char *)ind2, BSIZE);
			for(k=0; k<NINDIR; k++) {
				if(ind2[k] == 0)
					continue;
				if(i == NADDR-2) {
					ndirect++;
					chk(ind2[k], "data (huge)");
					continue;
				}
				niiindir++;
				if(chk(ind2[k], "3rd indirect"))
					continue;
				bread(ind2[k], (char *)ind3, BSIZE);
				for(l=0; l<NINDIR; l++)
					if(ind3[l]) {
						ndirect++;
						chk(ind3[l], "data (garg)");
					}
			}
		}
	}
}

chk(bno, s)
daddr_t bno;
char *s;
{
	register n;

	if (bno<(int)sblock.s_isize || bno>=sblock.s_fsize) {
		printf("%d bad; inode=%u, class=%s\n", bno, ino, s);
		return(1);
	}
	if(duped(bno)) {
		printf("%d dup; inode=%u, class=%s\n", bno, ino, s);
		ndup++;
	}
	for (n=0; blist[n] != -1; n++)
		if (bno == blist[n])
			printf("%d arg; inode=%u, class=%s\n", bno, ino, s);
	return(0);
}

duped(bno)
daddr_t bno;
{
	daddr_t d;
	register m, n;

	if(dflg)
		return(0);
	d = bno - (int)sblock.s_isize;
	m = 1 << (d%BITS);
	n = (d/BITS);
	if(bmap[n] & m)
		return(1);
	bmap[n] |= m;
	return(0);
}

daddr_t
alloc()
{
	int i;
	daddr_t bno;
	union {
		char	data[BSIZE];
		struct	fblk fb;
	} buf;

	sblock.s_tfree--;
	if (sblock.s_nfree<=0)
		return(0);
	if (sblock.s_nfree>NICFREE) {
		printf("Bad free list, s.b. count = %d\n", sblock.s_nfree);
		return(0);
	}
	bno = sblock.s_free[--sblock.s_nfree];
	sblock.s_free[sblock.s_nfree] = (daddr_t)0;
	if(bno == 0)
		return(bno);
	if(sblock.s_nfree <= 0) {
		bread(bno, buf.data, BSIZE);
		sblock.s_nfree = buf.fb.df_nfree;
		if (sblock.s_nfree<0 || sblock.s_nfree>NICFREE) {
			printf("Bad free list, entry count of block %d = %d\n",
				bno, sblock.s_nfree);
			sblock.s_nfree = 0;
			return(0);
		}
		for(i=0; i<NICFREE; i++)
			sblock.s_free[i] = buf.fb.df_free[i];
	}
	return(bno);
}

bfree(bno)
daddr_t bno;
{
	union {
		char	data[BSIZE];
		struct	fblk fb;
	} buf;
	int i;

	if (bno)
		sblock.s_tfree++;
	if(sblock.s_nfree >= NICFREE) {
		for(i=0; i<BSIZE; i++)
			buf.data[i] = 0;
		buf.fb.df_nfree = sblock.s_nfree;
		for(i=0; i<NICFREE; i++)
			buf.fb.df_free[i] = sblock.s_free[i];
		bwrite(bno, buf.data);
		sblock.s_nfree = 0;
	}
	sblock.s_free[sblock.s_nfree] = bno;
	sblock.s_nfree++;
}

/* */
bread(bno, buf, cnt)
daddr_t bno;
char *buf;
{
	char *tmpba;
	register i, size;

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
		if (sflg) {
			printf("No update\n");
			sflg = 0;
		}
		return(0);
	} else return(1);
}


/* Move data between two buffers quickly */
movebuf(dest, src, len)
char	*dest, *src;
{
#ifdef	STANDALONE
	while (len-- > 0) *dest++ = *src++;
#else

	assert((len % 4) == 0);
	len /= 4;
	asm("	movd	20(fp),r0	");	/* doubleword count */
	asm("	movd	16(fp),r1	");	/* source string */
	asm("	movd	12(fp),r2	");	/* destination address */
	asm("	movsd			");	/* do the copy */
#endif
}

bwrite(bno, buf)
daddr_t bno;
char	*buf;
{

	lseek(fi, bno*BSIZE, 0);
	movebuf(iobufptr, buf, BSIZE);
	if (write(fi, iobufptr, BSIZE) != BSIZE)
		printf("write error %d\n", bno);
}

makefree()
{
	char flg[MAXFN];
	int adr[MAXFN];
	register i, j;
	daddr_t f, d;
	int m, n;

	n = sblock.s_n;
	if(n <= 0 || n > MAXFN)
		n = MAXFN;
	sblock.s_n = n;
	m = sblock.s_m;
	if(m <= 0 || m > sblock.s_n)
		m = 3;
	sblock.s_m = m;

	for(i=0; i<n; i++)
		flg[i] = 0;
	i = 0;
	for(j=0; j<n; j++) {
		while(flg[i])
			i = (i+1)%n;
		adr[j] = i+1;
		flg[i]++;
		i = (i+m)%n;
	}

	sblock.s_nfree = 0;
	sblock.s_ninode = 0;
	sblock.s_flock = 0;
	sblock.s_ilock = 0;
	sblock.s_fmod = 0;
	sblock.s_ronly = 0;
#ifndef STANDALONE
	time(&sblock.s_time);
#endif
	sblock.s_tfree = 0;

	bfree((daddr_t)0);
	d = sblock.s_fsize-1;
	while(d%sblock.s_n)
		d++;
	for(; d > 0; d -= sblock.s_n)
	for(i=0; i<sblock.s_n; i++) {
		f = d - adr[i];
		if(f < sblock.s_fsize && f >= (int)sblock.s_isize)
			if(!duped(f))
				bfree(f);
	}
	bwrite((daddr_t)1, (char *)&sblock);
#ifndef STANDALONE
	sync();
#endif
	return;
}
