/*
 * mt.c: version 1.6 of 6/28/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)mt.c	1.6 (NSC) 6/28/83";
# endif

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/mtio.h>
#include <sys/ioctl.h>
#define	TAPENAME "/dev/nrtc0"
#define	GETCOUNT -1
#define	CLRCOUNT -2

struct commands {
	char *c_name;
	int c_code;
	int c_ronly;
} com[] = {
	"eof",	MTWEOF,	0,
	"fsf",	MTFSF,	1,
	"bsf",	MTBSF,	1,
	"fsr",	MTFSR,	1,
	"bsr",	MTBSR,	1,
	"rewind",	MTREW,	1,
	"offline",	MTOFFL,	1,
	"retension",	MTRETEN, 1,
	"erase",	MTERASE, 0,
	"display",	MTDISPL, 1,
	"getcounts",	GETCOUNT, 1,
	"clearcounts",	CLRCOUNT, 1,
	0
};

int mtfd;
struct mtop mt_com;
char *tape;

main(argc, argv)
char **argv;
{
	char line[80], *getenv();
	register char *cp;
	register struct commands *comp;

	if (argc < 2) {
		printf("usage: mt [ -t tape ] command [ count ]\ncommands:");
		for (comp = com; comp->c_name != NULL; comp++)
			printf(" %s", comp->c_name);
		printf("\n");
		exit(0);
	}
	if ((strcmp(argv[1], "-t") == 0) && argc > 2) {
		argc -= 2;
		tape = argv[2];
		argv += 2;
	} else
		if ((tape = getenv("TAPE")) == NULL)
			tape = TAPENAME;
	cp = argv[1];
	for (comp = com; comp->c_name != NULL; comp++)
		if (strncmp(cp, comp->c_name, strlen(cp)) == 0)
			break;
	if (comp->c_name == NULL) {
		fprintf(stderr, "mt: Unknown command \"%s\"\n", cp);
		exit(1);
	}
	if ((mtfd = open(tape, comp->c_ronly ? 0 : 2)) < 0) {
		perror(tape);
		exit(1);
	}
	if (comp->c_code == GETCOUNT) {		/* read error counts */
		if (getcounts()) {
			fprintf(stderr, "%s ", comp->c_name);
			perror("failed");
			exit(1);
		}
		exit(0);
	}
	if (comp->c_code == CLRCOUNT) {		/* clear error counts */
		if (ioctl(mtfd, MTIOCZERO, 0) < 0) {
			fprintf(stderr, "%s ",  comp->c_name);
			perror("failed");
			exit(1);
		}
		exit(0);
	}
	mt_com.mt_count = (argc > 2 ? atoi(argv[2]) : 1);
	mt_com.mt_op = comp->c_code;
	if (ioctl(mtfd, MTIOCTOP, &mt_com) < 0) {
		fprintf(stderr, "%s %d ", comp->c_name, mt_com.mt_count);
		perror("failed");
		exit(1);
	}
	exit(0);
}


/* type out error counters */
getcounts()
{
	struct	mtget	mt;

	if (ioctl(mtfd, MTIOCGET, &mt) < 0) {
		return(1);
	}
	printf("Reads/errors = %d/%d\n", mt.mt_reads, mt.mt_readerrors);
	printf("Writes/errors = %d/%d\n", mt.mt_writes, mt.mt_writeerrors);
	printf("Read/write retries = %d\n", mt.mt_retries);
	printf("Last error status = %x %x %x\n",
		(mt.mt_dsreg >> 8) & 0xff, mt.mt_dsreg & 0xff,
		mt.mt_erreg & 0xffff);
	return(0);
}
