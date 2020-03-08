
/*
 * rs232.c: version 1.4 of 8/16/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)rs232.c	1.4 (NSC) 8/16/83";
# endif

#include <sys/types.h>
/* ripped off mikep's driver */

#define		CP_Usart	0xfffa00
#define 	CP_OptBuffer	0xfff400

#define BYTE	*(char *)&	/*Causes compiler to generate byte movs*/

struct dev2651 {			/* 2651 serial io device */
	u_char  data;			/* This depends on word alignment */
	u_short status;			/* of structure members.... */
	u_short mode;
	u_short cmd;
};

/*
 *	Signetics 2651 mode and command bits
 */

/* Mode register 1 bit definitions */
#define X01	0x1		/* 01x clock rate */
#define X16	0x2		/* 16x clock rate */
#define X64	0x3		/* 64x clock rate */
#define BITS8	0xc		/* 8 bits per char*/
#define BITS7	0x8		/* 7 bits per char*/
#define PENABLE	0x10		/* parity enable  */
#define	EPAR	0x20		/* even parity	  */
#define STOP1	0x40		/* 1 stop bit     */
#define STOP2	0xc0		/* 2 stop bits	  */

#define MODE1	X16 | BITS8 | STOP1 & ~PENABLE
#define MODE2	0		/* external clocking */

/* Command register bit definitions */
#define TxEN	0x1		/* transmitter enable */
#define DTR	0x2		/* assert data terminal ready */
#define RxEN	0x4		/* receiver enable */
#define BRK	0x8		/* force a "break" */
#define ERESET	0x10		/* reset error flags */
#define RTS	0x20		/* assert request to send */

#define CMD	TxEN | DTR | RxEN | ERESET | RTS	/* command reg */

/* Status register bit definitions */
#define TxRDY	0x1		/* transmitter ready */
#define RxRDY	0x2		/* receiver ready */
#define TxEMT	0x4		/* transmitter empty/dschng */
#define PE	0x8		/* parity error */
#define OE	0x10		/* overrun error */
#define FE	0x20		/* framing error */
#define DCD	0x40		/* data carrier detect */
#define DSR	0x80		/* data set ready */

#define ERROR	PE | OE | FE	/* error bit mask */

/* special characters */
#define dbell	7
#define bs	8
#define lf	10	
#define cr	13	
#define dc1 	0x11	
#define dc3 	0x13	

getchar()
{
register char thechar;
int echolf, echoc;
	echoc = echolf = *((short *) CP_OptBuffer);
	echolf &= 2;	/* option switch 6 on pack in upper corner */
	echoc &= 2;	/* option switch 6 on pack in upper corner */
	while (1) {
		thechar = sget();
		switch (thechar) {
		    default: if ((thechar >= ' ') && (echoc))
			     	sput(thechar);
			     return(thechar);
		    case lf: if (echolf) continue;
			     return(thechar);
		    case cr: if (echoc) sput(thechar);
			     if (echolf) sput(lf);
			     return(thechar);
		    case bs: if (echoc) {
				sput(bs);
				sput(' ');
				sput(bs);
			     }
			     return(thechar);
		}
	}
}

putchar(c)
char c;
{
int echolf;
        if (c == lf) {
		echolf = *((short *) CP_OptBuffer);
		echolf &= 2;	/* option switch 6 on pack in upper corner */
		if (echolf) sput(cr);
	}
	sput(c);
	return(0);
}

sget()
{
register struct dev2651 *dp = (struct dev2651 *) CP_Usart;
register char s;
char inchar;
	while (1) {
	    if ((s = dp->status) & RxRDY) {		/* a char is present */
		    inchar = dp->data;
		    if ((s&FE) || (s&OE)) {		/* framing error or */
			    inchar = 0;			/* overrun error */
			    dp->cmd |= ERESET;		/* reset errors */
		     } else 	inchar &= 0x7f;
		    return(inchar);
	    }
	}
}

sput(c)
char c;
{
register struct dev2651 *dp = (struct dev2651 *) CP_Usart;
register char s;
char inchar;
	while (1) {
	    if ((s = dp->status) & RxRDY) {		/* a char is present */
		    inchar = dp->data;
		    if ((s&FE) || (s&OE)) {		/* framing error or */
			    inchar = 0;		/* overrun error */
			    dp->cmd |= ERESET;		/* reset errors */
		    } else 	inchar &= 0x7f;
		    if (inchar == dc3) 		/* tandem mode */
			    while (sget() != dc1);
	    }
	    if ((dp->status) & TxRDY) {	/* a char can go out */
		    dp->data = c;
		    return(0);
	    }
	}
}

setxy(x,y) /* epascal leftovers */
{
}

stdio() /* epascal leftovers */
{
}

assert(x) /* epascal leftovers */
{
	if (!x) asm("bpt");
	else;
}

beep(x) /* epascal leftovers */
{
	sput(dbell);
}

checki() /* epascal check input */
{
register struct dev2651 *dp = (struct dev2651 *) CP_Usart;
	if ((dp->status) & RxRDY) return(1);
	return(0);
}

