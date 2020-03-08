/*
 * @(#)clist.h	1.4	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Raw structures for the character list routines.
 */
struct cblock {
	struct cblock *c_next;
	char	c_info[CBSIZE];
};
#ifdef KERNEL
extern	struct	cblock *cfree;
extern	int	nclist;
extern	struct	cblock *cfreelist;
extern	int	cfreecount;
#endif
