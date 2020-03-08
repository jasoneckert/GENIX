/*
 * stats_ab.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)stats_ab.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

stats_ab(pkt,statp)
register struct packet *pkt;
register struct stats *statp;
{
	extern	char	*satoi();
	register char *p;

	p = pkt->p_line;
	if (getline(pkt) == NULL || *p++ != CTLCHAR || *p++ != STATS)
		fmterr(pkt);
	NONBLANK(p);
	p = satoi(p,&statp->s_ins);
	p = satoi(++p,&statp->s_del);
	satoi(++p,&statp->s_unc);
}
