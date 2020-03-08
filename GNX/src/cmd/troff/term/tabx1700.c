/*
 * tabx1700.c: version 1.2 of 4/12/83
 * Unix System Command Source File
 *
 * @(#)tabx1700.c	1.2	(NSC)	4/12/83
 */

#define INCH 240
/*
XEROX 1700
nroff driving tables
width and code tables
*** may not be complete & may print some greeks wrong ***
*/

struct termtable tx1700 = {
/*bset*/	0,
/*breset*/	0177420,
/*Hor*/		INCH/60,
/*Vert*/	INCH/48,
/*Newline*/	INCH/6,
/*Char*/	INCH/10,
/*Em*/		INCH/10,
/*Halfline*/	INCH/12,
/*Adj*/		INCH/10,
/*twinit*/	"",
/*twrest*/	"",
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
