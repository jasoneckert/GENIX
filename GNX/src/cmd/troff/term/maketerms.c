/*
 * maketerms.c: version 1.1 of 4/21/83
 * Mesa Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)maketerms.c	1.1 (NSC) 4/21/83";
# endif

/*	maketerms.c - make terminal driving tables for nroff	*/


#include "../tw.h"	/* terminal table structure */
#include "stdio.h"

#ifdef PART1
#include "tab300.c"	/* terminal table initializations */
#include "tab300s.c"
#include "tab300S.c"
#include "tab300X.c"
#include "tab37.c"
#include "tabdm.c"
#include "tabh2.c"
#endif
#ifdef PART2
#include "tab450.c"
#include "tab450-12.c"
#include "tab450-12-8.c"
#include "tab450X.c"
#include "tablpr.c"
#include "tablpr2.c"
#include "tabtn300.c"
#endif
#ifdef PART3
#include "tabx1700.c"
#endif


#define SSIZE 2000	/* string output buffer size */
#define mkterm(pname, tname)	file = fopen("pname","w"); \
				puttab(file, &tname); \
				fclose(file);

extern char *putstr();	/* it's down below */


struct termtable tbuf;	/* terminal structure output buffer */
char sbuf[SSIZE];	/* structure string output buffer */
int sbp;		/* string buffer pointer */
FILE *file;		/* file descriptor */


main()
{
#ifdef PART1
	mkterm(tab300, t300)
	mkterm(tab300s, t300s)
	mkterm(tab300S, t300S)
	mkterm(tab300X, t300X)
	mkterm(tab37, t37)
	mkterm(tabdm, tdm)
	mkterm(tabh2, th2)
#endif
#ifdef PART2
	mkterm(tab450, t450)
	mkterm(tab450-12, t45012)
	mkterm(tab450-12-8, t450128)
	mkterm(tab450X, t450X)
	mkterm(tablpr, tlpr)
	mkterm(tablpr2, tlpr2)
	mkterm(tabtn300, ttn300)
#endif
#ifdef PART3
	mkterm(tabx1700, tx1700)
#endif
}


puttab(fc, tab)
FILE *fc;
struct termtable *tab;
{	int i;

	sbp = 0;		/* reset string pointer */

	tbuf.bset = tab->bset;	/* copy values to output structure */
	tbuf.breset = tab->breset;
	tbuf.Hor = tab->Hor;
	tbuf.Vert = tab->Vert;
	tbuf.Newline = tab->Newline;
	tbuf.Char = tab->Char;
	tbuf.Em = tab->Em;
	tbuf.Halfline = tab->Halfline;
	tbuf.Adj = tab->Adj;

	tbuf.twinit = putstr(tab->twinit,0);	/* copy strings and pointers */
	tbuf.twrest = putstr(tab->twrest,0);
	tbuf.twnl = putstr(tab->twnl,0);
	tbuf.hlr = putstr(tab->hlr,0);
	tbuf.hlf = putstr(tab->hlf,0);
	tbuf.flr = putstr(tab->flr,0);
	tbuf.bdon = putstr(tab->bdon,0);
	tbuf.bdoff = putstr(tab->bdoff,0);
	tbuf.ploton = putstr(tab->ploton,0);
	tbuf.plotoff = putstr(tab->plotoff,0);
	tbuf.up = putstr(tab->up,0);
	tbuf.down = putstr(tab->down,0);
	tbuf.right = putstr(tab->right,0);
	tbuf.left = putstr(tab->left,0);

	for (i=0; i<(256-32); i++)	/* do the same for codetab */

		tbuf.codetab[i] = putstr(tab->codetab[i],2);

	tbuf.zzz = tab->zzz;	/* ??? */

	fwrite((char *)&sbp,sizeof(sbp),1,fc);
	fwrite((char *)&tbuf,sizeof(tbuf),1,fc);
	fwrite((char *)sbuf,sbp,1,fc);
}



char *putstr(pt, cnt)
char *pt;
int cnt;
{	char *p;
	register int i, j, k;

	if (pt == (char *)0) return (0);	/* null pointer */
	for (i=0;  i<sbp;  i++)	{	/* check for string in buffer */

	    for (p=pt,k=cnt,j=i;
		  ((j<sbp) && (sbuf[j]==*p) && ((k-->0) || *p));
		    j++,p++)  ;

	    if ((j<sbp) && !sbuf[j] && !*p)
		return ((char *)i);	}

	/* not in table now - add it */

	i = sbp;

	for (k=cnt,p=pt;  ((sbp<SSIZE) && ((k-->0) || *p)); p++)

		sbuf[sbp++] = *p;	/* put string */

	sbuf[sbp++] = 0;		/* terminate string */

	if (sbp >= SSIZE)	{

		fputs("? string area overflow\n",23);
		exit(-1);	}

	return ((char *)i);
}
