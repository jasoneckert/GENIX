/*
 * swab.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)swab.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Swap bytes in 16-bit [half-]words
 * for going between the 11 and the interdata
 */

swab(pf, pt, n)
register short *pf, *pt;
register n;
{

	n /= 2;
	while (--n >= 0) {
		*pt++ = (*pf << 8) + ((*pf >> 8) & 0377);
		pf++;
	}
}
