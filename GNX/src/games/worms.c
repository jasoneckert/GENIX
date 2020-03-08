/*
 * worms.c: version 1.7 of 3/11/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)worms.c	1.7 (NSC) 3/11/83";
# endif

/*

	 @@@        @@@    @@@@@@@@@@     @@@@@@@@@@@    @@@@@@@@@@@@
	 @@@        @@@   @@@@@@@@@@@@    @@@@@@@@@@@@   @@@@@@@@@@@@@
	 @@@        @@@  @@@@      @@@@   @@@@           @@@@ @@@  @@@@
	 @@@   @@   @@@  @@@        @@@   @@@            @@@  @@@   @@@
	 @@@  @@@@  @@@  @@@        @@@   @@@            @@@  @@@   @@@
	 @@@@ @@@@ @@@@  @@@        @@@   @@@            @@@  @@@   @@@
	  @@@@@@@@@@@@   @@@@      @@@@   @@@            @@@  @@@   @@@
	   @@@@  @@@@     @@@@@@@@@@@@    @@@            @@@  @@@   @@@
	    @@    @@       @@@@@@@@@@     @@@            @@@  @@@   @@@

				 Eric P. Scott
			  Caltech High Energy Physics
				 October, 1980

Modified by Peter Bishop, National Semiconductor, July, 1982.
*/

#include <stdio.h>
#include <sgtty.h>
#define cursor(col,row) tputs(tgoto(CM,col,row),1,fputchar)

