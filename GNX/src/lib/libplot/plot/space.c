/*
 * space.c: version 1.2 of 4/27/83
 * Unix plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)space.c	1.2 (NSC) 4/27/83";
# endif

#include <stdio.h>
space(x0,y0,x1,y1){
	putc('s',stdout);
	putsi(x0);
	putsi(y0);
	putsi(x1);
	putsi(y1);
}
