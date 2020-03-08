/*
 * dot.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)dot.c	1.2 (NSC) 4/28/83";
# endif

extern vti;
dot(xi,yi,dx,n,pat)
int pat[];
{
	struct {char pad,c; int xi,yi,dx;} p;
	p.c = 7;
	p.xi = xsc(xi);
	p.yi = ysc(yi);
	p.dx = xsc(dx);
	write(vti,&p.c,7);
	write(vti,pat,n?n&0377:256);
}
