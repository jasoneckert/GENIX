/*
 * rand.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)rand.c	1.1 (NSC) 9/5/82";
# endif

static	long	randx = 1;

srand(x)
unsigned x;
{
	randx = x;
}

rand()
{
	return((randx = randx * 1103515245 + 12345) & 0x7fffffff);
}
