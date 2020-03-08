/*
 * sysexits.c: version 1.1 of 2/14/83
 * Mesa Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)sysexits.c	1.1 (NSC) 2/14/83";
# endif

# include <sysexits.h>

/*
static char	SccsId[] = "@(#)sysexits.c	2.1	11/5/80";
*/

/*
**  SYSEXITS.C -- error messages corresponding to sysexits.h
*/

char	*SysExMsg[] =
{
	/* 64 */	"Bad usage",
	/* 65 */	"Data format error",
	/* 66 */	"Cannot open input",
	/* 67 */	"User unknown",
	/* 68 */	"Host unknown",
	/* 69 */	"Service unavailable",
	/* 70 */	"Internal error",
	/* 71 */	"Operating system error",
	/* 72 */	"System file missing",
	/* 73 */	"Can't create output",
	/* 74 */	"I/O error",
};

int	N_SysEx = sizeof SysExMsg / sizeof SysExMsg[0];
