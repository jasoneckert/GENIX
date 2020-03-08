/*
 * mailst.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)mailst.c	1.1 (NSC) 5/4/83";
# endif

#include "uucp.h"



/*******
 *	mailst(user, str, file)
 *
 *	mailst  -  this routine will fork and execute
 *	a mail command sending string (str) to user (user).
 *	If file is non-null, the file is also sent.
 *	(this is used for mail returned to sender.)
 */

mailst(user, str, file)
char *user, *str, *file;
{
	FILE *fp, *fi;
	extern FILE *popen(), *pclose();
	char cmd[100], buf[BUFSIZ];
	int nc;

	sprintf(cmd, "mail %s", user);
	if ((fp = popen(cmd, "w")) == NULL)
		return;
	fprintf(fp, "%s", str);

	if (*file != '\0' && (fi = fopen(file, "r")) != NULL) {
		while ((nc = fread(buf, sizeof (char), BUFSIZ, fi)) > 0)
			fwrite(buf, sizeof (char), nc, fp);
		fclose(fi);
	}

	pclose(fp);
	return;
}
