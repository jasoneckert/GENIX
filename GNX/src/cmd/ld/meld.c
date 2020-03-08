/*
 * %M%: version %I% of %G%
 * %Y%
 */

static char *copyright =
    "%Z%Copyright (c) 1982 National Semiconductor Corp.; version %I%";
# ifdef SCCS
static char *sccsid = "%W% (NSC) %G%";
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


/*
 * Mesa linker. 7/8/82
 * Initial coding, version 1 -  Ross Harvey.
 * Added support for "new" object file format, version 2 - Mike Puckett.
 * Added support for %initfun, version 2 -  Mike Puckett.
 * Added support for common, version 3 - Bill Jolitz.
 * Added library capibility, version 4 - David Bell (DIB).
 * Converted to mesa a.out format, version 5 - Mike Puckett
 *  now under SCCS control, refer to SCCS for update information
 */

#include <signal.h>
#include "ross.h"
#include <stdio.h>
#include "system.h"
#include <a.out.h>
#include <ar.h>
#include <stab.h>
#define export
#include "meld2.h"
#undef  export
#include "symbols.h"
#include "lookup.h"

int                      cleanup();
static struct sy_module  *globalmodfun, *lastmod;
static struct sy_import  *nexti;
static struct sy_locimport  *nextl;
static struct sy_import  *initi;
static struct sy_module  initmod;
static file	         *mapfile;
/* default map file name is "ldmap". But user can use -mfilename option to
   redefine it (mapnamesuffix and mapchar are used for catching it).       */
static char              *mapname = "ldmap";
static char		 *mapnamesuffix, *mapchar;
static char		 *fmtstr = "ld. -%c flag not implemented yet.\n";
static int	         outtxtloc;
static int	         outdatloc;
static struct exec       outfilehead;
static int	         text_follows_mod;
static int	         pass;
static int	         modn,modextend;
static int	         ac,ac1;
static char              **av,**av1;
static int               rom, magicvalue;
static int               modbias=2;	/* 2, unless -1, then 1 */
static int               progtotal,statictotal,
                         meld_end_pure,meld_end_image;
static logical           unlinkfun ;
int		outbuffer[BUFSIZ/sizeof(int)], inbuffer[BUFSIZ/sizeof(int)];
int		inbuffer1[BUFSIZ/sizeof(int)];

extern char              *malloc(), *alloc();
extern int               libsearch();

#ifndef fastio
#define fastread fread

ftransfer(count,in,out)
register count;
register FILE *in,*out;
	{
	while(count--)
		putc(getc(in),out);
	}
#endif

main(lac,lav)
char  **lav;
	{
	char  modifier;
	int modvalue;

        /* do cleanup when interrupted */
	signal(SIGINT,cleanup);

	/* allow the mod_addr and dat_addr to be set explicitly */
	outfilehead.a_mod_addr = 0;
	text_follows_mod = 1;
	outfilehead.a_dat_addr = -1;
	ac1 = ac = lac;
	av1 = av = lav;
	pass = 1;
	outfilehead.a_magic =  ZMAGIC;
	start.name = NULL;
	globalmodfun = modlist;
	opensymfile();
	for(;ac>1; --ac,++av) {
		rom = -1;
		if(av[1][0]=='-') {
			switch(av[1][1]) {
			    struct sy_export *exfun;
			    default:
				if(av[1][1]=='\0') {
					av[1] = "a16.out";
					break;
				}{  char msg[60];
					sprintf(msg,
					"option <%.30s> not valid",av[1]);
					fatal(msg);
				}
			    case '0':
			    case '1':
			    case '2':
			    case '3':
				modbias = av[1][1] & 7;
				continue;
			    case 'v':
				--debug;
				printf("ld. pass 1.\n");
				continue;
			    case 'm':
				--map;
				if(av[1][2] != '\0') {
				  mapchar = mapname;
				  mapnamesuffix = &av[1][2]; 
				  for(;*mapnamesuffix;)
				    *mapchar++ = *mapnamesuffix++;
                                  *mapchar = 0;
				  printf("User specified map file name = \t%s\n",mapname);
                                } 
				continue;
			    case 'M':
				--verbose;
				continue;
			    case 'w':
				warnings = false;
				continue;
			    case 'r':
				relinking = true;
				continue;
			    case 'R':
				next("-R");
				sscanf(av[1],"%X",&rom);
				next("-R #");
				break;
/*			    case 'q':
				next("-q");
				exfun = lookup(av[1],dt_magic);
				next("-q s");
				sscanf(av[1],"%X",&magicvalue);
				exfun->offset = magicvalue;
				exfun->dtype  = dt_magic;
				exfun->reltype= SB_rel;
				continue;
*/
			    case 'b':
				printf("ld. -b flag is obsolete, please use -T instead.\n");
			    case 'T':
				next("-T");
				sscanf(av[1],"%X",&outfilehead.a_text_addr);
				if (text_follows_mod)
				    outfilehead.a_mod_addr = outfilehead.a_text_addr;
				if(outfilehead.a_text_addr&0xf)
					fatal("base address not aligned");
				continue;
			    case 'a':
				modifier = av[1][2];
				next("-a");
				sscanf(av[1],"%X", &modvalue);
				if (modifier == 'm') {
					outfilehead.a_mod_addr = modvalue;
					text_follows_mod = 0;
					}
				else if (modifier == 's')
					outfilehead.a_dat_addr = modvalue;
				else printf("ld. -a%c option ignored\n",
						modifier);
				continue;
			    case 't':
				printf("ld. Please use -e to specify the entry point.\n");
			    case 'e':
				next("-e");
				start.name = av[1];
				continue;
			    case 'o':
				next("-o");
				imagename = av[1];
				continue;
			    case 'p':
				--staticpack;
				continue;
			    case 's':
				--stripsymbols;
			/*printf("ld. -s flag not implemented yet.\n");*/
				continue;
			    case 'S':
				/* strip everything but locals and
				globals and modules */
				--striprandom;
				printf(fmtstr,av[1][1]);
				continue;
			    case 'x':
				--striplocal;
				printf(fmtstr,av[1][1]);
				continue;
			    case 'X':
				--striplocalL;
				printf(fmtstr,av[1][1]);
				continue;
			    case 'A':
			    case 'D':
			    case 'd':
			    case 'N':
			    case 'n':
			    case 'u':
			    case 'y':
				printf(fmtstr,av[1][1]);
				continue;
			    case 'z':
				printf("ld. -z is the default.\n");
				continue;
			    case 'l':
				libsearch(&av[1][2], 0);
				continue;
			}
		}
		add_file(av[1]);	/* add file to module list */
	}
	output = fopen(imagename,"w");
	if(output==NULL) {
		fprintf(stderr, "ld. Can't create ");
		perror(imagename);
		exit(1);
	} 
	setbuf(output,outbuffer);
	if(modlist==NULL) {		/* no modules to link, crash out */
		printf(stderr,"ld. No modules to link ");
		exit(1);
	}
	if (~relinking)
		{
		if (debug) printf ("	module %s\n", initname);
		add_initmod(globalmodfun);
		}
	compute();
	if(debug)
		printf("ld. pass 2.\n");
	pass = 2;
	meld0();
	if(!stripsymbols) {
	  if(debug) printf("ld. writing symbol table.\n");
	  writesymbols();
        }
	else if(debug)printf("ld: -s option, symbol table stripped!\n");
	fclose(output);
#ifdef	ON16K + KERN
	chmod (imagename, 0777 & ~umask(0));
#endif	/* VAX or CUST */
	if(map)
		printmap();
	if (unlinkfun && ~relinking)
		exit (1);
        if (dum_file_lib) exit(1);       /* User has specified non-existing */
					 /* file or/and library */
	if (multipledef) exit(1);        /* multiply defined symbol */
	if(debug)
		printf("ld. end.\n");
	}

