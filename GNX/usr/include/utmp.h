/*
 * @(#)utmp.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Structure of utmp and wtmp files.
 *
 * Assuming the number 8 is unwise.
 */
struct utmp {
	char	ut_line[8];		/* tty name */
	char	ut_name[8];		/* user id */
	long	ut_time;		/* time on */
};
