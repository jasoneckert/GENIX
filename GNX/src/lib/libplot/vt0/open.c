/*
 * open.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)open.c	1.2 (NSC) 4/28/83";
# endif

int xnow;
int ynow;
float boty 0.;
float botx 0.;
float oboty 0.;
float obotx 0.;
float scalex 1.;
float scaley 1.;
int vti -1;

openvt ()
{
		vti = open("/dev/vt0",1);
		return;
}
openpl()
{
	vti = open("/dev/vt0",1);
	return;
}
