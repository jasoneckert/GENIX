/*
 * %M%: version %I% of %G%
 * %Y%
 */
# ifdef SCCS
static char *sccsid = "%W% (NSC) %G%";
# endif

/* Disk to disk copy for the 16K */

#include <sys/param.h>
#include <sys/dcusaio.h>
#include <sys/dcusize.h>

#define DCREAD 0		/* THESE SHOULD BE DEFINED IN ONE PLACE */
#define DCWRITE 1
#define	PAGESIZE 512
#define	PAGEOFFSET 511

#define DPAGECNT 1		/* max pgs in a disk transfer */
#define DBUFSIZE (PAGESIZE * DPAGECNT)

main()
{
	char diskbuf[DBUFSIZE + PAGEOFFSET];
	char *diskaddr;
	int fromunit, tounit, subunit;
	int errflg;
	int i, firstsector, sectorcount;

	diskaddr = (char *)(((int)diskbuf + PAGEOFFSET) & (~PAGEOFFSET));
	printf("DISKCOPY -- Disk to disk copy for the 16K\n\n");

	diskbuf[0] = '\0';
	while (diskbuf[0] < '0' || diskbuf[0] > '9') {
		printf("Source unit: ");
		gets(diskbuf);
	}
	fromunit = atol(diskbuf);
	if ((errflg = initunit(fromunit)) != NOERROR) {
		printf("Disk error %x on unit %d.\n",fromunit, errflg);
		exit();
	}

	diskbuf[0] = '\0';
	while (diskbuf[0] < '0' || diskbuf[0] > '9') {
		printf("Destination unit: ");
		gets(diskbuf);
	}
	tounit = atol(diskbuf);
	if (fromunit == tounit) {
		printf("Source and destination are the same\n");
		exit();
	}
	if ((errflg = initunit(tounit)) != NOERROR) {
		printf("Disk error %x on unit %d.\n",fromunit, errflg);
		exit();
	}

	if (unittable[fromunit] != unittable[tounit]) {
		printf("Disks are not of same type\n");
		exit();
	}

	printf("\nDisk on unit %d will be overwritten\n",tounit);
	printf("Type \"P\" to proceed: ");
	gets(diskbuf);
	if ((diskbuf[0] != 'P') && (diskbuf[0] != 'p')) exit();

	diskbuf[0] = '\0';
	while (diskbuf[0] < '0' || diskbuf[0] > '7') {
		printf("\nSubunit (typical is 0 or 6): ");
		gets(diskbuf);
	}
	subunit = atol(diskbuf);

	firstsector = unittable[fromunit]->subsizes[subunit].cyloff
		      * unittable[fromunit]->secpercyl;
	sectorcount = unittable[fromunit]->subsizes[subunit].nblocks;
	printf("%d sectors to transfer\n",sectorcount);

	for (i = 0 ; i < sectorcount ; i++) {
		errflg = dodiskio(DCREAD, fromunit, firstsector, 1, diskaddr);
		if (errflg) {
			printf("disk read returned error %x on sector %d\n",
				errflg,firstsector);
			exit();
		}
		errflg = dodiskio(DCWRITE, tounit, firstsector, 1, diskaddr);
		if (errflg) {
			printf("disk write returned error %x on sector %d\n",
				errflg,firstsector);
			exit();
		}
		firstsector++;
		if (i%100 == 0) printf("%d\n",i);
	}
	printf("Transfer complete, %d sectors transferred\n",sectorcount);
}
