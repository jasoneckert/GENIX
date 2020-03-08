/*
 * basename.c: version 1.1 of 10/12/82
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)basename.c	1.1 (NSC) 10/12/82";
# endif

/*
 * This version accepts a third argument, specifying a prefix
 * to be stripped away.  This stripping is done after all
 * other processing.
 */

# include "stdio.h"

main (argc, argv)
	int  argc;
	char **argv;
{
	register char *p1, *p2, *p3, *p4;

	/* If no arguments, we're done: */
	if (argc < 2)
	{       putchar ('\n');
		exit    (1);
	}

	/* Set p2 to point to the leafname part of the overall pathname: */
	p1 = argv[1];
	p2 = p1;
	while (*p1)
	{       if (*p1++ == '/')
			p2 = p1;
	}

	/* If we've been given a suffix, strip it off: */
	if (argc > 2)
	{       /* Find end of suffix: */
		for (p3 = argv[2]; *p3; p3++) 
			;
		/* Match it against the leafname: */
		while (p1 > p2 && p3 > argv[2])
			if(*--p3 != *--p1)
				goto output;
		/* Strip it off: */
		*p1 = '\0';
	}

	/* Finally, if we've been given a prefix, strip it: */
	if (argc > 3)
	{       /* Match, w/o disturbing what we have (in case of failure): */
		p3 = argv[3];
		p4 = p2;
		while (p4 < p1 && *p3)
			if (*p3++ != *p4++)
				goto output;
		if (*p3)
		{       /* We didn't match all of it: */
			goto output;
		}
		/* Strip the prefix: */
		p2 = p4;
	}

output:
	puts (p2, stdout);
	exit (0);
}
