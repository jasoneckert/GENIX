/*  3200uart.2a.h -- Define UART/TIMER bits and structures for AIS/3200 Rev-2A
 *  copyright (c) American Information Systems Corporation
 *	November, 1984
 */
#ifndef UART3200
#define UART3200

#define UARTS 4

/* Define register address offsets for SCN2681 Dual UARTs */

/* Define Chip-oriented offsets */
#define U_INPCHG	0x10		/* read:  input port change */
#define U_AUXCTL	0x10		/* write: auxilliary control */
#define U_ISR		0x14		/* read:  interrupt status reg */
#define U_IMR		0x14		/* write: interrupt mask reg */
#define U_CNTHI		0x18		/* r/w:   counter high byte */
#define	U_CNTLO		0x1C		/* r/w:   counter low byte */
#define U_INPORT	0x34		/* read:  input pins */
#define U_OUTCFG	0x34		/* write: output configuration reg */
#define U_START_CTR	0x38		/* read:  read to start counter */
#define U_SETOUT	0x38		/* write: set output port pins */
#define U_STOP_CTR	0x3C		/* read:  read to stop counter */
#define U_CLROUT	0x3C		/* write: clear output port pins */

/* Define UART-oriented offsets */
#define U_MODE		0x00		/* r/w:   mode byte */
#define U_STAT		0x04		/* read:  status byte */
#define U_BAUD		0x04		/* write: set baud rate */
#define U_CMD		0x08		/* write: command byte */
#define U_ODATA		0x0C		/* write: output data byte */
#define U_IDATA		0x0C		/* read:  input data byte */


/* Define bits in chip-oriented registers */

/* input port change register U_INPCHG */
#define	U_IP0		0x01
#define	U_IP1		0x02
#define	U_IP2		0x04
#define	U_IP3		0x08
#define	U_IP0CHG	0x10
#define	U_IP1CHG	0x20
#define	U_IP2CHG	0x40
#define	U_IP3CHG	0x80

/* Auxilliary Control Register U_AUXCTL */
#define	U_IP0INT	0x01
#define	U_IP1INT	0x02
#define	U_IP2INT	0x04
#define	U_IP3INT	0x08
#define U_BAUDMODE	0x80

/* Interrupt Status Register U_ISR */
/* Interrupt Mask Register   U_IMR */
#define U_ATXRDYINT	0x01
#define U_ARXRDYINT	0x02
#define U_ABRKINT	0x04
#define U_CTRINT	0x08
#define U_BTXRDYINT	0x10
#define U_BRXRDYINT	0x20
#define U_BBRKINT	0x40
#define U_INPINT	0x80

/* Output Port Configuration Register U_OUTCFG */

	/* define this later */


/* Define bits in UART-oriented registers */

/* mode control register U_MODE */
/* First Write Access: */
#define U_5B		0x00
#define U_6B		0x01
#define U_7B		0x02
#define U_8B		0x03

#define U_EVEN		0x04
#define U_ODD		0x00

#define U_PARITY	0x00
#define U_NOPARITY	0x10

#define U_CHRERR	0x00
#define U_RXINT		0x00

#define U_RXRTSON	0x80
#define U_RXRTSOFF	0x00

/* Second Write Access: */
#define U_1STOP		0x07
#define U_15STOP	0x08
#define U_2STOP		0x0F

#define U_CTSON		0x10
#define U_CTSOFF	0x00

#define U_TXRTSON	0x20
#define U_TXRTSOFF	0x00

#define	U_NMODE		0x00
#define U_LMODE		0x80
#define U_RMODE		0xC0


/* Status bits U_STAT */
#define U_RXRDY		0x01
#define U_RXFULL	0x02
#define U_TXRDY		0x04
#define U_TXEMPTY	0x08
#define U_OVERRUN	0x10
#define U_PARERR	0x20
#define	U_FRAME		0x40
#define U_BREAK		0x80

/* redefine some of the above bits as bit offsets for tbit instruction */
#define U_B_RXRDY	0
#define U_B_RXFULL	1
#define	U_B_TXRDY	2
#define U_B_TXEMPTY	3
#define U_B_OVERRUN	4
#define U_B_PARERR	5
#define U_B_FRAME	6
#define U_B_BREAK	7


/* Baud rate register U_BAUD */
/* transmit / receive speeds */
#define	U_75		0x00
#define	U_110		0x01
#define	U_134		0x02
#define	U_150		0x03
#define	U_300		0x04
#define	U_600		0x05
#define	U_1200		0x06
#define	U_2000		0x07
#define	U_2400		0x08
#define	U_4800		0x09
#define	U_1800		0x0A
#define	U_9600		0x0B
#define	U_19200		0x0C
#define	U_TIMER		0x0D
#define	U_EXT16		0x0E
#define	U_EXT1		0x0F

/* transmit speeds */
#define	U_TX75		U_75
#define	U_TX110		U_110
#define	U_TX134		U_134
#define	U_TX150		U_150
#define	U_TX300		U_300
#define	U_TX600		U_600
#define	U_TX1200	U_1200
#define	U_TX2000	U_2000
#define	U_TX2400	U_2400
#define	U_TX4800	U_4800
#define	U_TX1800	U_1800
#define	U_TX9600	U_9600
#define	U_TX19200	U_19200
#define	U_TXTIMER	U_TIMER
#define	U_TXEXT16	U_EXT16
#define	U_TXEXT1	U_EXT1

/* receive speeds */
#define	U_RX75			0x00
#define	U_RX110			0x10
#define	U_RX134			0x20
#define	U_RX150			0x30
#define	U_RX300			0x40
#define	U_RX600			0x50
#define	U_RX1200		0x60
#define	U_RX2000		0x70
#define	U_RX2400		0x80
#define	U_RX4800		0x90
#define	U_RX1800		0xA0
#define	U_RX9600		0xB0
#define	U_RX19200		0xC0
#define	U_RXTIMER		0xD0
#define	U_RXEXT16		0xE0
#define	U_RXEXT1		0xF0


/* command byte U_CMD */
#define U_RXENABLE	0x01
#define U_RXDISABLE	0x02
#define U_TXENABLE	0x04
#define U_TXDISABLE	0x08
#define	U_MRRESET	0x10
#define	U_RXRESET	0x20
#define	U_TXRESET	0x30
#define U_ERRESET	0x40
#define	U_BRKRESET	0x50
#define	U_STARTBRK	0x60
#define	U_STOPBRK	0x70


/* If C-compilation, define uart structures */

#ifdef CSOURCE
typedef struct
	{
	byt	mode;
	byt	stat;
	byt	cmd;
	byt	odata;
	}  uart_controller;
#define baud stat
#define idata odata

typedef struct
	{
	uart_controller	uart0;
	byt		auxctl;
	byt		imr;
	byt		hctr;
	byt		lctr;
	uart_controller	uart1;
	byt		reserved;
	byt		outcfg;
	byt		setout;
	byt		clrout;
	}  duart_controller;
#define inpchg auxctl
#define isr imr
#define inport outcfg
#define startctr setout
#define stopctr clrout


#endif

#endif
