/*
 * Copyright (C) 1982 by National Semiconductor Corporation
 *
 *	National Semiconductor Corporation
 *	2900 Semiconductor Drive
 *	Santa Clara, California 95051
 *
 *	All Rights Reserved
 *
 * This software is furnished under a license and may be used and copied only
 * in accordance with the terms of such license and with the inclusion of the
 * above copyright notice.  This software or any other copies thereof may not
 * be provided or otherwise made available to any other person.  No title to
 * or ownership of the software is hereby transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by National Semiconductor
 * Corporation.
 *
 * National Semiconductor Corporation assumes no responsibility for the use
 * or reliability of its software on equipment configurations that are not
 * supported by National Semiconductor Corporation.
 */

/*
 * mcc.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)mcc.c	5.1 (NSC) 9/8/83";
# endif
/* jima 5/17/82: Modified to accept compiler debugging options:

		 -Z<anything> causes -<anything> to be passed to the
		 compilers, both passes (if two passes are appropriate)

		 -B is equivalent to -B/usr/nsc/lib/o
		 -N is equivalent to -B/usr/nsc/lib/n

		 In the absence of -t, -B/N changes only the compiler
		 proper (ccom), not the location of the pre-pass (cpp).

   Also, the name of compiler was changed from com to ccom.
   jima 7/29/82: Fixes so meld is called properly for prototype 16K loads.
   JZ 7/30/82 fixed to call special asm and new call save return sequence.
   JZ 8/2/82: Calls new cpp for mesa includes
   JZ 8/27/82: -N Now calls Jima's compiler in /usr/nsc/xlib by default
   JZ 9/7/82: Added a continue after case 'p' in switch since it was falling
  		 through incorrectly when called with the eflag.
   JZ 9/23/82: changed the compiler to call op1 with the -O option 
   JZ 10/26/82: Changed to call c2 compiler
   JZ 10/26/82: ifdefs put in to compile for the 16k
   JZ 11/9/82: Changed the error checking to look for a whole word of
		status from wait instead of just 1 byte.
   JZ 11/9/82: Added an = so that the check in assemble for error code status
		is >= instead of just >1. Since the assembler returns a 1
		on an error we want to die when a 1 is returned.
   JZ 11/17/82: Changed the tmp file for the 16k /usr/tmp and changed the
		locations of the utilities to their standard places for the 16k
   JZ 11/17/82: Changed the sprintf for the tmp file location to get around the
		problem with sprintf.
   JZ 11/24/82: Added Fflag so that -F is sent to ccom for putting common in
		local static.
   JZ 11/24/82: Changed arg[0] to nasm and nmeld for calls to those 2 so that
		ps will show the correct program names.
   JZ 12/8/82:  Changed all the path names so that they are created dynamically
		based on defines in the makefile.
   JZ  2/2/83:  Added a path for crt0 and mcrt0 in LDLIBPATH which is set in
   		the makefile.  Also changed mcc to now load crt0.  Added new
		-v flag for verbose.  Will print what the compiler is executing
   JZ  2/3/83:  Removed g flag for now.
   JZ  2/3/83:  moved loading of crt0 to after emitting  -o
   JZ  2/9/83:  Changed to include <sys/dir.h> instead of <dir.h> since <dir.h>
   		is now the 4.2 directory simulation.
   JZ  2/14/83: Removed the -Z and -R flags, changed -g flag to call the 
		compiler with -l which puts comments on line numbers into the
		text and to also call the assembler with the -g flag.  Changed
		the string lengths to 40 characters, since some were coming
		close to running out of room with 30.
   JZ  2/24/83: Changed names of utilities from arrays back to pointers to
		go along with new makefile that will create the whole pathname.
		Removed the call of the loader with -lg since libg.a is
		undefined.  Declared malloc() and calloc(), so lint is happy. 
		Changed second argument to error() to be a (char *)NULL if
		nothing is there instead of a 0, since the argument is defined
		to be a	*char.
    JZ 3/8/83:  Changed vflag to tell about unlinking files and to allways use
		the same syntax which is "executing:" .
    JZ 3/30/83: Added new flag -q flag which is passed to the assembler. This
    		causes the assembler and the linker to use sb relative link
		tables
    JZ 4/18/83: Added -b flag that toggles the default setting for object and
                assembly files from upper to lowercase or vica-versa.
    JZ 4/29/83: Added -i flag to be passed to the optimiser to tell it the
    		program contains I/O register accesses which suppresses some
		optimisations.
    JZ 5/14/83: Added -vn flag this just prints what the compiler would do
    		without actually doing anything.  Can be used to find paths
		being used quickly.
    JZ 9/7/83:  Subtracted -N flag.  Changed -t and -B flags to default to
    		/usr/c/o by using DEFDIR to be passed from make.
		Changed arg[0] for as and ld to get their values
		from the makefile using AS and LD
		so ps will get the correct value.

 *
 * cc - front end for C compiler
 */
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/dir.h>
#define SRCSUF 'c'
#define PPSUF  'i'  /* cpp output */
/*  The following three lines work since all occurences of OBJSUF and ASMSUF
/*  except 1 which doesn't really matter are replaced by AOBJSUF and AASMSUF
/*  which reverse their case if the bflag is set.*/
#define revcase(ch) (((ch) >= 'a' && (ch) <= 'z')? (toupper(ch)):(tolower(ch)))
#define AOBJSUF (bflag? revcase(OBJSUF):OBJSUF)
#define AASMSUF (bflag? revcase(ASMSUF):ASMSUF)
char **calloc(), *malloc();
/*  The path name for the   utilities is set in the makefile for ccom
    depending on who this version of nmcc is being made for.
    DEFDIR is a pathname such as /usr/c/ that is the alternate
    starting directory for the utilities. It may be the same as the normal
    path in X-support */
