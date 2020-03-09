/*
 * bootop.h: version 1.3 of 7/4/83
 * 
 *
 * @(#)bootop.h	1.3	(National Semiconductor)	7/4/83
 */


/*	Software readable switches */

/*	Two switch packs are on the CPU board. The switch setting can
	be determined by reading the word at address 0xfff400(also CP_OptBuffer
	as defined in devvm.h). The logical values represented by each
	switch is TRUE(1) or FALSE(0).  Two types of switch packs exist:
	Grayhill or CR.  On a Grayhill pack the position marked OPEN
	means the switch is set to TRUE(1). On a CR pack the position marked
	ON means the switch is set to FALSE(0). The Two packs are located
	on the boards in different spots, one is on the inside face of the
	board(we will call it the inside pack) and the other is on the edge
	of the board between the rs and gpib edge connectors(we will call it
	the outside pack).  The rest of this explains what each switch on each
	pack does.
*/
^L
/*	
	The INSIDE PACK
	switch 1 	These four switches set the refresh rate.
	switch 2 	Switch 1 is bit position zero(lsb), switch 4(msb).
	switch 3 	The settings should be for 4Mhz clock 12, 6Mhz clock
	switch 4 	8, and 10Mhz 2. These switches can NOT be read.

	switch 5	Tells Rom monitor how much memory to initialize
			to zero and where to set to interrupt stack pointer(sp0)
			False is for the first 256K bytes, SP equals 0x40000
			True is for 1.2Mbytes, SP equals 0x140000.
			Normally is True when system has EMB board.
			This switch appears as bit 0 at address 0xfff400.

	switch 6	Tells Rom monitor to echo characters typed to it over
			the rs port.
			False means no echo.
			True means echo(Normal setting)
			This switch appears as bit 1 at address 0xfff400.

	switch 7	This switch is currently not being used.
			This switch appears as bit 2 at address 0xfff400.

	switch 8	This switch sets the burst size for offboard bus
			cycles(DMA).
			False is burst size 8 words.
			True is burst size 16 words. This is normal case.
			This switch can NOT be read.

			bit position 3,4,5,6, and 7 are not connected to
			any switch position. Therefore the values at these
			positions are UNKNOWN.
*/
^L
/*	
	The OUTSIDE PACK
	This pack appears at the high byte at 0xfff400.
	switch 1 matches bit position 8, switch 8 matches position 15.

	switch 1	This switch is set to tell kernel that an FPU(16081)
			is available. Is bit position 8, True if have an FPU.
				
	switch 2	This switch is set to tell the kernel that the
			movsu instruction works.  If set to false, the
			kernel will not use that instruction.
			This switch appears as bit 9 at address 0xfff400.

	switch 3	This switch indicates whether the system is a
			SYS16 system or a workstation system.  If set, the
			system is a SYS16 so that the rs232 port will be
			the console.  If false, the bitmap display will
			be the console, and the Rom monitor will read the
			flavor keys for the boot option, and ignore switch 4.
			This switch appears as bit 10 at address 0xfff400.

	switch 4	This switch is set to tell Rom monitor on Reset or
			halt() to load boot program for system reboot of
			/vmunix.
			False does autoload and boot of /vmunix
			True means Rom monitor will read for commands.
			Normal set to True.
			This switch appears as bit 11 at address 0xfff400.

	switch 5	This switch is set to tell kernel on a Panic to
			either execute a Breakpoint into the Rom monitor
			for debugging or do a system dump and then halt().
			False is execute a bpt.
			True is doadump(), this should be normal case.
				
	switch 6 	These three switches set the GPIB address.
	switch 7 	Switch 6 is address bit position zero(lsb),
	switch 8 	switch 8(msb). The Rom monitor sets this address
			into GPIB Listen and Talk address registers for
			all GPIB communication.
*/
#ifdef LOCORE

EMB:		.equ	1
DOECHO:		.equ	2
HAVEFPU:	.equ	h'100
USEMOVSU:	.equ	h'200
GRAPHSYS:	.equ	h'400
AUTOBOOT:	.equ	h'800
DOADUMP:	.equ	h'1000
GPIB0:		.equ	h'2000
GPIB1:		.equ	h'4000
GPIB2:		.equ	h'8000

#else	/* ! LOCORE follows */

#define	EMB			0x1
#define	DOECHO			0x2
#define	HAVEFPU			0x100
#define	USEMOVSU		0x200
#define	GRAPHSYS		0x400
#define	AUTOBOOT		0x800
#define	DOADUMP			0x1000
#define	GPIB0			0x2000
#define	GPIB1			0x4000
#define	GPIB2			0x8000

#endif LOCORE
