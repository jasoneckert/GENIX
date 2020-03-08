/*
 * dot.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)dot.c	1.2 (NSC) 4/27/83";
# endif

#include <stdio.h>
dot(xi,yi,dx,n,pat)
int  pat[];
{
	int i;
	putc('d',stdout);
	putsi(xi);
	putsi(yi);
	putsi(dx);
	putsi(n);
	for(i=0; i<n; i++)putsi(pat[i]);
}
