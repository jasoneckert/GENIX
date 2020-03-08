/*
 * who.c: version 1.2 of 5/12/83
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)who.c	1.2 (NSC) 5/12/83";
# endif

/*
 * who
 */

#include <stdio.h>
#include <utmp.h>
#include <pwd.h>
#include <ctype.h>

#define NMAX sizeof(utmp.ut_name)
#define LMAX sizeof(utmp.ut_line)
#define	HMAX 32

struct utmp utmp;
struct passwd *pw;
struct passwd *getpwuid();
int hostnamelen = HMAX;
char hostname[HMAX];

char *ttyname(), *rindex(), *ctime(), *strcpy();
main(argc, argv)
char **argv;
{
	register char *tp, *s;
	register FILE *fi;
	extern char _sobuf[];

	gethostname(hostname, &hostnamelen);
	setbuf(stdout, _sobuf);
	s = "/etc/utmp";
	if(argc == 2)
		s = argv[1];
	if (argc==3) {
		tp = ttyname(0);
		if (tp)
			tp = rindex(tp, '/') + 1;
		else {	/* no tty - use best guess from passwd file */
			pw = getpwuid(getuid());
			strcpy(utmp.ut_name, pw?pw->pw_name: "?");
			strcpy(utmp.ut_line, "tty??");
			time(&utmp.ut_time);
			putline();
			exit(0);
		}
	}
	if ((fi = fopen(s, "r")) == NULL) {
		puts("who: cannot open utmp");
		exit(1);
	}
	while (fread((char *)&utmp, sizeof(utmp), 1, fi) == 1) {
		if(argc==3) {
			if (strcmp(utmp.ut_line, tp))
				continue;
			printf("%s!", hostname);
			putline();
			exit(0);
		}
		if(utmp.ut_name[0] == '\0' && argc==1)
			continue;
		putline();
	}
}

putline()
{
	register char *cbuf;

	printf("%-*.*s %-*.*s", NMAX, NMAX, utmp.ut_name, LMAX, LMAX, utmp.ut_line);
	cbuf = ctime(&utmp.ut_time);
	printf("%.12s\n", cbuf+4);
}
