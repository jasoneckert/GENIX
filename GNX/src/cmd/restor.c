/*
 * restor.c: version 1.8 of 10/14/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)restor.c	1.8 (NSC) 10/14/83";
# endif


#define MAXINO	3000
#define BITS	8
#define MAXXTR	600
#define NCACHE	3

#ifndef STANDALONE
#include <stdio.h>
#include <signal.h>
#include <assert.h>
#endif
#include <sys/param.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/fblk.h>
#include <sys/filsys.h>
#include <sys/dir.h>
#include <errno.h>
#include <dumprestor.h>

#ifdef STANDALONE
#undef NTREC
#define NTREC 100
#define bread read
#define bwrite write
#endif
#define	MWORD(m,i) (m[(unsigned)(i-1)/MLEN])
#define	MBIT(i)	(1<<((unsigned)(i-1)%MLEN))
#define	BIS(i,w)	(MWORD(w,i) |=  MBIT(i))
#define	BIC(i,w)	(MWORD(w,i) &= ~MBIT(i))
#define	BIT(i,w)	(MWORD(w,i) & MBIT(i))

#define clearbuf(bp) movebuf(bp,zeroes,BSIZE)

struct	filsys	sblock;

int	fi;
ino_t	ino, maxi, curino;

int	mt;
char	tapename[] = "/dev/rtc0";
char	*magtape = tapename;
#ifdef STANDALONE
char	mbuf[50];
#endif

#ifndef STANDALONE
daddr_t	seekpt;
int	df, ofile;
char	dirfile[] = "rstXXXXXX";

struct {
	ino_t	t_ino;
	daddr_t	t_seekpt;
} inotab[MAXINO];
int	ipos;

#define ONTAPE	1
#define XTRACTD	2
#define XINUSE	4
struct xtrlist {
	ino_t	x_ino;
	char	x_flags;
} xtrlist[MAXXTR];

char	name[12];

char	drblock[BSIZE];
int	bpt;
#endif

int	eflag;

int	volno = 1;

struct dinode tino, dino;
daddr_t	taddr[NADDR];

daddr_t	curbno;

short	dumpmap[MSIZ];
short	clrimap[MSIZ];

char *writerr = "disk write error %d\n"; /* disk error message */

int bct = NTREC+1;
static char *tbf;		/* the page aligned tape buffer base */
static char *zeroes;		/* a pointer to a zeroed page */

/* The disk block cache */
struct	cache {
	daddr_t	c_bno;
	int	c_dirty;
	int	c_time;
	char	*c_block;
} cache[NCACHE];
int	curcache;

/* The inode block cache */
struct cache icache[NCACHE];
int	icurcache;

/* The cache buffer space for the block cache, and the inode cache */
char	cachebuf[2*NCACHE*BSIZE+PGOFSET];

struct cache *getdino();

#ifndef STANDALONE
main(argc, argv)
char *argv[];
#else
main()
#endif
{
	register char *cp;
	char command;
	int done();
	/* a buffer space for the tape, and a block of zeroes */
	char	mem[NTREC*BSIZE+BSIZE+PGOFSET];

	bufinit(mem);		/* setup all the page aligned buffers */
#ifndef STANDALONE
	mktemp(dirfile);
	if (argc < 2) {
usage:
		printf("Usage: restor x file file..., restor r filesys, or restor t\n");
		exit(1);
	}
	argv++;
	argc -= 2;
	for (cp = *argv++; *cp; cp++) {
		switch (*cp) {
		case '-':
			break;
		case 'f':
			magtape = *argv++;
			argc--;
			break;
		case 'r':
		case 'R':
		case 't':
		case 'x':
			command = *cp;
			break;
		default:
			printf("Bad key character %c\n", *cp);
			goto usage;
		}
	}
	if (command == 'x') {
		if (signal(SIGINT, done) == SIG_IGN)
			signal(SIGINT, SIG_IGN);
		if (signal(SIGTERM, done) == SIG_IGN)
			signal(SIGTERM, SIG_IGN);

		df = creat(dirfile, 0666);
		if (df < 0) {
			printf("restor: %s - cannot create directory temporary\n", dirfile);
			exit(1);
		}
		close(df);
		df = open(dirfile, 2);
	}
	doit(command, argc, argv);
	cflsh();
	if (command == 'x')
		unlink(dirfile);
	exit(0);
#else
	magtape = "tape";
	doit('r', 1, 0);
	cflsh();
	printf("restor: done\n");
	for (;;) ;
#endif
}

