/*
 * clri.c: version 1.2 of 3/17/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)clri.c	1.2 (NSC) 3/17/83";
# endif
/*
 * clri filsys inumber ...
 */

#include <sys/param.h>
#include <sys/ino.h>

#define ISIZE	(sizeof(struct dinode))
#define	NI	(BSIZE/ISIZE)
#define PAGESIZE	512	/* Used to page align buffer for raw io. */
#define PAGEOFFSET	511
struct	ino
{
	char	junk[ISIZE];
};
char	iobuf[BSIZE + PAGEOFFSET];	/* buf will point into this storage area
					 * on a page boundary.
					 */
struct	ino	*buf;	/* Originally struct ino buf[NI]. */
int	status;

main(argc, argv)
char *argv[];
{
	register i, f;
	unsigned n;
	int j, k;
	long off;

	if(argc < 3) {
		printf("usage: clri filsys inumber ...\n");
		exit(4);
	}
	buf = (struct ino *)(((int)iobuf + PAGEOFFSET) & (~PAGEOFFSET));
	f = open(argv[1], 2);
	if(f < 0) {
		printf("cannot open %s\n", argv[1]);
		exit(4);
	}
	for(i=2; i<argc; i++) {
		if(!isnumber(argv[i])) {
			printf("%s: is not a number\n", argv[i]);
			status = 1;
			continue;
		}
		n = atoi(argv[i]);
		if(n == 0) {
			printf("%s: is zero\n", argv[i]);
			status = 1;
			continue;
		}
		off = itod(n) * BSIZE;
		lseek(f, off, 0);
		if(read(f, (char *)buf, BSIZE) != BSIZE) {
			printf("%s: read error\n", argv[i]);
			status = 1;
		}
	}
	if(status)
		exit(status);
	for(i=2; i<argc; i++) {
		n = atoi(argv[i]);
		printf("clearing %u\n", n);
		off = itod(n) * BSIZE;
		lseek(f, off, 0);
		read(f, (char *)buf, BSIZE);
		j = itoo(n);
		for(k=0; k<ISIZE; k++)
			buf[j].junk[k] = 0;
		lseek(f, off, 0);
		write(f, (char *)buf, BSIZE);
	}
	exit(status);
}

isnumber(s)
char *s;
{
	register c;

	while(c = *s++)
		if(c < '0' || c > '9')
			return(0);
	return(1);
}
