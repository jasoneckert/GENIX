
/*
 * call.c: version 1.2 of 11/30/82
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)call.c	1.2 (NSC) 11/30/82";
# endif


/* perform a call to a user function */

#include <ctype.h>
#include <signal.h>
#include "main.h";
#include "symbol.h";
#include "parse.h";
#include "display.h";
#include "bpt.h";
#include "machine.h";

extern int	lastsig;

push(sptr,value)
int *sptr,value;
{
	*sptr -= 4;
	setdouble(*sptr,value);
}

docall()
{
int callpc,pcreturn,fakesp,savesp,modf,off,symatch;
int args,arg,i = 0;
char symfound[MAXSTRLEN];
	args = opcnt - 1;
	if (typeops[0] < 0)
		callpc = getreg(typeops[0]);
	else
		callpc = ops[0];
	symatch = lookbyval(callpc,&off,&modf,symfound);
	if ((symatch == FALSE) || (modf < 32) || (off != 0)) {
		printf("\r\ncannot call address\r\n");
		processabort();
	}
	savesp = fakesp = getreg(SSP);
	for (i = args; i > 0; i--) {
	    if (typeops[i] < 0)
		    arg = getreg(typeops[i]);
	    else
		    arg = ops[i];
	    push(&fakesp,arg);
	}
	push(&fakesp,getreg(SMOD));
	pcreturn = getreg(SPC);
	maketemp(pcreturn);
	push(&fakesp,pcreturn);
	setreg(SPC,callpc);
	setreg(SMOD,modf);
	setreg(SSP,fakesp);
	putinbpts(TEMP);
	proceed(GOGO,FALSE);
	outbpts(TEMP);
	if (pcreturn == getreg(SPC)) {
		if (lastsig == SIGBPT) lastsig = SIGTRAP;
		setreg(SSP,savesp);
	}
	showspot();
}