/*
 * bufinit - initialize the page aligned buffers.
 */

bufinit(buf)
register char *buf;
{
	register char *cbp;
	register int i;

	/* Initialize the disk block cache ... */
	cbp = (char *) (((int) cachebuf + PGOFSET) &~ PGOFSET);
	for (i=0;i<NCACHE;i++){
		cache[i].c_block = cbp;
		cbp += BSIZE;
	}

	/* ... and the inode cache ... */
	for (i=0;i<NCACHE;i++){
		icache[i].c_block = cbp;
		cbp += BSIZE;
	}

	/* Allocate and initialize a block of zeroes ... */
	cbp = (char *) (((int) buf + PGOFSET) &~ PGOFSET);
	zeroes = cbp;
	i = BSIZE;
	do
		*cbp++ = 0;
	while (--i);
	/* ... and finally, initialize the tape buffer pointer */
	tbf = cbp;

}

doit(command, argc, argv)
char	command;
int	argc;
char	*argv[];
{
	extern char *ctime();
	register i, k;
	ino_t	d;
#ifndef STANDALONE
	int	xtrfile(), skip(), null();
#endif
	int	rstrfile(), rstrskip();
	struct dinode *ip, *ip1;

#ifndef STANDALONE
	if ((mt = open(magtape, 0)) < 0) {
		printf("%s: cannot open tape\n", magtape);
		exit(1);
	}
#else
	do {
		printf("Tape? ");
		gets(mbuf);
		mt = open(mbuf, 0);
	} while (mt == -1);
	magtape = mbuf;
#endif
	switch(command) {
#ifndef STANDALONE
	case 't':
		if (readhdr(&spcl) == 0) {
			printf("Tape is not a dump tape\n");
			exit(1);
		}
		printf("Dump   date: %s", ctime(&spcl.c_date));
		printf("Dumped from: %s", ctime(&spcl.c_ddate));
		return;
	case 'x':
		if (readhdr(&spcl) == 0) {
			printf("Tape is not a dump tape\n");
			exit(1);
		}
		if (checkvol(&spcl, 1) == 0) {
			printf("Tape is not volume 1 of the dump\n");
			exit(1);
		}
		pass1();  /* This sets the various maps on the way by */
		i = 0;
		while (i < MAXXTR-1 && argc--) {
			if ((d = psearch(*argv)) == 0 || BIT(d, dumpmap) == 0) {
				printf("%s: not on the tape\n", *argv++);
				continue;
			}
			xtrlist[i].x_ino = d;
			xtrlist[i].x_flags |= XINUSE;
			printf("%s: inode %u\n", *argv, d);
			argv++;
			i++;
		}
newvol:
		flsht();
		close(mt);
getvol:
		printf("Mount desired tape volume: Specify volume #: ");
		if (gets(tbf) == NULL)
			return;
		volno = atoi(tbf);
		if (volno <= 0) {
			printf("Volume numbers are positive numerics\n");
			goto getvol;
		}
		mt = open(magtape, 0);
		if (readhdr(&spcl) == 0) {
			printf("tape is not dump tape\n");
			goto newvol;
		}
		if (checkvol(&spcl, volno) == 0) {
			printf("Wrong volume (%d)\n", spcl.c_volume);
			goto newvol;
		}
rbits:
		while (gethead(&spcl) == 0)
			;
		if (checktype(&spcl, TS_INODE) == 1) {
			printf("Can't find inode mask!\n");
			goto newvol;
		}
		if (checktype(&spcl, TS_BITS) == 0)
			goto rbits;
		readbits(dumpmap);
		i = 0;
		for (k = 0; xtrlist[k].x_flags; k++) {
			if (BIT(xtrlist[k].x_ino, dumpmap)) {
				xtrlist[k].x_flags |= ONTAPE;
				i++;
			}
		}
		while (i > 0) {
again:
			if (ishead(&spcl) == 0)
				while(gethead(&spcl) == 0)
					;
			if (checktype(&spcl, TS_END) == 1) {
				printf("end of tape\n");
checkdone:
				for (k = 0; xtrlist[k].x_flags; k++)
					if ((xtrlist[k].x_flags&XTRACTD) == 0)
						goto newvol;
					return;
			}
			if (checktype(&spcl, TS_INODE) == 0) {
				gethead(&spcl);
				goto again;
			}
			d = spcl.c_inumber;
			for (k = 0; xtrlist[k].x_flags; k++) {
				if (d == xtrlist[k].x_ino) {
					printf("extract file %u\n", xtrlist[k].x_ino);
					sprintf(name, "%u", xtrlist[k].x_ino);
					if ((ofile = creat(name, 0666)) < 0) {
						printf("%s: cannot create file\n", name);
						i--;
						continue;
					}
					chown(name, spcl.c_dinode.di_uid, spcl.c_dinode.di_gid);
					getfile(ino, xtrfile, skip, spcl.c_dinode.di_size);
					i--;
					xtrlist[k].x_flags |= XTRACTD;
					close(ofile);
					goto done;
				}
			}
			getfile(d, null, null, spcl.c_dinode.di_size);
done:
			;
		}
		goto checkdone;
#endif
	case 'r':
	case 'R':
#ifndef STANDALONE
		if ((fi = open(*argv, 2)) < 0) {
			printf("%s: cannot open\n", *argv);
			exit(1);
		}
#else
		do {
			char charbuf[50];

			printf("Disk? ");
			gets(charbuf);
			fi = open(charbuf, 2);
		} while (fi == -1);
#endif
#ifndef STANDALONE
		if (command == 'R') {
			printf("Enter starting volume number: ");
			if (gets(tbf) == EOF) {
				volno = 1;
				printf("\n");
			}
			else
				volno = atoi(tbf);
		}
		else
#endif
			volno = 1;
		printf("Last chance before scribbling on %s. ",
#ifdef STANDALONE
								"disk");
		gets(mbuf);
#else
								*argv);
		while (getchar() != '\n');
#endif
		dread((daddr_t)1, (char *)&sblock, sizeof(sblock));
		maxi = (sblock.s_isize-2)*INOPB;
		if (readhdr(&spcl) == 0) {
			printf("Missing volume record\n");
			exit(1);
		}
		if (checkvol(&spcl, volno) == 0) {
			printf("Tape is not volume %d\n", volno);
			exit(1);
		}
		gethead(&spcl);
		for (;;) {
ragain:
			if (ishead(&spcl) == 0) {
				printf("Missing header block\n");
				while (gethead(&spcl) == 0)
					;
				eflag++;
			}
			if (checktype(&spcl, TS_END) == 1) {
				printf("End of tape\n");
				close(mt);
				dwrite( (daddr_t) 1, (char *) &sblock);
				return;
			}
			if (checktype(&spcl, TS_CLRI) == 1) {
				readbits(clrimap);
				for (ino = 1; ino <= maxi; ino++)
					if (BIT(ino, clrimap) == 0) {
						getdino(ino, &tino);
						if (tino.di_mode == 0)
							continue;
						itrunc(&tino);
						clri(&tino);
						putdino(ino, &tino);
					}
				dwrite( (daddr_t) 1, (char *) &sblock);
				goto ragain;
			}
			if (checktype(&spcl, TS_BITS) == 1) {
				readbits(dumpmap);
				goto ragain;
			}
			if (checktype(&spcl, TS_INODE) == 0) {
				printf("Unknown header type\n");
				eflag++;
				gethead(&spcl);
				goto ragain;
			}
			ino = spcl.c_inumber;
			if (eflag)
				printf("Resynced at inode %u\n", ino);
			eflag = 0;
			if (ino > maxi) {
				printf("%u: ilist too small\n", ino);
				gethead(&spcl);
				goto ragain;
			}
			dino = spcl.c_dinode;
			getdino(ino, &tino);
			curbno = 0;
			itrunc(&tino);
			clri(&tino);
			for (i = 0; i < NADDR; i++)
				taddr[i] = 0;
			l3tol(taddr, dino.di_addr, 1);
			getfile(d, rstrfile, rstrskip, dino.di_size);
			ip = &tino;
			ltol3(ip->di_addr, taddr, NADDR);
			ip1 = &dino;
			ip->di_mode = ip1->di_mode;
			ip->di_nlink = ip1->di_nlink;
			ip->di_uid = ip1->di_uid;
			ip->di_gid = ip1->di_gid;
			ip->di_size = ip1->di_size;
			ip->di_atime = ip1->di_atime;
			ip->di_mtime = ip1->di_mtime;
			ip->di_ctime = ip1->di_ctime;
			putdino(ino, &tino);
		}
	}
}
/*
 * Read the tape, bulding up a directory structure for extraction
 * by name
 */
