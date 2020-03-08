/*
 * putsi.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)putsi.c	1.2 (NSC) 4/27/83";
# endif

#include <stdio.h>
putsi(a){
	putc((char)a,stdout);
	putc((char)(a>>8),stdout);
}
