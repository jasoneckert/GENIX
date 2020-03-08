/*
 * del_ab.c: version 1.2 of 11/6/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)del_ab.c	1.2 (NSC) 11/6/82";
# endif

# include	"../hdr/defines.h"

del_ab(p,dt,pkt)
register char *p;
register struct deltab *dt;
struct packet *pkt;
{
	extern	char	*satoi();
	int n;
	extern char *Datep;

	if (*p++ != CTLCHAR)
		fmterr(pkt);
	if (*p++ != BDELTAB)
		return(*--p);
	NONBLANK(p);
	dt->d_type = *p++;
	NONBLANK(p);
	p = sid_ab(p,&dt->d_sid);
	NONBLANK(p);
	date_ab(p,&dt->d_datetime);
	p = Datep;
	NONBLANK(p);
	if ((n = PWindex(p," ")) < 0)
		fmterr(pkt);
	move(p,dt->d_pgmr,n);
	dt->d_pgmr[n] = 0;
	p += n + 1;
	NONBLANK(p);
	p = satoi(p,&dt->d_serial);
	NONBLANK(p);
	p = satoi(p,&dt->d_pred);
	if (*p != '\n')
		fmterr(pkt);
	return(BDELTAB);
}
