/*
 * nm.c: version 1.6 of 11/13/83
 * print name list
 */
# ifdef SCCS
static char *sccsid = "@(#)nm.c	1.6	11/13/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

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



/******************************************************************************/
/* nm - print name list; 16032 UNIX string table version                      */
/*                                                                            */
/*                                                                            */
/* change history:                                                            */
/*                                                                            */
/*	CWS, 07-19-82, 	converted to 16032 UNIX a.out.h/stabs.h formats,      */
/*			added "-h", "-l", and "-m" options.                   */
/*                                                                            */
/*      DCF, 02-26-83,  Converted to new format of name list for "-a" option. */
/*                      It is compatible with the syntax of 16K asembler new  */
/*                      features.                                             */
/*                                                                            */
/******************************************************************************/
#include <sys/types.h>
#include <ar.h>
#include <stdio.h>
#include <ctype.h>
#include <a.out.h>
#include <stab.h>
#include <pagsiz.h>
#include <sys/stat.h>

#define	SELECT	archive ? archdr.ar_name : *xargv
#define NOTMSTE	0x4
#define T_XA    14      /*unused HLL type, should be defined in <sys/stab.h>  */
#define T_XB    15      /*unused HLL type, should be defined in <sys/stab.h>  */
#define A_R1  0x01      /*register 1 - r1, should be defined in <sys/stab.h>  */
#define A_R2  0x02      /*register 2 - r2, should be defined in <sys/stab.h>  */
#define A_R3  0x03      /*register 3 - r3, should be defined in <sys/stab.h>  */
#define A_R4  0x04      /*register 4 - r4, should be defined in <sys/stab.h>  */
#define A_R5  0x05      /*register 5 - r5, should be defined in <sys/stab.h>  */
#define A_R6  0x06      /*register 6 - r6, should be defined in <sys/stab.h>  */
#define A_R7  0x07      /*register 7 - r7, should be defined in <sys/stab.h>  */

struct module {
	u_long	stat,
		link,
		program,
		reserved;
};

/* argument flags                                                             */
                                                                              
int	aflg, gflg, hflg, lflg, mflg, nflg, oflg, pflg, uflg; 
int	translate_value = 1;   /*specifies whether we change the value field  */
                               /*in symbol table entries to absolute address. */  
int	rflg = 1;
int	nfsyms;		                 /* the number of symbols in the file */
struct module	*modtbl;
struct module	*curmod;
u_long	*linktbl = NULL;
int	linktblsz = 0;
off_t	txtloc, symloc;     /* locations of text and symbol table within file */
	
char	**xargv;
int	archive;
struct	ar_hdr	archdr;
union {
	char	mag_armag[SARMAG+1];
	struct	exec mag_exp;
} mag_un;
#define	OARMAG	0177545
FILE	*fi, *bfi;
off_t	off, off1;                  /* offset of a.out member in archive file */
off_t	strsiz;                     /* string table size                      */
char	*malloc();
char	*realloc();
char	*strp;

/* nm routines types declarations                                             */
             
off_t	ftell();
int	compare();
int	narg;
int	errs;
char	*amode();
char    *typem();
char    *hllt();
char    *bf();
char    *auxty();
int	etxt = 0;
int	edat = 0;
long    arsize;
char    *tfi,*ffi;
char    *name="/tmp/nmXXXXXX";
#ifdef vax
char *objfile = "a16.out";
#else
char *objfile = "a.out";
#endif
	 

/* Start the main program                                                     */
    
main(argc, argv)
char **argv;
{

	if (--argc>0 && argv[1][0]=='-' && argv[1][1]!=0) {
		argv++;
		while (*++*argv) switch (**argv) {

		case 'n':                         /* print in numerical order */
			nflg++;                   /* by 1st column            */
			continue;                
		case 'g':                         /* print global symbol only */
			gflg++;                   
			continue;
		case 'h':		          /* print a.out header       */
			hflg++;
			continue;
		case 'l':		          /* print link table         */
			lflg++;
			continue;
		case 'm':		          /* print mod table          */
			mflg++;
			continue;
		case 'u':                         /* print undefined symbol   */
			uflg++;
			continue;
		case 'r':                         /* sort name list in        */
			rflg = -1;                /* reversed order           */
			continue;
		case 'p':                         /* don't sort and print in  */
			pflg++;                   /* symbol table order       */
			continue;
		case 'o':                         /* prepend file or archive  */
			oflg++;                   /* name to each line        */
			continue;
		case 'a':                         /* print all symbols in the */
			aflg++;                   /* symbol table             */
			translate_value = 0;
			continue;
		default:
			fprintf(stderr, "nm: invalid argument -%c\n",
			    *argv[0]);
			exit(2);
		}
		argc--;
	}

	/* option for not specify the objective file name                     */
	if (argc == 0) {
		argc = 1;
		argv[1] = objfile;
	}
	narg = argc;
	xargv = argv;
	while (argc--) {
		++xargv;
		namelist();
	}
	exit(errs);
}

