/*
 * %M%: version %I% of %G%
 * %Y%
 */
# ifdef SCCS
static char *sccsid = "%W% (NSC) %G%";
# endif

# include <stdio.h>
# include "dlvrmail.h"
# ifdef LOG
# include <log.h>
# endif LOG


/*
**  SYSERR -- Print error message.
**
**	Prints an error message via printf to the diagnostic
**	output.  If LOG is defined, it logs it also.
**
**	Parameters:
**		f -- the format string
**		a, b, c, d, e -- parameters
**
**	Returns:
**		-1 always
**
**	Side Effects:
**		increments Errors.
**		sets ExitStat.
*/

/*VARARGS1*/
syserr(fmt, a, b, c, d, e)
	char *fmt;
{
	extern int errno;
	static char errbuf[MAXLINE+1];
	register char *p;
	extern char *sys_errlist[];
	extern int sys_nerr;

	sprintf(errbuf, fmt, a, b, c, d, e);
	if (errno != 0)
	{
		p = &errbuf[strlen(errbuf)];
		if (errno < sys_nerr && errno > 0)
			sprintf(p, ": %s", sys_errlist[errno]);
		else
			sprintf(p, ": error %d", errno);
	}
	printf("delivermail: %s\n", errbuf);
	fflush(stdout);
	Errors++;

	/* determine exit status if not already set */
	if (ExitStat == EX_OK)
	{
		if (errno == 0)
			ExitStat = EX_SOFTWARE;
		else
			ExitStat = EX_OSERR;
	}

# ifdef LOG
	logmsg(LOG_ERR, "%s->%s: %s", From.q_paddr, To, errbuf);
# endif LOG
	errno = 0;
	return (-1);
}
/*
**  USRERR -- Signal user error.
**
**	This is much like syserr except it is for user errors.
**
**	Parameters:
**		fmt, a, b, c, d -- printf strings
**
**	Returns:
**		-1
**
**	Side Effects:
**		increments Errors.
*/

/*VARARGS1*/
usrerr(fmt, a, b, c, d, e)
	char *fmt;
{
	extern char SuprErrs;

	if (SuprErrs)
		return;

	Errors++;
	if (To != NULL)
		printf("%s... ", To);
	printf(fmt, a, b, c, d, e);
	printf("\n");
	fflush(stdout);
	return (-1);
}
