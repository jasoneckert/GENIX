/*
 * logname.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)logname.c	1.1 (NSC) 11/3/82";
# endif

# include	"pwd.h"
# include	"sys/types.h"
# include	"macros.h"

char	*logname()
{
	struct passwd *getpwuid();
	struct passwd *log_name;
	int uid;

	uid = getuid();
	log_name = getpwuid(uid);
	endpwent();
	if (! log_name)
		return((char *)log_name);
	else
		return(log_name->pw_name);
}
