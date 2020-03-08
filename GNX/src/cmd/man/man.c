/*
 * man.c: version 1.4 of 8/30/83
 * Mesa Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)man.c	1.4 (NSC) 8/30/83";
# endif

#include <stdio.h>
#include <ctype.h>
#include <sgtty.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
/*
 * man
 */
int	nomore;
int	cflag;
char	*strcpy();
char	*strcat();
char	*trim();
char	*found();
char	*catwork();
char	*getpath();
int	remove();
int	section;
int	subsec;
int	troffit;
int	killtmp;

#define	eq(a,b)	(strcmp(a,b) == 0)

main(argc, argv)
	int argc;
	char *argv[];
{

	if (signal(SIGINT, SIG_IGN) == SIG_DFL) {
		signal(SIGINT, remove);
		signal(SIGQUIT, remove);
		signal(SIGTERM, remove);
	}
	umask(0);
	if (argc <= 1) {
		fprintf(stderr, "Usage: man [ section ] name ...\n");
		fprintf(stderr, "or: man -k keyword ...\n");
		fprintf(stderr, "or: man -f file ...\n");
		exit(1);
	}
	if (chdir("/usr/man") < 0) {
		fprintf(stderr, "Can't chdir to /usr/man.\n");
		exit(1);
	}
	argc--, argv++;
	while (argc > 0 && argv[0][0] == '-') {
		switch(argv[0][1]) {

		case 0:
			nomore++;
			break;

		case 't':
			troffit++;
			break;

		case 'k':
			apropos(argc-1, argv+1);
			exit(0);

		case 'f':
			whatis(argc-1, argv+1);
			exit(0);
		}
		argc--, argv++;
	}
	if (troffit == 0 && nomore == 0 && !isatty(1))
		nomore++;
	section = 0;
	do {
		if (eq(argv[0], "local")) {
			section = 'l';
			goto sectin;
		} else if (eq(argv[0], "new")) {
			section = 'n';
			goto sectin;
		} else if (eq(argv[0], "public")) {
			section = 'p';
			goto sectin;
		} else if (argv[0][0] >= '0' && argv[0][0] <= '9' && (argv[0][1] == 0 || argv[0][2] == 0)) {
			section = argv[0][0];
			subsec = argv[0][1];
sectin:
			argc--, argv++;
			if (argc == 0) {
				fprintf(stderr, "But what do you want from section %s?\n", argv[-1]);
				exit(1);
			}
			continue;
		}
		manual(section, argv[0]);
		argc--, argv++;
	} while (argc > 0);
	exit(0);
}

/*****************************************************************************/
/*			MANUAL						     */
/*	Function:							     */
/*		search the manual page directory and give the appropriate    */
/*		response.						     */
/*		The searching order of section number is : 1,n,l,6,8,2,3,4,5,*/
/*		7,p.							     */
/*	Input:  2 arguments - manual section number and manual name.	     */
/*	Output:	either nroff or cat the manual page or print the appropriate */
/*		message.						     */
/*	Caller:	main.							     */
/*	Callee:	pagexists, getpath, putchar, printf, stat, strcpy, found,    */
/*		catwork, troff, nroff.					     */
/*****************************************************************************/

