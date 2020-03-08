/*
 * line.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)line.c	1.2 (NSC) 4/27/83";
# endif

line(x0,y0,x1,y1){
	move(x0,y0);
	cont(x1,y1);
}
