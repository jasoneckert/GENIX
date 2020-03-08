/*
 * cat.c: version 2.4 of 8/18/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)cat.c	2.4 (NSC) 8/18/83";
# endif
#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/filsys.h>
#include <sys/dir.h>
#include <sys/saio.h>

main()
{
	int c, i;
	char buf[50];

	do {
		printf("File: ");
		gets(buf);
		i = open(buf, 0);
	} while (i <= 0);

	while ((c = getc(i)) > 0)
		putchar(c);
	exit(0);
}
