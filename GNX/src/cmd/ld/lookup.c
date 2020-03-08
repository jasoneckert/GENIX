/*
 * lookup.c: version 3.21 of 8/25/83
 * 
 */

# ifdef SCCS
static char *sccsid = "@(#)lookup.c	3.21 (NSC) 8/25/83";
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
#include <stdio.h>
#include "system.h"
#include "meld2.h"
#define export
#include "lookup.h"

#define nil 0

static	int	offset = -stablesize;

/*
 *	stralloc - move a string to the namespace. The namespace is allocated
 *	in "stablesize" chunks, with the start of each chunk "remembered" in
 *	"namelist". "Namespace" is the descriptor of the currently allocated
 *	chunk of the namespace. The namespace may be dumped out in stablesize
 *	contiguous chunks by walking the namelist.
 */

char *
stralloc(s1)
char	*s1;
{
int	l;
char	*newvalue;

	l = strlen(s1);
	if(strnext+l > strlast) {
		namespace = *(namespace==nil ? &namelist : &namespace->next)
			= (struct sy_name *) alloc(sizeof(struct sy_name));
		strnext = namespace->name = malloc((unsigned)stablesize);
		strlast = &strnext[stablesize-1];
		offset += stablesize;
	}
	strnext = &strcpy(newvalue=strnext,s1)[l+1];
	return newvalue;
}

hvalue(s1)
register char *s1;
{
int probe;

	probe = s1[0];
	if(s1[1]!='\0') {
		probe = (probe << 3) | (s1[1] & 7);
		if(s1[2]!='\0') {
			probe = (probe << 3) | (s1[2] & 7);
			if((s1[3]!='\0') && (s1[4]!='\0'))
				probe = (probe << 3) | (s1[4] & 7);
		}
	}
	return probe;
}

struct sy_export *
lookup(s1,newvalue)
char *s1;
enum definetype newvalue;
{
register struct sy_export **q,*qq;

	q = &htable[hvalue(s1) % htablesize];
	while((qq= *q)!=nil) {
		if(strcmp(s1,qq->name)==0)
			return qq;
		q = &qq->next;
	}
	qq = *q = (struct sy_export *)alloc(sizeof(struct sy_export));
	qq->next = nil;
	qq->name = stralloc(s1);
	qq->mod  = nil;
	qq->dtype= newvalue;
	qq->disp = qq->name - namespace->name + offset;
	return qq;
}

/*
 *	nlookup - lookup or insert a name into the string space. This is
 *	a parallel routine to the "lookup" routine above, but inserts names
 *	into the name space without cluttering up the symbol table used by
 *	the linker.  It does check the symbol table used by the
 *	linker to see if the symbol name needed exists there.
 *	The names which will go into the name table are names
 *	of symbols which are of interest only for maintaining the name space.
 *	note that the name space produced will contain no duplicate entries,
 *	but will be an "edited" version of each module's name space. Names
 *	which will go into this table include "type" names "local" names etc.
 */

struct sy_name*
nlookup(s1)
char *s1;
{
register struct sy_name **q, *qq;

	/* first check the main symbol table */
	q = (struct sy_name **)&htable[hvalue(s1) % htablesize];
	while((qq= *q)!=nil) {
		if(strcmp(s1,qq->name)==0)
			return qq;
		q = &qq->next;
	}
	/* now check our own symbol table */
	q = &ntable[hvalue(s1) % htablesize];
	while ((qq = *q)!=nil) {
		if (strcmp(s1,qq->name)==0)
			return qq;
		q = &qq->next;
	}
	qq = *q = (struct sy_name *)alloc(sizeof(struct sy_name));
	qq->next = nil;
	qq->name = stralloc(s1);
	qq->disp = qq->name - namespace->name + offset;
	return qq;
}

hashstat()
{
	register struct sy_export *qq,**q;
	register su,of,i;

	su=of=0;
	for(i=0;i<htablesize;i++) {
	    if(*(q= &htable[i])!=nil) {
		su++;
		qq = *q; q = &qq->next;
		printf("slot %d %s\n",i,qq->name);
		while((qq= *q)!=nil) {
		    printf("\t%s\n",qq->name);
		    of++;
		    q = &qq->next;
		}
	    }
	}
	printf("%d buckets of %d used, %d collisions\n",su,htablesize,of);
}
