/*
 * @(#)timeb.h	3.2	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Structure returned by ftime system call
 */
struct timeb {
	time_t	time;
	unsigned short millitm;
	short	timezone;
	short	dstflag;
};
