/*
 * @(#)mp.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

#define MINT struct mint
MINT
{	int len;
	short *val;
};
#define FREE(x) {if(x.len!=0) {free((char *)x.val); x.len=0;}}
#ifndef DBG
#define shfree(u) free((char *)u)
#else
#include "stdio.h"
#define shfree(u) { if(dbg) fprintf(stderr, "free %o\n", u); free((char *)u);}
extern int dbg;
#endif
#ifndef vax
struct half
{	short high;
	short low;
};
#else
struct half
{	short low;
	short high;
};
#endif
extern MINT *itom();
extern short *xalloc();

#ifdef lint
extern xv_oid;
#define VOID xv_oid =
#else
#define VOID
#endif