cleanup()
{
  
/* unlink temporary file and ld map file if they exist */

	if(mapfile != NULL) {
	  fclose(mapfile);
	  unlink(mapname);
	  /* printf("Ld:  Interrupt! Cleaning up mapfile.\n"); */
        }
	if(symfile != NULL) {
	  fclose(symfile);
	  unlink(symname);
	  if(debug) printf("ld:  Interrupt! Cleaning up symfile.\n"); 
	}
	if(debug) printf("ld:  Interrupt! Exit.\n"); 
	exit(1);
}

next(s)
char *s;
{
	if(--ac<=1) {
		printf("ld. error. expected argument after <%s>\n", s);
		error(nil);
		exit(1);
	}
	++av;
}

add_initmod(modfun)		/* Add the magic initmod */
struct sy_module *modfun;
	{
	int	entries;		/* # of import entries */
	int	bdisp;			/* #byte displacement instructions */
	int	wdisp;			/* #word      "           "        */
	int	rxp0 = 2;		/* size of rxp 0 instruction	   */
	register struct sy_export *ex;
	register struct sy_module *md;
	struct	nlist	msym1, msym2, msym3, isym;

	md = &initmod;
	entries = md->link_size;
	md->mod_number = modn++ + modbias;
	md->name = initname;
	md->rom  = -1;
	md->next = nil;
	bdisp = (entries < 64) ? entries:63;
	wdisp = entries - bdisp;
	md->program_size = bdisp*2 + wdisp*3 + rxp0;
	/* export this module name */
	/* need a symbol generated for this name */
	ex = lookup(initname,dt_export);
	ex->mod = md;
	ex->offset = 0;
	ex->reltype = PB_rel;
	ex->dtype = dt_export;
	/* link into module list */
	modfun->next = md;
	/* create STEs for module */
	msym1.n_name = 0;
	msym1.n_type = 0;
	msym1.n_namestr =  ex->disp;
	msym1.n_etype = N_MAUX;
	msym1.n_more = 1;
	msym1.n_value = md->mod_number * sizeof(struct modentry) +
						outfilehead.a_mod_addr;
	outsymbol (&msym1);
	symtotal++;
	msym2.n_type = 0;
	msym2.n_etype = N_AUX;
	msym2.n_more = 1;
	msym2.n_auxtype = N_TXTINFO;
	msym2.n_ldisp1 = md->program_size;
	msym2.n_ldisp2 = md->link_size*4;
	outsymbol (&msym2);
	symtotal++;
	/* prepare info for initfun symbol */
	isym.n_name = 0;
	isym.n_addrmode = A_PC;
	isym.n_type = 0;
	isym.n_namestr =  ex->disp;
	isym.n_etype = N_MAT;
	isym.n_ext = 1;
	isym.n_proc = 1;
	isym.n_typeinfo = 1;
	isym.n_value = ex->offset;
	{
	  register struct sy_module *modfun;
	  int	tmp;

	  ex=nil;
	  for(modfun=modlist; modfun!=nil; modfun = modfun->next) {
		register struct sy_import *ientry = modfun->ilist;
		register struct sy_export *eentry;

		for(;ientry!=nil;ientry = ientry->next) {
			if(ientry->common == false || ientry->common_size == 0)
				continue;
			eentry = lookup(ientry->name,dt_imponly);
			if(eentry->dtype != dt_imponly)
				continue;
			if (debug)
				printf ("COMMON symbol exporting %s\n", ientry->name);
			eentry->offset = align(md->static_size,1,&tmp);
			md->static_size += ientry->common_size + tmp;
			eentry->reltype = SB_rel;	 /* EXT mode */
			eentry->mod = (struct sy_module *)ex;
			eentry->dtype = dt_export;
			  /* we are making a list of the symbols so
			   * we can emit the symbol table entries soon. */
			ex = eentry;
		}
	  }
	}
	msym3.n_name = 0;
	msym3.n_type = 0;
	msym3.n_etype = N_AUX;
	msym3.n_auxtype = N_DATINFO;
	msym3.n_value = md->static_size;
	outsymbol (&msym3);
	symtotal++;
	/* create STE for entry name */
	outsymbol (&isym);
	symtotal++;

	/* Emit symbol table entries for common storage areas */
	{
		struct sy_name *nm;

		while (ex != nil) {
			nm = nlookup(ex->name);
			if (debug)
			  printf("Making common symbol table entry for %s\n",
			  	nm->name);
			isym.n_name = 0;
			isym.n_addrmode = A_SB;
			isym.n_namestr =  nm->disp;
			isym.n_type = 0;
			isym.n_etype = N_MAT;
			isym.n_ext = 1;
			isym.n_typeinfo = 1;
			isym.n_value = ex->offset;
			outsymbol (&isym);
			symtotal++;
			{register struct sy_module *tmod;
			    tmod = ex->mod;
			    ex->mod = md;
			    ex=(struct sy_export *)tmod;
			}
		}
	}
}


