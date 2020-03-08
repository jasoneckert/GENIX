/*
 * quot.c: version 1.2 of 2/24/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)quot.c	1.2 (NSC) 2/24/83";
# endif

/* Fixed to account "hole" files correctly, and option added to show
   amount of disc referenced within N days (-a or age flag) -wfj 1/6/81 */
/* Changed to use /etc/fstab if no disc is mentioned -wfj 1/6/81 */

/*
 * Disk usage by user
 */


#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <assert.h>
#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/fblk.h>
#include <sys/filsys.h>
#include <fstab.h>

#define	ITABSZ	256
#define	ISIZ	(BSIZE/sizeof(struct dinode))
#define	NUID	10000
struct	filsys	sblock;
/* The following rather messy definitions allow the inode table to be placed in
 * a page aligned buffer suitable for doing raw io reads directly.
 */
#define	PAGESIZE 512
#define	PAGEOFFSET 511
#define NITBLKS 16
#define ITSIZE (NITBLKS * BSIZE)
char	itbuf[ITSIZE + PAGEOFFSET];
struct  itble
{
	struct	dinode	itb[ITABSZ];
} *it;
#define itab	it->itb
char	iobuf[BSIZE + PAGEOFFSET]; /* Page aligned buffer for rest of raw io. */
char	*iobufptr;
struct du
{
	long	blocks;
	long	nfiles;
	int	uid;
	char	*name;
} du[NUID];
#define	TSIZE	500
int	sizes[TSIZE];
long	overflow;

int	nflg;
int	fflg;
int	cflg;
int	aflg;

int	fi;
unsigned	ino;
unsigned	nfiles;
#define DEFAULT_AGE	30
int	age = DEFAULT_AGE, today, period;

struct	passwd	*getpwent();
char	*malloc();
char	*copy();

main(argc, argv)
char **argv;
{
	register int n;
	register struct passwd *lp;
	register char **p;
	int seenadisc;

	it = (struct itble *)(((int)itbuf + PAGEOFFSET) & (~PAGEOFFSET));
	iobufptr = (char *)(((int)iobuf + PAGEOFFSET) & (~PAGEOFFSET));
	for(n=0; n<NUID; n++)
		du[n].uid = n;
	while((lp=getpwent()) != 0) {
		n = lp->pw_uid;
		if (n>NUID)
			continue;
		if(du[n].name)
			continue;
		du[n].name = copy(lp->pw_name);
	}
	seenadisc = 0;
	while (--argc) {
		argv++;
		if (argv[0][0]=='-') {
			if (argv[0][1]=='n')
				nflg++;
			else if (argv[0][1]=='f')
				fflg++;
			else if (argv[0][1]=='c')
				cflg++;
			else if (argv[0][1]=='a')
				aflg++;
		} else  if ((n = atoi(*argv)) > 0)
				age = n;
		else {
			seenadisc++;
			check(*argv);
			report();
		}
	}
	if ( !seenadisc ) {
		struct fstab *fsp;
		int passno, anygtr, sumstatus = 0;
		if (setfsent() == 0){
			printf("Can't open checklist file: %s\n",
					FSTAB);
			exit(1);
		}
		while ( (fsp = getfsent()) != 0 ){
			if ( !strcmp(FSTAB_RW,fsp->fs_spec)
			|| !strcmp(FSTAB_RO,fsp->fs_spec) ){
				check(fsp->fs_spec);
				report();
			}
		}
	}
	return(0);
}

check(file)
char *file;
{
	register unsigned i, j;
	register c;

	fi = open(file, 0);
	period =  age * 24*60*60;
	time(&today);
	period = today - period;
	if (fi < 0) {
		printf("cannot open %s\n", file);
		return;
	}
	printf("%s:\n", file);
	sync();
	bread(1, (char *)&sblock, sizeof sblock);
	nfiles = (sblock.s_isize-2)*(BSIZE/sizeof(struct dinode));
	ino = 0;
	if (nflg) {
		if (isdigit(c = getchar()))
			ungetc(c, stdin);
		else while (c!='\n' && c != EOF)
			c = getchar();
	}
	for(i=2; ino<nfiles; i += ITABSZ/ISIZ) {
		bread(i, (char *)itab, sizeof itab);
		for (j=0; j<ITABSZ && ino<nfiles; j++) {
			ino++;
			/* check for aged files */
			if (aflg)
				if (itab[j].di_atime <= period) 
					acct(&itab[j]);
				else;
			else
				acct(&itab[j]);
		}
	}
}