#ifndef STANDALONE
pass1()
{
	register i;
	struct dinode *ip;
	int	putdir(), null();

	while (gethead(&spcl) == 0) {
		printf("Can't find directory header!\n");
	}
	for (;;) {
		if (checktype(&spcl, TS_BITS) == 1) {
			readbits(dumpmap);
			continue;
		}
		if (checktype(&spcl, TS_CLRI) == 1) {
			readbits(clrimap);
			continue;
		}
		if (checktype(&spcl, TS_INODE) == 0) {
finish:
			flsh();
			close(mt);
			return;
		}
		ip = &spcl.c_dinode;
		i = ip->di_mode & IFMT;
		if (i != IFDIR) {
			goto finish;
		}
		inotab[ipos].t_ino = spcl.c_inumber;
		inotab[ipos++].t_seekpt = seekpt;
		getfile(spcl.c_inumber, putdir, null, spcl.c_dinode.di_size);
		putent("\000\000/");
	}
}
#endif

/*
 * Do the file extraction, calling the supplied functions
 * with the blocks
 */
getfile(n, f1, f2, size)
ino_t	n;
int	(*f2)(), (*f1)();
long	size;
{
	register i;
	struct spcl addrblock;
	char buffer[BSIZE+PGOFSET];
	register char *buf;

	buf = (char *) (((int) buffer + PGOFSET) &~ PGOFSET);
	addrblock = spcl;
	curino = n;
	goto start;
	for (;;) {
		if (gethead(&addrblock) == 0) {
			printf("Missing address (header) block\n");
			goto eloop;
		}
		if (checktype(&addrblock, TS_ADDR) == 0) {
			spcl = addrblock;
			curino = 0;
			return;
		}
start:
		for (i = 0; i < addrblock.c_count; i++) {
			if (addrblock.c_addr[i]) {
				readtape(buf);
				(*f1)(buf, size > BSIZE ? (long) BSIZE : size);
			}
			else {
				clearbuf(buf);
				(*f2)(buf, size > BSIZE ? (long) BSIZE : size);
			}
			if ((size -= BSIZE) <= 0) {
eloop:
				while (gethead(&spcl) == 0)
					;
				if (checktype(&spcl, TS_ADDR) == 1)
					goto eloop;
				curino = 0;
				return;
			}
		}
	}
}

