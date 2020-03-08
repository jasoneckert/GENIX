
/*
 * symbol.c: version 1.7 of 7/21/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)symbol.c	1.7 (NSC) 7/21/83";
# endif


#include <stdio.h>
#include <a.out.h>
#include <stab.h>
#include "main.h"

#define DOTSYM 40

#define tallocsize 4000

struct symtype {
	int value,modf;
	char *name;
	struct symtype *left;
	struct symtype *right;
};

struct myentry {
	int	value;		/* value of symbol */
	unsigned short	module;	/* module number symbol is exported from */
	char	sym[MAXSTRLEN] /* bytes of name follow, null-terminated */
};

struct exec ddtheader;
struct myentry symslot;
struct symtype *tree, *valtree, *node, *bestmatch;
int cmplimit,lastmiss,local;
int (*cmpare)();
char	*snext,*slast;

char *ddtalloc(ssize)
register int ssize;
{
register char	*rvalue;
	if(snext+ssize > slast) {
		snext = (char *)malloc(tallocsize);
		slast = &snext[tallocsize-1];
	}
	rvalue = snext;
	snext = (char *)(((int)snext)+ssize+1);
	return(rvalue);
}

combasic(s1)
struct	symtype *s1;
{
register char *sc1,*sc2;
	sc1 = s1->name;
	sc2 = node->name;
	while (*sc1 == *sc2++)
		if (*sc1++=='\0')
			return(0);
	return(*sc1 - *--sc2);
}

comtodot(s1)
struct	symtype *s1;
{
int dotfound;
	if((dotfound = index(s1->name,'.')) == 0) {
		return(strcmp(s1->name,node->name));
	} else {
		return(strncmp(s1->name,node->name,dotfound));
	}
}

comlimit(s1)
struct	symtype *s1;
{
		return(strncmp(s1->name,node->name,cmplimit));
}

acompare(s1)
register struct	symtype *s1;
{
register struct symtype *lnode = node;
register int comrst = -1;
    if ((lnode->value>=s1->value) && ((lnode->value-s1->value) < lastmiss)) {
	    lastmiss = lnode->value - s1->value;
	    bestmatch = s1;
    }
    if (s1->value >=  lnode->value) comrst++;
    if (s1->value > lnode->value) comrst++;
    return(comrst);
}

struct symtype *enter(start)
register struct symtype *start;
{
register int	comresult;
register char *sc1,*sc2;
register struct symtype *lnode = node;
	if (start == 0) {
		start = (struct symtype *) ddtalloc(sizeof(struct symtype));
		start->value = lnode->value;
		start->modf = lnode->modf;
		start->name = lnode->name;
		start->left = start->right = 0;
	} else {
	    sc1 = start->name;
	    sc2 = lnode->name;
	    while (*sc1 == *sc2++)
		    if (*sc1++=='\0')
			    comresult = 0;
	    comresult = (*sc1 - *--sc2);
	    if (comresult > 0) start->left = enter(start->left);
	    else if (comresult < 0) start->right = enter(start->right);
	    else if (local == FALSE) {
			start->value = lnode->value;
			start->modf = lnode->modf;
			start->name = lnode->name;
		}
	}
	return(start);
}


struct symtype *venter(start)
register struct symtype *start;
{
register struct symtype *lnode = node;
	if (start == 0) {
		start = (struct symtype *) ddtalloc(sizeof(struct symtype));
		start->value = lnode->value;
		start->modf = lnode->modf;
		start->name = lnode->name;
		start->left = start->right = 0;
	} else {
	    if (start->value > lnode->value) start->left = venter(start->left);
	    else if (start->value < lnode->value) start->right = venter(start->right);
	    else if (local == FALSE) {
			start->value = lnode->value;
			start->modf = lnode->modf;
			start->name = lnode->name;
		}
	}
	return(start);
}

struct symtype *match(start)
register struct symtype *start;
{
register int	comresult;
	if (start == 0) return(start);
	comresult = (*cmpare)(start);
	if (comresult > 0) start = match(start->left);
	else if (comresult < 0) start = match(start->right);
	return(start);
}

lookbyval(val,offby,modf,symbl)
int val,*offby,*modf;
char *symbl;
{
struct symtype matchnode;
	node = &matchnode;
	*symbl = '\0';	
	node->value = val;
	bestmatch = 0;
	lastmiss = 10000000;
	cmpare = acompare;
	match(valtree);
	if (bestmatch == 0) return(FALSE);
	*offby = lastmiss;
	*modf = bestmatch->modf;
	strcpy(symbl,bestmatch->name);
	return(TRUE);
}

lookbysym(val,modn,symbol)
int *val,*modn;
char *symbol;
{
char testname[MAXSTRLEN];
struct symtype s,sdash;
	testname[0] = '_';
	strcpy((testname+1),symbol);
	s.name = symbol;
	sdash.name = testname;
	cmpare = combasic;
	node = &s;
	node = match(tree);
	if (node == 0) {
	    node = &sdash;
	    node = match(tree);
	}
	if (node == 0) {
	    cmpare = comtodot;
	    node = &s;
	    node = match(tree);
	}
	if (node == 0) {
	    node = &sdash;
	    node = match(tree);
	}
	if (node == 0) {
	    cmplimit = strlen(symbol);
	    cmpare = comlimit;
	    node = &s;
	    node = match(tree);
	}
	if (node == 0) {
	    cmplimit++;
	    node = &sdash;
	    node = match(tree);
	}
	if (node == 0) {
		printf("\r\nsymbol <%s> not found\r\n",symbol);
		return(FALSE);
	} else {
		*val = node->value;
		*modn = node->modf;
		return(TRUE);
	}

}