/*----------------------------------------------------------------------------*/
/* process the name list in objective file                                    */
/*----------------------------------------------------------------------------*/
    
namelist()
{
	register int j;

	archive = 0;
	mktemp(name);
	fi = fopen(*xargv, "r");
	if (fi == NULL) {
		error(0, "cannot open");
		return;
	}
	off = SARMAG;
	fread((char *)&mag_un, 1, sizeof(mag_un), fi);
	if (mag_un.mag_exp.a_magic == OARMAG) {
		error(0, "old archive");
		return;
	}
	if (strncmp(mag_un.mag_armag, ARMAG, SARMAG)==0)
		archive++;
	else if (N_BADMAG(mag_un.mag_exp)) {
		error(0, "bad format");
		return;
	}
	if (mag_un.mag_exp.a_magic == XMAGIC) etxt = 1024;
	else etxt = 0;
	etxt += (mag_un.mag_exp.a_text + 1023) & 0xfffffc00;
	edat = etxt + mag_un.mag_exp.a_data;
	fseek(fi, 0L, 0);
	if (archive) {
		nextel(fi);
		if (narg > 1)
			printf("\n%s:\n", *xargv);
	} 
	else off1 = 0;
	do {
		off_t loc;
		register i, n, c;
		struct nlist *symp = NULL;
		struct nlist sym, junksym;
		struct stat stb;
		int sz, k1, k2, k3, w; 

		tfi = (char *)malloc(arsize);
		w = fread((char *)tfi, 1, arsize, fi);
		bfi = fopen(name,"w");
		if (archive){
		             w = fwrite(tfi,1,arsize,bfi);
                             fclose(bfi);
                             bfi = fopen(name,"r");
		   }
                else bfi = fi;
                fread((char *)&mag_un.mag_exp, 1, sizeof(struct exec), bfi);
                if (N_BADMAG(mag_un.mag_exp)){
					      fclose(bfi);
					      continue;
					      }
		if (archive == 0) 
	        	fstat(fileno(fi), &stb);
	        txtloc = off1 + N_TXTOFF(mag_un.mag_exp);
		 
                /* read in the module table and print header & module table   */
		 
	        fseek(bfi, txtloc, 0);
		modtbl = (struct module *)malloc(mag_un.mag_exp.a_mod);
		fread((char *)modtbl, 1, mag_un.mag_exp.a_mod, bfi);
	        if (hflg)
		       	headlist();		
	        if (mflg) 
		       	modlist(modtbl);
		symloc = off1 + N_SYMOFF(mag_un.mag_exp);
		fseek(bfi, symloc, 0);
		nfsyms = n = mag_un.mag_exp.a_syms / sizeof(struct nlist);
		if (n == 0) {
			error(0, "no name list");
			free((char *)modtbl);
			continue;
		}
		if (N_STROFF(mag_un.mag_exp) + sizeof (off_t) >
		    (archive ? off : stb.st_size))
			error(1, "old format .o (no string table) or truncated file");
		i = 0;
		if (strp)
			free(strp), strp = NULL;
		while (--n >= 0) {
		        fread((char *)&sym, 1, sizeof(sym), bfi);
			  
                        /* process the MAUX symbol table entries and print    */
			/* the link tables                                    */
	              	if N_MODULE(sym) {
				int sz = 0;
				int oldn = n;
				struct nlist localsym;

				curmod = (struct module *)
				   (((int) modtbl) - mag_un.mag_exp.a_mod_addr 
				   + sym.n_value);
				localsym.n_more = sym.n_more;
				while (localsym.n_more) {
				   --n;
				   fread((char *)&localsym, 1, sizeof(localsym),bfi);
				   if ((localsym.n_etype == N_AUX) &&
				       (localsym.n_auxtype == N_TXTINFO))
					 sz = localsym.n_ldisp2;
				};
				if (aflg) n = oldn;
				fseek(bfi, txtloc + curmod->link, 0);
				if (linktblsz < sz) linktblsz = sz;
				if (linktbl == NULL)
				    linktbl = (u_long *) malloc(sz);
				else linktbl = (u_long *)
					realloc((char *) linktbl, sz);
				fread((char *)linktbl, 1, sz, bfi);
				fseek(bfi, symloc +
					((nfsyms - n) * sizeof(sym)), 0);
	        		if (lflg) {
		       			linklist(sz/sizeof(u_long));
				}
			};
			if (gflg && (sym.n_type&N_EXT)==0)
				continue;
			if ((sym.n_etype & NOTMSTE) 
			    && (!aflg||gflg||uflg)) 
				continue;
			if (translate_value && (sym.n_etype < N_SLINE)) {
				int addrmode = sym.n_addrmode;
				struct nlist tempsym;
				int disp1,disp2;

				tempsym = sym;
				disp1 = sym.n_disp1;
				disp2 = sym.n_disp2;
				sym.n_more = 0;
				    
		                /* if translate_value is on, then the n_more  */
				/* bit means the symbol is common, then check */
				/* common or LDISP type information.          */
				    
				while (tempsym.n_more ) {
					--n;
					fread((char *)&tempsym, 1,
						sizeof(tempsym), bfi);
					if (tempsym.n_etype == N_AUX) {
					   if (tempsym.n_auxtype
						 == N_LDISP) {
						disp1 = tempsym.n_ldisp1;
						disp2 = tempsym.n_ldisp2;
					   }
					   if (tempsym.n_auxtype ==
					   	N_COMMON)
					      sym.n_more = 1;
					}
				}
				if (uflg && addrmode!=A_EX) continue;
				if (addrmode == A_SB) sym.n_value +=
					curmod->stat;
				else if (addrmode == A_PC)
					sym.n_value += curmod->program;
				else if (addrmode == A_EX) {
					sym.n_value = disp2 +
					     linktbl[disp1];
					if (sym.n_value != 0) continue;
				}
			}
			if (symp==NULL) 

				symp = (struct nlist *)
				    malloc(sizeof(struct nlist));
			else     
				symp = (struct nlist *)
				    realloc(symp,
					(i+1)*sizeof(struct nlist));
			if (symp == NULL)
				error(1, "out of memory");
			symp[i++] = sym;
		}
		if (archive && ftell(fi) > off) {
			error(0, "no string table (old format .o?)");
		}
		strsiz = mag_un.mag_exp.a_strings;
		strp = (char *)malloc(strsiz);
		if (strp == NULL)
			error(1, "ran out of memory");
		if (fread(strp,strsiz,1,bfi) != 1)
			error(1, "error reading string table");

                /* sort the symbol table                                      */
		     
		if (pflg==0){
			qsort(symp, i, sizeof(struct nlist), compare);
                }
		if ((archive || narg>1) && oflg==0)
			printf("\n%s:\n", SELECT);
		psyms(symp, strp, i);
		if (symp)
			free((char *)symp), symp = NULL;
		if (strp)
			free((char *)strp), strp = NULL;
		free((char *)modtbl);
		fclose(bfi);
		free((char *)tfi);
	} while(archive && nextel(fi));
out:
	fclose(fi);
}

