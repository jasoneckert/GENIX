/*
 * ln.c: version 1.2 of 9/2/82
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)ln.c	1.2 (NSC) 9/2/82";
# endif

/*
 * ln
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

struct	stat stb;
int	fflag;		/* force flag set? */
char	name[BUFSIZ];
char	*rindex();

main(argc, argv)
	int argc;
	register char **argv;
{
	register int i, r;

	argc--, argv++;
	if (argc && strcmp(argv[0], "-f") == 0) {
		fflag++;
		argv++;
		argc--;
	}
	if (argc == 0) 
		goto usage;
	else if (argc == 1) {
		argv[argc] = ".";
		argc++;
	}
	if (argc > 2) {
		if (stat(argv[argc-1], &stb) < 0)
			goto usage;
		if ((stb.st_mode&S_IFMT) != S_IFDIR) 
			goto usage;
	}
	r = 0;
	for(i = 0; i < argc-1; i++)
		r |= linkit(argv[i], argv[argc-1]);
	exit(r);
usage:
	fprintf(stderr, "Usage: ln f1\nor: ln f1 f2\nln f1 ... fn d2\n");
	exit(1);
}

linkit(from, to)
	char *from, *to;
{
	char *tail;

	/* is target a directory? */
	if (fflag == 0 && stat(from, &stb) >= 0
	    && (stb.st_mode&S_IFMT) == S_IFDIR) {
		printf("%s is a directory\n", from);
		return (1);
	}
	if (stat(to, &stb) >=0 && (stb.st_mode&S_IFMT) == S_IFDIR) {
		tail = rindex(from, '/');
		if (tail == 0)
			tail = from;
		else
			tail++;
		sprintf(name, "%s/%s", to, tail);
		to = name;
	}
	if (link(from, to) < 0) {
		perror(from);
		return (1);
	}
	return(0);
}
