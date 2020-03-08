/*
 * label.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)label.c	1.2 (NSC) 4/28/83";
# endif

#include "con.h"
label(s)
char *s;
{
	int i,c;
		while((c = *s++) != '\0'){
			xnow += 6;
			spew(c);
		}
		return;
}
