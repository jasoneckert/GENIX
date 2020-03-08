/*
 * fopenp.c: version 1.1 of 5/1/83
 * Section 3 Function (local)
 */
# ifdef SCCS
static char *sccsid = "@(#)fopenp.c	1.1 (NSC) 5/1/83";
# endif

# include <stdio.h>

/*
 * Fopenp opens the file fn with the given IO mode using the given search path.
 * Fnb is assumed to be a pointer to an area large enough
 * to hold the actual file name and the file name is copied into it.
 * The search path is interpreted in the same way
 * as the PATH environment variable is interpreted by exec?p().
 */
FILE *
fopenp (path, fn, fnb, mode)
	register char 	*path;
	char 		*fn, *fnb, *mode;
{
	register FILE	*fd;
	register char	*dst,
			*src;
	extern char	*strcpy();

	/* Fix bad arguments: */
	if (path == 0)
		path = "";

	/* We interpret absolute path names as is. */
	if (*fn == '/') {
		if ((fd = fopen (fn, mode)) != NULL) {
			strcpy (fnb, fn);
			return (fd);
		}
		return (NULL);
	}

	/* Try each component of the path in turn: */
	do {
		/* Copy next path component as a path prefix: */
		dst = fnb;
		while (*path && *path != ':')
			*dst++ = *path++;
		if (dst != fnb)
			*dst++ = '/';

		/* Copy in the file name as the final path component(s): */
		for (src = fn; *dst++ = *src++; )
			;

		/* Try opening the resulting pathname: */
		if ((fd = fopen (fnb, mode)) != NULL)
			return (fd);
	} while (*path++);

	/* Couldn't find it. */
	return (NULL);
}
