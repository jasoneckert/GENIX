/*
 * @(#)devvm.h	3.6	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * This module defines the virtual memory addresses used
 * to reference the device registers and RAMs.
 *
 * Because the device registers are located in widely separated parts
 * of physical memory, many page tables would be required to access
 * them all, if the kernel referred to them by their true physical
 * locations.  For this reason, the system page tables are setup
 * to remap most of the device registers into a single 64K segment,
 * just above the ROM monitor.
 *
 * WARNING:  All address are specified twice, once in a form suitable
 *           for assembly files (LOCORE conditional) and once in a form
 *           suitable for C files.  Be sure to make any changes in both
 *           places.
 *
 * The actual physical addresses are defined in module devpm.h.
 */

#ifdef LOCORE

#if NSIO > 0
; SIO board
SIO_BASE:	.equ	x'a00000
#endif

; Graphics board (screen and keyboard)
GB_VideoRAM:	.equ	x'c00000
GB_Video2RAM:	.equ	x'c20000
GB_WindowRAM:	.equ	x'c33400

; Rom monitor
PR_Monitor:	.equ	x'e00000  ;Needs PR_Pagelen pages

;Pages e02000 - e02600 are unused and available
GB_CRTC_Addr:	.equ	x'e02800
GB_CRTC_Data:	.equ	x'e02802
GB_IntMask:	.equ	x'e02a00
GB_IntStatus:	.equ	x'e02c00
GB_IntClear:	.equ	x'e02e00
GB_Control:	.equ	x'e03000
GB_Status:	.equ	x'e03000
GB_Tone:	.equ	x'e03200
GB_Keyboard:	.equ	x'e03400
GB_Joystick:	.equ	x'e03600
GB_CursDot:	.equ	x'e03a00
GB_CursRAM:	.equ	x'e03c00

; memory assignments for the disk port (byte addresses)
DCUA_STATUS:	.equ	x'e04008  ;immediate status (read only)
DCUA_CHAN0:	.equ	x'e04000  ;chan 0 controller bytes
DCUA_ACK0:	.equ	x'e04008  ;chan 0 interrupt acknowledge
DCUA_ERINT:	.equ	x'e04010  ;ERINT error status (on read)
DCUA_CHAN1:	.equ	x'e04020  ;chan 1 controller bytes
DCUA_ACK1:	.equ	x'e04028  ;chan 1 interrupt acknowledge
DCUA_SETDAFF:	.equ	x'e04002  ;set DAFF (write-enable drives)
DCUA_RESETDAFF:	.equ	x'e04004  ;reset DAFF (write-lock drives)

; memory assignments for the tape device (word addresses)
TCUA_STATUS:	.equ	x'e04200  ;immediate status (read only)
TCUA_CLRINHIBIT: .equ	x'e04202  ;clear inhibit tape action latch (read only)
TCUA_SETINHIBIT: .equ	x'e04204  ;set inhibit tape action latch (read only)
TCUA_CMMDFIRST:	.equ	x'e04200  ;write first command words
TCUA_CMMDLAST:	.equ	x'e04204  ;write last command word
TCUA_ACK:	.equ	x'e04208  ;interrupt acknowledge

; memory assignment for the temporary page (used for the memory device)
TEMPPAGE:	.equ	x'e04400  ;physical address set at runtime

; the U. area
BOTU:		.equ	x'e04800  ;Current user u. area (UPAGES long)
BOTXU:		.equ    x'e05000  ;Inactive user u. area (USIZE * NPROC long)

; Free pages after BOTXU + (USIZE * NPROC)

; CPU board
; Because these are used by the ROM monitor, the physical and
; virtual addresses must be identical.
;
CP_MasterICU:	.equ	x'fffe00
; CP_RemoteICU:	.equ	x'fffc00
CP_Usart:	.equ	x'fffa00
CP_PIO:		.equ	x'fff800
CP_Time:	.equ	x'fff600
CP_OptBuffer:	.equ	x'fff400
CP_ErrorReg:	.equ	x'fff200
CP_TestReg:	.equ	x'fff000
CP_DiagReg:	.equ	x'ffee00
; CP_IBTLsel:	.equ	x'ffec00
; CP_IBcontrol:	.equ	x'ffea00
; CP_Aux1:	.equ	x'ffe800
; CP_Aux2:	.equ	x'ffe600
; CP_Aux3:	.equ	x'ffe400
; CP_Aux4:	.equ	x'ffe200

