/*
 * ltol3.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)ltol3.c	1.1 (NSC) 9/5/82";
# endif

ltol3(cp, lp, n)
char	*cp;
long	*lp;
int	n;
{
	register i;
	register char *a, *b;

	a = cp;
	b = (char *)lp;
	for(i=0;i<n;i++) {
#ifdef interdata
		b++;
		*a++ = *b++;
		*a++ = *b++;
		*a++ = *b++;
#else
		*a++ = *b++;
		*a++ = *b++;
		*a++ = *b++;
		b++;
#endif
	}
}
