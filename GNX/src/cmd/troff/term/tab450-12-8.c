/*
 * tab450-12-8.c: version 1.2 of 4/12/83
 * Unix System Command Source File
 *
 * @(#)tab450-12-8.c	1.2	(NSC)	4/12/83
 */

#define INCH 240
/*
DASI450
12 chars/inch, 8 lines/inch
nroff driving tables
width and code tables
*/

struct termtable t450128 = {
/*bset*/	0,
/*breset*/	0177420,
/*Hor*/		INCH/60,
/*Vert*/	INCH/48,
/*Newline*/	INCH/8,
/*Char*/	INCH/12,
/*Em*/		INCH/12,
/*Halfline*/	INCH/16,
/*Adj*/		INCH/12,
/*twinit*/	"\0334\033\037\013\033\036\007",
/*twrest*/	"\0334\033\037\015\033\036\011",
/*twnl*/	"\015\n",
/*hlr*/		"\033D",
/*hlf*/		"\033U",
/*flr*/		"\033\n",
/*bdon*/	"",
/*bdoff*/	"",
/*ploton*/	"\0333",
/*plotoff*/	"\0334",
/*up*/		"\033\n",
/*down*/	"\n",
/*right*/	" ",
/*left*/	"\b",
/*codetab*/
#include "code.300"
