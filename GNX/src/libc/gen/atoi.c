/*
 * atoi.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)atoi.c	1.1 (NSC) 9/5/82";
# endif

atoi(p)
register char *p;
{
	register int n;
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