/*----------------------------------------------------------------------------*/
/* function print the symbols                                                 */
/*----------------------------------------------------------------------------*/

psyms(symp, strp, nsyms)
	register struct nlist *symp;
	int nsyms;
	char  *strp;
{
	register int n, c;

	for (n=0; n<nsyms; n++) {
		struct nlist *mysymp = &(symp[n]);

	        oprint();
		 
                /* print symbol table entries for all other options           */
                  
		if ((!(mysymp->n_etype & NOTMSTE)) && (!aflg||gflg||uflg)) {
			c = mysymp->n_addrmode;
			if (c == A_EX) {
				if (mysymp->n_more) c = 'c';
				else c = 'u';
			}
			else if (c == A_SB)
			  if (mysymp->n_value < edat) c = 'd';
			  else c = 'b';
			else if (c == A_PC) c = 't';
			else c = 'a';

			if (mysymp->n_ext) c = toupper(c);
			if (!uflg) {
				if (c=='u' || c=='U')
					printf("        ");
				else
				   printf("%08x", mysymp->n_value);
				printf(" %c ", c);
			}
			if(mysymp->n_namestr)
			   mysymp->n_namestr = mysymp->n_namestr +
					       (u_long)strp;
                        else
			   mysymp->n_namestr = (u_long)strp;
			printf("%s\n", mysymp->n_namestr);
	        	continue;
		}
		 
                /* if "-a" option is on then print all the symbols            */
		  
		printf("%08x - %08x %08x ",	
		    mysymp->n_value,		
		    mysymp->n_name,          
		    mysymp->n_type);
                if(mysymp->n_namestr)
		   mysymp->n_namestr = mysymp->n_namestr +
				       (u_long)strp;
		else
		   mysymp->n_namestr = (u_long)strp;
                if(mysymp->n_etype == N_AUX)
		    printf(".stab");
                else printf("%s",mysymp->n_namestr);

		/* print the type informations                                */
		 
                switch(mysymp->n_etype){
		      case 1 :
		      case 2 :
		      case 3 :
			      ntf(mysymp);
			      break;
                      case 0 :
			      nat(mysymp);
			      break;
                      case 6 :
		      case 7 :
			      if(mysymp->n_typeinfo) nat(mysymp);
			      else naux(mysymp);
			      break;
                      case 4 :
		      case 5 :
			      printf("{ .source }");
			      break;
                }

		/* print the addressing information                           */

		if(mysymp->n_etype <= N_SCHAR) amode(mysymp);
		else printf("\n");
        }
}
/******************************************************************************/
/*                                                                            */
/* Old stuff history.                                                         */
/*----------------------------------------------------------------------------*/
/*
		c = symp[n].n_type;
		if (c & N_STAB) {
			oprint();

			continue;
		}
		switch (c&N_TYPE) {

		case N_UNDF:
			c = 'u';
			if (symp[n].n_value)
				c = 'c';
			break;
		case N_ABS:
			c = 'a';
			break;
		case N_TEXT:
			c = 't';
			break;
		case N_DATA:
			c = 'd';
			break;
		case N_BSS:
			c = 'b';
			break;
		case N_FN:
			c = 'f';
			break;
		}
		if (uflg && c!='u')
			continue;
		oprint();
		if (symp[n].n_type&N_EXT)
			c = toupper(c);
		if (!uflg) {
			if (c=='u' || c=='U')
				printf("        ");
			else
				printf(N_FORMAT, symp[n].n_value);
			printf(" %c ", c);
		}
		printf("%s\n", symp[n].n_un.n_namestr);
l1:		;
                                                                              */
