/*
 * ub_sst.c: version 1.1 of 5/4/83
 * Uucp Source
 */
# ifdef SCCS
static char *sccsid = "@(#)ub_sst.c	1.1 (NSC) 5/4/83";
# endif

#include "uucp.h"
#ifdef UUSUB
#include <sys/types.h>
#include "uusub.h"
 
/*********
 *	ub_sst(flag)	record connection status
 *	short flag;
 *
 *	This routine searches thru L_sub file
 *	using "rmtname" as the key.
 *	If the entry is found, then modify the connection
 * 	status as indicated in "flag" and return.
 *
 *	return - 0 ok  | FAIL
 */
 
ub_sst(flag)
short flag;
{
	struct ub_l l;
	FILE *fp1, *fp2, *us_open();
	long pos, ftell();
 
	DEBUG(6, " enter ub_sst, status is : %d\n", flag);
	DEBUG(6,"Rmtname: %s\n", Rmtname);
	fp1 = us_open(L_sub, "r", LCKLSUB, 5, 1);
	fp2 = us_open(L_sub, "a", LCKLSUB, 5, 1);
	if (fp1 == NULL || fp2 == NULL) return(FAIL);
 
	fseek(fp1, 0L, 0);	/* rewind */
	pos = 0L;
	while (fread(&l, sizeof(l), 1, fp1) == 1)
	   {
		if (strcmp(l.sys, Rmtname) == SAME) {
		  switch(flag) {
			case ub_ok:	l.ok++;	 time(&l.oktime);  break;
			case ub_noacu:	l.noacu++;  break;
			case ub_login:	l.login++;  break;
			case ub_nack:	l.nack++;  break;
			default:	l.other++;  break;
		  }
			l.call++;
			DEBUG(6, "in ub_sst name=Rmtname: %s\n", l.sys);
			fseek(fp2, pos, 0);
			fwrite(&l, sizeof(l), 1, fp2);
			break;		/* go to exit */
		}
		pos = ftell(fp1);
	   }
	fclose(fp1);		/* exit point */
	fclose(fp2);		/* exit point */
	unlink(LCKLSUB);
	return(0);
}
#endif
