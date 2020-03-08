/*
 * @(#)modtable.h	3.2	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Format of an NS16000 module table entry:
 */

struct modtable {
	long	mt_sb,	/* static base register	*/
		mt_lb,	/* link table address	*/
		mt_pb,	/* program base address	*/
		mt_rs;	/* reserved		*/
};
