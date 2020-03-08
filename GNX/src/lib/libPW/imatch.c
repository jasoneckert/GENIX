/*
 * imatch.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)imatch.c	1.1 (NSC) 11/2/82";
# endif

/*
	initial match
	if `prefix' is a prefix of `string' return 1
	else return 0
*/

imatch(prefix,string)
register char *prefix, *string;
{
	while (*prefix++ == *string++)
		if (*prefix == 0)
			return(1);
	return(0);
}
