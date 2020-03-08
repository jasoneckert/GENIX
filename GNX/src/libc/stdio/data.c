/*
 * data.c: version 1.2 of 9/3/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)data.c	1.2 (NSC) 9/3/82";
# endif

#include <stdio.h>
char	_sibuf[BUFSIZ];
char	_sobuf[BUFSIZ];

struct	_iobuf	_iob[_NFILE] ={
	{ 0, _sibuf, _sibuf, _IOREAD, 0},
	{ 0, NULL, NULL, _IOWRT, 1},
	{ 0, NULL, NULL, _IOWRT+_IONBF, 2},
};
/*
 * Ptr to end of buffers
 */
struct	_iobuf	*_lastbuf /*={ &_iob[_NFILE] } TEMPORARY COMPILER BUG */;
