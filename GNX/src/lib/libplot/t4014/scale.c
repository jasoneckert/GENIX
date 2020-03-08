/*
 * scale.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)scale.c	1.2 (NSC) 4/27/83";
# endif

extern float scalex;
extern float scaley;
extern int scaleflag;
scale(i,x,y)
char i;
float x,y;
{
	switch(i) {
	default:
		return;
	case 'c':
		x *= 2.54;
		y *= 2.54;
	case 'i':
		x /= 200;
		y /= 200;
	case 'u':
		scalex = 1/x;
		scaley = 1/y;
	}
	scaleflag = 1;
}
