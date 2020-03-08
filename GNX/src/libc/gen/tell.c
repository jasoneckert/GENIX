/*
 * tell.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)tell.c	1.1 (NSC) 9/5/82";
# endif

/*
 * return offset in file.
 */

long	lseek();

long tell(f)
{
	return(lseek(f, 0L, 1));
}
