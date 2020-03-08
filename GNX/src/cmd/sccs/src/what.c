/*
 * what.c: version 1.2 of 2/14/83
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)what.c	1.2 (NSC) 2/14/83";
# endif

# include	"stdio.h"
# include	"sys/types.h"
# include	"macros.h"

char pattern[]  =  "@(#)";
char opattern[]  =  "~|^`";
char fflag;


main(argc,argv)
int argc;
register char **argv;
{
	register int i;
	register FILE *iop;

	while ((argc >= 2) && (argv[1][0] == '-')) {
		switch(argv[1][1]) {
			case 'f':
				fflag++;
				break;

			default:
				fprintf(stderr, "Bad option -%c\n", argv[1][1]);
				exit(1);
		}
		argc--;
		argv++;
	}
	if (argc < 2)
		dowhat(stdin);
	else
		for (i = 1; i < argc; i++) {
			if ((iop = fopen(argv[i],"r")) == NULL)
				fprintf(stderr,"can't open %s (26)\n",argv[i]);
			else {
				printf("%s:\n",argv[i]);
				dowhat(iop);
			}
		}
}


dowhat(iop)
register FILE *iop;
{
	register int c;

	while ((c = getc(iop)) != EOF) {
		if (c == pattern[0])
			if (trypat(iop, &pattern[1])) break;
		else if (c == opattern[0])
			if (trypat(iop, &opattern[1])) break;
	}
	fclose(iop);
}


trypat(iop,pat)
register FILE *iop;
register char *pat;
{
	register int c;

	for (; *pat; pat++)
		if ((c = getc(iop)) != *pat)
			break;
	if (!*pat) {
		putchar('\t');
		while ((c = getc(iop)) != EOF && c && !any(c,"\"\\>\n"))
			putchar(c);
		putchar('\n');
		return(fflag);
	}
	if (c != EOF)
		ungetc(c, iop);
	return(0);
}
