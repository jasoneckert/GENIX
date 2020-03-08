/*
 * getenv.c: version 1.2 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getenv.c	1.2 (NSC) 9/5/82";
# endif

# ifndef STATIC
# define STATIC static
# endif

/*
 *	getenv(name)
 *	returns ptr to value associated with name, if any, else NULL
 */
#define NULL	0
extern	char **environ;
char	*nvmatch();

char *
getenv(name)
register char *name;
{
	register char **p = environ;
	register char *v;

	while (*p != NULL)
		if ((v = nvmatch(name, *p++)) != NULL)
			return(v);
	return(NULL);
}

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 *	used for environment searching: see getenv
 */

STATIC char *
nvmatch(s1, s2)
register char *s1, *s2;
{

	while (*s1 == *s2++)
		if (*s1++ == '=')
			return(s2);
	if (*s1 == '\0' && *(s2-1) == '=')
		return(s2);
	return(NULL);
}
