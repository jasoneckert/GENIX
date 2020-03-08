/*
 * fmterr.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)fmterr.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

fmterr(pkt)
register struct packet *pkt;
{
	fclose(pkt->p_iop);
	sprintf(Error,"format error at line %u (co4)",pkt->p_slnno);
	fatal(Error);
}
