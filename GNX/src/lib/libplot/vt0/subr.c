/*
 * subr.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)subr.c	1.2 (NSC) 4/28/83";
# endif

extern float obotx;
extern float oboty;
extern float boty;
extern float botx;
extern float scalex;
extern float scaley;
xsc(xi){
	int xa;
	xa = (xi-obotx)*scalex+botx;
	return(xa);
}
ysc(yi){
	int ya;
	ya = (yi-oboty)*scaley+boty;
	return(ya);
}
