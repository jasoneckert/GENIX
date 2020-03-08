/*
 * expfile.c: version 1.2 of 5/12/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)expfile.c	1.2 (NSC) 5/12/83";
# endif

#include "uucp.h"
#include <sys/types.h>
#include <sys/stat.h>



/*******
 *	expfile(file)	expand file name
 *	char *file;
 *
 *	return codes:  none
 */

expfile(file)
char *file;
{
	char *fpart;
	char user[20], *up;
	char full[100];
	int uid;

	switch(file[0]) {
	case '=':
	case '/':
		return;
	case '~':
		for (fpart = file + 1, up = user; *fpart != '\0'
			&& *fpart != '/'; fpart++)
				*up++ = *fpart;
		*up = '\0';
		if (gninfo(user, &uid, full) != 0) {
			strcpy(full, PUBDIR);
		}
	
		strcat(full, fpart);
		strcpy(file, full);
		return;
	default:
		strcpy(full, Wrkdir);
		strcat(full, "/");
		strcat(full, file);
		strcpy(file, full);
		return;
	}
}


/***
 *	isdir(name)	check if directory name
 *	char *name;
 *
 *	return codes:  0 - not directory  |  1 - is directory
 */

isdir(name)
char *name;
{
	int ret;
	struct stat s;

	ret = stat(name, &s);
	if (ret < 0)
		return(0);
	if ((s.st_mode & S_IFMT) == S_IFDIR)
		return(1);
	return(0);
}


/***
 *	mkdirs(name)	make all necessary directories
 *	char *name;
 *
 *	return 0  |  FAIL
 */

mkdirs(name)
char *name;
{
	int ret, mask;
	char cmd[100], dir[100], *p;
	char *index();

	for (p = dir + 1;; p++) {
		strcpy(dir, name);
		if ((p = index(p, '/')) == NULL)
			return(0);
		*p = '\0';
		if (isdir(dir))
			continue;
		sprintf(cmd, "mkdir %s", dir);
		DEBUG(4, "mkdir - %s\n", dir);
		mask = umask(0);
		ret = shio(cmd, CNULL, CNULL, User);
		umask(mask);
		if (ret != 0)
			return(FAIL);
	}
}
