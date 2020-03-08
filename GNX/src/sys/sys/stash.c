# ifndef NOSCCS
static char *sccsid = "@(#)stash.c	3.1	8/8/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

/* stash.c -- Kernel debugging aid
 *
 * This module defines a routine stash() which may be called from elsewhere
 * in the kernel to store information about the kernel's state.
 * The routine stashprint() may be called at a later time (say, from
 * ddt) to print out the information saved by stash().  This is useful for
 * tracking down kernel problems for which the panic occurs long after
 * the kernel takes the wrong action.
 *
 * Because stash() does not print out any information at the time it is
 * called, but simply stores it to be printed later, it is particularly
 * useful for debugging interrupt- or timing-related problems.  Often such
 * bugs cannot be debugging by inserting printf's into the kernel, because
 * the call to printf consumes too much real time, and the change in timing
 * causes the bug to disappear.  Calls to stash() are less likely to cause
 * such problems.
 *
 * To use this routine, simply load this module into the kernel and add
 * calls to stash() where desired.  The last STASHNUM entries will be kept.
 */


#define STASHNUM	100
#define	ARGS	5

struct	stashrec	{
	char	*code;
	long	args[ARGS];
};

struct	stashrec	stashbuf[STASHNUM];
struct	stashrec	*stashptr = &stashbuf[0];
int	stashwrap = 0;

/*
 * stash -- Save kernel debugging information for printing later
 *
 * Example call:
 *	stash("printf-type string, data1 = %x, data2 = %d",data1,data2);
 *
 * Up to ARGS number of data arguments may be specified.
 */
stash(code, args)
	char	*code;
	long	args;
{
	int	i;

	stashptr->code = code;
	for (i = 0; i < ARGS; i++) stashptr->args[i] = (&args)[i];
	if (++stashptr >= &stashbuf[STASHNUM]) {
		stashwrap = -1;
		stashptr = &stashbuf[0];
	}
}



/*
 * Dump the last STASHNUM stash entries and breakpoint.
 * This routine may be called directly from the panic code, or may be
 * called from ddt a la
 *	stashprint$c
 */
stashprint()
{
	struct	stashrec	*prptr;

	if (stashwrap) {
		prptr = stashptr;
		while (prptr < &stashbuf[STASHNUM]) {
			prf(prptr->code, prptr->args, 0);
			printf("\n");
			prptr++;
		}
	}
	prptr = &stashbuf[0];
	while (prptr < stashptr) {
		prf(prptr->code, prptr->args, 0);
		printf("\n");
		prptr++;
	}
	printf("stashprint: done\n");
	asm("bpt");
}
