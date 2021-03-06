/*
 * size.c: version 1.5 of 8/26/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)size.c	1.5 (NSC) 8/26/83";
# endif

/*
 * size
 */

#include	<stdio.h>
#include 	<a.out.h>

int	header;
# ifdef vax
char *objfile = "a16.out";
# else
char *objfile = "a.out";
#endif


main(argc, argv)
char **argv;
{
	struct exec buf;
	long sum;
	int gorp,i;
	FILE *f;

	if (argc==1) {
		*argv = objfile;
		argc++;
		--argv;
	}
	gorp = argc;
	while(--argc) {
		++argv;
		if ((f = fopen(*argv, "r"))==NULL) {
			printf("size: %s not found\n", *argv);
			continue;
		}
		if (fread((char *)&buf, sizeof(buf), 1, f) != 1 ||
		    N_BADMAG(buf)) {
			printf("size: %s not an object file\n", *argv);
			fclose(f);
			continue;
		}
		if (header == 0) {
			printf("text\tdata\tbss\tdec\thex\n");
			header = 1;
		}
		printf("%u\t%u\t%u\t", buf.a_text,buf.a_data,buf.a_bss);
		sum = (long) buf.a_text + (long) buf.a_data + (long) buf.a_bss;
		printf("%ld\t%lx", sum, sum);
		if (gorp>2)
			printf("\t%s", *argv);
		printf("\n");
		fclose(f);
	}
}
