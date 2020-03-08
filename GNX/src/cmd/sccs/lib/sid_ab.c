/*
 * sid_ab.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)sid_ab.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

sid_ab(p,sp)
register char *p;
register struct sid *sp;
{
	extern	char	*satoi();
	if (*(p = satoi(p,&sp->s_rel)) == '.')
		p++;
	if (*(p = satoi(p,&sp->s_lev)) == '.')
		p++;
	if (*(p = satoi(p,&sp->s_br)) == '.')
		p++;
	p = satoi(p,&sp->s_seq);
	return(p);
}
