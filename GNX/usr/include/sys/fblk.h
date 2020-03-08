/*
 * @(#)fblk.h	3.2	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

struct fblk
{
	int    	df_nfree;
	daddr_t	df_free[NICFREE];
};
