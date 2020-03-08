
/*
 * nburn.c: version 1.3 of 8/17/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)nburn.c	1.3 (NSC) 8/17/83";
# endif

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <a.out.h>

#define aux_on	"\33[5i"
#define aux_off	"\33[4i"

char	*f=0;
FILE	*input;
int	tsize,dsize,base,len=2*1024;
char	stdfun[BUFSIZ];

int	auxreset();
int	loadstatic;
int	odd;

main(ac,av)
char  **av;
{
int	i;

	setbuf(stdout,stdfun);
#ifdef nothing
	for(i=1; i<ac; ++i)
		if(*av[i]!='-')
			if(f== 0)
				f = av[i];
			else	printf("Only one a.out file is allowed\n"),
				exit(1);
#endif
	signal(SIGINT,auxreset);
	while(ac>1 && *av[1]=='-') {
		if(av[1][1]=='s')
			--loadstatic;
		--ac, ++av;
	}
	if(ac>1 && isdigit(*av[1])) {
		base = 1024*atoi(av[1]);
		--ac, ++av;
	}
	if(ac>1 && isdigit(*av[1])) {
		len = 1024*atoi(av[1]);
		--ac, ++av;
	}
	--ac;
	if(ac>2 || ac<1 || (*av[1]!='e' && *av[1]!='o'))
		printf(
		"Usage: nburn [ -s ] [ b  [ l ] ] e/o [ file ]\n"), exit(3);
	if(*av[1]=='o')
		++odd;
	if(ac==2)
		f = av[2];
#ifdef VAX
	else	f = "a16.out";
#else
	else	f = "a.out";
#endif
	input = fopen(f,"r");
	if(input== 0)
		printf("Can't open <%s>\n", f), exit(2);
	pickhead();
	load();
}


pickhead()
{
int filetype, fileopen, readrst;
	fileopen = open(f,0);
	if (fileopen < 0) {
		printf("\r\nopen of %s fails\r\n",f);
		exit(-1);
	}
	readrst = read(fileopen, &filetype, sizeof filetype);
	if (readrst < sizeof filetype) {
		printf("Bad object format read\n");
		close(fileopen);
		exit(-1);
	}
	if (filetype == ZMAGIC) {
		zburn();
	} else {
		printf("Do not know this format %x\n",filetype);
		close(fileopen);
		exit(-1);
	}
}

zburn()
{
struct exec imagefun;

	fread(&imagefun,sizeof imagefun,1,input);
	if(imagefun.a_magic!=ZMAGIC)
		printf("%s: bad magic number.\n", f), exit(4);
	tsize = imagefun.a_text;
	dsize = imagefun.a_data;
	fseek(input,1024,0);
}

unsigned short chksum;
int	outcount=0;

int theend = 0;
int tend = -1;
int dend = -1;

load()
{
register i,c;
int	 t2;
unsigned short t;
int dstart = 0, tstart = 0;

	printf("text size %d data size %d\n",tsize,dsize);
	printf("%s\2$A0000,",aux_on);
	chksum = 0;
	for(i=0; i<tsize; ++i) {
		c = getc(input);
		if(i<base) {
			tstart = i;
			continue;
		}
		if((i&1)==odd)
			output(c);
		if (theend && (tend == -1)) {
			theend = 0;
			tend = i;
		}
	}
	if (tend == -1) tend = i;
	if(loadstatic)
		for(i=0; i<dsize; ++i) {
			c = getc(input);
			if(i+tsize < base) {
				dstart = i + tsize;
				continue;
			}
			if((i&1)==odd)
				output(c);
		if (theend && (dend == -1)) {
			theend = 0;
			dend = tsize + i;
		}
		}
chk:
	len = 1000*1000;

	t2 = outcount;
	printf("\3$S");
	t = chksum;
	output(t>>8);
	output(t&0xff);
	printf(",000000000%s", aux_off);
	printf("%d (0x%x) bytes transferred\n", t2, t2);
	printf("The check sum was %4x\n", t);
	fflush(stdout);
}

output(c)
unsigned int c;
{
unsigned register k;

	if(outcount >= len) {
		theend = 1;
		return;
	}
	++outcount;
	chksum += (k = c);
	k >>= 4;
	if(k>=0xa)
		k += 'A'-0xa;
	else	k += '0';
	putchar(k);
	k = c & 0xf;
	if(k>=0xa)
		k += 'A'-0xa;
	else	k += '0';
	putchar(k);
	putchar(' ');
}

auxreset()
{
	signal(SIGINT,auxreset);
	printf("%s\n", aux_off);
	exit(-1);
}