gen_initcode()		/* Generate code for the magic initmod */
{
    char cxp = 0x22;	/* cxp instruction */
    char rxp = 0x32;	/* rxp instruction */
    int bdisp,wdisp;	/* # of byte/word displacement instructions */
    register i,entries;

    entries = initmod.link_size;
    bdisp = (entries < 64) ? entries:63;
    wdisp = entries - bdisp;
    dosync(initmod.psync);
    for (i=0; i<bdisp; i++) {
	putc(cxp,output);		/* output cxp */
	putc(i,output);			/* and byte displacement */
    }
    for (i=0; i<wdisp; i++) {
	putc(cxp,output);		/* output cxp */
	putc((i >> 8)|0x80,output);		/* and word displacement */
	putc(i     ,output);
    }
    putc(rxp,output);			/* output rxp */
    putc(0  ,output);			/* output 0   */
}

meld0()
{
	register struct sy_module *modfun;
	register struct sy_file   *thisfile;
	struct modentry *modtable, *getmodtable();

	register nmods,i;
	int textmark,datamark;

	outtxtloc = N_TXTOFF(outfilehead);
	outdatloc = outtxtloc + outfilehead.a_text;
	end_of_image = outdatloc + outfilehead.a_data;
	writemod();
	for(thisfile=filelist; thisfile!=nil; thisfile=thisfile->next) {
		inputname = thisfile->name;
		input = fopen(thisfile->name,"r");
		if(input==NULL) {
		    fprintf(stderr, "ld. <%s> ",thisfile->name);
		    perror("");
		    exit (1);
		}
		setbuf(input,inbuffer);
		if (debug) if (thisfile->member == nil)
			printf ("\t%s\n",thisfile->name);
		    else
			printf ("\t%s <%s>\n",thisfile->name, thisfile->member);
		nmods = thisfile->nmods;
		fseek(input,(long)thisfile->txtloc,0);
		modtable = getmodtable(nmods);
		modfun = thisfile->mlist;
		for(i=modfun->inmodn;i<nmods;i++) {
			thismod = modfun;
			/* -R option will exclude program segment being written
			   out to object file                                */
			if(modfun->rom == -1)
			  copy_text(modfun,modtable[modfun->inmodn].modprogram);
			dosync(modfun->lsync);
			if (modfun->no_ext==0) writelinkpage(
				    modfun,modtable[modfun->inmodn].modlink);
			lastmod = modfun;
			modfun = modfun->next;
		}
		textmark = ftell(output);
		modfun = thisfile->mlist;
		if (modfun->no_ext!=0) datamark = modfun->link_begin;
		else datamark = modfun->static_begin;
		datamark += outdatloc - outfilehead.a_dat_addr;
		fseek(output,(long)datamark,0);
		for(i=modfun->inmodn;i<nmods;i++) {
			thismod = modfun;
			if (modfun->no_ext!=0) writelinkpage(
		       		    modfun,modtable[modfun->inmodn].modlink);
			cp_dt(modfun,modtable[modfun->inmodn].modstatic);
			lastmod = modfun;
			modfun = modfun->next;
		}	/* added by Tim */
		/* copy_data(thisfile); */
		fseek(output,(long)textmark,0);
		fclose(input);
	}
	gen_initcode();
	lastmod = &initmod;
	dosync(lastmod->lsync);
	writelinkpage(lastmod,0);
}

struct modentry *
getmodtable(nelems)		/* Allocate a module table */
{
	static struct modentry *modtable;
	static int modtsize;
	register size;

	size = nelems*sizeof(struct modentry);
	if(modtable==nil) {
		modtsize = size;
		modtable = (struct modentry *)malloc(modtsize);
	} else {
		if (size > modtsize) {
			modtsize = size;
			modtable = (struct modentry *)realloc(modtable,size);
		}
	}
	panic("modtable",modtable!=nil);
	fastread((char *)modtable,sizeof(*modtable),nelems,input);
	return modtable;
}

cp_dt(thismod,txtloc)				/* copy static data */
register int txtloc;
struct sy_module *thismod;
{
	register int size;

	size = thismod->static_size;
	if (size)  {
		txtloc =(txtloc-thismod->ifile->datorig)+thismod->ifile->datloc;
		fseek(input,(long)txtloc,0);
		ftransfer(size,input,output);
	}
}
copy_data(thisfile)				/* copy static data */
struct sy_file *thisfile;
{
	register int size;

	size = thisfile->datasize;
	if (size) {
		fseek(input,(long)thisfile->datloc,0);
		ftransfer(size,input,output);
	}
}

copy_text(thismod,txtloc)			/* copy code segment */
register int txtloc;
struct sy_module *thismod;
{
	register int size;

	size = thismod->program_size;
	if((thismod->rom==-1) || size) {
		txtloc =(txtloc-thismod->ifile->txtorig)+thismod->ifile->txtloc;
		fseek(input,(long)txtloc,0);
		dosync(thismod->psync);
		ftransfer(size,input,output);
	}
}

