
/*
 * gpib.c: version 1.5 of 7/14/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)gpib.c	1.5 (NSC) 7/14/83";
# endif

#include	<stdio.h>
#include	<a.out.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/dir.h>
#include	"main.h"
#include	"machine.h"
#include	"symbol.h"

char	*gpibstr = "@n %s\r";

char	ddot[]	= ".";
char	dotdot[] = "..";
char	name[BUFSIZ];
int	file;
int	off	= -1;
struct	stat	d, dd;
struct	direct	dir;

char pathat[80];
char *pathptr;

gpibabort()
{
	machineabort();
}

char *fname(filname)
char *filname;
{
	if (*filname == '/') return(filname);
	if (*filname == '.') {
		printf("\tcannot handle .. files yet\r\n");
		gpibabort();
	}
	getpath();
	while (*filname != '\0')
		*pathptr++ = *filname++;
	*pathptr = '\0';
	return(pathat);
}

gpibload()
{
char nstr[100];
	clearmem(ddtheader.a_dat_addr,(ddtheader.a_data+ddtheader.a_bss));
	sprintf(nstr,gpibstr,fname(imagename));
	flushline(TRUE);
	write(port,nstr,strlen(nstr));
	setalarm(20);
	if (getstar(FALSE) != STAR) {
		printf("\tgpib download fails\r\n");
		gpibabort();
	}
	alarm(0);
	printf("\tloaded\r\n");
}

getpath()
{
	int rdev, rino;

	pathptr = pathat;
	stat("/", &d);
	rdev = d.st_dev;
	rino = d.st_ino;
	for (;;) {
		stat(ddot, &d);
		if (d.st_ino==rino && d.st_dev==rdev) {
			prname();
			return;
		}
		if ((file = open(dotdot,0)) < 0) {
			printf("\tgpib: cannot open ..\r\n");
			gpibabort();
		}
		fstat(file, &dd);
		chdir(dotdot);
		if(d.st_dev == dd.st_dev) {
			if(d.st_ino == dd.st_ino) {
				prname();
				return;
			}
			do
				if (read(file, (char *)&dir, sizeof(dir)) < sizeof(dir)) {
					printf("\tread error in ..\r\n");
					gpibabort();
				}
			while (dir.d_ino != d.st_ino);
		}
		else do {
				if(read(file, (char *)&dir, sizeof(dir)) < sizeof(dir)) {
					printf("\tread error in ..\r\n");
					gpibabort();
				}
				stat(dir.d_name, &dd);
			} while(dd.st_ino != d.st_ino || dd.st_dev != d.st_dev);
		close(file);
		if (cat() < 0) break;
	}
}

prname()
{
int i;
	*pathptr++ = '/';
	if (off<0)
		off = 0;
	name[off] = '/';
	for (i = 0; i < off+1; i++)
		*pathptr++ = name[i];

}

cat()
{
	register i, j;

	i = -1;
	while (dir.d_name[++i] != 0);
	if ((off+i+2) > BUFSIZ-1) {
		prname();
		return(-1);
	}
	for(j=off+1; j>=0; --j)
		name[j+i+1] = name[j];
	off=i+off+1;
	name[i] = '/';
	for(--i; i>=0; --i)
		name[i] = dir.d_name[i];
	return(0);
}


