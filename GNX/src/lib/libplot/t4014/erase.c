/*
 * erase.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)erase.c	1.2 (NSC) 4/27/83";
# endif

extern int ohiy;
extern int ohix;
extern int oloy;
extern int oextra;
erase(){
	int i;
		putch(033);
		putch(014);
		ohiy= -1;
		ohix = -1;
		oextra = -1;
		oloy = -1;
		sleep(2);
		return;
}