dosync(n)
register n;
{
	while(n--)
		putc('\0',output);
}

align(a,b,c)
int  *c;
{
int	tmp;

	tmp = a + b &~b;
	*c = tmp - a;
	return tmp;
}

value(ex)
register struct sy_export *ex;
	{
	switch(ex->dtype) {
	    case dt_locimport:
		return ex->mod->program_begin + ex->offset;
	    case dt_imponly:
		if(ex->offset!=-1) {
			if (~relinking)
				{
				printf ("ld. error. undefined: <%s>\n", ex->name);
				error (nil);
				}
		 	else if (verbose || debug)
				printf ("ld. not defined in this link: <%s>\n",
					ex->name);
			ex->offset = -1;	/* no more msgs on this name */
		}
		return 0;
	    case dt_export:
		switch(ex->reltype) {
		    case PB_rel:
			return ex->mod->program_begin + ex->offset;
		    case SB_rel:
			return ex->mod->static_begin  + ex->offset;
		    case NO_rel:
			return ex->offset;
		    default:
			panic("obj file error: export/reltype",false);
		}
	    case dt_magic:
		return ex->offset;
	}
	panic("value",false);
	/*NOTREACHED*/
}

writemod()
{
	register struct sy_module *modfun;
	register int	i;
	int subextend = 0;
	static struct modentry	onemod;

	fseek(output,(long)outtxtloc,0);
	for(i=0; i<modbias; ++i)
		fwrite((char *)&onemod,sizeof(onemod),1,output);
	onemod.modreserved = '\0';
	for(modfun=modlist; modfun!=nil; modfun = modfun->next) {
		modfun->mod_number += subextend; /* correct for +64k mods */
		onemod.modstatic  = modfun->static_begin;
		onemod.modlink    = modfun->link_begin;
		onemod.modprogram = modfun->program_begin;
		fwrite((char *)&onemod,sizeof(onemod),1,output);
		if(modfun->program_size>0) {
			for(i=0; i < (modfun->program_size-1)/(64*1024); ++i) {
				onemod.modprogram += 64*1024;
				fwrite((char *)&onemod,sizeof(onemod),1,output);
				++subextend;
			}
		}
	}
	panic("mod extend",!(modextend-subextend));
}
/*
 * compare the last module (m1) with the current module (m2). If the
 * the import list of m2 is equal to a subsequence of the import list
 * of m1, then use m1's link page for m2. ``m1'' is not actually the
 * last module, but rather is the last module for which a link page
 * was written out.
 */
/* Temporary turn off because of local import implementation complexity
impcompare(m1,m2)
struct sy_module *m1,*m2;
{
register struct sy_import *i1,*i2;
	
	for(i1 = m1->ilist, i2 = m2->ilist;
	    i1!=nil && i2!=nil;
	    i1 = i1->next, i2 = i2->next)
		if(i1->name  != i2->name
		|| i1->ltype != i2->ltype)
			return false;
	if(i2==nil)
		return true;
	return false;
}
*/

compute()
{
register struct sy_module *modfun,*lastmod;
register int nextaddr;
int	tmp,staticstart;

	/* allocate module table */
	outfilehead.a_mod = tmp = (modn + modextend + modbias) * 16;
	if (text_follows_mod) {
	   nextaddr = outfilehead.a_text_addr + tmp;
	}
	else {
	   nextaddr = outfilehead.a_text_addr;
	   outfilehead.a_text_addr -= tmp;
	}
	lastmod = nil;
	for(modfun=modlist; modfun!=nil; modfun = modfun->next) {
		if(modfun->rom==-1) {
			nextaddr = align(nextaddr,3,&modfun->psync);
			modfun->program_begin = nextaddr;
			progtotal += modfun->program_size;
			nextaddr += modfun->program_size;
		}
		/*
		if(lastmod!=nil && impcompare(lastmod,modfun)) {
			modfun->link_begin = lastmod->link_begin;
			modfun->link_merge = true;
		}
		else 
		*/
		if (modfun->no_ext==0) {
			lastmod = modfun;
			nextaddr = align(nextaddr,3,&modfun->lsync);
			modfun->link_begin = nextaddr;
			outfilehead.a_link += modfun->link_size<<2;
			nextaddr  += modfun->link_size<<2;
		}
		else modfun->lsync = 0;
	}
	meld_end_pure = nextaddr;
	nextaddr = align(nextaddr,staticpack ? 3:1023,&tmp);
	outfilehead.a_text = nextaddr - outfilehead.a_text_addr;
	if (outfilehead.a_dat_addr == -1)
	    outfilehead.a_dat_addr = nextaddr;
	nextaddr = outfilehead.a_dat_addr;
	for(modfun=modlist; modfun!=nil; modfun = modfun->next) {
		nextaddr = align(nextaddr,3,&modfun->ssync);
		modfun->ssync += tmp;
		tmp = 0;
		if (modfun->no_ext!=0) {
			register int real_link_size;
			modfun->link_begin = nextaddr;
			real_link_size = modfun->link_size<<2;
			outfilehead.a_link += real_link_size;
			nextaddr  += real_link_size;
			statictotal += real_link_size;
		}
		modfun->static_begin = nextaddr;
		nextaddr += modfun->static_size;
		statictotal += modfun->static_size;
	}
	meld_end_image = nextaddr;
	outfilehead.a_data = nextaddr - outfilehead.a_dat_addr;
	add_magics();	/* Moved to here so string size will be correct */
	writeheader();
}

