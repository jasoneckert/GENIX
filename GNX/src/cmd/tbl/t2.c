/*
 * t2.c: version 1.1 of 1/13/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)t2.c	1.1 (NSC) 1/13/83";
# endif

 /* t2.c:  subroutine sequencing for one table */
# include "t..c"
tableput()
{
saveline();
savefill();
ifdivert();
cleanfc();
getcomm();
getspec();
gettbl();
getstop();
checkuse();
choochar();
maktab();
runout();
release();
rstofill();
endoff();
restline();
}
