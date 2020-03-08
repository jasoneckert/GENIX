/*
 * circle.c: version 1.2 of 4/27/83
 * Unix Plot Library Source file
 */
# ifdef SCCS
static char *sccsid = "@(#)circle.c	1.2 (NSC) 4/27/83";
# endif

#include <stdio.h>
circle(x,y,r){
	putc('c',stdout);
	putsi(x);
	putsi(y);
	putsi(r);
}
