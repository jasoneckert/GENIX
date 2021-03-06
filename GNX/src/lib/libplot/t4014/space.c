/*
 * space.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)space.c	1.2 (NSC) 4/27/83";
# endif

extern float botx;
extern float boty;
extern float obotx;
extern float oboty;
extern float scalex;
extern float scaley;
extern int scaleflag;
space(x0,y0,x1,y1){
	botx = 0.;
	boty = 0.;
	obotx = x0;
	oboty = y0;
	if(scaleflag)
		return;
	scalex = 3120./(x1-x0);
	scaley = 3120./(y1-y0);
}
