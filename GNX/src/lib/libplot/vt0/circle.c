/*
 * circle.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)circle.c	1.2 (NSC) 4/28/83";
# endif

extern vti;
circle(x,y,r){
	char c;
	c = 5;
	write(vti,&c,1);
	write(vti,&x,6);
}
