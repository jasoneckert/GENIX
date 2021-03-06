/*
 * ex_argv.h: version 1.2 of 5/31/83
 * 
 *
 * @(#)ex_argv.h	1.2	(National Semiconductor)	5/31/83
 */
/* Copyright (c) 1980 Regents of the University of California */
/* sccs id:	@(#)ex_argv.h	6.1 10/18/80  */
/*
 * The current implementation of the argument list is poor,
 * using an argv even for internally done "next" commands.
 * It is not hard to see that this is restrictive and a waste of
 * space.  The statically allocated glob structure could be replaced
 * by a dynamically allocated argument area space.
 */
char	**argv;
char	**argv0;
char	*args;
char	*args0;
short	argc;
short	argc0;
short	morargc;		/* Used with "More files to edit..." */

int	firstln;		/* From +lineno */
char	*firstpat;		/* From +/pat	*/

/* Yech... */
struct	glob {
	short	argc;			/* Index of current file in argv */
	short	argc0;			/* Number of arguments in argv */
	char	*argv[NARGS + 1];	/* WHAT A WASTE! */
	char	argspac[NCARGS + sizeof (int)];
} frob;
