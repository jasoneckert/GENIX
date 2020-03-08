/*
 * linmod.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)linmod.c	1.2 (NSC) 4/27/83";
# endif

#include <stdio.h>
linemod(s)
char *s;
{
	int i;
	putc('f',stdout);
	for(i=0;s[i];)putc(s[i++],stdout);
	putc('\n',stdout);
}
