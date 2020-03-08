/*
 * tw.h: version 1.3 of 4/21/83
 * Unix System Command Source File
 *
 * @(#)tw.h	1.3	(NSC)	4/21/83
 */

/* typewriter driving table structure*/
struct termtable {
	int bset;
	int breset;
	int Hor;
	int Vert;
	int Newline;
	int Char;
	int Em;
	int Halfline;
	int Adj;
	char *twinit;
	char *twrest;
	char *twnl;
	char *hlr;
	char *hlf;
	char *flr;
	char *bdon;
	char *bdoff;
	char *ploton;
	char *plotoff;
	char *up;
	char *down;
	char *right;
	char *left;
	char *codetab[256-32];
	char *zzz;
	};
