/*
 * del_ba.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)del_ba.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

del_ba(dt,str)
register struct deltab *dt;
char *str;
{
	register char *p;

	p = str;
	*p++ = CTLCHAR;
	*p++ = BDELTAB;
	*p++ = ' ';
	*p++ = dt->d_type;
	*p++ = ' ';
	p = sid_ba(&dt->d_sid,p);
	*p++ = ' ';
	date_ba(&dt->d_datetime,p);
	while (*p++)
		;
	--p;
	*p++ = ' ';
	copy(dt->d_pgmr,p);
	while (*p++)
		;
	--p;
	*p++ = ' ';
	sprintf(p,"%u",dt->d_serial);
	while (*p++)
		;
	--p;
	*p++ = ' ';
	sprintf(p,"%u",dt->d_pred);
	while (*p++)
		;
	--p;
	*p++ = '\n';
	*p = 0;
	return(str);
}