/*----------------------------------------------------------------------------*/
/* End of old stuff history.                                                  */
/*                                                                            */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* function determine qsort order based on nflg & rflg                        */
/*----------------------------------------------------------------------------*/
 
compare(p1, p2)
struct nlist *p1, *p2;
{
	register i;

	if (nflg) {
		if (p1->n_value > p2->n_value)
			return(rflg);
		if (p1->n_value < p2->n_value)
			return(-rflg);
	}
	return (rflg * strcmp(p1->n_namestr, p2->n_namestr));
}

/*----------------------------------------------------------------------------*/
/*  function find next element (a.out part) of archive file                   */
/*----------------------------------------------------------------------------*/
 
nextel(af)
FILE *af;
{
	register char *cp;
	register r;

	off1 = 0;
	fseek(af, off, 0);
	r = fread((char *)&archdr, 1, sizeof(struct ar_hdr), af);
	if (r != sizeof(struct ar_hdr))
		return(0);
	for (cp = archdr.ar_name; cp < &archdr.ar_name[sizeof(archdr.ar_name)]; cp++)
		if (*cp == ' ')
			*cp = '\0';
	arsize = atol(archdr.ar_size);
	if (arsize & 1)
		++arsize;
	off = ftell(af) + arsize;                /* beginning of next element */
	return(1);
}

/*----------------------------------------------------------------------------*/
/* function print error message, exit if serious error                        */
/*----------------------------------------------------------------------------*/
 
error(n, s)
char *s;
{
	fprintf(stderr, "nm: %s:", *xargv);
	if (archive) {
		fprintf(stderr, "(%s)", archdr.ar_name);
		fprintf(stderr, ": ");
	} else
		fprintf(stderr, " ");
	fprintf(stderr, "%s\n", s);
	if (n)
		exit(2);
	errs = 1;
}

/*----------------------------------------------------------------------------*/
/* function print formatted header record                                     */
/*----------------------------------------------------------------------------*/
 
