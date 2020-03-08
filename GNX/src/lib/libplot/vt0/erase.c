/*
 * erase.c: version 1.2 of 4/28/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)erase.c	1.2 (NSC) 4/28/83";
# endif

extern vti;
erase(){
	int i;
	i=0401;
	write(vti,&i,2);
}
