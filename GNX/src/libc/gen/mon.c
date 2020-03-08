/*
 * mon.c: version 1.2 of 1/27/83
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)mon.c	1.2 (NSC) 1/27/83";
# endif

monitor(lowpc, highpc, buf, bufsiz, cntsiz)
char *lowpc, *highpc;
int *buf, bufsiz;
{
	register o;
	register n;
	static *sbuf, ssiz;
	struct phdr {
		int *lpc;
		int *hpc;
		int ncnt;
	};
	struct cnt {
		int *pc;
		long ncall;
	};

	if (lowpc <= 0) {
		profil(0, 0, 0, 0);
		if (lowpc == 0) o = creat("mon.out", 0666);
		else o = creat(mktemp("mon.outXXXXXX"), 0666);
		write(o, sbuf, ssiz);
		close(o);
		return;
	}
	sbuf = buf;
	ssiz = bufsiz;
	buf[0] = (int)lowpc;
	buf[1] = (int)highpc;
	buf[2] = cntsiz;
	o = sizeof(struct phdr) + cntsiz*sizeof(struct cnt);
	buf = (int *) (((int)buf) + o);
	bufsiz -= o;
	if (bufsiz<=0)
		return;
	o = ((highpc - lowpc)>>1);
	if(bufsiz < o)
		{ n = bufsiz;
		  asm("movd r6,r1");
		  asm("movqd 0,r0");
		  asm("deid  r7,r0");
		  asm("lshd -17,r1");
		  asm("movd r1,r7");
		  /* r6 is n, r7 is o */
		  /* o = ((float) bufsiz / o) * 32768; */
		}
	else
		o = 0177777;
	profil(buf, bufsiz, lowpc, o);
}
