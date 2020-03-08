/*
 * prim.c: version 3.4 of 5/26/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)prim.c	3.4 (NSC) 5/26/83";
# endif

#include "../h/param.h"
#include "../h/tty.h"
#include "../h/systm.h"
#include "../h/conf.h"
#include "../h/buf.h"
#include "../h/clist.h"

struct cblock *cfreelist = 0;

/*
 * Character list get/put
 */
getc(p)
register struct clist *p;
{
	register struct cblock *bp;
	register int c, s;

	s = spl6();
	if (p->c_cc <= 0) {
		c = -1;
		p->c_cc = 0;
		p->c_cf = p->c_cl = NULL;
	} else {
		c = *p->c_cf++ & 0377;
		if (--p->c_cc<=0) {
			bp = (struct cblock *)(p->c_cf-1);
			bp = (struct cblock *) ((int)bp & ~CROUND);
			p->c_cf = NULL;
			p->c_cl = NULL;
			bp->c_next = cfreelist;
			cfreelist = bp;
		} else if (((int)p->c_cf & CROUND) == 0){
			bp = (struct cblock *)(p->c_cf);
			bp--;
			p->c_cf = bp->c_next->c_info;
			bp->c_next = cfreelist;
			cfreelist = bp;
		}
	}
	splx(s);
	return(c);
}

putc(c, p)
register struct clist *p;
{
	register struct cblock *bp;
	register char *cp;
	register s;

	s = spl6();
	if ((cp = p->c_cl) == NULL || p->c_cc < 0 ) {
		if ((bp = cfreelist) == NULL) {
			splx(s);
			return(-1);
		}
		cfreelist = bp->c_next;
		bp->c_next = NULL;
		p->c_cf = cp = bp->c_info;
	} else if (((int)cp & CROUND) == 0) {
		bp = (struct cblock *)cp - 1;
		if ((bp->c_next = cfreelist) == NULL) {
			splx(s);
			return(-1);
		}
		bp = bp->c_next;
		cfreelist = bp->c_next;
		bp->c_next = NULL;
		cp = bp->c_info;
	}
	*cp++ = c;
	p->c_cc++;
	p->c_cl = cp;
	splx(s);
	return(0);
}

/*
 * Return count of contiguous characters
 * in clist starting at q->c_cf.
 * Stop counting if flag&character is non-null.
 */
ndqb (q, flag)
	register struct clist	*q;
	int			flag;
{
	register int	cc,
			s;

	s = spl5 ();
	if (q->c_cc <= 0) {
		cc = -q->c_cc;
		goto out;
	}
	cc  = ((int) q->c_cf + CBSIZE) & ~CROUND;
	cc -=  (int) q->c_cf;
	if (q->c_cc < cc)
		cc = q->c_cc;
	if (flag) {
		register char *p, *end;

		p = q->c_cf;
		end  = p;
		end += cc;
		while (p < end) {
			if (*p & flag) {
				cc  = (int) p;
				cc -= (int) q->c_cf;
				break;
			}
			p++;
		}
	}
out:
	splx (s);
	return (cc);
}

/*
 * Flush cc bytes from q.
 */
ndflush (q, cc)
	register struct clist	*q;
	register int		cc;
{
	register struct cblock	*bp;
	char			*end;
	int			rem;
	register int		s;

	s = spl5 ();
	if (q->c_cc < 0) {
		printf ("neg q flush\n");
		goto out;
	}
	if (q->c_cc == 0) {
		goto out;
	}
	while (cc > 0 && q->c_cc) {
		bp = (struct cblock *) ((int) q->c_cf & ~CROUND);
		if ((int) bp == (((int) q->c_cl-1) & ~CROUND)) {
			end = q->c_cl;
		} else {
			end = (char *) ((int) bp + sizeof (struct cblock));
		}
		rem = end - q->c_cf;
		if (cc >= rem) {
			cc -= rem;
			q->c_cc    -= rem;
			q->c_cf     = bp->c_next->c_info;
			bp->c_next  = cfreelist;
			cfreelist   = bp;
#			ifdef notdef
			cfreecount += CBSIZE;
			if (cwaiting) {
				wakeup (&cwaiting);
				cwaiting = 0;
			}
#			endif
		} else {
			q->c_cc -= cc;
			q->c_cf += cc;
			if (q->c_cc <= 0) {
				bp->c_next  = cfreelist;
				cfreelist   = bp;
#				ifdef notdef
				cfreecount += CBSIZE;
				if (cwaiting) {
					wakeup (&cwaiting);
					cwaiting = 0;
				}
#				endif
			}
			break;
		}
	}
	if (q->c_cc <= 0) {
		q->c_cf = q->c_cl = NULL;
		q->c_cc = 0;
	}
out:
	splx (s);
}

