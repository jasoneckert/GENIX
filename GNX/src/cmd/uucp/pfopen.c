/*
 * pfopen.c: version 1.1 of 5/12/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)pfopen.c	1.1 (NSC) 5/12/83";
# endif

/*
 * Routine like fopen, but checks for processes to open.
 * The process name begins with =, and any underscores
 * are translated into blanks.  We don't do things in the
 * obvious way (start with | or !, use blanks as themselves)
 * because getargs can't parse strings containing blanks in
 * all versions of uucp.
 */

#include <stdio.h>

static FILE *prevval = NULL;

FILE *popen();

FILE *
pfopen(name, mode)
char *name, *mode;
{
	char cmdbuf[256];
	register char *p;

	if (*name != '=') {
		prevval = NULL;
		return fopen(name, mode);
	}

	strcpy(cmdbuf, name);
	for (p=cmdbuf; *p; p++)
		if (*p == '_')
			*p = ' ';
	p = cmdbuf+1;
	prevval = popen(p, mode);
	return prevval;
}

pfclose(fd)
FILE *fd;
{
	if (fd == prevval) {
		pclose(fd);
		prevval = NULL;
	} else
		fclose(fd);
}
