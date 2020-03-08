/*
 * getpw.c: version 1.1 of 9/2/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getpw.c	1.1 (NSC) 9/2/82";
# endif

#include	<stdio.h>

getpw(uid, buf)
int uid;
char buf[];
{
	static FILE *pwf;
	register n, c;
	register char *bp;

	if(pwf == 0)
		pwf = fopen("/etc/passwd", "r");
	if(pwf == NULL)
		return(1);
	rewind(pwf);

	for (;;) {
		bp = buf;
		while((c=getc(pwf)) != '\n') {
			if(c == EOF)
				return(1);
			*bp++ = c;
		}
		*bp++ = '\0';
		bp = buf;
		n = 3;
		while(--n)
		while((c = *bp++) != ':')
			if(c == '\n')
				return(1);
		while((c = *bp++) != ':') {
			if(c<'0' || c>'9')
				continue;
			n = n*10+c-'0';
		}
		if(n == uid)
			return(0);
	}
}
