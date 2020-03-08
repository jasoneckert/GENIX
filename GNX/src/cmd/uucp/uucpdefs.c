/*
 * uucpdefs.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)uucpdefs.c	1.1 (NSC) 5/4/83";
# endif

#include "uucp.h"
int Ifn, Ofn;
char Rmtname[10];
char User[10];
char Loginuser[10];
char Myname[8];
int Bspeed;
char Wrkdir[WKDSIZE];

char *Thisdir = THISDIR;
char *Spool = SPOOL;
int Debug = 0;
int Pkdebug = 0;
int Packflg = 0;
int Pkdrvon = 0;
long Retrytime;
short Usrf = 0;			/* Uustat global flag */
