/*
 * pf_ab.c: version 1.3 of 11/15/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)pf_ab.c	1.3 (NSC) 11/15/82";
# endif

# include	"../hdr/defines.h"

pf_ab(s,pp,all)
char *s;
register struct pfile *pp;
int all;
{
	register char *p;
	register int i;
	extern char *Datep;
	char *xp, *svxp;

	svxp = xp = p = malloc (size(s));	/* rewrote to avoid alloca */
	copy(s,p);
	for (; *p; p++)
		if (*p == '\n') {
			*p = 0;
			break;
		}
	p = xp;
	p = sid_ab(p,&pp->pf_gsid);
	++p;
	p = sid_ab(p,&pp->pf_nsid);
	++p;
	i = PWindex(p," ");
	pp->pf_user[0] = 0;
	if (((unsigned)i) < LOGSIZE) {
		move(p,pp->pf_user,i);
		pp->pf_user[i] = 0;
	}
	else
		fatal("bad p-file format (co17)");
	p = p + i + 1;
	date_ab(p,&pp->pf_date);
	p = Datep;
	pp->pf_ilist = 0;
	pp->pf_elist = 0;
	if (!all || !*p)
		goto release;
	p += 2;
	xp = malloc (size(p));
	copy(p,xp);
	p = xp;
	if (*p == 'i') {
		pp->pf_ilist = ++p;
		for (; *p; p++)
			if (*p == ' ') {
				*p++ = 0;
				p++;
				break;
			}
	}
	if (*p == 'x')
		pp->pf_elist = ++p;

	/*
	 * Get rid of storage formerly grabbed at top.
	 * (This was formerly done implicitly, since it was grabbed
	 * with alloca.)
	 */
release:
	free (svxp);
	return;
}
