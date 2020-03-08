/*
 * mkpt.c: version 1.1 of 10/7/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)mkpt.c	1.1 (NSC) 10/7/83";
# endif

#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/saio.h>
#include <sys/disk.h>

#define NULL 0
#define HEADERBLOCK 0

static union diskun {
	struct disktab tab;
	char pad[PGSIZE];
}dun;

#define dtsize sizeof(dun)
#define disktab dun.tab

char *baddev = "Bad device name\n";
char *openfail = "Open failure: %s\n";
char *iofail = "Header %s failed\n";

char devname[20];			/* device name buffer */
static int curunit = -1;		/* the current unit in core */
static int maxcyl,maxblocks;		/* should be obvious */
static int partition;

main()
{
	struct partition *getpt();
	register struct partition *pp;
	int offset,nblocks;

	for(;;) {
		pp = getpt(devname);
		printf("Current offset %d, size %d\n",pp->cyloff,pp->nblocks);
		printf("Cylinder offset(0-%d)? ",maxcyl);
		offset = check(0,maxcyl);
		if (offset < 0)
			continue;
		maxblocks -= offset * disktab.dt_secpercyl;
		printf("size(0-%d)? ",maxblocks);
		nblocks = check(0,maxblocks);
		if (nblocks < 0)
			continue;
		pp->cyloff = offset;
		pp->nblocks = nblocks;
		update(devname);
	}
}

struct partition*
getpt(str)
char *str;
{
	register char *cp;
	register struct iob *file;
	register int pt;
	int fn;

restart:
	printf("Disk? ");
	gets(str);
	if (*str == 0) _stop("Done\n");
	for (cp = str; *cp && *cp != ','; cp++)
			;
	if (*cp++ != ',') {
		printf(baddev);
		goto restart;
	}
	pt = *cp++ - '0';
	if (*cp >= '0' && *cp <= '9')
		pt = pt * 10 + *cp++ - '0';

	if (pt < 0 || pt > 7) {
		printf("Bad offset specification\n");
		goto restart;
	}
	while ( *cp && *cp++ != ')' )
		;
	if (*cp != '\0') {
		printf(baddev);
		goto restart;
	}
	if((fn = open(str,0)) < 0) {
		printf(openfail,str);
		goto restart;
	}
	file = &iob[fn-3];
	if (file->i_unit != curunit) {
		/*
	 	 * We have to be able to get at sector 0, so we fudge
	 	 * the iob.
	 	 */
		file->i_boff = HEADERBLOCK;
		file->i_cyloff = 2;
		/* read the header */
		if (read(fn,&disktab,dtsize) != dtsize) {
			printf(iofail,"read");
			close(fn);
			goto restart;
		}
		/* Verify the header */
		if (disktab.dt_magic != DMAGIC) {
			printf("Unit %d has no header\n",file->i_unit);
			close(fn);
			goto restart;
		}
		curunit = file->i_unit;
	}
	maxcyl = disktab.dt_ncylinders-1;
	maxblocks = disktab.dt_secperunit;
	partition = pt;
	close(fn);
	return(&disktab.dt_partition[pt]);
}

update(name)
char *name;
{
	int fd;
	register struct iob *file;

	fd = open(name,1);
	if (fd < 0) {
		printf(openfail,name);
		return;
	}
	file = &iob[fd-3];
	file->i_boff = HEADERBLOCK;
	file->i_cyloff = 2;
	if (write(fd,&disktab,dtsize) != dtsize) {
		printf(iofail,"write");
	}
	close(fd);
}

getnum()
{
	int c;
	char buffer[30];		/* a string buffer */
	register int n,base;
	register char *cp = buffer;

	gets(cp);
	n = 0;
	base = 10;
	while(c = *cp++) {
		if(c<'0' || c>'9') {
			printf("%s: bad number\n", buffer);
			return(-1);
		}
		n = n*base + (c-'0');
	}
	return(n);
}

check(lo,hi)
register int lo,hi;
{
	register int n;

	n = getnum();
	if (n < lo || n > hi) {
		printf("number out of range\n");
		return(-1);
	}
	return(n);
}

