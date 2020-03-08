/*
 * uucpdefs.h: version 1.1 of 5/4/83
 * Uucp Header
 *
 * @(#)uucpdefs.h	1.1	(National Semiconductor)	5/4/83
 */

char *Thisdir = THISDIR;
char *Spool = SPOOL;
int Debug = 0;
int Pkdebug = 0;

char *Sysfiles[] = {
	SYSFILE,
	SYSFILECR,
	NULL
};
char *Devfile = DEVFILE;
char *Dialfile = DIALFILE;
int Packflg = 0;
int Pkdrvon = 0;
