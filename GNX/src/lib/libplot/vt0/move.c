/*
 * move.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)move.c	1.2 (NSC) 4/28/83";
# endif

extern vti;
extern xnow,ynow;
move(xi,yi){
	struct {char pad,c; int x,y;} p;
	p.c = 9;
	p.x = xnow = xsc(xi);
	p.y = ynow = ysc(yi);
	write(vti,&p.c,5);
}