/*
 * Do the tape i\/o, dealling with volume changes
 * etc..
 */

readtape(b)
char *b;
{
	register i;
	struct spcl tmpbuf;

	if (bct >= NTREC) {
		for (i = 0; i < NTREC; i++)
			((struct spcl *)&tbf[i*BSIZE])->c_magic = 0;
		bct = 0;
		if ((i = bread(mt, tbf, NTREC*BSIZE)) < 0) {
			printf("Tape read error: inode %u\n", curino);
			eflag++;
			exit(1);
		}
		if (i == 0) {
			bct = NTREC + 1;
			volno++;
loop:
			flsht();
			close(mt);
			printf("Mount volume %d\n", volno);
			while (getchar() != '\n')
				;
			if ((mt = open(magtape, 0)) == -1) {
				printf("Cannot open tape!\n");
				goto loop;
			}
			if (readhdr(&tmpbuf) == 0) {
				printf("Not a dump tape.Try again\n");
				goto loop;
			}
			if (checkvol(&tmpbuf, volno) == 0) {
				printf("Wrong tape. Try again\n");
				goto loop;
			}
			readtape(b);
			return;
		}
	}
	movebuf(b, &tbf[(bct++*BSIZE)], BSIZE);
}

flsht()
{
	bct = NTREC+1;
}

/*
clearbuf(cp)
char *cp;
{
	movebuf(cp, zeroes, BSIZE);
}
*/

/*
 * Put and get the directory entries from the compressed
 * directory file
 */
