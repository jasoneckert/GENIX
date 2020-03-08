/*
 * @(#)devpm.h	3.3	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/* Virtual to physical address mappings for devices */

/* This file defines the physical memory addresses which correspond to
 * the virtual addresses assigned to the various device registers and RAM
 * areas defined in devvm.h.  The physical memory addresses must correspond
 * to the actual locations defined by the hardware.
 *
 * Certain areas accessed by the ROM monitor must be mapped so that virtual
 * and physical addresses are the same.  These include the ROM monitor itself,
 * the graphics screens, and the CPU registers.
 *
 * Keep the spytable structure up to date in vmsys.c.  This table contains
 * physical memory addresses which are accessable to unprivileged users.
 */

#include "devvm.h"

struct devmap{
	int	virtaddr;	/* virtual address */
	int	physaddr;	/* corresponding physical address */
	int	pagelen;	/* length in pages */
};

#ifdef KERNEL
extern struct devmap devmap[];
#endif