headlist()
{
	oprint(); printf(" \n");
	oprint(); printf("--header record--\n");
	oprint(); printf(" \n");
	oprint(); printf("a_magic:     %08x %012lu\n",
		mag_un.mag_exp.a_magic, mag_un.mag_exp.a_magic);
	oprint(); printf("a_text:      %08x %012lu\n",
		mag_un.mag_exp.a_text, mag_un.mag_exp.a_text);
	oprint(); printf("a_data:      %08x %012lu\n",
		mag_un.mag_exp.a_data, mag_un.mag_exp.a_data);
	oprint(); printf("a_bss:       %08x %012lu\n",
		mag_un.mag_exp.a_bss, mag_un.mag_exp.a_bss);
	oprint(); printf("a_syms:      %08x %012lu\n",
		mag_un.mag_exp.a_syms, mag_un.mag_exp.a_syms);
	oprint(); printf("a_entry:     %08x %012lu\n",
		mag_un.mag_exp.a_entry, mag_un.mag_exp.a_entry);
	oprint(); printf("a_entry_mod: %08x %012lu\n",
		mag_un.mag_exp.a_entry_mod, mag_un.mag_exp.a_entry_mod);
	oprint(); printf("a_trsize:    %08x %012lu\n",
		mag_un.mag_exp.a_trsize, mag_un.mag_exp.a_trsize);
	oprint(); printf("a_drsize:    %08x %012lu\n",
		mag_un.mag_exp.a_drsize, mag_un.mag_exp.a_drsize);
	oprint(); printf("a_mod:       %08x %012lu\n",
		mag_un.mag_exp.a_mod, mag_un.mag_exp.a_mod);
	oprint(); printf("a_link:      %08x %012lu\n",
		mag_un.mag_exp.a_link, mag_un.mag_exp.a_link);
	oprint(); printf("a_strings:   %08x %012lu\n",
		mag_un.mag_exp.a_strings, mag_un.mag_exp.a_strings);
        oprint(); printf("a_text_addr: %08x %012lu\n",
		mag_un.mag_exp.a_text_addr, mag_un.mag_exp.a_text_addr);
        oprint(); printf("a_mod_addr:  %08x %012lu\n",
		mag_un.mag_exp.a_mod_addr, mag_un.mag_exp.a_mod_addr);
        oprint(); printf("a_dat_addr:  %08x %012lu\n",
		mag_un.mag_exp.a_dat_addr, mag_un.mag_exp.a_dat_addr);
	oprint(); printf(" \n");
}

/*----------------------------------------------------------------------------*/
/* function print formatted module table                                      */
/*----------------------------------------------------------------------------*/
 
modlist(modp)
struct module *modp;
{
	int	i, n;
	oprint(); printf(" \n");
	oprint(); printf("--module table--\n");
	oprint(); printf(" \n");
	n = mag_un.mag_exp.a_mod / 16;	/* number of modules(/entries) */
	for (i = 0; i < n; i++) {
		oprint();
		printf("static base:  %08x %012lu\n", modp->stat,
			modp->stat);
		oprint();
		printf("link base:    %08x %012lu\n", modp->link,
			modp->link);
		oprint();
		printf("program base: %08x %012lu\n", modp->program,
			modp->program);
		oprint();
		printf("(reserved):   %08x %012lu\n", modp->reserved,
			modp->reserved);
		++modp;
		oprint(); printf(" \n");
	}
}

/*----------------------------------------------------------------------------*/
/* function print formatted link table                                        */
/*----------------------------------------------------------------------------*/
 
linklist(nitems)
{
	int	i, n;
	struct {
		union {
			u_long linkval;
			struct {
				u_short	linkval1;
				u_short linkval2;
			} l_s
		} l_u
	} l_v;	
	oprint(); printf(" \n");
	oprint(); printf("--link table for module %04x--\n",
			  ((int)curmod) - ((int)modtbl));
	oprint(); printf(" \n");
	n = nitems;
	if (n == 0) {
		printf("no link table\n");
		return;
	}
	for (i = 0; i < n; i++) {
		l_v.l_u.linkval = linktbl[i];
		oprint(); 
		printf("%08x %04x %04x  %012lu %06lu %06lu\n", 
		    	l_v.l_u.linkval, l_v.l_u.l_s.linkval1, 
			l_v.l_u.l_s.linkval2, l_v.l_u.linkval, 
			l_v.l_u.l_s.linkval1, l_v.l_u.l_s.linkval2);
	}
	oprint(); printf(" \n");
}

