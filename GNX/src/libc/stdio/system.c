/*
 * system.c: version 1.3 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)system.c	1.3 (NSC) 9/5/82";
# endif

#include	<signal.h>

system(s)
char *s;
{
	int status, pid, w;
	register int (*istat)(), (*qstat)();

	pid = vfork();
	if (pid < 0) {
		return(127);
	}
	if (pid == 0) {
		execl("/bin/sh", "sh", "-c", s, 0);
		_exit(127);
	}
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	return(status);
}
