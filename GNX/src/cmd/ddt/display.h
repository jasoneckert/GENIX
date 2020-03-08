
/*
 * display.h: version 1.2 of 1/7/83
 * 
 *
 * @(#)display.h	1.2	(National Semiconductor)	1/7/83
 */

/* output modes for outputmode */
#define INSTRUCT	0x1
#define NUMERIC		0x2
#define ABSNUMERIC	0x3
#define FLOATING	0x4
#define CHARACTER	0x5
#define STRING		0x6

extern int firsthex;	/* flag for printing leading zeros on hex output */
extern int acontext;	/* last size(b,w,d) of a displayed object */

extern int snprint();	/* printnum as numeric of outradix */
extern char *regstr();	/* return register characters for printing */