/*----------------------------------------------------------------------------*/
/* function prefix lines with (archive and) file name(s)                      */
/*----------------------------------------------------------------------------*/
 
oprint()
{
	if (oflg) {
		if (archive)
			printf("%s:", *xargv);
		printf("%s:", SELECT);
	}
}

/*----------------------------------------------------------------------------*/
/* function print the addressing mode information.                            */
/*----------------------------------------------------------------------------*/

struct	amodenames {
	int	amode_value;
	char	*amode_name;
} amodenames[] ={
	A_R0,	"r0",
	A_R1,   "r1",
	A_R2,   "r2",
	A_R3,   "r3",
	A_R4,   "r4",
	A_R5,   "r5",
	A_R6,   "r6",
	A_R7,   "r7",
	A_R0R,	"r0",
	A_R1R,	"r1",
	A_R2R,	"r2",
	A_R3R,	"r3",
	A_R4R,	"r4",
	A_R5R,	"r5",
	A_R6R,	"r6",
	A_R7R,	"r7",
	A_FPR,	"FP",
	A_SPR,	"SP",
	A_SBR,	"SB",
	A_IMM,	"",
	A_ABS,	"@",
	A_EX,	"EXT",
	A_TOS,	"TOS",
	A_FP,	"FP",
	A_SP,	"SP",
	A_SB,	"SB",
	A_PC,	"PC",
	0, 	0
};

char *
amode(mysymp)
 struct nlist *mysymp;
{
	register struct amodenames *sp;
	int i, j, k, m;
	     
	for (sp = amodenames; sp->amode_name; sp++){
	    i = sp->amode_value;
	    j = mysymp->n_addrmode;
	    if ((i == j) && (j <= A_R7)){
	       printf("[%s]\n",sp->amode_name);
	       return;
	       }
            else if ((i == j) && (j <= A_R7R)){
		    printf("[%d(%s)]\n",mysymp->n_value,sp->amode_name);
		    return;
		    }
                 else if ((i == j) && (j <= A_SBR)){
			 printf("[%d(%d(%s))]\n",mysymp->n_disp2,
				mysymp->n_disp1,sp->amode_name);
                         return;
			 }
            if ((i == j) && (j == A_IMM)){
	       printf("[%d]\n",mysymp->n_value);
	       return;
	       }
            if ((i == j) && (j == A_ABS)){
	       printf("[%s%d]\n",sp->amode_name,mysymp->n_value);
	       return;
	       }
            if ((i == j) && (j == A_EX)){
	       printf("[%s(%d)+%d]\n",sp->amode_name,mysymp->n_disp1,
						     mysymp->n_disp2);
               return;
	       }
            if ((i == j) && (j == A_TOS)){
	       printf("[%s]\n",sp->amode_name);
	       return;
	       }
            if ((i == j) && (j >= A_FP) && (j <= A_PC)){
	       printf("[%d(%s)]\n",mysymp->n_value,sp->amode_name);
	       return;
	       }
        }
	printf("[%02x]\n",mysymp->n_addrmode);
}
/******************************************************************************/
/*                                                                            */
/*                         OLD STUFF HISTORY                                  */
/*----------------------------------------------------------------------------*/
/*

struct	stenames {
	int	ste_value;
	char	*ste_name;
} stenames[] ={
	N_MAT,	"N_MAT  ",
	N_MTN,	"N_MTN  ",
	N_MPTN,	"N_MPTN ",
	N_METN,	"N_METN ",
	N_SLINE,"N_SLINE",
	N_SCHAR,"N_SCHAR",
	N_MAUX,	"N_MAUX ",
	N_AUX,	"N_AUX  ",
	0, 	0
};

******************************************************************
* return name/string corresponding to a n_etype (STE type) value *
******************************************************************
char *
ste(val)
{
	register struct stenames *sp;
	static char prbuf[32];

	for (sp = stenames; sp->ste_name; sp++)
		if (sp->ste_value == val)
			return (sp->ste_name);
	sprintf(prbuf, "%02x", val);
	return (prbuf);
}

struct	hllptnames {
	int	hllpt_value;
	char	*hllpt_name;
} hllptnames[] ={
	T_UNDEF,	"T_UNDEF ",
	T_INT,		"T_INT   ",
	T_CHAR,		"T_CHAR  ",
	T_UINT,		"T_UINT  ",
	T_SETE,		"T_SETE  ",
	T_SET,		"T_SET   ",
	T_FLOAT,	"T_FLOAT ",
	T_DFLOAT,	"T_DFLOAT",
	T_STRUCT,	"T_STRUCT",
	T_UNION,	"T_UNION ",
	T_ENUM,		"T_ENUM  ",
	T_ENUME,	"T_ENUME ",
	T_BOOL,		"T_BOOL  ",
	T_MORE,		"T_MORE  ",
	0, 		0
};

********************************************************
* return name/string corresponding to n_hllptype value *
********************************************************
char *
hllpt(val)
{
	register struct hllptnames *sp;
	static char prbuf[32];

	for (sp = hllptnames; sp->hllpt_name; sp++)
		if (sp->hllpt_value == val)
			return (sp->hllpt_name);
	sprintf(prbuf, "%02x", val);
	return (prbuf);
}

struct	tmvalnames {
	int	tmval_value;
	char	*tmval_name;
} tmvalnames[] ={
	M_NONE,	"M_NONE ",
	M_PTR,	"M_PTR  ",
	M_FUN,	"M_FUN  ",
	M_ARRAY,"M_ARRAY",
	0,	0
};

***************************************************************************
* return name/string corresponding to a type modifier (n_hllmod1/2) value *
***************************************************************************
char *
tmval(val)
{
	register struct tmvalnames *sp;
	static char prbuf[32];

	for (sp = tmvalnames; sp->tmval_name; sp++)
		if (sp->tmval_value == val)
			return (sp->tmval_name);
	sprintf(prbuf, "%02x", val);
	return (prbuf);
}

struct	auxtnames {
	int	auxt_value;
	char	*auxt_name;
} auxtnames[] ={
	N_MODTYPE,	"N_MODTYPE",
	N_TXTINFO,	"N_TXTINFO",
	N_DATINFO,	"N_DATINFO",
	N_COMMON,	"N_COMMON ",
	N_LDISP,	"N_LDISP  ",
	N_LIFE,		"N_LIFE   ",
	N_ARBND,	"N_ARBND  ",
	0,		0
};

*********************************************************
* return name/string corresponding to a n_auxtype value *
*********************************************************
char *
auxt(val)
{
	register struct auxtnames *sp;
	static char prbuf[32];

	for (sp = auxtnames; sp->auxt_name; sp++)
		if (sp->auxt_value == val)
			return (sp->auxt_name);
	sprintf(prbuf, "%02x", val);
	return (prbuf);
}

/*----------------------------------------------------------------------------*/
/*                       END OF OLD STUFF HISTORY                             */
/*                                                                            */
/******************************************************************************/



