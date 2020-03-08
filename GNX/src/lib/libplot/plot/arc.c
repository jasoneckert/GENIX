/*
 * arc.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)arc.c	1.2 (NSC) 4/27/83";
# endif

#include <stdio.h>
arc(xi,yi,x0,y0,x1,y1){
	putc('a',stdout);
	putsi(xi);
	putsi(yi);
	putsi(x0);
	putsi(y0);
	putsi(x1);
	putsi(y1);
}
