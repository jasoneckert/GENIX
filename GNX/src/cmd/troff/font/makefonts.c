/*
 * makefonts.c: version 1.1 of 5/11/83
 * Mesa Unix System Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)makefonts.c	1.1 (NSC) 5/11/83";
# endif

#include "stdio.h"

#include "ftB.c"
#include "ftBC.c"
#include "ftC.c"
#include "ftCE.c"
#include "ftCI.c"
#include "ftCK.c"
#include "ftCS.c"
#include "ftCW.c"
#include "ftG.c"
#include "ftGI.c"
#include "ftGM.c"
#include "ftGR.c"
#include "ftI.c"
#include "ftL.c"
#include "ftLI.c"
#include "ftPA.c"
#include "ftPB.c"
#include "ftPI.c"
#include "ftR.c"
#include "ftS.c"
#include "ftSB.c"
#include "ftSI.c"
#include "ftSM.c"
#include "ftUD.c"
#include "ftXM.c"

FILE *i;
#define mkfont(name)	i = fopen("name","w"); \
				fwrite(name,sizeof(name),1,i); \
				fclose(i);

main()
{	mkfont(ftB)
	mkfont(ftBC)
	mkfont(ftC)
	mkfont(ftCE)
	mkfont(ftCI)
	mkfont(ftCK)
	mkfont(ftCS)
	mkfont(ftCW)
	mkfont(ftG)
	mkfont(ftGI)
	mkfont(ftGM)
	mkfont(ftGR)
	mkfont(ftI)
	mkfont(ftL)
	mkfont(ftLI)
	mkfont(ftPA)
	mkfont(ftPB)
	mkfont(ftPI)
	mkfont(ftR)
	mkfont(ftS)
	mkfont(ftSB)
	mkfont(ftSI)
	mkfont(ftSM)
	mkfont(ftUD)
	mkfont(ftXM)
}