writeheader()
{
	union 	header {
		struct exec e;
		int filler[1024/sizeof(int)];
	}h;
#define hdr h.e

	 register i;
	 register struct sy_name *nm;
	 int modsize;

	outfilehead.a_data -= initmod.static_size; /* correct for bss */
	modsize = sizeof(struct modentry);	/* size of a mod descriptor */

	fseek(output,0,0);
	for(i=0; i<1024/sizeof(int); ++i)
		h.filler[i] = 0;
	hdr = outfilehead;
	hdr.a_bss	= initmod.static_size;
	if(!stripsymbols) hdr.a_syms	= symtotal * sizeof(struct nlist);
	else hdr.a_syms	= 0;
	hdr.a_trsize	= 0;	/* for now */
	hdr.a_drsize	= 0;	/* for now */
/*
 * get the default transfer address, mod number
 */
	hdr.a_entry_mod = (modlist->mod_number)*modsize + hdr.a_mod_addr;
	hdr.a_entry	= modlist->program_begin;
	if(start.name != NULL) {
	    struct sy_export *ex;
		ex = lookup(start.name,dt_imponly);
		hdr.a_entry_mod = 
		     (ex->mod->mod_number)*modsize + hdr.a_mod_addr;
		hdr.a_entry     = value(ex);
	} else printf("start is NULL\n");
/*
 * Compute the string table size
 */
	i = 0;
	if(!stripsymbols) {
	  for(nm=namelist; nm != namespace; nm = nm->next) {
	    i += stablesize;
	  };
	  i += (strnext-nm->name);
        };
	hdr.a_strings	= i;
/*
 * Write the header out to the file
 */
	fwrite((char *)&h,sizeof(h),1,output);
}

panic(s,l)
char *s;
{
	if (!l) {
		printf("ld. Panic.\n");
		error(s);
		abort();
	}
}

fatal(s)
char *s;
{
	error(s);
	exit(1);
}

error(s)
char *s;
{
	if(s!=nil)
		printf("ld. error. %s\n", s);

/*  this section of code used to eliminate some files if the link
 *  the link didn't work
 *	if(!unlinkfun)
 *		{
 *		unlink(imagename);
 *		unlink(symname);
 *		}
 */
	unlinkfun = true;
}

one(n) { if (n != 1) {
		printf("ld. unexpected end of data in file <%s>\n", inputname);
	 	panic("fastread",-1);
		}
}

multiple(ex)
struct sy_export *ex;
{
	printf("ld. symbol is multiply defined:\t<%s>\n", ex->name);
	printf("ld. first encountered in module: \t<%s>\n",
		ex->mod->name);
	printf("ld. later encountered in module: \t<%s>\n",
		thismod->name);
	multipledef = true;
}

add_init(name)
char	*name;
{
    /* Generate a unique initcode name by appending the module
     * number to the name, generate an import symbol and
     * link into the initmodule import list
     */

    /* Now need to create stabs on the fly for these symbols...
     */
    char *p = name;
    int link_entry;

    ++initmod.link_size;
    sprintf(p,"%s%d", p, thismod->mod_number);
    link_entry = (initi==nil ? 0 : initi->link_index+1);
    initi = *(initi==nil ? &initmod.ilist : &initi->next)
	  =  (struct sy_import *)
		    alloc(sizeof(struct sy_import));
    initi->name  = lookup(name, dt_imponly)->name;
    initi->ltype = ext_proc;
    initi->nargs = 0;
    initi->next  = nil;
    initi->link_index = link_entry;
    initi->common = false;
    initi->common_size =  0;
    if (debug) {
	printf("new initname %s\n", initi->name);
    }
}

sym_value(symbol)			/* Get the value of a symbol */
struct nlist *symbol;
{
	/*
	 * This will have to be altered to read additional symbols
	 * if the value has overflowed the value field
	 */
	return(symbol->n_value);
}

do_importsym(symbol)
struct nlist *symbol;
{
        file            *save; 
	register char	*name;
	register struct sy_export    *ex;
	register struct sy_name      *na;
	register	symtype;
	struct modentry *modtable, *getmodtable();
	link_type	ltype;
	int		nmods,linkloc,nargs,txtloc,link_entry,offset1,vword;
	int		mark;

	name = getname(symbol->n_name);
	if (debug) printf("importing <%s>\n",name);
	symtype = symbol->n_type;
	ltype = (symtype & N_PROC) ? ext_proc : ext_data;
	nargs = (ltype == ext_proc) ? symbol->n_nargs : 0;
	if(pass==1) {
	    ++thismod->link_size;
	    if((symtype & N_EXT) == 0) { /* local import */
		nextl = *(nextl==nil ? &thismod->llist : &nextl->next)
		      = (struct sy_locimport *)
				alloc(sizeof(struct sy_locimport));
		na = nlookup(name);
		nextl->name  = na->name;
		nextl->next  = nil;
		nextl->dtype = dt_locimport;
		nextl->mod   = thismod;
		nextl->link_index = symbol->n_disp1;
		link_entry = nextl->link_index;
		/* Assembler has already written offset into link table entry */

		nmods = thismod->ifile->nmods;
		/* Don't want to out order the symbol table sequence. */
		/*if(inbuffer1 == nil) setbuf(input1,inbuffer1);*/
		mark = ftell(input);
		fseek(input,(long)thismod->ifile->txtloc,0);
		/*
		save = input; 
		input = input1; 
		*/
		modtable = getmodtable(nmods);
		/*input = save; */
		linkloc = modtable[thismod->inmodn].modlink;
		if (thismod->no_ext != 0)
		      offset1 = ((linkloc - thismod->ifile->datorig) +
			       thismod->ifile->datloc) + (link_entry<<2);
		else  offset1 = thismod->ifile->txtloc+linkloc+(link_entry<<2);
		if(debug)
		printf("textloc = %x offset1 = %x\n",thismod->ifile->txtloc,offset1);
		fseek(input,offset1,0);
		one(fastread((char *)&vword, sizeof(int), 1, input));
		if (debug) printf("Link table entry = %x %d\n",vword,vword);   
		nextl->offset = vword >> 16;
		if (debug) printf("offset part = %x\n",nextl->offset);
		nextl->nargs = nargs;
		nextl->ltype = ltype;
		nextl->common= false;
		symbol->n_namestr = na->disp;
		fseek(input,mark,0);
		while (symtype & N_MORE) {   /* find common info, if any */
			outsymbol(symbol);
			next_sym(symbol);
			symtype=symbol->n_type;
			if (((symtype & N_ETYPE) == N_AUX) &&
			    (symbol->n_auxtype == N_COMMON)) {
				nextl->common = true;
				nextl->common_size = symbol->n_value;
			}
		} /* while */
		outsymbol(symbol);
		if(debug) if(thismod->llist == nil)printf("local sym not appended!\n");
	    } else {                     /* normal import */
		nexti = *(nexti==nil ? &thismod->ilist : &nexti->next)
		      = (struct sy_import *)
				alloc(sizeof(struct sy_import));
		ex = lookup(name,dt_imponly);
		nexti->name  = ex->name;
		nexti->ltype = ltype;
		nexti->nargs = nargs;
		nexti->next  = nil;
		nexti->common= false;
		nexti->link_index = symbol->n_disp1;
		symbol->n_namestr = ex->disp;
		while (symtype & N_MORE) {   /* find common info, if any */
			outsymbol(symbol);
			next_sym(symbol);
			symtype=symbol->n_type;
			if (((symtype & N_ETYPE) == N_AUX) &&
			    (symbol->n_auxtype == N_COMMON)) {
				nexti->common = true;
				nexti->common_size = symbol->n_value;
			}
		} /* while */
		outsymbol(symbol);
	    } /* else */
        } /* if (pass == 1) */ 
}