#ifndef STANDALONE
putent(cp)
char	*cp;
{
	register i;

	for (i = 0; i < sizeof(ino_t); i++)
		writec(*cp++);
	for (i = 0; i < DIRSIZ; i++) {
		writec(*cp);
		if (*cp++ == 0)
			return;
	}
	return;
}

getent(bf)
register char *bf;
{
	register i;

	for (i = 0; i < sizeof(ino_t); i++)
		*bf++ = readc();
	for (i = 0; i < DIRSIZ; i++)
		if ((*bf++ = readc()) == 0)
			return;
	return;
}

/*
 * read/write te directory file
 */
writec(c)
char c;
{
	drblock[bpt++] = c;
	seekpt++;
	if (bpt >= BSIZE) {
		bpt = 0;
		write(df, drblock, BSIZE);
	}
}

readc()
{
	if (bpt >= BSIZE) {
		read(df, drblock, BSIZE);
		bpt = 0;
	}
	return(drblock[bpt++]);
}

mseek(pt)
daddr_t pt;
{
	bpt = BSIZE;
	lseek(df, pt, 0);
}

flsh()
{
	write(df, drblock, bpt+1);
}

/*
 * search the directory inode ino
 * looking for entry cp
 */
ino_t
search(inum, cp)
ino_t	inum;
char	*cp;
{
	register i;
	struct direct dir;

	for (i = 0; i < MAXINO; i++)
		if (inotab[i].t_ino == inum) {
			goto found;
		}
	return(0);
found:
	mseek(inotab[i].t_seekpt);
	do {
		getent((char *)&dir);
		if (direq(dir.d_name, "/"))
			return(0);
	} while (direq(dir.d_name, cp) == 0);
	return(dir.d_ino);
}

/*
 * Search the directory tree rooted at inode 2
 * for the path pointed at by n
 */
psearch(n)
char	*n;
{
	register char *cp, *cp1;
	char c;

	ino = 2;
	if (*(cp = n) == '/')
		cp++;
next:
	cp1 = cp + 1;
	while (*cp1 != '/' && *cp1)
		cp1++;
	c = *cp1;
	*cp1 = 0;
	ino = search(ino, cp);
	if (ino == 0) {
		*cp1 = c;
		return(0);
	}
	*cp1 = c;
	if (c == '/') {
		cp = cp1+1;
		goto next;
	}
	return(ino);
}

direq(s1, s2)
register char *s1, *s2;
{
	register i;

	for (i = 0; i < DIRSIZ; i++)
		if (*s1++ == *s2) {
			if (*s2++ == 0)
				return(1);
		} else
			return(0);
	return(1);
}
#endif

/*
 * read/write a disk block, be sure to update the buffer
 * cache if needed.
 */
dwrite(bno, b)
daddr_t	bno;
char	*b;
{
	register i;

	for (i = 0; i < NCACHE; i++) {
		if (cache[i].c_bno == bno) {
			movebuf(cache[i].c_block, b, BSIZE);
			cache[i].c_time = 0;
			cache[i].c_dirty++;
			return;
		}
		else
			cache[i].c_time++;
	}
	lseek(fi, bno*BSIZE, 0);
	if(bwrite(fi, b, BSIZE) != BSIZE) {
#ifdef STANDALONE
		printf(writerr, bno);
#else
		fprintf(stderr, writerr, bno);
#endif
		exit(1);
	}
}

dread(bno, buf, cnt)
daddr_t bno;
char *buf;
{
	register struct cache *cp;
	register i, j;

	j = 0;
	for (i = 0; i < NCACHE; i++) {
		if (++curcache >= NCACHE)
			curcache = 0;
		if (cache[curcache].c_bno == bno) {
			movebuf(buf, cache[curcache].c_block, cnt);
			cache[curcache].c_time = 0;
			return;
		}
		else {
			cache[curcache].c_time++;
			if (cache[j].c_time < cache[curcache].c_time)
				j = curcache;
		}
	}

	cp = &cache[j];
	if (cp->c_dirty){
		lseek(fi, cp->c_bno*BSIZE, 0);
		if(bwrite(fi, cp->c_block, BSIZE) != BSIZE) {
#ifdef STANDALONE
		    printf(writerr, cp->c_bno);
#else
		    fprintf(stderr,writerr, cp->c_bno);
#endif
			exit(1);
		}
		cp->c_dirty = 0;
	}
	lseek(fi, bno*BSIZE, 0);
	if (bread(fi, cp->c_block, BSIZE) != BSIZE) {
#ifdef STANDALONE
		printf("read error %D\n", bno);
#else
		printf("read error %ld\n", bno);
#endif
		exit(1);
	}
	movebuf(buf, cp->c_block, cnt);
	cp->c_time = 0;
	cp->c_bno = bno;
}

