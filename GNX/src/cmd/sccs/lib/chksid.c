/*
 * chksid.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)chksid.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

chksid(p,sp)
char *p;
register struct sid *sp;
{
	if (*p ||
		(sp->s_rel == 0 && sp->s_lev) ||
		(sp->s_lev == 0 && sp->s_br) ||
		(sp->s_br == 0 && sp->s_seq))
			fatal("invalid sid (co8)");
}
