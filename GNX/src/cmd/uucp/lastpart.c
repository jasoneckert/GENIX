/*
 * lastpart.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)lastpart.c	1.1 (NSC) 5/4/83";
# endif

/*******
 *	char *
 *	lastpart(file)	find last part of file name
 *	char *file;
 *
 *	return - pointer to last part
 */

char *
lastpart(file)
char *file;
{
	char *c;

	c = file + strlen(file);
	while (c >= file)
		if (*(--c) == '/')
			break;
	return(++c);
}
