/*
 * getline.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)getline.c	1.1 (NSC) 11/3/82";
# endif

#include	"../hdr/defines.h"

/*
	Routine to read a line into the packet.  The main reason for
	it is to make sure that pkt->p_wrttn gets turned off,
	and to increment pkt->p_slnno.
*/

getline(pkt)
register struct packet *pkt;
{
	register int n;
	register char *p;

	if(pkt->p_wrttn==0)
		putline(pkt,0);
	if ((n = fgets(pkt->p_line,sizeof(pkt->p_line),pkt->p_iop)) != NULL) {
		pkt->p_slnno++;
		pkt->p_wrttn = 0;
		for (p = pkt->p_line; *p; )
			pkt->p_chash += *p++;
	}
	else {
		if (!pkt->p_reopen) {
			fclose(pkt->p_iop);
			pkt->p_iop = 0;
		}
		if (!pkt->p_chkeof)
			fatal("premature eof (co5)");
		if (pkt->do_chksum && (pkt->p_chash ^ pkt->p_ihash)&0xFFFF)
			fatal("corrupted file (co6)");
		if (pkt->p_reopen) {
			fseek(pkt->p_iop,0L,0);
			pkt->p_reopen = 0;
			pkt->p_slnno = 0;
			pkt->p_ihash = 0;
			pkt->p_chash = 0;
			pkt->p_nhash = 0;
			pkt->p_keep = 0;
			pkt->do_chksum = 0;
		}
	}
	return(n);
}
