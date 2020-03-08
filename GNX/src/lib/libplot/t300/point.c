/*
 * point.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)point.c	1.2 (NSC) 4/27/83";
# endif

point(xi,yi){
		move(xi,yi);
		label(".");
		return;
}
