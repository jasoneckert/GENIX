# ifndef NOSCCS
static char *sccsid = "@(#)lock.c	1.3	8/10/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sgtty.h>

/*
 * Lock a terminal up until the knowledgeable Joe returns.
 */
struct	sgttyb tty, ntty;
char	s[BUFSIZ], s1[BUFSIZ];

main(argc, argv)
	char **argv;
{
	register int t;
	struct stat statb;

	for (t = 1; t <= 32; t++)
		if (t != SIGHUP)
		signal(t, SIG_IGN);
	if (argc > 0)
		argv[0] = 0;
	if (gtty(0, &tty))
		exit(1);
	ntty = tty; ntty.sg_flags &= ~ECHO;
	stty(0, &ntty);
	printf("Key: ");
	fgets(s, sizeof s, stdin);
	printf("\nAgain: ");
	fgets(s1, sizeof s1, stdin);
	putchar('\n');
	if (strcmp(s1, s)) {
		putchar(07);
		stty(0, &tty);
		exit(1);
	}
	s[0] = 0;
	for (;;) {
		fgets(s, sizeof s, stdin);
		if (strcmp(s1, s) == 0)
			break;
		putchar(07);
		if (gtty(0, &ntty))
			exit(1);
	}
	stty(0, &tty);
}
