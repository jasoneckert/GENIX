/*
 * mktemp.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)mktemp.c	1.1 (NSC) 9/5/82";
# endif

char *
mktemp(as)
char *as;
{
	register char *s;
	register unsigned pid;
	register i;

	pid = getpid();
	s = as;
	while (*s++)
		;
	s--;
	while (*--s == 'X') {
		*s = (pid%10) + '0';
		pid /= 10;
	}
	s++;
	i = 'a';
	while (access(as, 0) != -1) {
		if (i=='z')
			return("/");
		*s = i++;
	}
	return(as);
}
