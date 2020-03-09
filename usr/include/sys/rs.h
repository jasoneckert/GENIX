/*
 * @(#)rs.h	1.2	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */


/*************************************************************************
**									**
**		Signetics 2651 serial io device				**
**									**
**************************************************************************/

struct dev2651 {			/* 2651 serial io device */
	u_short data;
	u_short status;
	u_short mode;
	u_short cmd;
};

#define RSDEV struct dev2651

/*
 *	Mode and command bits
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

#define CMD	TxEN | DTR | RxEN | ERESET | RTS /* default command reg */

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

