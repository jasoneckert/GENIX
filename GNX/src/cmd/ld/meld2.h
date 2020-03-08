/*
 * meld2.h: version 3.21 of 8/25/83
 * 
 */

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

/*	This software has been built to compile into four versions:
 *		1)	Runs on the VAX, produces code for general usage
 *		2)	Runs on the 16k, produces code for general usage
 *		3)	Runs on the VAX, produces code for the kernel group
 *		4)	Runs on the VAX, produces code for customer usage
 *
 *	The differences are:
 *
 *	1)	make name		nmeld
 *		default file name	a16.out
 *		mode			not executable
 *		temp file directory	/tmp
 *		library directory	/v/mesaux/usr/lib
 *		defined name		VAX (or none)
 *
 *	2)	make name		meld16
 *		default file name	a.out
 *		mode			executable
 *		temp file directory	/usr/tmp
 *		library directory	/usr/lib
 *		defined name		ON16K
 *
 *	3)	make name		kmeld
 *		default file name	a.out
 *		mode			executable
 *		temp file directory	/tmp
 *		library directory	/v/mesaux/usr/lib
 *		defined name		KERN
 *
 *	4)	make name		cmeld
 *		default file name	a16.out
 *		mode			not executable
 *		temp file directory	/tmp
 *		library directory	/usr/NSC/lib
 *		defined name		CUST
 */

#define namelen	       (50)

typedef enum { ext_proc, ext_data } link_type;

typedef enum { PB_rel, SB_rel, NO_rel } mode;

enum definetype { dt_locimport, dt_imponly, dt_export, dt_magic, dt_common };

struct sy_file {
	char	*name,		/* for all files */
		*member;	/* for archive files only */
	int	txtloc,		/* offset of text section
				 * and mod table in file */
		datloc,		/* offset of data section in file */
		symloc,
		strloc,
				/* origins in address space */
		txtorig,	/* origin of text section */
		modorig,	/* origin of mod table */
		datorig,	/* origin of data section */
		stringsiz,
		datasize,
		nmods;
	struct	sy_file *next;
	struct	sy_module *mlist;
};

struct sy_module {
	int	mod_number;
	char	*name;
	int	rom;		/* if > -1, rom/shared text at this addr */
	int	static_begin,
		link_begin,
		program_begin,
		static_size,
		link_size,		/* in entries */
		program_size;
	short	psync,lsync,ssync;	/* add this many alignment bytes */
	logical	link_merge;		/* merge this link page w/ the last */
	struct	sy_import    *ilist;    /* import list */
	struct	sy_locimport *llist;    /* local import list */
	struct	sy_module *next;
	struct	sy_file	  *ifile;
	int	inmodn;			/* input module number */
	int	no_ext;			/* no external addressing, link table
					 * must be placed before static base */
};

struct sy_import {
	char	*name;
	struct sy_import *next;
	char	nargs;
	link_type ltype;
	int	common_size;
	logical	common;
	int	link_index;
};

struct sy_locimport {
	char    *name;
	struct  sy_locimport *next;
	int     disp;
	enum definetype dtype;
	struct  sy_module *mod;
	int     offset;
	char    nargs;
	link_type ltype;
	int     common_size;
	logical common;
	int     link_index;
};

struct sy_export {
	char	*name;
	struct  sy_export *next;
	int	disp;
	enum definetype dtype;
	struct	sy_module *mod;
	int	offset;
	char	nargs;
	mode	reltype;
};

/* this structure is the same as the beginning of sy_export -
   nlookup depends upon this */
struct sy_name {
	char	*name;
	struct sy_name *next;
	int	disp;
};

struct modentry {
	int	modstatic,
	    	modlink,
	    	modprogram,
	    	modreserved;
};

#ifdef export

char	*inputname = nil;
file	*input = nil,
    	*input1 = nil,
	*output = nil;
struct	sy_module	*modlist = nil, *thismod = nil;
struct	sy_file		*filelist = nil, *thisfile = nil;
int	end_of_image = 0;
#ifdef	ON16K + CUST
char	*imagename = "a.out";
#else	/* VAX or KERN */
char	*imagename = "a16.out";
#endif
char	*initname  = "%initfun";
struct sy_import start = {nil, nil, 0, ext_data, 0, false, 0};
logical	warnings = true,
	verbose = false,
	map = false,
	wide = false,
	staticpack = false,
	stripsymbols = false,
	striprandom = false,
	striplocal = false,
	striplocalL = false,
	debug = false,
	relinking = false,
	dum_file_lib = false;
	multipledef = false;
#else

extern int	outbuffer[BUFSIZ/sizeof(int)], inbuffer[BUFSIZ/sizeof(int)];
extern int	inbuffer1[BUFSIZ/sizeof(int)];
extern char	*inputname;
extern file	*input, *input1, *output;
extern struct	sy_module	*modlist, *thismod;
extern struct	sy_file		*filelist,	*thisfile;
extern int	end_of_image;
extern char	*imagename;
extern char	*initname;
extern struct sy_import start;
extern logical	warnings, verbose, map, wide, staticpack, stripsymbols,
                dum_file_lib,multipledef,
		striprandom, striplocal, striplocalL, debug, relinking;

extern int	do_importsym(), do_exportsym(), module(), one(),
		panic(), ftransfer();
#endif

/* static int	outbuffer[BUFSIZ/sizeof(int)], inbuffer[BUFSIZ/sizeof(int)]; */
