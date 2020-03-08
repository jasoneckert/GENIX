/*
 * sid_ba.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)sid_ba.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

sid_ba(sp,p)
register struct sid *sp;
register char *p;
{
	sprintf(p,"%u.%u",sp->s_rel,sp->s_lev);
	while (*p++)
		;
	--p;
	if (sp->s_br) {
		sprintf(p,".%u.%u",sp->s_br,sp->s_seq);
		while (*p++)
			;
		--p;
	}
	return(p);
}
