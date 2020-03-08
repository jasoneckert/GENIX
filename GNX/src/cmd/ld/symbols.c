/*
 * symbols.c: version 3.21 of 8/25/83
 * 
 */

# ifdef SCCS
static char *sccsid = "@(#)symbols.c	3.21 (NSC) 8/25/83";
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


#include <stdio.h>
#include <stab.h>
#include "ross.h"
#include "meld2.h"
#include "lookup.h"
#define export
#include "symbols.h"

/* static file     *symfile; */
file            *symfile;
/* static  int	symbuffer[BUFSIZ/sizeof(int)];  */
int	        symbuffer[BUFSIZ/sizeof(int)];  
static struct nlist	globalsym;  /* The symbol currently being processed */

extern char *mktemp();

#ifndef fastio
#define fastread fread
#endif


int	do_mat(), do_mtn(),
	do_slinechar(), do_maux(), do_aux();

int	(*symbol[])() = {
	do_mat, do_mtn, do_mtn, do_mtn,
	do_slinechar, do_slinechar, do_maux, do_aux
};

do_symbols()			/* Process symbol table for this file */
	{
	register int type;
	int	first;

	do_strings();
	fseek(input,(long)thisfile->symloc,0);
	first = true;
	while (nsymbs--)
		{
		one(fastread((char *)&globalsym,sizeof(globalsym),1,input));
		type = globalsym.n_type & N_ETYPE;
		if (first && !N_MODULE(globalsym))
			{
			fflush (stdout);
			fprintf (stderr, "ld. missing module symbol, file ignored");
			goto abort;
			}
		first = false;
		symtotal++;
		(*symbol[type])();
		}
	abort: ;
	}

do_strings()		/* get the string table for the file into memory */
{
	register int size;

	size = thisfile->stringsiz;
	if (stringbuf == nil) {
		glstringsiz = size;
		stringbuf = (char *)malloc(glstringsiz);
	} else {
		if (size > glstringsiz) {
/*			printf("realloc from %d to %d\n",glstringsiz,size); */
			glstringsiz = size;
			stringbuf = (char *)realloc(stringbuf,glstringsiz);
		}
	}
	if (stringbuf==nil) {
		printf("File %s exhausted string space with %x bytes.\n",
		         thisfile->name, glstringsiz);
		printf(
	"Archive member %s, txtloc: %x, datloc: %x, symloc: %x, strloc: %x\n",
			thisfile->member, thisfile->txtloc, thisfile->datloc,
			thisfile->symloc, thisfile->strloc);
		printf("stringsiz: %x, datasize: %x, nmods: %x\n",
			thisfile->stringsiz, thisfile->datasize,
			thisfile->nmods);
	}
	panic("strings",stringbuf!=nil);
	fseek(input,(long)thisfile->strloc,0);
	one(fastread(stringbuf,size,1,input));
}

do_mat()		/* Main symbol type */
{
	register char *name;
	register struct sy_name *n;
	register adrmode;

	if (debug) printf("MAT symbol ");
	adrmode = globalsym.n_name & N_AMODE;
	if ((adrmode == A_EX) && (globalsym.n_disp2 == 0)) {
		do_importsym(&globalsym);
		return;
	}
	if (globalsym.n_type & N_EXT) {
		if (adrmode != A_EX) {		/* this is an export symbol */
			do_exportsym(&globalsym);
		}
		return;
	}

	name = doname(&globalsym.n_name);
	outsymbol(&globalsym);
	if (debug) printf("local <%s>\n",name);
}

do_maux()		/* main aux symbol */
{
	if (debug) printf("MAUX symbol ");
	if (N_MODULE(globalsym)) {
		module(&globalsym);
		return;
	}
	outsymbol(&globalsym);
}

do_aux()		/* aux symbol */
{
	panic("extraneous aux symbol",false);
}

do_mtn()		/* Process mtn/mptn/metn symbols */
{
	if (debug) printf("MTN symbol\n");
	doname(&globalsym.n_name);
	doname(&globalsym.n_type);
	outsymbol(&globalsym);
}

do_slinechar()		/* Process sline/schar symbols */
{
	if (debug) printf("SLINE/SCHAR symbol\n");
	doname(&globalsym.n_name);
	outsymbol(&globalsym);
}

char *
getname(n)			/* Convert symbol string offset to address */
{
	register char *name;
	register size;
	size = thisfile->stringsiz;
	name = N_STRX(n) + stringbuf;
	if ((name < stringbuf) || (name > (stringbuf+size))) {
		name = "**BOGUS**";
	}
	return name;
}

char *
doname(n)
struct nmdesc *n;
{
	register char *nm;
	register struct sy_name *name;

	nm = getname(*n);		/* convert from offset to address */
	name = nlookup(nm);		/* insert in symbol table */
	n->N_namestr = name->disp;	/* relocate to new offset */
	return name->name;
}

next_sym(symbol)
struct nlist *symbol;
{
	if (nsymbs--) {
	    one(fastread((char *)symbol,sizeof(*symbol),1,input));
	    symtotal++;
	} else {
	    panic("no more symbols",false);
	}
}

opensymfile()
{
	symname = mktemp(symname);
	symfile = fopen(symname,"w+");
	if(symfile==NULL) {
		fprintf(stderr, "ld. Can't create ");
		perror(symname);
		exit(1);
	}
	setbuf(symfile,symbuffer);
}

outsymbol(symbol)
struct nlist *symbol;
{
	if(!(fwrite(symbol,sizeof(struct nlist),1,symfile))){
		panic("symbol out",false);
	}
}

writesymbols()
{
register	size;
register struct sy_name *nm;

	fseek(symfile,(long)0,0);
	fseek(output,(long)end_of_image,0);
	size = symtotal * sizeof(struct nlist);
	ftransfer(size,symfile,output);
	fclose(symfile); unlink(symname);
/*
 * Write out the full buffers...
 */
	size = stablesize;

	for(nm=namelist; nm != namespace; nm = nm->next) {
		fwrite(nm->name,size,1,output);
	}
/*
 * Write out the partial buffer...
 */
	size = strnext - nm->name;
	fwrite(nm->name,size,1,output);

}
