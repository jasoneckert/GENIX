/*
 * getpwent.c: version 1.3 of 9/3/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getpwent.c	1.3 (NSC) 9/3/82";
# endif

#include <stdio.h>
#include <pwd.h>

static char XPASSWD[]	= "/etc/passwd";
static char EMPTY[] = "";
static FILE *pwf = NULL;
static char line[BUFSIZ+1];
static struct passwd passwd;

setpwent()
{
	if( pwf == NULL )
		pwf = fopen( XPASSWD, "r" );
	else
		rewind( pwf );
}

endpwent()
{
	if( pwf != NULL ){
		fclose( pwf );
		pwf = NULL;
	}
}

char *
pwskip(p)
register char *p;
{
	while( *p && *p != ':' )
		++p;
	if( *p ) *p++ = 0;
	return(p);
}

struct passwd *
getpwent()
{
	register char *p;

	if (pwf == NULL) {
		if( (pwf = fopen( XPASSWD, "r" )) == NULL )
			return(0);
	}
	p = fgets(line, BUFSIZ, pwf);
	if (p==NULL)
		return(0);
	passwd.pw_name = p;
	p = pwskip(p);
	passwd.pw_passwd = p;
	p = pwskip(p);
	passwd.pw_uid = atoi(p);
	p = pwskip(p);
	passwd.pw_gid = atoi(p);
	passwd.pw_quota = 0;
	passwd.pw_comment = EMPTY;
	p = pwskip(p);
	passwd.pw_gecos = p;
	p = pwskip(p);
	passwd.pw_dir = p;
	p = pwskip(p);
	passwd.pw_shell = p;
	while(*p && *p != '\n') p++;
	*p = '\0';
	return(&passwd);
}
