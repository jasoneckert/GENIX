/*
 * stty.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)stty.c	1.1 (NSC) 9/5/82";
# endif

/*
 * Writearound to old stty and gtty system calls
 */

#include <sgtty.h>

stty(fd, ap)
struct sgtty *ap;
{
	return(ioctl(fd, TIOCSETP, ap));
}

gtty(fd, ap)
struct sgtty *ap;
{
	return(ioctl(fd, TIOCGETP, ap));
}