struct mod_entry {
	int statc,link,code,reserved;
};

struct sym_entry {
	int name,typ,value;
};

#define STRMASK 0xfffffe00
#define SYMTABSIZE 128
#define MAXMODTAB 64
#define MAXBUF 4096

aoutformat()
{
struct symtype sym;
int	symcount = 0,tcount = 0,count;
int	f = imagefd,i;
int	strtable = 1024,modtable,symtable,modseek = 0;
int	strnow = -MAXBUF,modnow = -10000,maxmodnow = -1030;
int	amod,etype,thismod,namestr,symsleft;
struct exec *h = &ddtheader;
struct sym_entry *fsym;
struct sym_entry symtab[SYMTABSIZE];
struct mod_entry *modp;
struct mod_entry modtab[MAXMODTAB];
char strings[MAXBUF],*pstr;
	printf("reading a.out symbols.");
	if (printsyms) printf("\r\n");
	fflush(stdout);
	sym.left = 0;
	sym.right = 0;
	if (h->a_magic == ZMAGIC) {
	    modseek = -h->a_mod_addr;
	} else if (h->a_magic == NMAGIC) {
	    modseek = 0;
	    strtable = sizeof(struct exec);
	}
	modtable = (h->a_magic == XMAGIC) ? strtable-1024 : strtable;
	symtable = N_SYMOFF(*h);
	strtable = N_STROFF(*h);
	symsleft = h->a_syms / 12;
	do {
		lseek(f,symtable,0);
		read(f,symtab,sizeof(symtab));
		count = 0;
		while ((count < SYMTABSIZE) && (symsleft > 0)) {
			fsym = &symtab[count];
			etype = fsym->typ & N_ETYPE;
			amod = fsym->name & N_AMODE;
			if ((fsym->typ & N_MODMASK) == N_MOD) {
				thismod = fsym->value;
				if ((thismod >= modnow) &&
				    (thismod < maxmodnow)) 
				    modp = &modtab[(thismod - modnow)/16];
			} else if ((etype <= N_METN) &&
			((amod == A_PC) || (amod == A_SB) || (amod == A_ABS)))
			{
				namestr = fsym->name / 64;
				if ((namestr < strnow) ||
				(namestr > (strnow+MAXBUF-80))) {
				    strnow = namestr & STRMASK;
				    lseek(f,strtable+strnow,0);
				    read(f,strings,sizeof(strings));
				}
				symcount++;
				if (tcount >= DOTSYM) {
					if (!printsyms) {
					    printf(".");
					    fflush(stdout);
					}
					tcount = 0;
				} else tcount++;
				pstr = &strings[namestr-strnow];
				if ((*pstr != 'L') && (*pstr != 'S')) {
			    if (thismod < modnow) {
				modnow = thismod -((MAXMODTAB*
					sizeof(struct mod_entry)) / 2);
				if (modnow < 1024) 
				    if (thismod < 1024) modnow = thismod;
				    else modnow = 1024;
				maxmodnow = modnow +
				 (MAXMODTAB*sizeof(struct mod_entry));
				lseek(f,modtable+modnow+modseek,0);
				read(f,modtab,sizeof(modtab));
				modp = &modtab[(thismod - modnow)/16];
			    }
				if (thismod >= maxmodnow) {
				   modnow = thismod;
				    maxmodnow = modnow +
				     (MAXMODTAB*sizeof(struct mod_entry));
				     lseek(f,modtable+modnow+modseek,0);
				     read(f,modtab,sizeof(modtab));
				     modp = modtab;
				}
				if (amod==A_PC)
				    sym.value=fsym->value+modp->code;
				else if (amod==A_SB)
				    sym.value=fsym->value+modp->statc;
				else if (amod==A_ABS) sym.value = fsym->value;
				sym.modf = thismod;
				    if (printsyms)
		printf("%8s\tvalue\t%5x module %d\r\n",pstr,sym.value,sym.modf);
				    i = strlen(pstr);
				    sym.name = (char *) ddtalloc(i+1);
				    strcpy(sym.name,pstr);
				    cmpare = combasic;
				    node = &sym;
				    local = ((fsym->typ & N_EXT) != N_EXT);
				    tree = enter(tree);
				    local = FALSE;
				    valtree = venter(valtree);
			    }

			}
			symsleft--;
			count++;
		}
		symtable += sizeof(symtab);
	} while(symsleft > 0);
	printf("found %d\r\n",symcount);
	fflush(stdout);
}

getsymboltable()
{
struct exec *h = &ddtheader;
	read(imagefd,h,sizeof ddtheader);
	if (N_BADMAG(*h)) {
	    printf("\r\nobject file has wrong type\r\n");
	    ddtdied();
	} else aoutformat();
	if (printsyms) ddtdied();
}