do_exportsym(symbol)			/* Process an exported symbol */
struct nlist *symbol;
{
	register struct sy_export *ex;
	register char	*name;
	register symtype;
	char	iname[50];
	char	nargs;
	int	offset;
	mode	reltype;

	name = getname(symbol->n_name);
	if (debug) printf("exporting <%s>\n",name);
	symtype = symbol->n_type;
	offset  = symbol->n_value;
	nargs = 0;
/*****	if ((symbol->n_name & N_AMODE)==A_PC) {  *****/
	if (symbol->n_addrmode==A_PC)
	    {
	    reltype = PB_rel;
	    if (symtype & N_PROC)
		{
		nargs = symbol->n_nargs;
		}
	    }
	else
	    if (symbol->n_addrmode==A_ABS)
		reltype = NO_rel;
	    else
		reltype = SB_rel;
	if(pass==1) {
		if (strcmp(initname,name) == 0) {	/* Check magic name */
		    strcpy(iname,name);		/* makes room for mod # */
		    name = iname;
		    add_init(name);
		};
		ex = lookup(name,dt_export);
		if(ex->dtype==dt_magic) {
			if(warnings)
				printf(
			"ld. definition of symbol <%s> in <%s> ignored.\n",
				ex->name, thismod->name);
		} else {
			if(ex->mod != nil) {
				/* allow a multiple definition if
				 * it matches the first definition
				 */
				if(ex->offset != offset
				|| ex->nargs  != nargs
				|| ex->reltype!= reltype
				|| ex->mod    != thismod
				|| ex->dtype  != dt_export)
					multiple(ex);
			} else {
				ex->offset	= offset;
				ex->nargs	= nargs;
				ex->reltype	= reltype;
				ex->mod		= thismod;
				ex->dtype	= dt_export;
			}
			symbol->n_namestr = ex->disp;
			outsymbol(symbol);
		}
	}
}


do_fileheader(filename)
char *filename;
{
	struct	exec header;

	one(fastread((char *)&header,sizeof(header),1,input));
	if (N_BADMAG(header)) {
	    printf("Bad magic number in file %s\n",filename);
	    exit(1);
	}
	thisfile = *(thisfile==nil ? &filelist : &thisfile->next)
		 =  (struct sy_file *) alloc(sizeof(struct sy_file));
	thisfile->next = nil;
	thisfile->mlist= nil;
	thisfile->name = nlookup(filename)->name;
	thisfile->member = nil;
	thisfile->txtloc = N_TXTOFF(header);
	thisfile->datloc = thisfile->txtloc + header.a_text;
	thisfile->symloc = N_SYMOFF(header);
	thisfile->txtorig = header.a_text_addr;
	thisfile->modorig = header.a_mod_addr;
	thisfile->datorig = header.a_dat_addr;
	thisfile->strloc = thisfile->symloc + header.a_syms;
	thisfile->nmods  = header.a_mod / sizeof(struct modentry);
	thisfile->stringsiz = header.a_strings;
	thisfile->datasize  = header.a_data;
	nsymbs = header.a_syms / sizeof(struct nlist);
}

/* Load a file containing multiple modules */

static	char	arch[SARMAG+1];		/* archive file character tag */

add_file(filename)
char	*filename;
{
	inputname = filename;
	input = fopen(filename,"r");
	if(input==NULL) {
		fprintf(stderr, "ld. <%s> ", filename);
		perror("");
		dum_file_lib = true ;
		return;
	}
	input1= fopen(filename,"r");
	setbuf(input,inbuffer);
	if(verbose || debug)
		printf("%s\n", filename);

	fastread (arch, SARMAG, 1, input);
	if (strcmp(arch, ARMAG) == 0)
		{				/* handle as an archive file */
		fclose(input);
		fclose(input1);
		libsearch (filename, 1);
		}
	    else
		{				/* Main linking loop resides here */
		fseek (input, 0, 0);
		do_fileheader(filename);
		do_symbols();
		fclose(input);
		fclose(input1);
		}
}

module(symbol)			/* process a module symbol */
struct nlist *symbol;
{
	static struct sy_module zeromod;
	register int i,size,type;

