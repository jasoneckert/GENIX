/*
 * move.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)move.c	1.2 (NSC) 4/27/83";
# endif

move(xi,yi){
		movep(xconv(xsc(xi)),yconv(ysc(yi)));
		return;
}
