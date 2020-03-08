/*
 * versys.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)versys.c	1.1 (NSC) 5/4/83";
# endif

#include "uucp.h"


#define SNAMESIZE 7

/*******
 *	versys(name)	verify system names n1 and n2
 *	char *name;
 *
 *	return codes:  0  |  FAIL
 */

versys(name)
char *name;
{
	FILE *fp;
	char line[300];
	char s1[SNAMESIZE + 1];
	char myname[SNAMESIZE + 1];
	char *index();

	sprintf(myname, "%.7s", Myname);
	sprintf(s1, "%.7s", name);
	if (strcmp(s1, myname) == 0)
		return(0);
	fp = fopen(SYSFILE, "r");
	if (fp == NULL)
		return(FAIL);
	
	while (fgets(line, 300, fp) != NULL) {
		*(index(line, ' ')) = '\0';
		line[7] = '\0';
		if (strcmp(s1, line) == SAME) {
			fclose(fp);
			return(0);
		}
	}
	fclose(fp);
	return(FAIL);
}