	globalmodfun = *(globalmodfun==nil ? &modlist : &globalmodfun->next)
	       = (struct sy_module *) alloc(sizeof(struct sy_module));
	*globalmodfun = zeromod;
	globalmodfun->name = doname(&symbol->n_name);
	if (debug) printf("module <%s>\n",globalmodfun->name);
	globalmodfun->mod_number = modn++ + modbias;
	globalmodfun->ifile = thisfile;
	if (thisfile->mlist == nil) thisfile->mlist = globalmodfun;
	globalmodfun->ilist = nil;
	globalmodfun->llist = nil;
	globalmodfun->next = nil;
	globalmodfun->rom  = rom;
	if(rom!=-1)
		globalmodfun->program_begin = rom;
	globalmodfun->inmodn = symbol->n_disp1 / sizeof(struct modentry);
	symbol->n_disp1 = globalmodfun->mod_number*sizeof(struct modentry)
					+ outfilehead.a_mod_addr;
	globalmodfun->no_ext  = symbol->n_no_ext;
	outsymbol(symbol);
	for (i=0;i<2;i++) {
		next_sym(symbol);
		outsymbol(symbol);
		type = symbol->n_type;
		panic("aux symbol expected",(type&N_ETYPE)==N_AUX);
		if (symbol->n_auxtype == N_TXTINFO) {
		    size = globalmodfun->program_size = symbol->n_name;
		} else {
		    globalmodfun->static_size = symbol->n_value;
		}
	}
	if(size>0)	/* allocate extra entries for big code segments */
		modextend += (size-1) / (64*1024);
	nexti = nil;
	nextl = nil;
	thismod = globalmodfun;
}

writelinkpage(thismod,linkloc)
struct sy_module *thismod;
{
register struct sy_import    *ientry = thismod->ilist;
register struct sy_locimport *lentry = thismod->llist;
register struct sy_export    *eentry;
int	vword, link_entry, max_link;

	/*
	if(thismod->link_merge) {
		if(debug)printf("link table merge!\n");
		return;
	}
	*/
	link_entry=0;
	max_link= -1;
	/* now go thru local import list */
	for(;lentry!=nil;lentry = lentry->next) {
		if (lentry->link_index != link_entry) {
		    fseek(output, (lentry->link_index-link_entry)<<2,1);
		    link_entry = lentry->link_index;
		}
		if (link_entry > max_link) {max_link = link_entry;}
		if(debug)printf("local import :%s\n",lentry->name);
		vword =((((unsigned)lentry->offset>>16)
					 + lentry->mod->mod_number) << 4)
			 +(outfilehead.a_mod_addr & 0xffff)
			 | lentry->offset << 16;
		fwrite((char *)&vword,4,1,output);
		link_entry += 1;
	}
	for(;ientry!=nil;ientry = ientry->next) {
		if (ientry->link_index != link_entry) {
		    fseek(output, (ientry->link_index-link_entry)<<2,1);
		    link_entry = ientry->link_index;
		}
		if (link_entry > max_link) {max_link = link_entry;}
		eentry = lookup(ientry->name,dt_imponly);
		if(debug)printf("import :%s\n",eentry->name);
		vword = value(eentry);
		if(eentry->mod==nil) {
			fwrite((char *)&vword,4,1,output);
		        link_entry += 1;
			continue;
		}
	      if(warnings &&
		   eentry->dtype != dt_magic &&
		   (ientry->ltype == ext_proc && eentry->reltype == SB_rel)) {
			printf("ld. warning: import of <%s> in <%s>\n",
				ientry->name, thismod->name);
			printf("      clashes with mode of export from <%s>\n",
				eentry->mod->name);
			printf("      import is <%s>; export is <%s>\n",
				ientry->ltype==ext_proc?
					"procedure" : "data",
				eentry->reltype==PB_rel?
					"procedure" : "data");
		}
		if(ientry->ltype==ext_proc) {
			if(eentry->nargs != ientry->nargs) {
				printf("argument mismatch.\n");
				printf(
		"export of <%s> by <%s> requires %d bytes of arguments,\n",
		eentry->name,eentry->mod->name,eentry->nargs*4);
				printf("imported by <%s> with %d bytes.\n",
					thismod->name,ientry->nargs*4);
			}
			if(eentry->dtype==dt_magic)
				printf(
				"ld. error. descriptor for <%s> impossible\n",
					eentry->name);
			else	vword =((((unsigned)eentry->offset>>16)
					 + eentry->mod->mod_number) << 4)
					 +(outfilehead.a_mod_addr & 0xffff)
					 | eentry->offset << 16;
		}
		fwrite((char *)&vword,4,1,output);
		link_entry += 1;
	}
	if (link_entry!=(max_link+1)) {
		fseek(output, (max_link+1-link_entry)<<2, 1);
	}
}

add_magics ()
	{
	add_1_magic ("_end_pure", meld_end_pure);
	add_1_magic ("_end_image", meld_end_image);

	add_1_magic ("_edata", outfilehead.a_dat_addr + outfilehead.a_data);
	add_1_magic ("_etext", meld_end_pure);
	add_1_magic ("_end", meld_end_image);
	}

add_1_magic(s,v)
char *s;
{
register struct sy_export *exfun;

	exfun = lookup(s,dt_magic);
	exfun->offset = v;
	exfun->dtype  = dt_magic;
	exfun->reltype= SB_rel;
}

