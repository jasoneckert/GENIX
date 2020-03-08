
/*
 * monfix.c: version 1.3 of 8/17/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)monfix.c	1.3 (NSC) 8/17/83";
# endif

#include <stdio.h>
#include <ctype.h>
#include <a.out.h>

int	ac;
char	**av;
char	*monfile = 0;
int	mfile;
int	loadat = 0;
int 	modat = -1;
int	sbat = -1;
int	entryat = -1;


next(s)
char *s;
{
	if(--ac<=1) {
		printf("monfix. error. expected argument after <%s>\n", s);
		exit(1);
	}
	++av;
}

main(lac,lav)
char  **lav;
{
	ac = lac;
	av = lav;
	for(;ac>1; --ac,++av) {
		if(av[1][0]=='-') {
			switch(av[1][1]) {
			    default:
				if(av[1][1]=='\0')
				    break;
			    case 'b':
				next("-b");
				sscanf(av[1],"%X",&loadat);
				break;
			    case 'e':
				next("-e");
				sscanf(av[1],"%X",&entryat);
				break;
			    case 's':
				next("-s");
				sscanf(av[1],"%X",&sbat);
				break;
			    case 'm':
				next("-m");
				sscanf(av[1],"%X",&modat);
				break;
			}
		} else break;
	}
	if(ac == 2)
		monfile = av[1];
#ifdef VAX
	else	monfile = "a16.out";
#else
	else	monfile = "a.out";
#endif
	mfile = open(monfile,2);
	if(mfile <= 0)
		printf("Can't open <%s>\n", monfile), exit(2);
	fixhead();
}


fixhead()
{
int filetype, readrst;
	readrst = read(mfile, &filetype, sizeof filetype);
	if (readrst < sizeof filetype) {
		printf("Bad object format read\n");
		close(mfile);
		exit(-1);
	}
	if (filetype == ZMAGIC) {
		zfix();
	} else {
		printf("Do not know this format %x\n",filetype);
		close(mfile);
		exit(-1);
	}
}

int lmod = 0xa7ed;
int lsb = 0xa56f;
int lbr = 0xc0ea;

zfix()
{
struct exec imagefun;
char bytes[4];
int *dptr;

	dptr = (int *)(&bytes[0]);
	lseek(mfile,0,0);
	read(mfile,&imagefun,sizeof imagefun);
	if(imagefun.a_magic!=ZMAGIC)
		printf("%s: bad magic number.\n", monfile), exit(4);
	if (modat == -1) modat = imagefun.a_entry_mod;
	if (entryat == -1) entryat = imagefun.a_entry;
	if (sbat == -1) {
		lseek(mfile,(1024+(modat-imagefun.a_text_addr)),0);
		read(mfile,&sbat,4);
	}
	printf("mod at %x\n",modat);
	printf("sb at %x\n",sbat);
	printf("entry at %x\n",entryat);
	lseek(mfile,1024,0);
	write(mfile,&lmod,2);
	*dptr = modat;
	write(mfile,&bytes[1],1);
	write(mfile,&bytes[0],1);
	write(mfile,&lsb,2);
	*dptr = sbat;
	write(mfile,&bytes[3],1);
	write(mfile,&bytes[2],1);
	write(mfile,&bytes[1],1);
	write(mfile,&bytes[0],1);
	write(mfile,&lbr,2);
	entryat -= loadat + 10;
	entryat -= imagefun.a_text_addr;
	*dptr = entryat;
	write(mfile,&bytes[2],1);
	write(mfile,&bytes[1],1);
	write(mfile,&bytes[0],1);
	close(mfile);
}