#else	/* ! LOCORE */

#define BADR(n)	((unsigned char *)(n))	/* byte address recast */
#define WADR(n)	((unsigned short *)(n))	/* word address recast */

#if NSIO > 0
/*
 * SIO board:
 *	0x10000 bytes of address space consumed,
 *	but only first 0x1c02 need be mapped.
 */
#define SIO_BASE:	0xa00000
#endif NSIO

/* Graphics board (screen and keyboard) */
#define GB_VideoRAM	0xc00000
#define GB_Video2RAM	0xc20000
#define	GB_WindowRAM	0xc33400	/* 153 pages needed */

/* Rom monitor */
# define PR_Monitor	0xe00000
# define PR_Pagelen	16

/* Pages e02000 - e02600 are unused and available */
#define	GB_CRTC_Addr	WADR(0xe02800)
#define	GB_CRTC_Data	WADR(0xe02802)
#define	GB_IntMask	WADR(0xe02a00)
#define	GB_IntStatus	WADR(0xe02c00)
#define	GB_IntClear	WADR(0xe02e00)
#define	GB_Control	WADR(0xe03000)
#define	GB_Status	WADR(0xe03000)
#define	GB_Tone		WADR(0xe03200)
#define	GB_Keyboard	WADR(0xe03400)
#define	GB_Joystick	WADR(0xe03600)
#define	GB_CursDot	WADR(0xe03a00)
#define	GB_CursRAM	0xe03c00

/* memory assignments for the disk port (byte addresses) */
#define DCUA_STATUS	BADR(0xe04008)  /* immediate status (read only) */
#define	DCUA_CHAN0	BADR(0xe04000)  /* chan 0 controller bytes */
#define DCUA_ACK0	BADR(0xe04008)  /* chan 0 interrupt acknowledge */
#define DCUA_ERINT	BADR(0xe04010)  /* ERINT error status (on read) */
#define DCUA_CHAN1	BADR(0xe04020)  /* chan 1 controller bytes */
#define DCUA_ACK1	BADR(0xe04028)  /* chan 1 interrupt acknowledge */
#define	DCUA_SETDAFF	BADR(0xe04002)	/* set DAFF (write-enable drives) */
#define	DCUA_RESETDAFF	BADR(0xe04004)	/* reset DAFF (write-lock drives) */

/* memory assignments for the tape device (word addresses) */
#define	TCUA_STATUS	WADR(0xe04200)  /* immediate status (read only) */
#define	TCUA_CLRINHIBIT	WADR(0xe04202)  /* clear inhibit tape (read only) */
#define	TCUA_SETINHIBIT	WADR(0xe04204)  /* set inhibit tape (read only) */
#define	TCUA_CMMDFIRST	WADR(0xe04200)  /* write first command words */
#define	TCUA_CMMDLAST	WADR(0xe04204)  /* write last command word */
#define	TCUA_ACK	WADR(0xe04208)  /* interrupt acknowledge */

/* memory assignment for the temporary page (used for the memory device) */
#define	TEMPPAGE	0xe04400	/* physical address set at runtime */

/* the U. area */
#define BOTU		0xe04800	/* Current u. area (UPAGES long) */
#define BOTXU		0xe05000	/* Inactive user u. area */
					/* (USIZE * NPROC) long */
/* Free pages after BOTXU + (USIZE * NPROC) */

/* CPU board */
/* Because these are used by the ROM monitor, the physical and
 * virtual addresses must be identical.
 */
#define 	CP_MasterICU	0xfffe00
/* #define 	CP_RemoteICU	0xfffc00	*/
#define		CP_Usart	0xfffa00
#define 	CP_PIO		0xfff800
#define 	CP_Time		0xfff600
#define 	CP_OptBuffer	0xfff400
#define 	CP_ErrorReg	0xfff200
#define 	CP_TestReg	0xfff000
#define 	CP_DiagReg	0xffee00
/* #define 	CP_IBTLsel	0xffec00	*/
/* #define 	CP_IBcontrol	0xffea00	*/
/* #define 	CP_Aux1		0xffe800	*/
/* #define 	CP_Aux2		0xffe600	*/
/* #define 	CP_Aux3		0xffe400	*/
/* #define 	CP_Aux4		0xffe200	*/

#endif