printmap()
{
long timefun;
register i;
static char format[]		= "   %-18s%06x\t\t";
static char dformat0[]		= "   %-18s%6d\n";
static char dformat[]		= "   %-18s%6d%8d\n";
static char headerformat[]	= "   %-17s%7s%8s";
static char newformat[]		= "   %-18s%06x\t\t   %-18s%6d\n";
static char imp80format[]	= "%c%c%c%c %06x %-14.14s";
static char imp132format[]	= "%c%c%c%c %06x %-20.20s";
static char exp80format[]	= "%c %06x %-17.17s";
static char exp132format[]	= "%c %06x %-23.23s";
struct sy_module *modfun;
char	*impformat = wide ? imp132format:imp80format;
char	*expformat = wide ? exp132format:exp80format;
register struct sy_import    *ifun;
register struct sy_locimport *lfun;
register struct sy_export    *ex;

	if(debug)
		printf("ld. begin map\n");
/*      mapfile = fopen("ldmap","w");                    */
        mapfile = fopen(mapname,"w");
	if(mapfile==NULL) {
		printf("ld. can't create <ldmap>\n");
		return;
	}
	time(&timefun);
	fprintf(mapfile,"\n\nld version %I% (SCCS) %s", ctime(&timefun));
	fflush(mapfile);
	fprintf(mapfile,"\noptions:");
	fflush(mapfile);
	for(i=1; i<ac1; ++i) {
		if(av1[i][0]=='-') {
			fprintf(mapfile," %s",av1[i]);
	fflush(mapfile);
			if(index("rxbto",av1[i][1])!=0) {
				fprintf(mapfile," %s",av1[++i]);
	fflush(mapfile);
				if(index("rx",av1[i-1][1])!=0)
					fprintf(mapfile," %s",av1[++i]);
	fflush(mapfile);
			}
		}
	}
	if(modlist==nil)
		return;
	fprintf(mapfile,"\n\n");
	fflush(mapfile);
	if(modlist->rom == -1) {
		fprintf(mapfile,headerformat,"image segment","address","\t");
	fflush(mapfile);
		fprintf(mapfile,headerformat,"segment type","size",
			"   total\n");
	fflush(mapfile);
		fprintf(mapfile,
			headerformat,"---------------","-------","\t");
	fflush(mapfile);
		fprintf(mapfile,
			headerformat, "---------------","-------"," -------\n");
	fflush(mapfile);
		fprintf(mapfile,format, "first mod entry", outfilehead.a_mod_addr);
		fprintf(mapfile,dformat0,"mod table",modn*16+16*modbias);
	fflush(mapfile);
		fprintf(mapfile,format, "first program",
			modlist->program_begin);
	fflush(mapfile);
		fprintf(mapfile,dformat,"program",progtotal,
			progtotal+modn*16+16*modbias);
	fflush(mapfile);
		fprintf(mapfile,format, "first static",
			modlist->static_begin);
	fflush(mapfile);
		fprintf(mapfile,dformat,
			"link",outfilehead.a_link,
			outfilehead.a_link+progtotal+modn*16+16*modbias);
	fflush(mapfile);
		fprintf(mapfile,format, "last address+1",
			lastmod->static_begin+lastmod->static_size);
		fprintf(mapfile,dformat,"static",
			statictotal,
			statictotal+outfilehead.a_link+progtotal+modn*16+16*modbias);
	}
	for(modfun=modlist; modfun!=nil; modfun = modfun->next) {
	   int prnumber = 0;
		fprintf(mapfile,"\nmodule %02d <%s>\n",
			modfun->mod_number,modfun->name);
	fflush(mapfile);
		fprintf(mapfile,"------ --\n\n");
	fflush(mapfile);
		fprintf(mapfile,newformat, "static base",
			modfun->static_begin,
			"static size", modfun->static_size);
	fflush(mapfile);
		fprintf(mapfile,newformat, "program base",
			modfun->program_begin, "program size",
			modfun->program_size);
	fflush(mapfile);
		fprintf(mapfile,newformat,"link table",
			modfun->link_begin,
			"link size", modfun->link_size<<2);
	fflush(mapfile);
		if(modfun->ilist!=nil || modfun->llist!=nil)
			fprintf(mapfile,"\nimports\n-------\n\n");
	fflush(mapfile);
		for(ifun=modfun->ilist; ifun!=nil; ifun = ifun->next) {
		    char exmodc[2];
			newline(&prnumber);
			ex = lookup(ifun->name,dt_imponly);
			exmodc[0] = exmodc[1] = '*';
			if(ex->mod!=nil) {
			    int exmod;
				exmod = ex->mod->mod_number;
				exmodc[0] = exmod / 10 | '0';
				exmodc[1] = exmod % 10 | '0';
			}
			fprintf(mapfile,impformat,
				ifun->ltype==ext_proc ? 'd':'a',
				ex->dtype==dt_imponly ? 'u':
				ex->dtype==dt_magic   ? 'm':
				ex->reltype==SB_rel   ?'v':'c',
				exmodc[0], exmodc[1], value(ex), ifun->name);
	fflush(mapfile);
		}
		for(lfun=modfun->llist; lfun!=nil; lfun = lfun->next) {
		    char exmodc[2];
			newline(&prnumber);
			exmodc[0] = exmodc[1] = '*';
			if(lfun->mod!=nil) {
			    int exmod;
				exmod = lfun->mod->mod_number;
				exmodc[0] = exmod / 10 | '0';
				exmodc[1] = exmod % 10 | '0';
			}
			fprintf(mapfile,impformat,'d','c',exmodc[0],exmodc[1],
			        value((struct sy_export *)lfun), lfun->name);
	fflush(mapfile);
		}
		if(prnumber!=0)
			fprintf(mapfile,"\n");
	fflush(mapfile);
		prnumber = -1;
		for(i=0; i<htablesize; ++i) {
			for(ex=htable[i]; ex!=nil; ex = ex->next) {
				if(ex->dtype==dt_export
				&& ex->mod!=nil
				&& ex->mod->mod_number==modfun->mod_number) {
					if(prnumber==-1) {
						prnumber = 0;
						fprintf(mapfile,
						"\nexports\n-------\n\n");
	fflush(mapfile);
					}
					newline(&prnumber);
					fprintf(mapfile,expformat,
						ex->reltype==SB_rel ?
							'v':'c',
						value(ex), ex->name);
	fflush(mapfile);
				}
			}
		}
		if(prnumber!=0)
			fprintf(mapfile,"\n");
	fflush(mapfile);
	}
	fclose(mapfile);
	if(debug) printf("ld. end map\n");
}

newline(prnumber)
int    *prnumber;
{
	if(*prnumber==(wide?4:3)) {
		fprintf(mapfile,"\n");
	fflush(mapfile);
		*prnumber = 0;
	}
	if(*prnumber!=0)
		fprintf(mapfile," ");
	fflush(mapfile);
	++*prnumber;
}
