/*
 * printf.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)printf.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

printf(fmt, args)
char *fmt;
{
	_doprnt(fmt, &args, stdout);
	return(ferror(stdout)? EOF: 0);
}
