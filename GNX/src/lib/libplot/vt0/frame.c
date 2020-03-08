/*
 * frame.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)frame.c	1.2 (NSC) 4/28/83";
# endif

frame(n)
{
	extern vti;
	n=n&0377 | 02000;
	write(vti,&n,2);
}