manual(sec, name)
	char sec;
	char *name;
{
	int  	find = 0;
	char 	msection = sec;
	char	subsection = 0;
	char 	work[200];
	char	*cp;
	char	*foundresult, *catresult;
	char 	*sp = "1nl6823457p";
	struct stat	stbuf;

	if (msection == '1') 
	{
		sp = "1nl";
		msection = 0;
	}
	if (msection == 0) 
	{
		msection = *sp;
		while (msection && (!find))
		{
			if (pagexists(name, &msection, &subsection))
				find = 1;
			else
			{
				if (msection >= '1' && msection <= '3')
				{
search:
				    switch (msection) 
				    {
					case '1': cp = "mcg"; break;
					case '2': cp = "jv"; break;
					case '3': cp = "jxmsf"; break;
				    }
				    while (*cp && (!find)) 
				    {
					subsection = *cp;
					if (pagexists(name, &msection, &subsection))
						find = 1;
					else
						*cp++;
				    }
				}
				if(!find)
				{
					msection = *sp++;
					subsection = 0;
				}
			}
		}
		if (!find) 
		{
			if (sec == 0)
				printf("No manual entry for %s.\n", name);
			else
				printf("No entry for %s in section %c of the manual.\n", name, sec);
			return;
		}
	}
	else 
	{
		subsection = subsec;
		if (!pagexists(name, &msection, &subsection))
			if ((msection >= '1' && msection <= '3') && subsec == 0) 
			{
				sp = "\0";
				goto search;
			}
			else
			{
				printf("No entry for %s in section %c", name, msection);
				if (subsec)
					putchar(subsec);
				printf(" of the manual.\n");
				return;
			}
	}
	strcpy(work, getpath("man", name, &msection, &subsection));
	if (troffit)
		if (stat(work, &stbuf) >= 0)
			troff(work);
		else
			printf("Sorry, no troff\n");
	else 
	{
		if (!nomore) 
		{
			foundresult = found(work);
			if (foundresult == 0)
				return;
			else
				catresult = catwork(work);
				if (catresult == 0)
					return;
		}
		nroff(work);
	}
}	/* manual */

/*****************************************************************************/
/*			FOUND						     */
/*	Function:							     */
/*		Check the manual page if its 1st line contains '.so'.	     */
/*		If '.so', then return the pathname of the referenced 	     */
/*		manual page.						     */
/*	Input:	the pathname of the required manual page.		     */
/*	Output:	the real pathname(if it contained in '.so' statement) of     */
/*		the manual page						     */
/*	Caller:	manual						     	     */
/*	Callee:	stat, fopen, perror, fgets, strcpy, fclose.		     */
/*****************************************************************************/

char *found(work)
	char	*work;
{
	struct stat 	stbuf;
	FILE	*it;
	char	abuf[BUFSIZ];
	register char *cp = abuf+strlen(".so ");
	char *dp;


	if (stat(work, &stbuf) >= 0)
	{
		if ((it = fopen(work, "r")) == NULL) 
		{
			perror(work);
			return(0);
		}
		else
			if (fgets(abuf, BUFSIZ-1, it) &&
			   	abuf[0] == '.' && abuf[1] == 's' &&
			   		abuf[2] == 'o' && abuf[3] == ' ') 
			{
				while (*cp && *cp != '\n')
					cp++;
				*cp = 0;
				while (cp > abuf+3 && *--cp != '/')
					;
				while (cp > abuf+3 && *--cp != '/')
					;
				strcpy(work, cp+1);
			}
		fclose(it);
	}
	return(work);
}	/* found */

/*****************************************************************************/
/*		CATWORK							     */
/*	Function:							     */
/*		Check if manual page existed in cat area. Reformat it if     */
/*		there is no cat page or the manual page been updated.	     */
/*	Input:	the pathname of manual page.				     */
/*	Output:	the pathname of manual page or cat page which is going to    */
/*		be printed out for requester.				     */
/*	Caller:	manual.							     */
/*	Callee:	strcpy, stat, unlink, printf, fflush, system, remove.	     */
/*****************************************************************************/