/*
 *	cflsh - flush out all write behind blocks left in the
 *	disk cache.
 */

cflsh()
{
	register struct cache *cp;
	register int i;

	for(i=0; i<NCACHE; i++) {
		cp = &cache[i];
		if (cp->c_dirty){
			lseek(fi, cp->c_bno*BSIZE, 0);
			if(bwrite(fi, cp->c_block, BSIZE) != BSIZE) {
#ifdef STANDALONE
			    printf(writerr, cp->c_bno);
#else
			    fprintf(stderr,writerr, cp->c_bno);
#endif
				exit(1);
			}
		}
		cp->c_dirty = 0;
	}
	for(i=0; i<NCACHE; i++) {
		cp = &icache[i];
		if (cp->c_dirty){
			lseek(fi, cp->c_bno*BSIZE, 0);
			if(bwrite(fi, cp->c_block, BSIZE) != BSIZE) {
#ifdef STANDALONE
			    printf(writerr, cp->c_bno);
#else
			    fprintf(stderr,writerr, cp->c_bno);
#endif
				exit(1);
			}
		}
		cp->c_dirty = 0;
	}
}

/*
 * the inode manpulation routines. Like the system.
 *
 * clri zeros the inode
 */
clri(ip)
struct dinode *ip;
{
	int i, *p;
	if (ip->di_mode&IFMT)
		sblock.s_tinode++;
	i = sizeof(struct dinode)/sizeof(int);
	p = (int *)ip;
	do
		*p++ = 0;
	while(--i);
}

/*
 * itrunc/tloop/bfree free all of the blocks pointed at by the inode
 */
itrunc(ip)
register struct dinode *ip;
{
	register i;
	daddr_t bn, iaddr[NADDR];

	if (ip->di_mode == 0)
		return;
	i = ip->di_mode & IFMT;
	if (i != IFDIR && i != IFREG)
		return;
	l3tol(iaddr, ip->di_addr, NADDR);
	for(i=NADDR-1;i>=0;i--) {
		bn = iaddr[i];
		if(bn == 0) continue;
		switch(i) {

		default:
			bfree(bn);
			break;

		case NADDR-3:
			tloop(bn, 0, 0);
			break;

		case NADDR-2:
			tloop(bn, 1, 0);
			break;

		case NADDR-1:
			tloop(bn, 1, 1);
		}
	}
	ip->di_size = 0;
}

tloop(bn, f1, f2)
daddr_t	bn;
int	f1, f2;
{
	register i;
	daddr_t nb;
	union {
		char	data[BSIZE];
		daddr_t	indir[NINDIR];
	} ibuf;

	dread(bn, ibuf.data, BSIZE);
	for(i=NINDIR-1;i>=0;i--) {
		nb = ibuf.indir[i];
		if(nb) {
			if(f1)
				tloop(nb, f2, 0);
			else
				bfree(nb);
		}
	}
	bfree(bn);
}

bfree(bn)
daddr_t	bn;
{
	register i;
	union {
		char	data[BSIZE];
		struct	fblk frees;
	} fbun;
#define	fbuf fbun.frees

	if(sblock.s_nfree >= NICFREE) {
		fbuf.df_nfree = sblock.s_nfree;
		for(i=0;i<NICFREE;i++)
			fbuf.df_free[i] = sblock.s_free[i];
		sblock.s_nfree = 0;
		dwrite(bn, fbun.data);
	}
	sblock.s_free[sblock.s_nfree++] = bn;
	sblock.s_tfree++;
}

/*
 * allocate a block off the free list.
 */