char	*cpp  = CPPNAME;
char	*ccom = CCOMNAME;
char	*c2   = C2NAME;
char	*as   = ASNAME;
char	*ld   = MELDNAME;
char    tmpstr1[40] = DEFDIR;	/* Default alternate directory usually /usr/c*/
char    tmpstr2[40] = DEFDIR;
char	*ldfirst = "";
char	*ldlast = "-lc";  /* crock search-library option */
char	*crt0 = CRTNAME;

char	tmp0[40];		/* big enough for /tmp/ctm%05.5d */
char	*tmp1, *tmp2, *tmp3, *tmp4, *tmp5;
char	*outfile;
char	*savestr(), *strspl(), *setsuf();
int	idexit();
char	**av, **clist, **llist, **plist;
int	vflag,cflag, eflag,Fflag,gflag, nflag, oflag, pflag, qflag, sflag,
	wflag, Rflag, exflag, proflag, bflag, iflag;
char	*dflag;
int	exfail;
char	*chpass;
char	*npassname;
char	*Zarg;  /* zero means no -Z<xxx> arg, otherwise points to -<xxx> */

int	nc, nl, np, nxo, na;
extern char *strcat();

#define	cunlink(st)	if (st){ if (vflag) printf("%s ",(st)); if (!nflag)unlink(st);}
#define OPTIMISER

