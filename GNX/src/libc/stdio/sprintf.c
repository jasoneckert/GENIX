/*
 * sprintf.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)sprintf.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

char *sprintf(str, fmt, args)
char *str, *fmt;
{
	struct _iobuf _strbuf;

	_strbuf._flag = _IOWRT+_IOSTRG;
	_strbuf._ptr = str;
	_strbuf._cnt = 32767;
	_doprnt(fmt, &args, &_strbuf);
	putc('\0', &_strbuf);
	return(str);
}
