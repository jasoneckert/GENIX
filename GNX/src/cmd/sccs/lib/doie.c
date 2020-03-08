/*
 * doie.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)doie.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

doie(pkt,ilist,elist,glist)
struct packet *pkt;
char *ilist, *elist, *glist;
{
	if (ilist) {
		if (pkt->p_verbose & DOLIST)
			fprintf(pkt->p_stdout,"Included:\n");
		dolist(pkt,ilist,INCLUDE);
	}
	if (elist) {
		if (pkt->p_verbose & DOLIST)
			fprintf(pkt->p_stdout,"Excluded:\n");
		dolist(pkt,elist,EXCLUDE);
	}
	if (glist)
		dolist(pkt,glist,IGNORE);
}
