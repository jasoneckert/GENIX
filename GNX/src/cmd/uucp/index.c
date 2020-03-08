/*
 * index.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)index.c	1.1 (NSC) 5/4/83";
# endif

#include <stdio.h>


/*******
 *	char *
 *	index(str, c)	return pointer to character c
 *	char c, *str;
 *
 *	return codes:
 *		NULL  -  character not found
 *		pointer  -  pointer to character
 */

char *
index(str, c)
char c, *str;
{
	for (; *str != '\0'; str++) {
		if (*str == c)
			return(str);
	}

	return(NULL);
}