daddr_t
balloc()
{
	daddr_t	bno;
	register i;
	union {
		char	data[BSIZE];
		struct	fblk frees;
	} fbun;
#undef	fbuf
#define	fbuf fbun.frees

	if(sblock.s_nfree == 0 || (bno=sblock.s_free[--sblock.s_nfree]) == 0) {
#ifdef STANDALONE
		printf("Out of space\n");
#else
		fprintf(stderr, "Out of space.\n");
#endif
		exit(1);
	}
	if(sblock.s_nfree == 0) {
		dread(bno, (char *)&fbuf, BSIZE);
		sblock.s_nfree = fbuf.df_nfree;
		for(i=0;i<NICFREE;i++)
			sblock.s_free[i] = fbuf.df_free[i];
	}
	sblock.s_tfree--;
	return(bno);
}

/*
 * map a block number into a block address, ensuring
 * all of the correct indirect blocks are around. Allocate
 * the block requested.
 */
daddr_t
bmap(iaddr, bn)
daddr_t	iaddr[NADDR];
daddr_t	bn;
{
	register i;
	int j, sh;
	daddr_t nb, nnb;
	daddr_t indir[NINDIR];

	/*
	 * blocks 0..NADDR-4 are direct blocks
	 */
	if(bn < NADDR-3) {
		iaddr[bn] = nb = balloc();
		return(nb);
	}

	/*
	 * addresses NADDR-3, NADDR-2, and NADDR-1
	 * have single, double, triple indirect blocks.
	 * the first step is to determine
	 * how many levels of indirection.
	 */
	sh = 0;
	nb = 1;
	bn -= NADDR-3;
	for(j=3; j>0; j--) {
		sh += NSHIFT;
		nb <<= NSHIFT;
		if(bn < nb)
			break;
		bn -= nb;
	}
	if(j == 0) {
		return((daddr_t)0);
	}

	/*
	 * fetch the address from the inode
	 */
	if((nb = iaddr[NADDR-j]) == 0) {
		iaddr[NADDR-j] = nb = balloc();
		dwrite(nb, zeroes);
	}

	/*
	 * fetch through the indirect blocks
	 */
	for(; j<=3; j++) {
		dread(nb, (char *)indir, BSIZE);
		sh -= NSHIFT;
		i = (bn>>sh) & NMASK;
		nnb = indir[i];
		if(nnb == 0) {
			nnb = balloc();
			indir[i] = nnb;
			dwrite(nb, (char *)indir);
			dwrite(nnb, zeroes);
		}
		nb = nnb;
	}
	return(nb);
}

/*
 * read the tape into buf, then return whether or
 * or not it is a header block.
 */

gethead(buf)
struct spcl *buf;
{
	readtape((char *)buf);
	if (buf->c_magic != MAGIC || checksum((int *) buf) == 0)
		return(0);
	return(1);
}

/*
 * return whether or not the buffer contains a header block
 */
ishead(buf)
struct spcl *buf;
{
	if (buf->c_magic != MAGIC || checksum((int *) buf) == 0)
		return(0);
	return(1);
}

checktype(b, t)
struct	spcl *b;
int	t;
{
	return(b->c_type == t);
}


checksum(b)
int *b;
{
	register i, j;

	j = BSIZE/sizeof(int);
	i = 0;
	do
		i += *b++;
	while (--j);
	if (i != CHECKSUM) {
		printf("Checksum error %o\n", i);
		return(0);
	}
	return(1);
}

checkvol(b, t)
struct spcl *b;
int t;
{
	if (b->c_volume == t)
		return(1);
	return(0);
}

readhdr(b)
struct	spcl *b;
{
	if (gethead(b) == 0)
		return(0);
	if (checktype(b, TS_TAPE) == 0)
		return(0);
	return(1);
}

/*
 * The next routines are called during file extraction to
 * put the data into the right form and place.
 */
#ifndef STANDALONE
xtrfile(b, size)
char	*b;
long	size;
{
	write(ofile, b, (int) size);
}

null() {;}

skip()
{
	lseek(ofile, (long) BSIZE, 1);
}
#endif


rstrfile(b, s)
char *b;
long s;
{
	daddr_t d;

	d = bmap(taddr, curbno);
	dwrite(d, b);
	curbno += 1;
}

rstrskip(b, s)
char *b;
long s;
{
	curbno += 1;
}

#ifndef STANDALONE
putdir(b)
char *b;
{
	register struct direct *dp;
	register i;

	for (dp = (struct direct *) b, i = 0; i < BSIZE; dp++, i += sizeof(*dp)) {
		if (dp->d_ino == 0)
			continue;
		putent((char *) dp);
	}
}
#endif