/*
 * Initialize clist by freeing all character blocks, then count
 * number of character devices. (Once-only routine)
 */
cinit()
{
	register int ccp;
	register struct cblock *cp;
	register struct cdevsw *cdp;

	ccp = (int)cfree;
	ccp = (ccp+CROUND) & ~CROUND;
	for(cp=(struct cblock *)ccp; cp <= &cfree[nclist-1]; cp++) {
		cp->c_next = cfreelist;
		cfreelist = cp;
#		ifdef notdef
		cfreecount += CBSIZE;
#		endif
	}
	ccp = 0;
	for(cdp = cdevsw; cdp->d_open; cdp++)
		ccp++;
	nchrdev = ccp;
}

/*
 * copy buffer to clist.
 * return number of bytes not transfered.
 */
b_to_q(cp, cc, q)
register char *cp;
struct clist *q;
register int cc;
{
	register char *cq;
	register struct cblock *bp;
	register s, acc;

	if (cc <= 0)
		return(0);
	acc = cc;


	s = spl6();
	if ((cq = q->c_cl) == NULL || q->c_cc < 0) {
		if ((bp = cfreelist) == NULL) 
			goto out;
		cfreelist = bp->c_next;
		bp->c_next = NULL;
		q->c_cf = cq = bp->c_info;
	}

	while (cc) {
		if (((int)cq & CROUND) == 0) {
			bp = (struct cblock *) cq - 1;
			if ((bp->c_next = cfreelist) == NULL) 
				goto out;
			bp = bp->c_next;
			cfreelist = bp->c_next;
			bp->c_next = NULL;
			cq = bp->c_info;
		}
		*cq++ = *cp++;
		cc--;
	}
out:
	q->c_cl = cq;
	q->c_cc += acc-cc;
	splx(s);
	return(cc);
}
#ifdef UCB_NTTY


/*
 * Given a non-NULL pointter into the list (like c_cf which
 * always points to a real character if non-NULL) return the pointer
 * to the next character in the list or return NULL if no more chars.
 *
 * Callers must not allow getc's to happen between nextc's so that the
 * pointer becomes invalid.  Note that interrupts are NOT masked.
 */
char *
nextc(p, cp)
register struct clist *p;
register char *cp;
{
	register char *rcp;

	if (p->c_cc && ++cp != p->c_cl) {
		if (((int)cp & CROUND) == 0)
			rcp = ((struct cblock *)cp)[-1].c_next->c_info;
		else
			rcp = cp;
	}
	else {
		rcp = 0;
	}
	return rcp;
}

/*
 * Remove the last character in the list and return it.
 */
unputc(p)
register struct clist *p;
{
	register struct cblock *bp;
	register int c, s;
	struct cblock *obp;

	s = spl6();
	if (p->c_cc <= 0)
		c = -1;
	else {
		c = *--p->c_cl;
		if (--p->c_cc <= 0) {
			bp = (struct cblock *)p->c_cl;
			bp = (struct cblock *)((int)bp & ~CROUND);
			p->c_cl = p->c_cf = NULL;
			bp->c_next = cfreelist;
			cfreelist = bp;
		} else if (((int)p->c_cl & CROUND) == sizeof(bp->c_next)) {
			p->c_cl = (char *)((int)p->c_cl & ~CROUND);
			bp = (struct cblock *)p->c_cf;
			bp = (struct cblock *)((int)bp & ~CROUND);
			while (bp->c_next != (struct cblock *)p->c_cl)
				bp = bp->c_next;
			obp = bp;
			p->c_cl = (char *)(bp + 1);
			bp = bp->c_next;
			bp->c_next = cfreelist;
			cfreelist = bp;
			obp->c_next = NULL;
		}
	}
	splx(s);
	return (c);
}

/*
 * Put the chars in the from que
 * on the end of the to que.
 */
catq(from, to)
struct clist *from, *to;
{
	register c;

	while ((c = getc(from)) >= 0)
		 putc(c, to);
}
#endif
