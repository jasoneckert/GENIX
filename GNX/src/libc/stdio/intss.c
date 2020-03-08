/*
 * intss.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)intss.c	1.1 (NSC) 9/2/82";
# endif

#include <stdio.h>
/*
 * Name refers to ``in TSS'', i.e. in the
 * TSS operating system and thus that input is a terminal.
 */
intss()
{

	return (isatty(fileno(stdin)));
}
