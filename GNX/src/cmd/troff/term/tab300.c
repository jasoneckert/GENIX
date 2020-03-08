/*
 * tab300.c: version 1.2 of 4/12/83
 * Unix System Command Source File
 *
 * @(#)tab300.c	1.2	(NSC)	4/12/83
 */

#define INCH 240
/*
DASI300
nroff driving tables
width and code tables
*/

struct termtable t300 = {
/*bset*/	0,
/*breset*/	0177420,
/*Hor*/		INCH/60,
/*Vert*/	INCH/48,
/*Newline*/	INCH/6,
/*Char*/	INCH/10,
/*Em*/		INCH/10,
/*Halfline*/	INCH/12,
/*Adj*/		INCH/10,
/*twinit*/	"\007",
/*twrest*/	"\007",
/*twnl*/	"\015\n",
/*hlr*/		"",
/*hlf*/		"",
/*flr*/		"\013",
/*bdon*/	"",
/*bdoff*/	"",
/*ploton*/	"\006",
/*plotoff*/	"\033\006",
/*up*/		"\013",
/*down*/	"\n",
/*right*/	" ",
/*left*/	"\b",
/*codetab*/
#include "code.300"
