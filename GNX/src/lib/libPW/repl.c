/*
 * repl.c: version 1.1 of 11/2/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)repl.c	1.1 (NSC) 11/2/82";
# endif

/*
	Replace each occurrence of `old' with `new' in `str'.
	Return `str'.
*/

repl(str,old,new)
char *str;
char old,new;
{
	return(trnslat(str, &old, &new, str));
}
