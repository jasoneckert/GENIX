/*
 * xmsg.c: version 1.2 of 11/15/82
 * System III libPW Source
 */
# ifdef SCCS
static char *sccsid = "@(#)xmsg.c	1.2 (NSC) 11/15/82";
# endif

/*
	Call fatal with an appropriate error message
	based on errno.  If no good message can be made up, it makes
	up a simple message.
	The second argument is a pointer to the calling function's
	name (a string); it's used in the manufactured message.
*/

# include	"errno.h"
# include	"sys/types.h"
# include	"macros.h"

xmsg(file,func)
char *file, *func;
{
	register char *str, *svstr;
	int retcode;
	extern int errno;
	extern char Error[];

	svstr = 0;

	switch (errno) {
	case ENFILE:
		str = "no file (ut3)";
		break;
	case ENOENT:
		sprintf(str = Error,"`%s' nonexistent (ut4)",file);
		break;
	case EACCES:
		/* Rewrote to avoid use of alloca... */
		svstr = str = (char *) malloc (size (file));
		copy(file,str);
		file = str;
		sprintf(str = Error,"directory `%s' unwritable (ut2)",
			dname(file));
		break;
	case ENOSPC:
		str = "no space! (ut10)";
		break;
	case EFBIG:
		str = "write error (ut8)";
		break;
	default:
		sprintf(str = Error,"errno = %d, function = `%s' (ut11)",errno,
			func);
		break;
	}

	retcode = fatal (str);

	/* Explicitly free storage that was formerly freed implicitly: */
	if (svstr)
		free (svstr);

	return (retcode);
}