char *catwork(work)
	char	*work;
{
	struct stat	stbuf, stbuf2;
	char	work2[200], cmdbuf[200];

	strcpy(work2, "cat");
	strcpy(work2+3, work+3);
	if (stat(work, &stbuf) < 0)
	{
		if (stat(work2, &stbuf2) < 0)
		{
			printf("no manual entry\n");
			return(0);
		}
	}
	else
	{
		if (stat(work2, &stbuf2) >= 0 && stbuf2.st_mtime < stbuf.st_mtime) 
			unlink(work2);
		if (stat(work2, &stbuf2) < 0)
		{
			printf("Reformatting page.  Wait...");
			fflush(stdout);
			sprintf(cmdbuf, "nroff -man %s > /tmp/man%d; trap '' 1 15; mv /tmp/man%d %s", work, getpid(), getpid(), work2);
			if (system(cmdbuf)) 
			{
				printf(" aborted (sorry)\n");
				remove();
					/*NOTREACHED*/
			}
			printf(" done\n");
		}
	}
	strcpy(work, work2);
	return(work);
}  /* catwork */

/*****************************************************************************/
/*		PAGEXISTS						     */
/*	Function:							     */
/*		Check if manual page or cat page existed. Either one existed */
/*		return 1.						     */
/*	Input:	The name, section number and subsection number of manual     */
/*		page.							     */
/*	Output:	return value 1 or 0 depends on manual page or cat page	     */
/*		existed or not.						     */
/*	Caller:	manual.							     */
/*	Callee:	stat, getpath.						     */
/*****************************************************************************/

pagexists(name, section, subsection)
char	*name, *section, *subsection;
{
	struct stat	stbuf;

  	int	a;  
	
	a = stat(getpath("man", name, section, subsection), &stbuf); 
	if(a>=0)		
		return 1;
	a = stat(getpath("cat", name, section, subsection), &stbuf);
	return a>=0;
} /* pagexists */

/*****************************************************************************/
/*		GETPATH							     */
/*	Function:							     */
/*		Get the pathname of either manual page or cat page.	     */
/*	Input:	directory of the manual, name, section number and subsection */
/*	Output:	pathname of the required manual.			     */
/*	Caller:	manual, pagexists.					     */
/*	Callee:	strcpy, strcat, strncat.				     */
/*****************************************************************************/

char *
getpath(prefix, name, section, subsection)
	char	*prefix, *name, *section, *subsection;
{
static	char pathname [200];

	strcpy(pathname,"");
	strcat(pathname, prefix);
	strncat(pathname, section, 1);
	strcat(pathname, "/");
	strcat(pathname, name);
	strcat(pathname, ".");
	strncat(pathname, section, 1);
	strncat(pathname, subsection, 1);
	return(pathname);

}  /* getpath */

/*****************************************************************************/
/*		NROFF							     */
/*	Function:							     */
/*		print the required manual from either cat area or nroff      */
/*		the manual page to terminal.				     */
/*	Input:	the pathname from which the manual is to be printed out.     */
/*	Output:	the manual user requests.				     */
/*	Caller:	manual.							     */
/*	Callee:	sprintf, cat, more, system, nroff.			     */
/*****************************************************************************/

nroff(cp)
	char *cp;
{
	char cmd[BUFSIZ];

	sprintf(cmd, nomore ?
	    "%s %s" : "%s %s|/usr/ucb/more -f",
	    cp[0] == 'c' ? "cat -s" : "nroff -man", cp);
	system(cmd);
}

troff(cp)
	char *cp;
{
	char cmdbuf[BUFSIZ];

	sprintf(cmdbuf,
"troff -t -man /usr/lib/tmac/tmac.vcat %s|/usr/lib/rvsort|/usr/ucb/vpr -t",
		cp);
	system(cmdbuf);
}

any(c, sp)
	register int c;
	register char *sp;
{
	register int d;

	while (d = *sp++)
		if (c == d)
			return (1);
	return (0);
}

remove()
{
	char name[15];

	sprintf(name, "/tmp/man%d", getpid());
	unlink(name);
	exit(1);
}

