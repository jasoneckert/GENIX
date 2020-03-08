/*
 * date_ba.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)date_ba.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"
# define DO2(p,n,c)	*p++ = n/10 + '0'; *p++ = n%10 + '0'; *p++ = c;

date_ba(bdt,adt)
long *bdt;
char *adt;
{
	register struct tm *lcltm;
	register char *p;

	lcltm = localtime(bdt);
	p = adt;
	DO2(p,lcltm->tm_year,'/');
	DO2(p,(lcltm->tm_mon + 1),'/');
	DO2(p,lcltm->tm_mday,' ');
	DO2(p,lcltm->tm_hour,':');
	DO2(p,lcltm->tm_min,':');
	DO2(p,lcltm->tm_sec,0);
	return(adt);
}
