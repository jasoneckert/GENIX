# ifndef NOSCCS
static char *sccsid = "@(#)mmerr.c	1.1	7/12/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

typedef unsigned field;

struct msr {
    field ate:1;
    field xx1:1;
    field brk:1;
    field plv:1;
    field il1:1;
    field il2:1;
    field bn :1;
    field xx2:1;
    field ed :1;
    field bd :1;
    field est:3;
    field bst:3;
    field tu :1;
    field ts :1;
    field ds :1;
    field ao :1;
    field ben:1;
    field ub :1;
    field ai :1;
    field ft :1;
    field ut :1;
    field nt :1;
    field res:6;
};

static struct msr msr;

char *busStatus[] ={
	"sequential instruction fetch",
	"non-sequential instruction fetch",
	"data transfer",
	"read RMW operand",
	"read for effective address",
	"transfer slave operand",
	"read slave status word",
	"broadcast slave id",
};

main(argc,argv)
int argc;
char **argv;
{
	if (argc < 2) {
		printf("Usage: mmerr hex-status\n");
		exit(1);
	}
	++argv;
	sscanf(*argv,"%x",&msr);
	modes();
	if (msr.ate)
		translation();
	else
		breakpt();
}

modes()
{
	static char *xlate = "translated",
		    *noxlate = "untranslated",
		    *en = "en",
		    *dis = "dis";
	char *space, *tu, *ts, *be;

	printf("Translation mode:\n");
	space = msr.ds ? "Dual" : "Single";
	ts = msr.ts ? xlate : noxlate;
	tu = msr.tu ? xlate : noxlate;
	printf("%s space, %s supervisor, %s user\n",space,ts,tu);
	be = msr.ao ? en : dis;
	printf("Access override %sabled\n",be);
	printf("%s on breaks\n",msr.ai?"Interrupt":"Abort");
	be = msr.ben ? en : dis;
	tu = msr.ub  ? "user": "both";
	printf("Breakpoint %sabled for %s\n",be,tu);
	be = msr.ft ? en : dis;
	tu = msr.ut ? "user": "both";
	printf("Flow trace %sabled for %s\n",be,tu);
	be = msr.nt ? en : dis;
	printf("Nonsequential trace %sabled\n",be);
}

translation()
{

	static char *badpte = "invalid level %c pte\n";

	printf("\nData %s error during %s\n",
		msr.ed?"read":"write",busStatus[msr.est]);
	if(msr.plv) {
		printf("protection violation\n");
	}
	if (msr.il1) {
		printf(badpte,'1');
	}
	if (msr.il2) {
		printf(badpte,'2');
	}
}

breakpt()
{
	char *bd;

	if ((msr.nt == 1) && (msr.brk == 0)) {
		printf("\nNonsequential trace trap\n");
		return;
	}
	bd = msr.bd ? "Read" : "Write";
	printf("\n%s breakpoint %d during %s\n",
		bd,msr.bn,busStatus[msr.bst]);
}
