/*
 * ie.c: version 2.6 of 8/18/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)ie.c	2.6 (NSC) 8/18/83";
# endif
/*  device driver for GPIB interface bus  */

#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/saio.h>

int ropen = 1;			/* remember if dev has been opened for read */
int wopen = 1;			/*    or for write */
int rfildes,rwfildes;		/* file descriptors for reading, writing */

dieopen (io)

register struct iob *io;

{
if (ropen)
   {ropen = 0;			/* not first read open any more */
    rfildes = ieopen("/dev/up0a",0);
    if (rfildes <= 0) _stop("Couldn't open ie for read\n");
   }
}

dieclose(io)

register struct iob *io;

{
if (ropen == 0)
   {ieclose(rfildes); ropen = 1;}
if (wopen == 0)
   {ieclose(rwfildes); wopen = 1;}
}

diestrategy(io,func)

   register struct iob *io;
   register func;
{
register char *ptr;

if (ropen != 0) _stop("diestrategy - device not open\n");
if ((func != READ) && (func !=WRITE))
   _stop("diestrategy - func not READ or WRITE\n");

/* printf("diestrategy - io->i_offset = %d\n",io->i_offset);
 * printf("diestrategy - io->i_bn = %d\n",io->i_bn);
 */
ptr = io->i_ma;
if (func == READ)
   {
   ielseek(rfildes,io->i_bn * 512,0);
   io->i_cc = ieread(rfildes,ptr,io->i_cc);
/* printf("diestrategy -- returning %d chars\n",io->i_cc); */
   }
else
   {
    if (wopen != 0)
    {
    printf("diestrategy -- opening ie device for write!!!\n");
    rwfildes = ieopen("/dev/up0a",2);
    if (rwfildes <= 0) _stop("Couldn't open ie device for writing\n");
    wopen = 0;
    }
    ielseek(rwfildes,io->i_bn * 512,0);
    io->i_cc = iewrite(rwfildes,ptr,io->i_cc);
   }
return(io->i_cc);
}
