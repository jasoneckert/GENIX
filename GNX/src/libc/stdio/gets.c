/*
 * gets.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)gets.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

char *
gets(s)
char *s;
{
	register c;
	register char *cs;

	cs = s;
	while ((c = getchar()) != '\n' && c >= 0)
		*cs++ = c;
	if (c<0 && cs==s)
		return(NULL);
	*cs++ = '\0';
	return(s);
}
