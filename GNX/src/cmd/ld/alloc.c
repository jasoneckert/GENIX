/*
 * alloc.c: version 3.18 of 5/18/83
 * 
 */

# ifdef SCCS
static char *sccsid = "@(#)alloc.c	3.18 (NSC) 5/18/83";
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


#include "ross.h"

#define NITEMS 100

extern	char *malloc();
extern	int panic();
extern	logical debug;

static	char	*nextfree=nil,*lastfree=nil;

/*	Very simple minded storage allocator. This should be used for
 *	allocation of generally small items in order to reduce the overhead
 *	of going to malloc for lots of small items.
 */

char *
alloc(nbytes)
unsigned nbytes;
{
	register char *newvalue;
	register size;

	if(nextfree+nbytes > lastfree) {
		size = nbytes*NITEMS;
		nextfree = malloc(size);
		lastfree = &nextfree[size-1];
		panic("alloc",nextfree != nil);
		if(debug && nextfree != nil)printf("ld. allocate %d bytes\n",size);
	}
	newvalue = nextfree;
	nextfree += nbytes;
	return newvalue;
}
