/*
 * atol.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)atol.c	1.1 (NSC) 9/5/82";
# endif

long
atol(p)
register char *p;
{
	long n;
	register int f;

	n = 0;
	f = 0;
	for(;;p++) {
		switch(*p) {
		case ' ':
		case '\t':
			continue;
		case '-':
			f++;
		case '+':
			p++;
		}
		break;
	}
	while(*p >= '0' && *p <= '9')
		n = n*10 + *p++ - '0';
	return(f? -n: n);
}
