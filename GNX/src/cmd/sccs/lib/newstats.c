/*
 * newstats.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)newstats.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

newstats(pkt,strp,ch)
register struct packet *pkt;
register char *strp;
register char *ch;
{
	char fivech[6];
	repeat(fivech,ch,5);
	sprintf(strp,"%c%c %s/%s/%s\n",CTLCHAR,STATS,fivech,fivech,fivech);
	putline(pkt,strp,0);
}