/******************************************************************************/
/* Functions print out the new version symbol table according to              */
/* NS16K assembler syntax.                                                    */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* function print n_tf information                                            */
/*----------------------------------------------------------------------------*/

ntf(mysymp)
 struct nlist *mysymp;
{

 if (mysymp->n_etype == N_MTN) printf("{ tn ");
 else if (mysymp->n_etype == N_MPTN) printf("{ ptn ");
      else printf("{ etn ");
 bfg(mysymp);
 printf(", %s } ",mysymp->n_typestr);
}

/*----------------------------------------------------------------------------*/
/* function print n_at information                                            */
/*----------------------------------------------------------------------------*/

nat(mysymp)
 struct nlist *mysymp;
{
 if(mysymp->n_etype == N_MAT) printf("{");
 else if(mysymp->n_etype == N_MAUX) printf("{ mtaux");
      else printf("{ taux");
 printf("%s", typem(mysymp->n_hllmod1));
 printf("%s", typem(mysymp->n_hllmod2));
 printf("%s", hllt(mysymp->n_hllptype));
 printf("%s", bf(mysymp->n_bfield));
 bfg(mysymp);
 tfg(mysymp);
 printf(" } ");
}

/*----------------------------------------------------------------------------*/
/* function print n_aux information                                           */
/*----------------------------------------------------------------------------*/

naux(mysymp)
 struct nlist *mysymp;
{
 if(mysymp->n_etype == N_AUX) printf("{ aux");
 else if(mysymp->n_etype == N_MAUX) printf("{ maux");
 printf("%s", auxty(mysymp->n_auxtype));
 bfg(mysymp);
 aufg(mysymp);
 printf(" } ");
}

/*----------------------------------------------------------------------------*/
/* function print type_mod                                                    */
/*----------------------------------------------------------------------------*/

