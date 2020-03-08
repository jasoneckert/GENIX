/*
 * arc.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)arc.c	1.2 (NSC) 4/28/83";
# endif

extern vti;
arc(xi,yi,x0,y0,x1,y1){
	char c;
	c = 6;
	write(vti,&c,1);
	write(vti,&xi,12);
}
