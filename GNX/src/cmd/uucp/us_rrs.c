/*
 * us_rrs.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)us_rrs.c	1.1 (NSC) 5/4/83";
# endif

#include "uucp.h"
#ifdef UUSTAT
#include <sys/types.h>
#include "uust.h"
 
/*********
 *	us_rrs(cfilel, stat) 	record request status
 *	char *cfilel;	e.g. /usr/spool/uucp/C.mhtsvn1234
 *	short stat;	the "Usrf" flag.
 *
 *	We get the job number from a command file "cfile".
 *	using the jobn as the key to search thru "R_stat"
 *	file and modify the corresponding status as indicated
 *	in "stat".	"Stat" is defined in "uust.h".
 *	return - 0 ok  | FAIL
 */
 
us_rrs(cfilel,stat)
char *cfilel;
short stat;
{
	struct us_rsf u;
	FILE *fp1, *fp2, *us_open();
	long pos, ftell();
	short n;
	char cfile[20], *lastpart(), *name, buf[BUFSIZ];
	long time();
 
	strcpy(cfile, lastpart(cfilel));	/* strip path info */
	DEBUG(6, "\nenter us_rrs, cfile: %s", cfile);
	DEBUG(6, "  request status: %o\n", stat);
	name = cfile + strlen(cfile) - 4;  /* extract the last 4 digits */
	n = atoi(name);			/* convert to digits */
	fp1 = us_open(R_stat, "r", LCKRSTAT, 10, 1);
	fp2 = us_open(R_stat, "a", LCKRSTAT, 10, 1);
	if (fp1 == NULL || fp2 == NULL) return(FAIL);
	fseek(fp1, 0L, 0);	/* rewind */
	pos = 0L;
	while (fscanf(fp1, "%d%s%s%ld", &u.jobn,u.user, u.rmt,&u.qtime)!=NULL) {
		if (u.jobn == n) {
			DEBUG(6, " jobn : %d\n", u.jobn);
			fseek(fp2, pos, 0);	/* no effect: for printf */
			fprintf(fp2,"%04d\t%7.7s\t%7.7s\t",u.jobn,u.user,u.rmt);
			fprintf(fp2, "%09ld", u.qtime);
			fprintf(fp2, "\t%09ld\t%7o\n", time((long *) 0), stat);
			break;
		}
		if(fgets(buf, BUFSIZ, fp1)==NULL)	/* skip the line */
			break;
		pos = ftell(fp1);
	}
	fclose(fp1);
	fclose(fp2);
	unlink(LCKRSTAT);
	return(FAIL);
}
#endif