main(argc, argv)
	char **argv;
{
	char *t;
	char *assource;
	int i, j, c;
	
	/* ld currently adds upto 5 args; 10 is room to spare */
	av = (char **)calloc(argc+10, sizeof (char **));
	clist = (char **)calloc(argc, sizeof (char **));
	llist = (char **)calloc(argc, sizeof (char **));
	plist = (char **)calloc(argc, sizeof (char **));
	for (i = 1; i < argc; i++) {
		if (*argv[i] == '-') switch (argv[i][1]) {
		case 'b': /* Reverse default case of suffixes */
		    bflag++;  /* all the rest will be done by macros */
		    continue;

		case 'B': /* "Backup" (old) version */ /* JZ 8-27: changed to Jima's compiler */
			if (npassname)
				error("-B overwrites earlier option",(char*)NULL);
			npassname = argv[i]+2;
			if (npassname[0]==0)
			{	/* was: npassname = "/usr/nsc/o"; */
				npassname = strcat(tmpstr1,"o");
			}        /* use files named /usr/nsc/lib/occom, etc */
			continue;
		case 'D':
		case 'I':
		case 'U':
		case 'C':
			plist[np++] = argv[i];
			continue;
		case 'c':
			cflag++;
			continue;
		case 'd':
			dflag = argv[i];
			continue;
		case 'g':
			gflag++;
			continue;
		case 'i':
			iflag++;
			continue;
/*		case 'N':  /* "New" (experimental) version */
/*			if (npassname)
/*				error("-N overwrites earlier option",(char *)NULL);
/*			npassname = argv[i]+2;
/*			if (npassname[0]==0)
/*				{ strcat(tmpstr1,LIBDIR);
/*				  npassname = strcat(tmpstr1,"n");
/*				}
/*			        /* use files named /usr/nsc/lib/nccom, etc */
/*			continue;*/
		case 'F':
			Fflag++;
			continue;
		case 'o':
			if (++i < argc) {
				outfile = argv[i];
				switch (getsuf(outfile)) {

				case SRCSUF:
				case OBJSUF:
					error("-o would overwrite %s",
					    outfile);
					exit(8);
				}
			}
			continue;
		case 'O':
			oflag++; 
			continue;
		case 'p':
			proflag++;
			continue;
		case 'q':
			qflag++;
			continue;
		case 'R':
		/*	Rflag++; */ /* Not implemented yet*/
			continue;
		case 'S':
			sflag++;
			cflag++;
			continue;
		case 'v':
			vflag++;
			if (argv[i][2]=='n') nflag++; /* Don't do anything */
			continue;
		case 'w':
			wflag++;
			continue;
		case 'E':
			exflag++;
		case 'P':
			pflag++;
			if (argv[i][1]=='P')
				fprintf(stderr,
	"cc: warning: -P option obsolete; you should use -E instead\n");
			plist[np++] = argv[i];
			continue;
		case 't':
			if (chpass)
				error("-t overwrites earlier option",(char *)NULL);
			chpass = argv[i]+2;
			if (chpass[0]==0)
				chpass = "012p";
			continue;
		case 'Z':   /* special compiler option - pass sans the Z */
		/*	{ char *src, *dst; /*squish out the Z*/
		/*	  for(  dst = &argv[i][1], src=dst+1;
		/*	        *dst != '\0'; 
		/*	        src++,dst++  ) *dst = *src;
                /*       }
		/*	Zarg = argv[i];*/
			continue;
		} /* switch */
		t = argv[i];
		c = getsuf(t);
		if (c==SRCSUF || c==AASMSUF || exflag) {
			clist[nc++] = t;
			t = setsuf(t, AOBJSUF);
		}
		if (nodup(llist, t)) {
			llist[nl++] = t;
			if (getsuf(t)==AOBJSUF)
				nxo++;
		}
	}
	if (gflag) {
		if (oflag)
			fprintf(stderr, "cc: warning: -g disables -O\n");
		oflag = 0;
	}
	if (npassname && chpass ==0)
		chpass = "012";   
		/* was: chpass = "012p";  jima*/  
	if (chpass && npassname==0)
		{/* was: npassname = "/usr/new";  jima*/
		npassname = strcat(tmpstr2,"o");
		}
	if (chpass)
	for (t=chpass; *t; t++) {
		switch (*t) {

		case '0':
			ccom = 	strspl(npassname, "ccom");
			continue;
		case '2':
			c2   = strspl(npassname, "c2");
			continue;
		case 'p':
			cpp  = strspl(npassname, "cpp");
			continue;
		}
	}
	if (proflag)
		crt0 = MCRTNAME;
	if (nc==0)
		goto nocom;
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, idexit);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, idexit);
	if (pflag==0)
		sprintf(tmp0, "%sctm%05.5d",TEMPDIR,getpid());
	tmp1 = strspl(tmp0, "1");
	tmp2 = strspl(tmp0, "2");
	tmp3 = strspl(tmp0, "3");
	if (pflag==0)
		tmp4 = strspl(tmp0, "4");
	if (oflag)
		tmp5 = strspl(tmp0, "5");
	for (i=0; i<nc; i++) {
		if (nc > 1) {
			printf("%s:\n", clist[i]);
			fflush(stdout);
		}
		if (getsuf(clist[i]) == AASMSUF) {
			assource = clist[i];
			goto assemble;
		} else
			assource = tmp3;
		if (pflag)
			tmp4 = setsuf(clist[i], PPSUF);
		av[0] = "cpp"; av[1] = clist[i]; av[2] = exflag ? "-" : tmp4;
		if (vflag) printf("executing: %s %s %s ",cpp,av[1],av[2]);
		na = 3;
		for (j = 0; j < np; j++)
		{
			av[na++] = plist[j];
			if (vflag) printf(" %s",av[na-1]);
		}
		if (vflag) printf("\n");
		av[na++] = 0;
		if (!nflag) if (callsys(cpp, av)) {
			exfail++;
			eflag++;
		}
		if (pflag || exfail) {
			cflag++;
			continue;
		}
		if (sflag)
			assource = tmp3 = setsuf(clist[i], AASMSUF);
		av[0] = "ccom"; av[1] = tmp4; av[2] = (oflag)?tmp5:tmp3; na =3 ;
		if (vflag) printf("executing: %s  %s -o %s",ccom,av[1],av[2]);
		if (proflag)
		{
			av[na++] = "-XP";
			if (vflag) printf(" %s",av[na-1]);
		}
		if (Fflag)
		{
			av[na++] = "-F";
			if (vflag) printf(" %s",av[na-1]);
		}			
		if (gflag)
		{
			av[na++] = "-l";/* used to pass -Xg */
			if (vflag) printf(" %s",av[na-1]);
		}
		if (wflag)
		{
			av[na++] = "-w";
			if (vflag) printf(" %s",av[na-1]);
		}
		if (Zarg)
		{
			av[na++] = Zarg;
			if (vflag) printf(" %s",av[na-1]);
		}
		if (vflag) printf("\n");
		av[na] = 0;
		if (!nflag) if (callsys(ccom, av)) {
			cflag++;
			eflag++;
			continue;
		}
		if (oflag) {
			av[0] = "c2"; av[1] = tmp5; av[2] = tmp3;
			if (iflag) { av[3] = "-i"; av[4] = (char *)NULL;}
			else av[3] = (char *) NULL;
			
			if (vflag)
			{
			    printf("executing: %s %s %s",c2,av[1],av[2]);
			    printf(iflag?" -i\n":"\n");
			}
			if (!nflag) if (callsys(c2, av)) {
				if (vflag) printf("executing: unlink %s\n",tmp3);
				unlink(tmp3);
				tmp3 = assource = tmp5;
			} else
			{
				unlink(tmp5);
				if (vflag) printf("executing: unlink %s\n",tmp5);
			}
		}
		if (sflag)
			continue;
	assemble:
		if (vflag) printf("executing: unlink ");
		cunlink(tmp1); cunlink(tmp2); cunlink(tmp4);
		if (vflag) printf("\n");
		av[0] = AS; av[1] = "-o"; av[2] = setsuf(clist[i], AOBJSUF);
		if (vflag) printf ("executing: %s %s  %s ",as,av[1],av[2]);
		na = 3;
		if (gflag) /* pass this flag on to the assembler for stabs */
		{
			av[na++] = "-g";
			if (vflag) printf(" %s",av[na-1]);
		}
		if (qflag)
		{	
			av[na++] = "-q";
			if (vflag) printf(" %s",av[na-1]);
		}
		if (Rflag)
		{
			av[na++] = "-R";
			if (vflag) printf(" %s",av[na-1]);
		}
		if (dflag)
		{
			av[na++] = dflag;
			if (vflag) printf(" %s",av[na-1]);
		}
		av[na++] = assource;
		if (vflag) printf(" %s\n",av[na-1]);
		av[na] = 0;
		if (!nflag) if (callsys(as, av) >= 1) {/* added = so 1 is an error status*/
			cflag++;
			eflag++;
			continue;
		}
	}
