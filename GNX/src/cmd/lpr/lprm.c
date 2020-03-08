/*
 * lprm.c: version 1.4 of 8/12/83
 * Mesa Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)lprm.c	1.4 (NSC) 8/12/83";
# endif

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <stdio.h>

char	line[128];
int	linel;
int	all;
char	plpdir[] = "/usr/spool/lpd";	/* parallel	*/
char	slpdir[] = "/usr/spool/slp";	/* serial	*/
char	*lpddir = plpdir;
FILE	*df;
FILE	*dfb;

main(argc, argv)
	int argc;
	char *argv[];
{
	register int i;
	char *cp;
	int cnt;

	argc--, argv++;
	if (argc == 0) {
		printf("usage: lprm [-[s][a]] [ id ... ] [ filename ... ] [ user ... ]\n");
		exit(1);
	}
	cp = *argv;
	if (*cp == '-') {
		argv++; argc--;
		while (*(++cp)) {
			if (*cp == 's') lpddir = slpdir;
			if (*cp == 'a') all++;
		}
	}
	if (chdir(lpddir) < 0)
		perror(lpddir), exit(1);
	df = fopen(".", "r");
	if (df == NULL)
		perror(lpddir), exit(1);
	if (all) clobber (*(--argv));
	else do {
		clobber(*argv++);
	} while (--argc);
}

clobber(cp)
	char *cp;
{
	struct direct dirent;
	int did = 0;

	rewind(df);
	while (fread(&dirent, sizeof dirent, 1, df) == 1) {
		if (dirent.d_ino == 0)
			continue;
		if (dirent.d_name[0] != 'd' || dirent.d_name[1] != 'f')
			continue;
		if (dirent.d_name[7] == 0 || dirent.d_name[8] != 0)
			continue;
		if (chkclob(cp, dirent.d_name)) {
			did++;
			printf("removing %s\n", dirent.d_name+3);
			drop(dirent.d_name);
			dirent.d_name[0] = 'c'; drop(dirent.d_name);
			dirent.d_name[0] = 'l'; drop(dirent.d_name);
			dirent.d_name[0] = 't'; drop(dirent.d_name);
			dirent.d_name[0] = 'd';
		}
	}
	if (did == 0)
		fprintf(stderr, "%s: nothing to remove\n", cp);
}

chkclob(pattern, file)
	char *pattern, *file;
{
	register char *id = pattern;
	char *cp;

	/* if removing all, match all files starting with "cfa" or "dfa" */
	if (all && any(file[0], "cd")  && file[1] == 'f' && file[2] == 'a')
		return (1);
	/*
	 * Quick check for matching id
	 */
	if (any(id[0], "cd") && id[1] == 'f' && id[2] == 'a')
		id += 3;
	if (strcmp(file+3, id) == 0)
		return (1);
	/*
	 * Now check for matching filename 'B', 'F' or id 'L'
	 */
	dfb = fopen(file, "r");
	if (dfb == NULL)
		return (0);
	while (getline()) switch (line[0]) {

	case 'L':
	case 'B':
	case 'F':
		/* line looks like "name,original name"; stop at "," */
		cp = line+1;
		while (*cp++) {
			if (*cp == ',') {
				*cp = '\0';
				break;
			}
		}
		if (strcmp(line+1, pattern) == 0) {
			fclose(dfb);
			return (1);
		}
		continue;
	}
	fclose(dfb);
	return (0);
}

drop(file)
	char *file;
{
	close(creat(file, 0666));
	unlink(file);
}

any(c, cp)
	char c;
	register char *cp;
{

	while (*cp)
		if (c == *cp++)
			return (1);
	return (0);
}

getline()
{
	register int i, c;

	i = 0;
	while ((c = getc(dfb)) != '\n') {
		if (c <= 0)
			return(0);
		if (i < 100)
			line[i++] = c;
	}
	line[i++] = 0;
	return (1);
}
