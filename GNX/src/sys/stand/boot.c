/*
 * boot.c: version 2.16 of 9/16/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)boot.c	2.16 (NSC) 9/16/83";
# endif

#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/filsys.h>
#include <sys/dir.h>
#include <a.out.h>
#include <sys/saio.h>
#include <sys/reboot.h>

/*
 * Boot program... arguments passed in r6 and r7 determine
 * whether boot stops to ask for system name and which device
 * boot comes from.
 */
#define STACKSIZE 2048		/* allow 2K minimum for stack */

char	devname[][2] = {
	'd','c',	/* 0 = dc */
	'i','e',	/* 1 = ie */
	't','c',	/* 2 = tc */
	0,0,		/* 3 = xx */
};

char line[100] = "xx(1,0)vmunix";

int	retry = 0;

main()
{
	register howto, devtype;
	int io;

	setxy(1,0);	/* get to bottom of screen */
#ifdef JUSTASK
	howto = RB_ASKNAME|RB_SINGLE;
#else
#ifndef EBOOT
	howto = 0; devtype = 0;
#endif
	if ((howto&RB_ASKNAME)==0) {
		if (devtype>=0 && devtype<sizeof(devname)/2
		    && devname[devtype][0]) {
			line[0] = devname[devtype][0];
			line[1] = devname[devtype][1];
		} else
			howto = RB_SINGLE|RB_ASKNAME;
	}
#endif
	for (;;) {
		if (howto & RB_ASKNAME) {
			printf("\nBoot: ");
			gets(line);
		} else
			printf("\nBoot: %s\n", line);
		io = open(line, 0);
		if (io >= 0)
			copyunix(howto, io);
		if (++retry > 2)
			howto = RB_SINGLE|RB_ASKNAME;
	}
}
#define SHORTREAD	"Short read\n"
#define BADFMT		"Bad format\n"

/*ARGSUSED*/
copyunix(howto, io)
	register howto,io;
{
	register struct exec *x;	/* pointer to file header */
	register int staddr;		/* user's start address */
	register int totsize;		/* Total size of program */
	register int readsize;		/* text + data */
	int newmod;			/* user's starting mod register */
	int bootstart,i;
	char	buf[BSIZE];		/* header buffer */

	x = (struct exec *)buf;

	/* read in 1024 byte header */
	if (read(io, buf, BSIZE) != BSIZE || x->a_magic != ZMAGIC)
		_stop(BADFMT);
	readsize = x->a_text + x->a_data;
	totsize = (readsize + x->a_bss + STACKSIZE + 511) & (~511);
	staddr = x->a_entry - x->a_trsize;
	newmod = x->a_entry_mod;
	printf("%d+%d+%d\n",x->a_text,x->a_data,x->a_bss);
	bootstart = greloc();		/* get amt of free space below boot */
	if (readsize <= bootstart) {	/* program will fit below boot */
		if (read(io, (char *)0, readsize) != readsize) goto shread;
	}
	else {		/* program is too big, read in two pieces */
		if (read(io, (char *)0, bootstart) != bootstart) goto shread;
		readsize -= bootstart;
		if (read(io,(char *)gbreak(),readsize) != readsize) goto shread;
		readsize += bootstart;
	}
	close(io); 
	if (totsize <= bootstart) {	/* program fits below boot */
		dispatch(staddr,newmod,readsize,totsize);
		return;
	}
	else {		/* program does not fit, boot will be destroyed */
		bdispatch(staddr,newmod,readsize,totsize);
		/* no return */
	}
shread:
	_stop(SHORTREAD);
}
