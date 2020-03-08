/*
 * lpr.c: version 1.2 of 8/12/83
 * Mesa Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)lpr.c	1.2 (NSC) 8/12/83";
# endif

#include <signal.h>
#include <pagsiz.h>

#define	BUFSIZ	BSIZE
/*
 * lpr -- on-line printer spooler
 */

char	tfname[]	= "/usr/spool/lpd/tfaXXXXX";
char	cfname[]	= "/usr/spool/lpd/cfaXXXXX";
char	lfname[]	= "/usr/spool/lpd/lfaXXXXX";
char	dfname[]	= "/usr/spool/lpd/dfaXXXXX";
char	*rindex();
int	siolp;
int	nact;
int	tff;
int	mailflg,writeflg;
char	person[10];
int	inchar;
int	maxrec	= 1000;

main(argc, argv)
	int argc;
	char *argv[];
{
	register char *arg, *remote;
	int c, f, flag;
	int out();
	char *cp;

	pidfn();
	if (signal(SIGINT, SIG_IGN) == SIG_DFL)
		signal(SIGINT, out);
	if (signal(SIGQUIT, SIG_IGN) == SIG_DFL)
		signal(SIGQUIT, out);
	if (signal(SIGHUP, SIG_IGN) == SIG_DFL)
		signal(SIGHUP, out);
	if (signal(SIGTERM, SIG_IGN) == SIG_DFL)
		signal(SIGTERM, out);
	remote = "$	remote	**,onl";
	flag = 0;
	while (argc>1 && (arg = argv[1])[0]=='-') {
		if (arg[1] && arg[2]) {
			remote[12] = arg[1];
			remote[13] = arg[2];
			remote[14] = 0;
		} else switch (arg[1]) {

		case '-':
			remote[12] = 'r';
			remote[13] = '1';
			remote[14] = '\0';
			break;

		case 'c':
			flag = '+';
			break;

		case 'r':
			flag = '-';
			break;

		case 'm':
			mailflg = 1;
			break;
		case 'w':
			writeflg = 1;
			break;
		case 's':	/* serial printer */
			siolp++;
			cp = (char *) &tfname[11];		
			*cp++ = 's';
			*cp++ = 'l';
			*cp++ = 'p';
			cp = (char *) &cfname[11];		
			*cp++ = 's';
			*cp++ = 'l';
			*cp++ = 'p';
			cp = (char *) &lfname[11];		
			*cp++ = 's';
			*cp++ = 'l';
			*cp++ = 'p';
			cp = (char *) &dfname[11];		
			*cp++ = 's';
			*cp++ = 'l';
			*cp++ = 'p';
			break;
		}
		argc--;
		argv++;
	}
	tff = nfile(tfname);
	ident();
	if(argc == 1)
		copy(0,"(pipe)");
	while(--argc) {
		arg = *++argv;
		if(flag == '+')
			goto cf;
/*
 * This may not work because the daemon runs as someone else, so don't bother
		if(*arg == '/' && flag != '-') {
			card('F', arg, "");
			nact++;
			continue;
		}
		if(link(arg, lfname) < 0)
			goto cf;
		chmod(lfname,0666);
		card('F', lfname, arg);
		card('U', lfname, "");
		lfname[inchar]++;
		nact++;
		goto df;
 */

	cf:
		f = open(arg, 0);
		if(f < 0) {
			printf("Cannot open %s\n", arg);
			continue;
		}
		copy(f,arg);
		close(f);

	df:
		if(flag == '-') {
			f = unlink(arg);
			if(f < 0)
				printf("Cannot remove %s\n", arg);
		}
	}

	if(nact) {
		tfname[inchar]--;
		f = link(tfname, dfname);
		if(f < 0) {
			printf("Cannot rename %s\n", dfname);
			tfname[inchar]++;
			out();
		}
		unlink(tfname);
		if (siolp)
			execl ("/usr/lib/slpd", "slpd", 0);
		else execl("/usr/lib/lpd", "lpd", 0);
		dfname[inchar]++;
		printf("Daemon doesn't exist\n");
		exit(0);
	}
	out();
}

copy(f,s1)
int f;
char s1[];
{
	int ff, i, nr, nc;
	static char buf[BUFSIZ];

	card('F', cfname, s1);
	card('U', cfname, "");
	ff = nfile(cfname);
	nc = 0;
	nr = 0;
	while((i = read(f, buf, BUFSIZ)) > 0) {
		write(ff, buf, i);
		nc += i;
		if(nc >= BUFSIZ) {
			nc -= BUFSIZ;
			nr++;
			if(nr > maxrec) {
				printf("Copy file is too large\n");
				break;
			}
		}
	}
	close(ff);
	nact++;
}

card(c, s1, s2)
char c;
char *s1,*s2;
{
	static char buf[BUFSIZ];
	char *tmp;

	if((tmp = rindex(s2,'/')) != 0) s2 = tmp + 1;
	if(strlen(s2) > 14) *(s2+14) = '\0';
	if(s2[0] == '\0') sprintf(buf,"%c%s\n",c,s1);
		     else sprintf(buf,"%c%s,%s\n",c,s1,s2);
	write(tff, buf, strlen(buf));
}

ident()
{
	int c, n;
	register char *b1p, *pp, *b2p;
	static char b1[100], b2[100];

	b1p = b1;
	if(getpw(getuid(), b1p)) {
		b1p = "pdp::::m0000,m000:";
	}
	n = 0;
	b2p = b2;
	while(*b2p++ = "$	ident	"[n++]);
	b2p--;
	n = 5;
	while(--n) while(*b1p++ != ':');
	while((*b2p++ = *b1p++) != ':');
	b2p[-1] = ',';
	b1p = b1;
	pp = person;
	while((c = *b1p++) != ':') {
		*b2p++ = c;
		*pp++ = c;
	}
	*b2p++ = 0;
	*pp++ = 0;
	card('L', person, "");
	if (mailflg)
		card('M', person, "");
	if (writeflg)
		card('W', person, "");
}

pidfn()
{
	register i, j, c;
	int s;
	int p;

	s = p = getpid();
	p &= 077777;
	i = 0;
	while(tfname[i] != 'X')
		i++;
	i += 4;
	for(j=0; j<5; j++) {
		c = (p%10) + '0';
		if(s<0 && j==4)
			c += 4;
		p /= 10;
		tfname[i] = c;
		cfname[i] = c;
		lfname[i] = c;
		dfname[i] = c;
		i--;
	}
	inchar = i;
}

nfile(name)
char *name;
{
	register f;

	f = creat(name, 0644);
	chmod(name,0666);
	if(f < 0) {
		printf("Cannot create %s\n", name);
		out();
	}
	name[inchar]++;
	return(f);
}

out()
{
	register i;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	i = inchar;
	while(tfname[i] != 'a') {
		tfname[i]--;
		unlink(tfname);
	}
	while(cfname[i] != 'a') {
		cfname[i]--;
		unlink(cfname);
	}
	while(lfname[i] != 'a') {
		lfname[i]--;
		unlink(lfname);
	}
	while(dfname[i] != 'a') {
		dfname[i]--;
		unlink(dfname);
	}
	exit(0);
}
