/*
 * @(#)lastlog.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

struct lastlog {
	time_t	ll_time;
	char	ll_line[8];
};
