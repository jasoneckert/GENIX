/*
 * label.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)label.c	1.2 (NSC) 4/27/83";
# endif

#include <stdio.h>
label(s)
char *s;
{
	int i;
	putc('t',stdout);
	for(i=0;s[i];)putc(s[i++],stdout);
	putc('\n',stdout);
}