/*
 * read/write an inode from the disk. first search the inode cache for
 * the corresponding block, if it is in memory, return the inode, if it
 * isn't in memory, select the oldest block, writing it to disk if it
 * is dirty, and read it in.
 */

struct cache *
getdino(inum, b)
ino_t	inum;
register struct	dinode *b;
{
	daddr_t	bno;
	register struct cache *ic;
	register struct	dinode *ip;
	register i, j;

	bno = (inum - 1)/INOPB;
	bno += 2;
	j = 0;
	for (i = 0; i < NCACHE; i++) {
		if (++icurcache >= NCACHE)
			icurcache = 0;
		ic = &icache[icurcache];
		if (ic->c_bno == bno) {
			goto incore;
		}
		else {
			ic->c_time++;
			if (icache[j].c_time < ic->c_time)
				j = icurcache;
		}
	}
	ic = &icache[j];
	if (ic->c_dirty) {
		lseek(fi, ic->c_bno*BSIZE, 0);
		bwrite(fi, ic->c_block, BSIZE);
		ic->c_dirty = 0;
	}
	lseek(fi, bno*BSIZE, 0);
	bread(fi, ic->c_block, BSIZE);
	ic->c_bno = bno;
incore:
	ip = &((struct dinode *)ic->c_block)[((inum-1)%INOPB)];
	*b = *ip;
	ic->c_time = 0;
	return(ic);
}

putdino(inum, b)
ino_t	inum;
register struct	dinode *b;
{
	daddr_t bno;
	register struct dinode *ip;
	register struct cache *ic;
	struct dinode junk;

	if (b->di_mode&IFMT)
		sblock.s_tinode--;
	/* force the correct disk block into memory by doing dummy get */
	ic = (struct cache*)getdino(inum, &junk);
	ip = &((struct dinode *)ic->c_block)[((inum-1)%INOPB)];
	*ip = *b;
	ic->c_dirty++;
	ic->c_time = 0;
}

/*
 * read a bit mask from the tape into m.
 */
readbits(m)
short	*m;
{
	register i;

	i = spcl.c_count;

	while (i--) {
		readtape((char *) m);
		m += (BSIZE/(MLEN/BITS));
	}
	while (gethead(&spcl) == 0)
		;
}

#ifndef STANDALONE
long
bwrite(fd,ma,size)
int fd;
char *ma;
register int size;
{
	register int nwrite,cursize;
	register char *iobufptr,*buf;
	char	iobuf[BSIZE+PGOFSET];

	buf = ma;
	if (!((long)buf % NBPG) && !(size % BSIZE)) {
		return(write(fd, buf, size));
	} else	{
		iobufptr = (char *) (((int) iobuf + PGOFSET) &~ PGOFSET);
		cursize = BSIZE;
		while (size > 0)	{
			if (size < BSIZE) cursize = size;
			movebuf(iobufptr, buf, cursize);
			if ((nwrite = write(fd, iobufptr, BSIZE)) < 0) 
				return(cursize);
			if (nwrite == BSIZE) {
				size -= cursize;
				buf += cursize;
			} else {
				size = 0;
				buf += nwrite;
			}
		}
	}
	return(buf-ma);
}

long
bread(fd,ma,size)
int fd;
char *ma;
register int size;
{
	register int bsize = BSIZE;
	register int nread;
	register char *iobufptr,*buf;
	char	iobuf[BSIZE+PGOFSET];

	buf = ma;
	if (!((long)buf % NBPG) && !(size % NBPG)) {
		return(read(fd, buf, size));
	} else	{
		iobufptr = (char *) (((int) iobuf + PGOFSET) &~ PGOFSET);
		while (size > 0) {
			if ((nread = read(fd, iobufptr, bsize)) < 0) {
				return(nread);
			} else {
				if (nread == bsize) {
					if (size < bsize) nread = size;
					movebuf(buf, iobufptr, nread);
					buf += nread;
					size -= nread;
				} else {
					size = 0; /* terminate the loop */
				}
			}
		}
	}
	return(buf-ma);
}
#endif

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

done()
{
#ifndef STANDALONE
	unlink(dirfile);
#endif
	cflsh();
	exit(0);
}
