/*
 * prf.c: version 2.3 of 12/8/82
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)prf.c	2.3 (NSC) 12/8/82";
# endif

/*
 * Scaled down version of C Library printf.
 * Only %s %u %d (==%u) %o %x %D %c are recognized.
 */
/*VARARGS1*/
printf(fmt, x1)
register char *fmt;
unsigned x1;
{
	register c;
	register unsigned int *adx;
	char *s;

	adx = &x1;
loop:
	while((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		putchar(c);
	}
	c = *fmt++;
	if(c == 'X')
		printx((long)*adx);
	else if(c == 'd' || c == 'u' || c == 'o' || c == 'x')
		printn((long)*adx, c=='o'? 8: (c=='x'? 16:10));
	else if(c == 'c') {
		putchar(*adx);
	}
	else if(c == 's') {
		s = (char *)*adx;
		while(c = *s++) {
			putchar(c);
		}
	} else if (c == 'D') {
		printn(*(long *)adx, 10);
		adx += (sizeof(long) / sizeof(int)) - 1;
	}
	adx++;
	goto loop;
}

printx(x)
register long x;
{
	register int i;

	for (i = 0; i < 8; i++)
		putchar("0123456789ABCDEF"[(x>>((7-i)*4))&0xf]);
}

/*
 * Print an unsigned integer in base b.
 */
printn(n, b)
register long n;
register b;
{
	register long a;

	if (n<0) {	/* shouldn't happen */
		putchar('-');
		n = -n;
	}
	if(a = n/b)
		printn(a, b);
	putchar("0123456789ABCDEF"[(int)(n%b)]);
}
