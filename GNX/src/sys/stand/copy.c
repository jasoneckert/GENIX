/*
 * copy.c: version 2.8 of 5/31/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)copy.c	2.8 (NSC) 5/31/83";
# endif
/* Copy a file system from the VAX to the 16K */

#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/filsys.h>

#define DCREAD 0		/* THESE SHOULD BE DEFINED IN ONE PLACE */
#define DCWRITE 1
#define	PAGESIZE 512
#define	PAGEOFFSET 511

#define IPAGECNT 24		/* max pages in an IEEE transfer */
#define IBUFSIZE (PAGESIZE * IPAGECNT)
#define DPAGECNT 24		/* max pgs in a disk transfer */
#define DBUFSIZE (PAGESIZE * DPAGECNT)

static struct filsys fil;
static struct filsys *sbptr = &fil;
char diskbuf[DBUFSIZE];

main()
{
char *ptr;
char *curptr;
int Vfildes,Kfildes,i,j, bytecount, error;
int thisdxframt, thisIExframt, ndiskxfrs, nIExfrs;
int fssize;
long firstblock,numblock;

ptr = (char *)((int)diskbuf);
printf("COPY -- Copy file system from VAX (/dev/rup0a) to 16K\n");

Vfildes = ieopen("/dev/rup0a",0);
if (Vfildes <= 0) _stop("Couldn't open /dev/rup0a\n");
printf("\n16K filesystem: ");
gets(diskbuf); Kfildes = open(diskbuf,1);
if (Kfildes <= 0) _stop("Couldn't open 16K filesystem\n");

printf("\nFirst block to transfer (<return> is first): ");
gets(diskbuf); firstblock = atol(diskbuf);

printf("\nNumber of blocks to transfer (<return> for everything): ");
gets(diskbuf); numblock = atol(diskbuf);
if (numblock == 0) {
   ielseek(Vfildes,(SUPERB*2)*PAGESIZE,0);
   ieread(Vfildes,sbptr,sizeof(fil));
   fssize = sbptr->s_fsize;
   printf("Size of file structure = %d\n",fssize);
   numblock = fssize*2;
   if (firstblock) numblock -= firstblock;
   }
ielseek(Vfildes,firstblock*PAGESIZE,0);
ndiskxfrs = (numblock + (DPAGECNT -1))/DPAGECNT; /* no. of disk xfrs needed */
if (numblock)
   printf("%d disk transfers required\n",ndiskxfrs);

for (i=0; (i < ndiskxfrs)||(numblock==0); i++) {
/* calculate how much to transfer to the disk this time */
   if ((numblock == 0)||(i < (ndiskxfrs - 1)))
        thisdxframt = DPAGECNT;
   else thisdxframt = numblock - (ndiskxfrs - 1)*DPAGECNT;
/* now calculate how many IEEE transfers to move this much disk */
   nIExfrs = (thisdxframt + (IPAGECNT - 1))/IPAGECNT;
/* Now read the data into the disk buffer */
   printf("Reading %d pages from ie...",thisdxframt);
   curptr = ptr; bytecount = 0;
   for (j=0; j<nIExfrs; j++) {
      thisIExframt = IPAGECNT;
      if (j==(nIExfrs - 1))
	 thisIExframt = thisdxframt - (IPAGECNT*(nIExfrs-1));
      thisIExframt *= PAGESIZE;		/* convert to bytes */
      error = ieread(Vfildes,curptr,thisIExframt);
      if (error <= 0) {
	 printf("Unsuccessful read from VAX\n");
	 break;
	 }
      bytecount += error;
      curptr += error;
      if (error < thisIExframt) break; /* IEEE has given us all it can */
      }
   printf("\n");
   if (bytecount == 0) break;
   lseek(Kfildes,firstblock*PAGESIZE,0);
   error=write(Kfildes,ptr,((bytecount+PAGEOFFSET)& ~PAGEOFFSET));
   if (error != bytecount) {
      printf("write failed, %d bytes transferred\n",error);
      _stop(0);
      }
   firstblock += (bytecount + PAGEOFFSET) / PAGESIZE;
   printf("Wrote disk to %dth sector\n", firstblock - 1);
   }

printf("%d blocks (512 bytes) transferred\n",firstblock);
ieclose(Vfildes);
close(Kfildes);
}
