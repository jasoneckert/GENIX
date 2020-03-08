/*
 * cont.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)cont.c	1.2 (NSC) 4/27/83";
# endif

#include <stdio.h>
cont(xi,yi){
	putc('n',stdout);
	putsi(xi);
	putsi(yi);
}
