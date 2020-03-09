/*
 * @(#)icu.h	3.6	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/* Interrupt vector assignments */
#define TIMR		 1
#define DISK		 3
#define TAPE		 4
#define GPIB		 6
#define RX		 9
#define TX		10
#define GRAF		11
#define BPIO		12
#define APIO		13
#define SIO		14
#define SOFTCLOCK	15

/* processor priority for each interrupt */
#define	TIMRLEVEL	6
#define	DISKLEVEL	5
#define	TAPELEVEL	5
#define	GPIBLEVEL	4
#define	GRAFLEVEL	4
#define	RXLEVEL		4
#define	TXLEVEL		4
#define	BPIOLEVEL	4
#define	APIOLEVEL	4
#define	SIOLEVEL	4
#define SOFTLEVEL	1

#ifdef LOCORE

TIMRMASK:	.equ	(1 shl TIMR)
DISKMASK:	.equ	(1 shl DISK)
TAPEMASK:	.equ	(1 shl TAPE)
GPIBMASK:	.equ	(1 shl GPIB)
RXMASK:		.equ	(1 shl RX)
TXMASK:		.equ	(1 shl TX)
GRAFMASK:	.equ	(1 shl GRAF)
BPIOMASK:	.equ	(1 shl BPIO)
APIOMASK:	.equ	(1 shl APIO)
SIOMASK:	.equ	(1 shl SIO)
SOFTMASK:	.equ	(1 shl SOFTCLOCK)

;  ICU mode register definitions 
T16N8:	.equ	1
NTAR:	.equ	2
FRZ:	.equ	4
CLKM:	.equ	h'10
COUTM:	.equ	h'20
COUTD:	.equ	h'40
CFRZ:	.equ	h'80

;  ICU counter control definitions 
CDCRL:	.equ	1
CDCRH:	.equ	2
CRUNL:	.equ	4
CRUNH:	.equ	8
COUT0:	.equ	h'10
COUT1:	.equ	h'20
CFNPS:	.equ	h'40
CCON:	.equ	h'80

;  counter interrupt control 
WENL:	.equ	1
CIEL:	.equ	2
CIRL:	.equ	4
CERL:	.equ	8
WENH:	.equ	h'10
CIEH:	.equ	h'20
CIRH:	.equ	h'40
CERH:	.equ	h'80

;  set pending interrupt 
ISET:	.equ	h'80

;  ICU register offsets 
IHVCT:	.equ	h'00	;  hardware vector 
ISVCT:	.equ	h'02	;  software vector 
IELTG:	.equ	h'04	;  edge/level triger 
ITPL:	.equ	h'08	;  triggering polarity 
IPND:	.equ	h'0c	;  interrupts pending 
ISRV:	.equ	h'10	;  interrupts in service 
IMASK:	.equ	h'14	;  interrupt mask 
ICSRC:	.equ	h'18	;  cascaded source 
IFPRT:	.equ	h'1c	;  first priority 
IMCTL:	.equ	h'20	;  mode control 
IOCASN:	.equ	h'22	;  output clock assignment 
ICIPTR:	.equ	h'24	;  counter interrupt pointer 
IPDAT:	.equ	h'26	;  port data 
IPS:	.equ	h'28	;  interrupt/port select 
IPDIR:	.equ	h'2a	;  port direction 
ICCTL:	.equ	h'2c	;  counter control 
ICICTL:	.equ	h'2e	;  counter interrupt control 
ILCSV:	.equ	h'30	;  l-counter starting value 
IHCSV:	.equ	h'34	;  h-counter starting value 
ILCCV:	.equ	h'38	;  l-counter current value 
IHCCV:	.equ	h'3c	;  h-counter current value 

;  parameters for Master ICU 
MICURUPTS:	.equ	com(GRAFMASK or DISKMASK or TAPEMASK or SOFTMASK)
MICUBIAS:	.equ	h'10 	;  vector bias 
FREQ:		.equ	25000000
;  trigger all interrupts on level except disk and tape on edge 
ELTG:		.equ	com(BPIOMASK or APIOMASK or DISKMASK or TAPEMASK or TIMRMASK or TXMASK)
TPL:		.equ	(APIOMASK or GPIBMASK)