apropos(argc, argv)
	int argc;
	char **argv;
{
	char buf[BUFSIZ];
	char *gotit;
	register char **vp;

	if (argc == 0) {
		fprintf(stderr, "man: -k what?\n");
		exit(1);
	}
	if (freopen("/usr/lib/whatis", "r", stdin) == NULL) {
		perror("/usr/lib/whatis");
		exit (1);
	}
	gotit = (char *) calloc(1, blklen(argv));
	while (fgets(buf, sizeof buf, stdin) != NULL)
		for (vp = argv; *vp; vp++)
			if (match(buf, *vp)) {
				printf("%s", buf);
				gotit[vp - argv] = 1;
				for (vp++; *vp; vp++)
					if (match(buf, *vp))
						gotit[vp - argv] = 1;
				break;
			}
	for (vp = argv; *vp; vp++)
		if (gotit[vp - argv] == 0)
			printf("%s: nothing apropriate\n", *vp);
}

match(buf, str)
	char *buf, *str;
{
	register char *bp, *cp;

	bp = buf;
	for (;;) {
		if (*bp == 0)
			return (0);
		if (amatch(bp, str))
			return (1);
		bp++;
	}
}

amatch(cp, dp)
	register char *cp, *dp;
{

	while (*cp && *dp && lmatch(*cp, *dp))
		cp++, dp++;
	if (*dp == 0)
		return (1);
	return (0);
}

lmatch(c, d)
	char c, d;
{

	if (c == d)
		return (1);
	if (!isalpha(c) || !isalpha(d))
		return (0);
	if (islower(c))
		c = toupper(c);
	if (islower(d))
		d = toupper(d);
	return (c == d);
}

blklen(ip)
	register int *ip;
{
	register int i = 0;

	while (*ip++)
		i++;
	return (i);
}

whatis(argc, argv)
	int argc;
	char **argv;
{
	register char **avp;

	if (argc == 0) {
		fprintf(stderr, "man: -f what?\n");
		exit(1);
	}
	if (freopen("/usr/lib/whatis", "r", stdin) == NULL) {
		perror("/usr/lib/whatis");
		exit (1);
	}
	for (avp = argv; *avp; avp++)
		*avp = trim(*avp);
	whatisit(argv);
	exit(0);
}

whatisit(argv)
	char **argv;
{
	char buf[BUFSIZ];
	register char *gotit;
	register char **vp;

	gotit = (char *)calloc(1, blklen(argv));
	while (fgets(buf, sizeof buf, stdin) != NULL)
		for (vp = argv; *vp; vp++)
			if (wmatch(buf, *vp)) {
				printf("%s", buf);
				gotit[vp - argv] = 1;
				for (vp++; *vp; vp++)
					if (wmatch(buf, *vp))
						gotit[vp - argv] = 1;
				break;
			}
	for (vp = argv; *vp; vp++)
		if (gotit[vp - argv] == 0)
			printf("%s: not found\n", *vp);
}

wmatch(buf, str)
	char *buf, *str;
{
	register char *bp, *cp;

	bp = buf;
again:
	cp = str;
	while (*bp && *cp && lmatch(*bp, *cp))
		bp++, cp++;
	if (*cp == 0 && (*bp == '(' || *bp == ',' || *bp == '\t' || *bp == ' '))
		return (1);
	while (isalpha(*bp) || isdigit(*bp))
		bp++;
	if (*bp != ',')
		return (0);
	bp++;
	while (isspace(*bp))
		bp++;
	goto again;
}

char *
trim(cp)
	register char *cp;
{
	register char *dp;

	for (dp = cp; *dp; dp++)
		if (*dp == '/')
			cp = dp + 1;
	if (cp[0] != '.') {
		if (cp + 3 <= dp && dp[-2] == '.' && any(dp[-1], "cosa12345678npP"))
			dp[-2] = 0;
		if (cp + 4 <= dp && dp[-3] == '.' && any(dp[-2], "13") && isalpha(dp[-1]))
			dp[-3] = 0;
	}
	return (cp);
}

system(s)
char *s;
{
	int status, pid, w;

	if ((pid = vfork()) == 0) {
		execl("/bin/sh", "sh", "-c", s, 0);
		_exit(127);
	}
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	return (status);
}