nocom:
	if (cflag==0 && nl!=0) {
		i = 0;
		av[0] = LD; na = 1;
		if (vflag) printf("executing: %s",ld);
		if (outfile) {
			av[na++] = "-o";
			if (vflag) printf(" %s",av[na-1]);
			av[na++] = outfile;
			if (vflag) printf(" %s",av[na-1]);
		}
		av[na++]=crt0;		
		if (vflag) printf(" %s",av[na-1]);
		if (ldfirst[0]!=0)
		{
			av[na++] = ldfirst;   /* e.g., "s.O" */
			if (vflag) printf(" %s",av[na-1]);
		}
		while (i < nl)
		{
			av[na++] = llist[i++];
			if (vflag) printf(" %s",av[na-1]);
		}
		if (ldlast[0]!=0)
		{
			av[na++] = ldlast; 
			if (vflag) printf(" %s",av[na-1]);
		}
		/* The following is commented out until it is needed, it comes
		/* from the vax and may never be necessary*/
		/*if (gflag)
		/*{
		/*	av[na++] = "-lg";
		/*	if (vflag) printf(" %s",av[na-1]);
		/*}*/
		if (vflag) printf("\n");
		av[na++] = 0;
		if (!nflag) eflag |= callsys(ld, av);
		if (nc==1 && nxo==1 && eflag==0)
		{
		    if (vflag)
		       printf("executing: unlink %s\n",setsuf(clist[0],AOBJSUF));
		    if (!nflag) unlink(setsuf(clist[0], AOBJSUF));
		}
	}
	dexit();
}

