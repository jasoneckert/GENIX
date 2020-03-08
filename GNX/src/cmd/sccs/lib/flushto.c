/*
 * flushto.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)flushto.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

flushto(pkt,ch,put)
register struct packet *pkt;
register char ch;
int put;
{
	register char *p;

	while ((p = getline(pkt)) != NULL && !(*p++ == CTLCHAR && *p == ch))
		pkt->p_wrttn = put;

	if (p == NULL)
		fmterr(pkt);

	putline(pkt,0);
}
