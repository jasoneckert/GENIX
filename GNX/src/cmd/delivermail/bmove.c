/*
 * bmove.c: version 1.1 of 2/14/83
 * Mesa Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)bmove.c	1.1 (NSC) 2/14/83";
# endif

/* @(#)bmove.c	2.1	11/5/80 */

bmove(s, d, l)
	register char *s, *d;
	register int l;
{
	while (l-- > 0)
		*d++ = *s++;
}