struct hllmod{
       int    hll1_value;
       char   *hll1_name;
}hllmod[] = {
		M_NONE,    "",
		M_PTR,     " ptrto",
		M_FUN,     " funret",
		M_ARRAY,   " arrayof",
		-1,        0
               };

char *
typem(val)
{
 register struct hllmod    *sp;
 static char prbuf[32];

 for (sp = hllmod; sp->hll1_name; sp++)
     if (sp->hll1_value == val)
	return(sp->hll1_name);
 sprintf(prbuf," %02x",val);
 return(prbuf);
}

/*----------------------------------------------------------------------------*/
/* function print hlltype                                                     */
/*----------------------------------------------------------------------------*/

struct hlltype{
       int     hll_value;
       char    *hll_name;
}hlltype[] = {
	       T_UNDEF,  " undef",
	       T_INT,    " int",
	       T_CHAR,   " char",
	       T_UINT,   " uint",
	       T_SETE,   " sete",
	       T_SET,    " set",
	       T_FLOAT,  " float",
	       T_DFLOAT, " dfloat",
	       T_STRUCT, " struct",
	       T_UNION,  " union",
	       T_ENUM,   " enum",
	       T_ENUME,  " enume",
	       T_BOOL,   " bool",
	       T_MORE,   " tmore",
	       T_XA,     " xa",
	       T_XB,     " xb",
	       -1,       0 
              };

char *
hllt(val)
{
 register struct hlltype    *sp;
 static char  prbuf[32];

 for (sp = hlltype; sp->hll_name; sp++)
     if (sp->hll_value == val)
	return (sp->hll_name);
 sprintf(prbuf," %02x",val);
 return(prbuf);
}

/*----------------------------------------------------------------------------*/
/* function print bfield                                                      */
/*----------------------------------------------------------------------------*/

typedef unsigned char    BYTE;
struct bfield{
       int    b_value;
       char   *b_name;
}bfield[] = {
	     0,  "",
	     8,  " b",
	     16, " w",
	     32, " d",
	     -1, 0
            };
struct bsfield{
	 union{
	       BYTE bs_value;
	       struct{
		      BYTE
		      b1   :3,
		      b2   :5;
                     }bsst;
              }bsu;
              }bs_field;
char *
bf(val)
{
 register struct bfield    *sp;
 for (sp = bfield; sp->b_name; sp++)
     if (sp->b_value == val) return(sp->b_name);
 bs_field.bsu.bs_value = val;
 printf(" : %d,%d", bs_field.bsu.bsst.b1,
		   bs_field.bsu.bsst.b2);
 return(0);
}

/*----------------------------------------------------------------------------*/
/* function print auxtype                                                     */
/*----------------------------------------------------------------------------*/

struct autype{
       int   aut_value;
       char  *aut_name;
}autype[]={
	      N_MODTYPE, " module",
	      N_TXTINFO, " txt",
	      N_DATINFO, " dat",
	      N_COMMON,  " common",
	      N_LDISP,   " ldisp",
	      N_LIFE,    " life",
	      N_ARBND,   " arbnd"
	      -1,        0
             };

char *
auxty(val)
{
 register struct autype    *sp;
 static char prbuf[32];

 for (sp = autype; sp->aut_name; sp++)
     if(sp->aut_value == val)
       return(sp->aut_name);
 sprintf(prbuf," %02x",val);
 return(prbuf);
}

/*----------------------------------------------------------------------------*/
/* print basic_flag.                                                          */
/*----------------------------------------------------------------------------*/

bfg(mysymp)
 struct nlist *mysymp;
{
 if(mysymp->n_last) printf(" end");
 else if(mysymp->n_ext) printf(" ex");
      else if(mysymp->n_more) printf(" more");
}

/*----------------------------------------------------------------------------*/
/* function print type_flag                                                   */
/*----------------------------------------------------------------------------*/

tfg(mysymp)
 struct nlist *mysymp;
{
 if(mysymp->n_begin) printf(" begin");
 else if(mysymp->n_proc) printf(" f,%d", mysymp->n_nargs);
}

/*----------------------------------------------------------------------------*/
/* function print aux_flag                                                    */
/*----------------------------------------------------------------------------*/

aufg(mysymp)
 struct nlist *mysymp;
{
 if(mysymp->n_initialized) printf(" init");
 else if(mysymp->n_rom) printf(" rom");
      else if(mysymp->n_no_ext) printf(" noext");
}

/******************************************************************/
/* End of the new functions.                                      */
/******************************************************************/

