/*
 * @(#)vmsystm.h	1.3	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */


/*
 * Fork/vfork accounting.
 */
struct	forkstat
{
	int	cntfork;
	int	cntvfork;
	int	sizfork;
	int	sizvfork;
};