acct(ip)
register struct dinode *ip;
{
	register n;
	register char *np;
	static fino;

	if ((ip->di_mode&IFMT) == 0)
		return;
	if (cflg) {
		if ((ip->di_mode&IFMT)!=IFDIR && (ip->di_mode&IFMT)!=IFREG)
			return;
		n = (ip->di_size+BSIZE-1)/BSIZE;
		if (n >= TSIZE) {
			overflow += n;
			n = TSIZE-1;
		}
		sizes[n]++;
		return;
	}
	if (ip->di_uid >= NUID)
		return;
	/* if (((ip->di_size+BSIZE-1)/BSIZE) != siz(ip))
		printf("imag %d,real %d\n",(ip->di_size+BSIZE-1)/BSIZE,siz(ip)); */
	du[ip->di_uid].blocks += siz(ip);
	/* du[ip->di_uid].blocks += (ip->di_size+BSIZE-1)/BSIZE; */
	du[ip->di_uid].nfiles++;
	if (nflg) {
	tryagain:
		if (fino==0)
			if (scanf("%d", &fino)<=0)
				return;
		if (fino > ino)
			return;
		if (fino<ino) {
			while ((n=getchar())!='\n' && n!=EOF)
				;
			fino = 0;
			goto tryagain;
		}
		if (np = du[ip->di_uid].name)
			printf("%.7s	", np);
		else
			printf("%d	", ip->di_uid);
		while ((n = getchar())==' ' || n=='\t')
			;
		putchar(n);
		while (n!=EOF && n!='\n') {
			n = getchar();
			putchar(n);
		}
		fino = 0;
	}
}
/* */


daddr_t	iaddr[NADDR];
siz(ip)
register struct dinode *ip;
{
	daddr_t ind1[NINDIR];
	daddr_t ind2[NINDIR];
	daddr_t ind3[NINDIR];
	register i, j, blks;
	int k, l;

	blks = 0;
	i = ip->di_mode & IFMT;
	if(i == 0) {
		return(0);
	}
	if(i == IFCHR) {
		return(0);
	}
	if(i == IFBLK) {
		return(0);
	}
	l3tol(iaddr, ip->di_addr, NADDR);
	for(i=0; i<NADDR; i++) {
		if(iaddr[i] == 0)
			continue;
		if(i < NADDR-3) {
			blks++;
			continue;
		}
		if (iaddr[i] == 0)
				continue;
		bread(iaddr[i], (char *)ind1, BSIZE);
		for(j=0; j<NINDIR; j++) {
			if(ind1[j] == 0)
				continue;
			if(i == NADDR-3) {
				blks++;
				continue;
			}
			if(iaddr[i] == 0)
				continue;
			bread(ind1[j], (char *)ind2, BSIZE);
			for(k=0; k<NINDIR; k++) {
				if(ind2[k] == 0)
					continue;
				if(i == NADDR-2) {
					blks++;
					continue;
				}
				bread(ind2[k], (char *)ind3, BSIZE);
				for(l=0; l<NINDIR; l++)
					if(ind3[l]) {
						blks++;
					}
			}
		}
	}
	return(blks);
}
/* */
bread(bno, buf, cnt)
unsigned bno;
char *buf;
{
	char *tmpba;

	if (!((long)buf % PAGESIZE) && !(cnt % PAGESIZE)) {
		diskread(bno, buf, cnt);
	} else	{
		tmpba = buf;
		while (cnt > 0)	{
			diskread(bno, iobufptr, BSIZE);
			if (cnt >= BSIZE) 	{
				movebuf(tmpba, iobufptr, BSIZE);
				tmpba += BSIZE;
			} else
				movebuf(tmpba, iobufptr, cnt);
			cnt -= BSIZE;
		}
	}
}

diskread(bno, buf, cnt)
unsigned bno;
char *buf;
{

	lseek(fi, (long)bno*BSIZE, 0);
	if (read(fi, buf, cnt) != cnt) {
		printf("read error %u\n", bno);
		exit(1);
	}
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

qcmp(p1, p2)
register struct du *p1, *p2;
{
	if (p1->blocks > p2->blocks)
		return(-1);
	if (p1->blocks < p2->blocks)
		return(1);
	return(strcmp(p1->name, p2->name));
}

report()
{
	register i;

	if (nflg)
		return;
	if (aflg)
		printf("For files older than %s\n",ctime(&period));
	if (cflg) {
		long t = 0;
		for (i=0; i<TSIZE-1; i++)
			if (sizes[i]) {
				t += i*sizes[i];
				printf("%d	%d	%D\n", i, sizes[i], t);
			}
		printf("%d	%d	%D\n", TSIZE-1, sizes[TSIZE-1], overflow+t);
		return;
	}
	qsort(du, NUID, sizeof(du[0]), qcmp);
	for (i=0; i<NUID; i++) {
		if (du[i].blocks==0)
			return;
		printf("%5D\t", du[i].blocks);
		if (fflg)
			printf("%5D\t", du[i].nfiles);
		if (du[i].name)
			printf("%s\n", du[i].name);
		else
			printf("#%d\n", du[i].uid);
	}
}

char *
copy(s)
char *s;
{
	register char *p;
	register n;

	for(n=0; s[n]; n++)
		;
	p = malloc((unsigned)n+1);
	for(n=0; p[n] = s[n]; n++)
		;
	return(p);
}
