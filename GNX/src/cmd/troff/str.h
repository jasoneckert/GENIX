/*
 * str.h: version 1.1 of 1/28/83
 * Unix System Command Source File
 *
 * @(#)str.h	1.1	(NSC)	1/28/83
 */

struct s {
	int nargs;
	struct s *pframe;
	filep pip;
	int pnchar;
	int prchar;
	int ppendt;
	int *pap;
	int *pcp;
	int pch0;
	int pch;
	};
