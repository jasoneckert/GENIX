/*
 * anyread.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)anyread.c	1.1 (NSC) 5/4/83";
# endif

#include "uucp.h"
#include <sys/types.h>
#include <sys/stat.h>


/*******
 *	anyread		check if anybody can read
 *	return 0 ok: FAIL not ok
 */

anyread(file)
char *file;
{
	struct stat s;

	if (stat(file, &s) != 0)
		/* for security check a non existant file is readable */
		return(0);
	if (!(s.st_mode & ANYREAD))
		return(FAIL);
	return(0);
}
