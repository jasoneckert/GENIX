/*
 * label.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)label.c	1.2 (NSC) 4/28/83";
# endif

extern vti;

label(s)
char *s;
{
	int i, o;

	o = 01401;
	write(vti, &o, 2);
	for(i=0; s[i++]; )
		;
	write(vti, s, i);
}
