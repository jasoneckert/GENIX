/*
 * sleep.c: version 1.1 of 9/5/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)sleep.c	1.1 (NSC) 9/5/82";
# endif

#include <signal.h>
#include <setjmp.h>

static jmp_buf jmp;

sleep(n)
unsigned n;
{
	int sleepxxx();
	unsigned altime;
	int (*alsig)() = SIG_DFL;

	if (n==0)
		return;
	altime = alarm(1000);	/* time to maneuver */
	if (setjmp(jmp)) {
		signal(SIGALRM, alsig);
		alarm(altime);
		return;
	}
	if (altime) {
		if (altime > n)
			altime -= n;
		else {
			n = altime;
			altime = 1;
		}
	}
	alsig = signal(SIGALRM, sleepxxx);
	alarm(n);
	for(;;)
		pause();
	/*NOTREACHED*/
}

sleepxxx()
{
	longjmp(jmp, 1);
}
