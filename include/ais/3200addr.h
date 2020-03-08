/*  3200addr.2a.h -- Address Map for AIS/3200 Rev-2A CPU Board
 *  copyright (c) American Information Systems Corporation
 *
 *	Daniel Steinberg
 *	November, 1984
 */
#ifndef ADDR3200
#define ADDR3200

#define SYSMEM		0x00000000	/* system bus memory (15.5MB) */
#define	ENDMEM		0x00F80000	/* first address after local memory */

/* 
#define	ENDMEM		0x00800000	/* first address after 8MB
*/

#define SBUS_WINDOW	0x00F80000	/* Extended AIS/Bus window (64kB)*/
#define END_SBUS_WINDOW	0x00F90000	/* first address after S-Bus Window */

#define	HOST_WINDOW	0x00F90000	/* Start of Host Memory window (64kB)*/
#define	END_HOST_WINDOW	0x00FA0000	/* 1st addr after Host Memory window */

#define ROMSTART	0x00FA0000	/* local rom starts here (128kB) */

#define END_ROM		0x00FC0000	/* first address after ROM */
#define	ROM_EXPANSION	0x00FC0000	/* reserved (128kB)*/


#define	EXTIOP		0x00FE0000	/* external io page */
#define	SYSIOP		0x00FF0000	/* system bus i/o page (64kB) */

					/* MM????? time-of-day clock */
#define	TODCLK		0x00FF0003	/* time of day */

						/* 2681, DUART-A */
#define UARTA		0x00FFFF02	/* UART-Chip-A */
#define UART0		0x00FFFF02	/* UART 0 */
#define UART1		0x00FFFF22	/* UART 1 */

						/* 2681, DUART-B */
#define UARTB		0x00FFFF43	/* UART-Chip-B */
#define UART2		0x00FFFF43	/* UART 2 */
#define UART3		0x00FFFF63	/* UART 3 */


						/* Host Interrupt Initiator */
#define HOST_INT	0x00FFFF80	/* write: Host interrupt vector */

						/* Host CSR */
#define HOST_CSR	0x00FFFF80	/* read: CSR loaded by Host */

						/* Host Mapping Base Register */
#define HOST_MAP	0x00FFFF92	/* write: top 8-bits of host address */

						/* S-Bus Mapping Base Register */
#define SBUS_MAP	0x00FFFF90	/* write: top 16-bits of S-Bus address */

						/* 3210 Failure Register */
#define FAILURE		0x00FFFFA0	/* write: user # / read: error status */


						/* 16202 interrupt controller */
#define INTCTLR 	0x00FFFE00	/* NS16202 interrupt controller */
#define	INT_HVCT	0x00fffe00	/* hardware vector */
#define	INT_SVCT	0x00fffe04	/* software vector */
#define	INT_ELTG0	0x00fffe08	/* edge/level triggering */
#define	INT_ELTG1	0x00fffe0c	/* edge/level triggering */
#define	INT_TPL0	0x00fffe10	/* triggering polarity */
#define	INT_TPL1	0x00fffe14	/* triggering polarity */
#define	INT_IPND0	0x00fffe18	/* interrupts pending */
#define	INT_IPND1	0x00fffe1c	/* interrupts pending */
#define	INT_ISRV0	0x00fffe20	/* interrupts in-service */
#define	INT_ISRV1	0x00fffe24	/* interrupts in-service */
#define	INT_IMSK0	0x00fffe28	/* interrupt mask */
#define	INT_IMSK1	0x00fffe2c	/* interrupt mask */
#define	INT_CSRC0	0x00fffe30	/* cascaded source */
#define	INT_CSRC1	0x00fffe34	/* cascaded source */
#define	INT_FPRT0	0x00fffe38	/* first priority */
#define	INT_FPRT1	0x00fffe3c	/* first priority */
#define	INT_MCTL	0x00fffe40	/* mode control */
#define	INT_OCASN	0x00fffe44	/* ouput clock assignment */
#define	INT_CIPTR	0x00fffe48	/* counter interrupt pointer */
#define	INT_PDAT	0x00fffe4c	/* port data */
#define	INT_IPS		0x00fffe50	/* interrupt/port select */
#define	INT_PDIR	0x00fffe54	/* port direction */
#define	INT_CCTL	0x00fffe58	/* counter control */
#define	INT_CICTL	0x00fffe5c	/* counter interrupt control */
#define	INT_LCSV0	0x00fffe60	/* L-counter starting value */
#define	INT_LCSV1	0x00fffe64	/* L-counter starting value */
#define	INT_HCSV0	0x00fffe68	/* H-counter starting value */
#define	INT_HCSV1	0x00fffe6c	/* H-counter starting value */
#define	INT_LCCV0	0x00fffe70	/* L-counter current value */
#define	INT_LCCV1	0x00fffe74	/* L-counter current value */
#define	INT_HCCV0	0x00fffe78	/* H-counter current value */
#define	INT_HCCV1	0x00fffe7c	/* H-counter current value */


#endif
