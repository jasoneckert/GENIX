/*
 * csym.c: version 1.1 of 2/7/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)csym.c	1.1 (NSC) 2/7/83";
# endif

/* This program extracts namelist entries from a file (the kernel, usually)
 * and places them into another file so that they can be found quickly.  Not
 * all entries are extracted, only the ones that are specified.  This is
 * useful for speeding up "ps" and "w", for example.  The produced file can
 * be read with the cnlist routine.  David I. Bell.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <nlist.h>
#include "cnlist.h"


struct	nlist	*symtable;		/* pointer to list of symbols */
long	symcount;			/* number of symbols */
long	stringsize;			/* space used by all strings */
char	*strings;			/* pointer to list of strings */
char	*curstr;			/* current string */
char	*source;			/* source file name */
char	*destination;			/* destination file name */
char	force;				/* force output file */
char	verbose;			/* verbose output */
int	fail;				/* failure flag */
struct	cnlist	cn;			/* header for new file */
struct	stat	statbuf;		/* status of source file */
char	*stuff();			/* routine to stuff string */


main(argc, argv)
	register int	argc;
	register char	**argv;
{
	register struct	nlist	*sym;	/* current symbol entry */
	register int	io;		/* file descriptor */

	++argv;
	if ((argc > 1) && (**argv == '-')) {
		source = *argv;
		source++;
		while (*source) {
			switch(*source) {
			case 'f':
				force = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			default:
				printf("Bad option -%c\n", *source);
				exit(1);
			}
			source++;
		}
		argv++;
		argc--;
	}
	if (argc < 4) {
		printf("Usage: csym [-fv] sourcefile destfile symbol [symbol] ...\n");
		exit(1);
	}
	source = *argv;
	++argv;
	destination = *argv;
	argc -= 3;
	symcount = argc;
	symtable = (struct nlist *) calloc(argc + 1, sizeof(*sym));
	sym = symtable;
	stringsize = strlen(source) + 1;
	while (argc--) {
		++argv;
		sym->n_name = *argv;
		stringsize += (strlen(*argv) + 1);
		sym++;
	}
	strings = (char *) malloc(stringsize);
	curstr = strings;
	if (access(source, 4) || stat(source, &statbuf)) {	/* file data */
		perror(source);
		exit(1);
	}
	nlist(source, symtable);	/* get nlist entries */
	sym = symtable;
	argc = symcount;
	fail = 0;
	while (argc--) {
		if (sym->n_type == 0) {
			printf("symbol %s: not found\n", sym->n_name);
			fail++;
			sym++;
			continue;
		}
		if (verbose) {
			printf("symbol %s: type %x value %x\n",
				sym->n_name, sym->n_type, sym->n_value);
		}
		sym->n_name = stuff(sym->n_name);
		sym++;
	}
	if ((force == 0) && fail) {
		exit(1);
	}
	io = creat(destination, 0666);
	if (io < 0) {
		perror(destination);
		exit(1);
	}
	cn.cn_magic = CN_MAGIC;
	cn.cn_time = statbuf.st_mtime;
	cn.cn_count = symcount;
	cn.cn_file = stuff(source);		/* store file name */
	write(io, &cn, sizeof(cn));
	write(io, symtable, symcount * sizeof(*symtable));
	write(io, strings, stringsize);
	close(io);
}



/* Store a string and return to-be file offset */
char *
stuff(str)
	register char	*str;		/* string to store */
{
	int	offset;			/* offset into file */

	offset = (curstr - strings) + sizeof(cn) + (symcount*sizeof(*symtable));
	strcpy(curstr, str);
	curstr += strlen(str);
	*curstr++ = '\0';
	return((char *) offset);
}