;;
;;	Processor level to in service mapping
;;

ISR_LEVEL0:	.equ	h'0000
ISR_LEVEL1:	.equ	h'8000
ISR_LEVEL2:	.equ	h'8000
ISR_LEVEL3:	.equ	h'8000
ISR_LEVEL4:	.equ	h'0040
ISR_LEVEL5:	.equ	h'0008
ISR_LEVEL6:	.equ	h'0002
ISR_LEVEL7:	.equ	h'0001

#else	/* ! LOCORE follows */

#define TIMRMASK	(1 << TIMR)
#define DISKMASK	(1 << DISK)	
#define TAPEMASK	(1 << TAPE)
#define GPIBMASK	(1 << GPIB)
#define RXMASK		(1 << RX)
#define TXMASK		(1 << TX)
#define GRAFMASK	(1 << GRAF)
#define BPIOMASK	(1 << BPIO)
#define APIOMASK	(1 << APIO)
#define SIOMASK		(1 << SIO)

/* ICU mode register definitions */
#define T16N8	0x01
#define NTAR	0x02
#define FRZ	0x04
#define CLKM	0x10
#define COUTM	0x20
#define COUTD	0x40
#define CFRZ	0x80

/* ICU counter control definitions */
#define CDCRL	0x01
#define CDCRH	0x02
#define CRUNL	0x04
#define CRUNH	0x08
#define COUT0	0x10
#define COUT1	0x20
#define CFNPS	0x40
#define	CCON	0x80

/* counter interrupt control */
#define WENL	0x01
#define	CIEL	0x02
#define	CIRL	0x04
#define	CERL	0x08
#define	WENH	0x10
#define	CIEH	0x20
#define	CIRH	0x40
#define	CERH	0x80

/* set pending interrupt */
#define ISET	0x80

/* ICU register offsets */
#define	IHVCT	0x00		/* hardware vector		*/
#define	ISVCT	0x02		/* software vector		*/
#define	IELTG	0x04		/* edge/level triger		*/
#define	ITPL	0x08		/* triggering polarity		*/
#define	IPND	0x0c		/* interrupts pending		*/
#define	ISRV	0x10		/* interrupts in service	*/
#define	IMASK	0x14		/* interrupt mask		*/
#define	ICSRC	0x18		/* cascaded source		*/
#define	IFPRT	0x1c		/* first priority		*/
#define	IMCTL	0x20		/* mode control			*/
#define	IOCASN	0x22		/* output clock assignment	*/
#define	ICIPTR	0x24		/* counter interrupt pointer	*/
#define	IPDAT	0x26		/* port data			*/
#define	IPS	0x28		/* interrupt/port select	*/
#define	IPDIR	0x2a		/* port direction		*/
#define	ICCTL	0x2c		/* counter control		*/
#define	ICICTL	0x2e		/* counter interrupt control	*/
#define	ILCSV	0x30		/* l-counter starting value	*/
#define	IHCSV	0x34		/* h-counter starting value	*/
#define	ILCCV	0x38		/* l-counter current value	*/
#define	IHCCV	0x3c		/* h-counter current value	*/

/* parameters for Master ICU */
#define MICURUPTS 	~(GRAFMASK | DISKMASK | TAPEMASK | SOFTMASK)
#define MICUBIAS	0x10 	/* vector bias */
#define FREQ		25000000
/* trigger all interrupts on level except disk and tape on edge */
#define	ELTG	~(BPIOMASK | APIOMASK | DISKMASK | TAPEMASK | TIMRMASK | TXMASK)
#define	TPL	(APIOMASK | BPIOMASK)

#ifdef KERNEL
extern int icuenable();
extern int icudisable();
#define setsoftclock() interrupt(SOFTCLOCK);
#endif KERNEL

#endif LOCORE
