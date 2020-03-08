/*
 * conf.c: version 2.7 of 8/18/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)conf.c	2.7 (NSC) 8/18/83";
# endif
/*  device configuration table for stand-alone programs */

#include <sys/param.h>
#include <sys/inode.h>
#include <sys/saio.h>

int nullsys();
int diestrategy(), dieopen(), dieclose();
int dcstrategy(), dcopen();
int tcstrategy(), tcopen(), tcclose();

struct devsw devsw[] = {
	"ie",	diestrategy,	dieopen,	dieclose,	/* gpib */
	"dc",   dcstrategy,	dcopen,		nullsys,	/* disk */
	"tc",	tcstrategy,	tcopen,		tcclose,	/* tape */
	0,0,0,0
};

devread(io)
	register struct iob *io;
{
	return( (*devsw[io->i_ino.i_dev].dv_strategy)(io, READ) );
}

devwrite(io)
	register struct iob *io;
{
	return( (*devsw[io->i_ino.i_dev].dv_strategy)(io, WRITE) );
}

devopen(io)
	register struct iob *io;
{
	(*devsw[io->i_ino.i_dev].dv_open)(io);
}

devclose(io)
	register struct iob *io;
{
	(*devsw[io->i_ino.i_dev].dv_close)(io);
}

nullsys(io)
{
	;
}
