/*
 * %M%: version %I% of %G%
 * %Y%
 */

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


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ross.h"
#include "meld2.h"
#include "lookup.h"
#include "symbols.h"
#include <ar.h>
#include <a.out.h>
#include <ranlib.h>
#include <stab.h>

#define	LIBSTART "start"		/* default start address */

#ifndef fastio
#define fastread fread
#endif

extern	char	*rindex();

static	int	l_offs, l_strs;		/* offsets in *data (returned by */
					/* libopen) to ranlib tables */
static	struct	{
	char	*libnam;
	int	liboff;
	} libs[] = {
	{"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",  0},	/* 0 */
#ifdef	ON16K
	{"/usr/lib/libxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 12},	/* 1 */
	{"/lib/libxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",  8},	/* 2 */
#else
#ifdef	CUST
	{"/usr/NSC/lib/libxxxxxxxxxxxxxxxxxxxxxxxxx", 16},	/* 1 */
	{"/usr/NSC/lib/libxxxxxxxxxxxxxxxxxxxxxxxxx", 16},	/* 2 */
# else	/* KERN or VAX */
	{"/v/mesaux/usr/lib/libxxxxxxxxxxxxxxxxxxxx", 21},	/* 1 */
	{"/mesa/lib/libxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 13},	/* 2 */
#endif
#endif
	};
/*#define	libmax	1			highest numbered part of libs */
static	file	*lib;			/* archive file pointer */
static	file	*lib1;			/* archive file pointer */
static	char	*libnam;		/* archive file name */
static	int	liboff, libnum, libmax;

static	char	arch[SARMAG+1];		/* archive file character tag */
static	struct	ar_hdr	hdr;		/* archive file header */
static	struct	stat	statbuf;	/* archive file statistics */

static	struct	nlist	globalsym;	/* current symbol being processed */

/* Search the "library" list for unresolved names, and load modules satisfying
 * the undefined symbols.
 */

libsearch(libsuffix, flag)
char	*libsuffix;		/* identify library */
int	flag;			
	{
	typedef struct { char s[13]; } library;
	register struct	sy_module *mod;	/* module entry */
	register struct sy_import *ix;	/* import entry */
	register struct sy_export *ex;	/* export entry */
	char	*data;			/* pointer to file data */
	char	*eodata;		/* end of file data */
	int	again;			/* true if we go again */
	int	passes;			/* number of passes through file */
	int	libret;
	register char	*tempchn, *tempchs, *templib;

	templib = libsuffix;
	if (start.name == NULL) start.name = LIBSTART;
	if (flag)
		libnum = 0;
	    else
		if (*templib++ == 'c' && !*templib) {
			libnum = 2;
			libmax = 2;
		} else {
			libnum = 1;
			libmax = 1;
		}
libloop:
	libnam = libs[libnum].libnam;
	liboff = libs[libnum].liboff;
	tempchn = &libnam[liboff];
	for (tempchs = libsuffix; *tempchs;)
		*tempchn++ = *tempchs++;
	if (flag)
		*tempchn = 0;
	    else
		*(library *)tempchn = *(library *)".a";
	if (debug) printf("Searching library %s\n", libnam);
	if (verbose) printf("%s\n", libnam);

	if (start.name != NULL ) {
		ex = lookup(start.name, dt_imponly);
		if (ex->dtype == dt_imponly) {
			if ((libret = libopen(libnam, &data, &eodata)) < 0) {
				if ((libnum) && (libnum < libmax) && (libret == -2)) {
					libnum++;
					goto libloop;
					}
				fprintf(stderr, "ld. library file <%s> ", libnam);
				perror("");
				return;
				}
			libload(data, eodata, &start);
			}
		}
	passes = 0;
	again = 1;
	while (again) {
		again = 0;
		passes++;
		for (mod=modlist; mod!=nil; mod = mod->next) {
			for (ix=mod->ilist; ix!=nil; ix = ix->next) {
/**	temp out to allow c library to work
				if (ix->common)
					continue;
**/
				ex = lookup(ix->name, dt_imponly);
				if (strcmp(ix->name, initname) == 0)
					continue;
				if (ex->dtype == dt_imponly) {
					if ((libret = libopen(libnam, &data, &eodata)) < 0) {
						if ((libnum) && (libnum < libmax) && (libret == -2)) {
							libnum++;
							goto libloop;
							}
						fprintf(stderr, "ld. library file <%s> ", libnam);
						perror("");
						return;
						}
					again |= libload(data, eodata, ix);
					}
				}
			}
		}
	if (debug) printf("Library search done, passes: %d\n", passes);
	if(lib != NULL) fclose (lib);
	if(lib1!= NULL) fclose (lib1);
	lib = NULL;
	lib1 = NULL;
	}

/* Open the library file and read it into memory */

libopen(libnam, data, eodata)
char	*libnam;		/* library file name */
char	**data;			/* returned address of file data */
char	**eodata;		/* returned end of file data */
	{
	char	*tmp;
	int	ar_date;

	if (lib != NULL) {      /* already opened */
	      if (lib1 == NULL) lib1 = fopen(libnam, "r");
	      return(0);
	}
	lib = fopen(libnam, "r");
	if (lib == NULL) {
		/* no library file there */
		dum_file_lib = true ;
		return(-2);
		}
	if (fstat(fileno(lib), &statbuf) < 0) {
		/* something wrong while accessing library file */
		fflush (stdout);
		fprintf(stderr, "ld. library file <%s> ", libnam);
		perror("");
		fclose(lib);
		return(-1);
		}
	fastread (arch, SARMAG, 1, lib);
	if (strcmp(arch, ARMAG) != 0) {
		/* not an archive file */
		fflush (stdout);
		fprintf (stderr, "ld. <%s> not an archive file\n", libnam);
		fclose(lib);
		return (-1);
		}
	fastread (&hdr, sizeof(hdr), 1, lib);
	if (strncmp(hdr.ar_name, "__.SYMDEF", 9) != 0) {
		/* hasn't been ranlib'd */
		fflush (stdout);
		fprintf (stderr, "ld. <%s> has not been put thru ranlib\n",
			libnam);
		fclose(lib);
		return (-1);
		}
	sscanf (hdr.ar_date, "%ld", &ar_date);
	if (statbuf.st_mtime > ar_date)
		{
		fflush (stdout);
		fprintf (stderr, "ld warning. <%s> may be out of date\n",
			libnam);
		}

	fastread (&l_offs, sizeof(l_offs), 1, lib);
	fseek (lib, l_offs, 1);
	fastread (&l_strs, sizeof(l_strs), 1, lib);
	fseek (lib, SARMAG + sizeof(hdr) + sizeof(l_offs), 0);
	tmp = (char*)malloc(l_offs + l_strs + 1);
	fastread (tmp, l_offs, 1, lib);
	tmp += l_offs;
	fseek (lib, sizeof(l_offs), 1);
	fastread (tmp, l_strs, 1, lib);
	tmp -= l_offs;
	*data = tmp;
	*eodata = tmp + l_offs + l_strs;
	return(0);
	}

/* Find a symbol name in the archive toc, and load the associated module. */

libload(data, eodata, symbol)
register char	*data;		/* data scanning pointer */
char	*eodata;		/* end of file data */
struct sy_import *symbol;		/* symbol to find */
	{

	register char	*sym;		/* scanner for symbol */
	char	*modname;		/* current module name */
	char	*libfile;		/* filename of module */
	char	*mods, *mod;		/* address of modules */
	int	tmp, off, comsize;

	mods = data;
	data += l_offs;
	if (debug) printf("     Look for <%s>\n", symbol->name);
	while (data < eodata) {
		sym = symbol->name;
		tmp = strcmp (data, sym);
		if (tmp == 0) {
			/* find offset in data & read in module from lib */
			tmp = data - (mods + l_offs);
			off = 0;
			for (mod = mods; mod < mods + l_offs;
						mod = mod + sizeof(struct ranlib))
				{
				if (((struct ranlib*)mod)->ran_un.ran_strx == tmp) {
					comsize = ((struct ranlib*)mod)->
							ran_comsize_or_dt;
					off = ((struct ranlib*)mod)->ran_off;
					((struct ranlib*)mod)->ran_off = 0;
					if (off == 0) {
					   /* if we have processed
					    * this symbol before, then
					    * we dont need another pass
					    * through the library for
					    * it */
					   return(0);
					   }
					if (debug)
					    printf(
					      " Found <%s>, comsize = %d, common = %d\n",
					      symbol->name, comsize,
					      symbol->common);
					/* common symbols in the
					 * library do not cause modules
					 * to be loaded, but they do
					 * change simple external
					 * symbols into common even
					 * without loading the module
					 * in which the symbol is
					 * declared to be common */
					/* this complexity is for
					 * VAX compatibility */
					if ((symbol->common == false)
					    && (comsize > 0)) {
					   /* The VAX eliminates the first part
					    * of the conditional above,
					    * then does a max of
					    * comsize and common_size */
					   symbol->common = true;
					   symbol->common_size = comsize;
					   off = 0;  /* dont load
						      * because of common */
					   continue;
					}
					else if ((symbol->common == true)
					   && (comsize != 0)) {
					      off = 0; /* dont load
							* due to
							this symbol*/
					      continue;
					}
					     /* common symbols do
					      * not pull text symbols
					      * from library, nor do
					      * they pull modules that
					      * use the same common
					      * symbol.  The size of
					      * common will only be
					      * increased if we really
					      * load this module. */
					/* load this module */
					break;
					}
			}
			if (off != 0) {
				add_libmodule(data, off);
				return (1);
				}
			}
		while (*data++ != '\0');
		}
	return(0);
	}

/* Load a module from an archive file */

add_libmodule(symname, foffset)
char	*symname;  /* the name of the symbol we hope to
			get from the module */
int	foffset;
	{
	file	*save,*save1;
	char	*nam, *modname;

	save = input;
	save1 = input1;
	input = lib;			/* for symbol routines */
	input1 = lib1;			
	setbuf (input, inbuffer);
	if (fseek (input, foffset, 0) != NULL) {
		fprintf (stderr, "ld. <%s> (archive header) ", libnam);
		perror ("");
		return;
		}
	fastread (&hdr, sizeof(hdr), 1, input);
	for (nam = hdr.ar_name; (*nam != ' ') && (nam < hdr.ar_date); nam++);
	*nam = '\0';
	inputname = modname = hdr.ar_name;
	if (debug)
		printf ("	Load <%s> from %s\n", hdr.ar_name, libnam);
	if (verbose) printf("\t%s\n", hdr.ar_name);
	if (fseek (input, foffset + sizeof(struct ar_hdr), 0) != NULL) {
		fprintf (stderr, "ld. <%s> ", libnam);
		perror ("");
		return;
		}
	/* allocate space, read module into it */
	lib_fileheader (modname, foffset + sizeof(struct ar_hdr));

	do_symbols();			/* main linking loop resides here */
	input = save;
	input1 = save1;
	if(input != NULL) setbuf (input, inbuffer);
	}

lib_fileheader(modname, moffset)
char *modname;
int	moffset;
	{
	struct	exec	header;

	one(fastread((char *)&header, sizeof(header), 1, input));
	if (N_BADMAG(header)) {
		printf("Bad magic number in module %s(archive %s)\n",
			modname, libnam);
		exit(1);
		}
	thisfile = *(thisfile==nil ? &filelist : &thisfile->next)
		 =  (struct sy_file *) alloc(sizeof(struct sy_file));
	thisfile->next = nil;
	thisfile->mlist= nil;
	thisfile->name = nlookup (libnam)->name;
	thisfile->member = nlookup (modname)->name;
	thisfile->txtloc = moffset + N_TXTOFF(header);
	thisfile->datloc = thisfile->txtloc + header.a_text;
	thisfile->symloc = moffset + N_SYMOFF(header);
	thisfile->txtorig = header.a_text_addr;
	thisfile->modorig = header.a_mod_addr;
	thisfile->datorig = header.a_dat_addr;
	thisfile->strloc = thisfile->symloc + header.a_syms;
	thisfile->nmods  = header.a_mod / sizeof(struct modentry);
	thisfile->stringsiz = header.a_strings;
	thisfile->datasize  = header.a_data;
	nsymbs = header.a_syms / sizeof(struct nlist);
	}
