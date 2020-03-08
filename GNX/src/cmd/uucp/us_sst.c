/*
 * us_sst.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)us_sst.c	1.1 (NSC) 5/4/83";
# endif

#include "uucp.h"
#ifdef UUSTAT
#include <sys/types.h>
#include "uust.h"
 
/*********
 *	us_sst(flag)	record system status
 *	short flag;
 *
 *	This routine searches thru L_stat file
 *	using "rmtname" as the key.
 *	If the entry is found, then modify the system
 * 	status as indicated in "flag" and return.
 *
 *	return - 0 ok  | FAIL
 */
 
us_sst(flag)
short flag;
{
	struct us_ssf s;
	FILE *fp1, *fp2, *us_open();
	char buf[BUFSIZ];
	long time(), pos, ftell();
 
	DEBUG(16, " enter us_sst, status is : %02d\n", flag);
	fp1 = us_open(L_stat, "r", LCKLSTAT, 10, 1);
	fp2 = us_open(L_stat, "a", LCKLSTAT, 10, 1);
	if (fp1 == NULL || fp2 == NULL) return(FAIL);
 
	fseek(fp1, 0L, 0);	/* rewind */
	pos = 0L;
	while (fscanf(fp1, "%s", s.sysname) == 1) {
		DEBUG(16, "s.sysname : %s\n", s.sysname);
		if (strcmp(s.sysname, Rmtname) == SAME) {
			fseek(fp2,pos,0);		/* for stdio */
			fprintf(fp2, "%s", s.sysname);
			fprintf(fp2, "\t%09ld\t%02d\n", time((long *) 0), flag);
			goto out;
		}
		else  fgets(buf, BUFSIZ, fp1);	/* skip a line */
		pos = ftell(fp1);
	}
 
	/** system name is not known, create an entry **/
	fseek(fp2, 0L, 2);		/* for stdio */
	fprintf(fp2, "%s\t%09ld\t%02d\n", Rmtname, time((long *) 0), flag);
  out:	unlink(LCKLSTAT);
	fclose(fp1);
	fclose(fp2);
	return(0);
}
#endif