idexit()
{

	eflag = 100;
	dexit();
}

dexit()
{

	if (!pflag) {
		if (vflag) printf("executing: unlink ");
		cunlink(tmp1);
		cunlink(tmp2);
		if (sflag==0)
			cunlink(tmp3);
		cunlink(tmp4);
		cunlink(tmp5);
		if (vflag) printf("\n");
	}
	exit(eflag);
}

error(s, x)
	char *s, *x;
{
	FILE *diag = exflag ? stderr : stdout;

	fprintf(diag, "cc: ");
	fprintf(diag, s, x);
	putc('\n', diag);
	exfail++;
	cflag++;
	eflag++;
}

getsuf(as)
char as[];
{
	register int c;
	register char *s;
	register int t;

	s = as;
	c = 0;
	while (t = *s++)
		if (t=='/')
			c = 0;
		else
			c++;
	s -= 3;
	if (c <= DIRSIZ && c > 2 && *s++ == '.')
		return (*s);
	return (0);
}

char *
setsuf(as, ch)
	char *as;
{
	register char *s, *s1;

	s = s1 = savestr(as);
	while (*s)
		if (*s++ == '/')
			s1 = s;
	s[-1] = ch;
	return (s1);
}

callsys(f, v)
	char *f, **v;
{
	int t, status;

	t = vfork();
	if (t == -1) {
		printf("No more processes\n");
		return (100);
	}
	if (t == 0) {
		execv(f, v);
		printf("Can't find %s\n", f);
		fflush(stdout);
		_exit(100);
	}
	while (t != wait(&status))
		;
	if ((t=(status&0377)) != 0 && t!=14) {
		if (t!=2) {
			printf("Fatal error in %s\n", f);
			eflag = 8;
		}
		dexit();
	}
	return ((status>>8) & 0377);
}

nodup(l, os)
	char **l, *os;
{
	register char *t, *s;
	register int c;

	s = os;
	if (getsuf(s) != AOBJSUF)
		return (1);
	while (t = *l++) {
		while (c = *s++)
			if (c != *t++)
				break;
		if (*t==0 && c==0)
			return (0);
		s = os;
	}
	return (1);
}

#define	NSAVETAB	1024
char	*savetab;
int	saveleft;

char *
savestr(cp)
	register char *cp;
{
	register int len;

	len = strlen(cp) + 1;
	if (len > saveleft) {
		saveleft = NSAVETAB;
		if (len > saveleft)
			saveleft = len;
		savetab = (char *)malloc(saveleft);
		if (savetab == 0) {
			fprintf(stderr, "ran out of memory (savestr)\n");
			exit(1);
		}
	}
	strncpy(savetab, cp, len);
	cp = savetab;
	savetab += len;
	saveleft -= len;
	return (cp);
}

char *
strspl(left, right)
	char *left, *right;
{
	char buf[BUFSIZ];

	strcpy(buf, left);
	strcat(buf, right);
	return (savestr(buf));
}