extern char *UP;
extern short ospeed;
int Wrap;
short *ref[70];
static char flavor[][2]={
    "O", "*", "#", "$", "%", "0"
};
static short xinc[]={
     1,  1,  1,  0, -1, -1, -1,  0
}, yinc[]={
    -1,  0,  1,  1,  1,  0, -1, -1
};
static struct worm {
    int orientation, head;
    short *xpos, *ypos;
    char *wormstring, *curchar;
} worm[40];
static char *field;
static char sawseed;
static int length=16, number=3, trail=' ', seed;
static struct options {
    int nopts;
    int opts[3];
} normal[8]={
    { 3, { 7, 0, 1 } },
    { 3, { 0, 1, 2 } },
    { 3, { 1, 2, 3 } },
    { 3, { 2, 3, 4 } },
    { 3, { 3, 4, 5 } },
    { 3, { 4, 5, 6 } },
    { 3, { 5, 6, 7 } },
    { 3, { 6, 7, 0 } }
}, upper[8]={
    { 1, { 1, 0, 0 } },
    { 2, { 1, 2, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 2, { 4, 5, 0 } },
    { 1, { 5, 0, 0 } },
    { 2, { 1, 5, 0 } }
}, left[8]={
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 2, { 2, 3, 0 } },
    { 1, { 3, 0, 0 } },
    { 2, { 3, 7, 0 } },
    { 1, { 7, 0, 0 } },
    { 2, { 7, 0, 0 } }
}, right[8]={
    { 1, { 7, 0, 0 } },
    { 2, { 3, 7, 0 } },
    { 1, { 3, 0, 0 } },
    { 2, { 3, 4, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 2, { 6, 7, 0 } }
}, lower[8]={
    { 0, { 0, 0, 0 } },
    { 2, { 0, 1, 0 } },
    { 1, { 1, 0, 0 } },
    { 2, { 1, 5, 0 } },
    { 1, { 5, 0, 0 } },
    { 2, { 5, 6, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } }
}, upleft[8]={
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 1, { 3, 0, 0 } },
    { 2, { 1, 3, 0 } },
    { 1, { 1, 0, 0 } }
}, upright[8]={
    { 2, { 3, 5, 0 } },
    { 1, { 3, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 1, { 5, 0, 0 } }
}, lowleft[8]={
    { 3, { 7, 0, 1 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 1, { 1, 0, 0 } },
    { 2, { 1, 7, 0 } },
    { 1, { 7, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } }
}, lowright[8]={
    { 0, { 0, 0, 0 } },
    { 1, { 7, 0, 0 } },
    { 2, { 5, 7, 0 } },
    { 1, { 5, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } },
    { 0, { 0, 0, 0 } }
};

main(argc,argv)
int argc;
char *argv[];
{
    extern fputchar();
    char *malloc();
    char *getenv();
    char *tgetstr(), *tgoto();
    float ranf();
    register int x, y;
    register int n;
    register struct worm *w;
    register struct options *op;
    register int h;
    register short *ip;
    char *AL, *BC, *CM, *EI, *HO, *IC, *IM, *IP, *SR;
    int CO, IN, LI, last, bottom, wormno;
    char *tcp;
    register char *term;
    char tcb[100];
    struct sgttyb sg;

    setbuf(stdout,malloc(BUFSIZ));
    wormno=0;
    for (x=0;x<40;x++) {
	tcp=flavor[x%6];
	worm[x].wormstring=tcp;
	worm[x].curchar=tcp;
    };
    for (x=1;x<argc;x++) {
	register char *p;
	p=argv[x];
	if (*p=='-') p++;
	switch (*p) {
	case 'f':
	    field="WORM";
	    break;
	case 'l':
	    if (++x==argc) goto usage;
	    if ((length=atoi(argv[x]))<2||length>1024) {
		fprintf(stderr,"%s: Invalid length\n",*argv);
		exit(1);
	    }
	    break;
	case 'n':
	    if (++x==argc) goto usage;
	    if ((number=atoi(argv[x]))<1||number>40) {
		fprintf(stderr,"%s: Invalid number of worms\n",*argv);
		exit(1);
	    }
	    break;
	case 's':
	    if (++x==argc) goto usage;
	    seed=atoi(argv[x]);
	    sawseed=1;
	    break;
	case 't':
	    trail='.';
	    break;
	case 'w':
	    if (++x==argc) goto usage;
	    tcp=argv[x];
	    worm[wormno].wormstring=tcp;
	    worm[wormno].curchar=tcp;
	    if (++wormno >= number) wormno=0;
	    break;
	default:
	usage:
	    fprintf(stderr,
		"usage: %s [-field] [-length #] [-number #] [-trail]\n",*argv);
	    exit(1);
	    break;
	}
    }
    if (sawseed) srand(seed); else srand(time(0));
    if (!(term=getenv("TERM"))) {
	fprintf(stderr,"%s: TERM: parameter not set\n",*argv);
	exit(1);
    }
    if (tgetent(malloc(1024),term)<=0) {
	fprintf(stderr,"%s: %s: unknown terminal type\n",*argv,term);
	exit(1);
    }
    tcp=tcb;
    if (!(CM=tgetstr("cm",&tcp))) {
	fprintf(stderr,"%s: terminal not capable of cursor motion\n",*argv);
	exit(1);
    }
    AL=tgetstr("al",&tcp);
    BC=tgetflag("bs") ? "\b" : tgetstr("bc",&tcp);
    if ((CO=tgetnum("co"))<=0) CO=80;
    last=CO-1;
    EI=tgetstr("ei",&tcp);
    HO=tgetstr("ho",&tcp);
    IC=tgetstr("ic",&tcp);
    IM=tgetstr("im",&tcp);
    IN=tgetflag("in");
    IP=tgetstr("ip",&tcp);
    if ((LI=tgetnum("li"))<=0) LI=24;
    bottom=LI-1;
    SR=tgetstr("sr",&tcp);
    UP=tgetstr("up",&tcp);
    gtty(fileno(stdout),&sg);
    ospeed=sg.sg_ospeed;
    Wrap=tgetflag("am");
    ip=(short *)malloc(LI*CO*sizeof (short));
    for (n=0;n<LI;) {
	ref[n++]=ip; ip+=CO;
    }
    for (ip=ref[0],n=LI*CO;--n>=0;) *ip++=0;
    if (Wrap) ref[bottom][last]=1;
    for (n=number, w= &worm[0];--n>=0;w++) {
	w->orientation=w->head=0;
	if (!(ip=(short *)malloc(length*sizeof (short)))) {
	    fprintf(stderr,"%s: out of memory\n",*argv);
	    exit(1);
	}
	w->xpos=ip;
	for (x=length;--x>=0;) *ip++ = -1;
	if (!(ip=(short *)malloc(length*sizeof (short)))) {
	    fprintf(stderr,"%s: out of memory\n",*argv);
	    exit(1);
	}
	w->ypos=ip;
	for (y=length;--y>=0;) *ip++ = -1;
    }
    tputs(tgetstr("cl",&tcp),1,fputchar);
    if (field) {
	register char *p;
	p=field;
	for (y=bottom;--y>=0;) {
	    for (x=CO;--x>=0;) {
		putchar(*p++);
		if (!*p) p=field;
	    }
            if (!Wrap) putchar('\n');
            fflush(stdout);
        }
	if (Wrap) {
	    if (IM&&!IN) {
		for (x=last;--x>0;) {
		    putchar(*p++);
		    if (!*p) p=field;
		}
		y= *p++; if (!*p) p=field;
		putchar(*p);
		if (BC) tputs(BC,1,fputchar);
		else cursor(last-1,bottom);
		tputs(IM,1,fputchar);
		if (IC) tputs(IC,1,fputchar);
		putchar(y);
		if (IP) tputs(IP,1,fputchar);
		tputs(EI,1,fputchar);
	    }
	    else if (SR||AL) {
		if (HO) tputs(HO,1,fputchar);
		else cursor(0,0);
		if (SR) tputs(SR,1,fputchar);
		else tputs(AL,LI,fputchar);
		for (x=CO;--x>=0;) {
		    putchar(*p++);
		    if (!*p) p=field;
		}
	    }
	    else for (x=last;--x>=0;) {
		    putchar(*p++);
		    if (!*p) p=field;
	    }
	}
	else for (x=CO;--x>=0;) {
	    putchar(*p++);
	    if (!*p) p=field;
	}
    }
    fflush(stdout);
    for (;;) {
	for (n=0,w= &worm[0];n<number;n++,w++) {
	    if ((x=w->xpos[h=w->head])<0) {
		cursor(x=w->xpos[h]=0,y=w->ypos[h]=bottom);
		if (w->curchar[0]==0) w->curchar=w->wormstring;
		putchar(w->curchar++[0]);
		ref[y][x]++;
	    }
	    else y=w->ypos[h];
	    if (++h==length) h=0;
	    if (w->xpos[w->head=h]>=0) {
		register int x1, y1;
		x1=w->xpos[h]; y1=w->ypos[h];
		if (--ref[y1][x1]==0) {
		    cursor(x1,y1); putchar(trail);
		}
	    }
            op= &(x==0 ? (y==0 ? upleft : (y==bottom ? lowleft : left)) :
                (x==last ? (y==0 ? upright : (y==bottom ? lowright : right)) :
		(y==0 ? upper : (y==bottom ? lower : normal))))[w->orientation];
	    switch (op->nopts) {
	    case 0:
		fflush(stdout);
		abort();
		return;
	    case 1:
		w->orientation=op->opts[0];
		break;
	    default:
		w->orientation=op->opts[rani(op->nopts)];
	    }
	    cursor(x+=xinc[w->orientation], y+=yinc[w->orientation]);

	    if (!Wrap||x!=last||y!=bottom) {
		if (w->curchar[0]==0) w->curchar=w->wormstring;
		putchar(w->curchar++[0]);
	    };
	    ref[w->ypos[h]=y][w->xpos[h]=x]++;
	}
	fflush(stdout);
    }
}

fputchar(c)
char c;
{
    putchar(c);
}

rani(n)
int n;
{
    return(rand()%n);
}
