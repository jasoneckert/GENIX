/* @(#)hmem.c	6.1 */
/*
 *	Host Memory special file
 *	major device 7, minor device 0 is host memory at a pre-defined address
 */

/*
	*** This driver is intended as a template only for ***
	*** generalized Host Memory access and should be   ***
	*** tailored to your specific application.         ***
	*** This driver is supplied unsupported by AIS.    ***
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/proc.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/buf.h"
#include "sys/page.h"

#include "ais/viostatus.h"	/* vios definitions */
#include "ais/osiopkt.h"
#include "ais/viocmds.h"

/* virtual hostmem device structure */
struct hmdev {
	PKT_IOSTATUS	hm_stat;	/* status from vios goes here */
	unsigned int	hm_VDD;		/* VDD for virtual tape unit */
	/* vios packet number for killing current request */
	unsigned int	hm_kill;
} hmdev;

#define HMEMSTART 0600000	/* host memory start address (octal) */
#define HMEMLEN	  0x1000	/* 4K */

hmopen(dev, flag)
{
	register tppdd;

	if (dev != 0) {
		u.u_error = ENXIO;
		return;
	}

	/* create PD for the device AIS:HOSTMEM */
	vios_iorequest( FCODE(PCTRL_FUNCTION, CREATEPD,
				(LOOKUP_PD | (flag? READ_ONLY : WRITE_ACCESS))),
			NULL, 0, 0, 0,
			"AIS:HOSTMEM", BUF_ADDR, HMEMSTART, HMEMLEN, 0,
			&hmdev.hm_stat, NULL, 0, 0);
	while(hmdev.hm_stat.code == 0)
		nulldev();
	if (hmdev.hm_stat.code < 0) {
		u.u_error = ENXIO;
		return;
	}
	tppdd = hmdev.hm_stat.aux_stat2;

	/* create VD for the device hmem */
	vios_iorequest(FCODE(VCTRL_FUNCTION,CREATEVD, 0),
			NULL, 0, 0, 0,
			"HMEM", BUF_ADDR, 0 , 0 , 0,
			&hmdev.hm_stat, NULL, 0, 0);
	while(hmdev.hm_stat.code == 0)
		nulldev();
	if (hmdev.hm_stat.code < 0) {
		u.u_error = ENXIO;
		return;
	}
	hmdev.hm_VDD = hmdev.hm_stat.aux_stat2;

	/* assign VD to PD */
	vios_iorequest( FCODE(VCTRL_FUNCTION, ASSIGNVD, (PRI_INP | PRI_OUT)),
			hmdev.hm_VDD, 0, 0, 0,
			NULL, tppdd, 0, 0, 0,
			&hmdev.hm_stat, NULL, 0, 0);
	while(hmdev.hm_stat.code == 0)
		nulldev();
	if (hmdev.hm_stat.code < 0) {
		u.u_error = ENXIO;
		return;
	}

}

hmread(dev)
{
	if (dev != 0 || hmdev.hm_VDD == NULL) {
		u.u_error = ENXIO;
		return;
	}

	if (u.u_count == 0 || (useracc(u.u_base, u.u_count, B_READ) == NULL)) {
		u.u_error = EFAULT;
		return;
	}

	/* call VIOS to do the read */
	hmdev.hm_kill = vios_iorequest(
		FCODE(READ_FUNCTION, READ_DATA, 0),
		hmdev.hm_VDD,
		u.u_base, u.u_count, 
		ptob(u.u_procp->p_addr),
		NULL, u.u_offset, 0 , 0, 0,
		&hmdev.hm_stat, NULL, 0, 0);
	while(hmdev.hm_stat.code == 0)
		nulldev();
	if (hmdev.hm_stat.code < 0) {
		u.u_error = EINVAL;
		return;
	}

	u.u_offset += u.u_count;
	u.u_base += u.u_count;
	u.u_count = 0;
}

hmwrite(dev)
{
	if (dev != 0 || hmdev.hm_VDD == NULL) {
		u.u_error = ENXIO;
		return;
	}

	/* check for valid args and accessibility of user address */
	if (u.u_count == 0 || (useracc(u.u_base, u.u_count, B_READ) == NULL)) {
		u.u_error = EFAULT;
		return;
	}

	/* call VIOS to do the write */
	hmdev.hm_kill = vios_iorequest(
		FCODE(WRITE_FUNCTION, WRITE_DATA, 0),
		hmdev.hm_VDD,
		u.u_base, u.u_count, 
		ptob(u.u_procp->p_addr),
		NULL, u.u_offset, 0 , 0, 0,
		&hmdev.hm_stat, NULL, 0, 0);
	while(hmdev.hm_stat.code == 0)
		nulldev();
	if (hmdev.hm_stat.code < 0) {
		u.u_error = EINVAL;
		return;
	}

	u.u_offset += u.u_count;
	u.u_base += u.u_count;
	u.u_count = 0;
	
}

hmclose(dev)
{
	if (dev != 0) {
		u.u_error = ENXIO;
		return;
	}

	/* remove the VD associated with the device */
	vios_iorequest( FCODE(VCTRL_FUNCTION, REMOVEVD, 0),
			hmdev.hm_VDD, 0, 0, 0,
			NULL, 0, 0, 0, 0,
			&hmdev.hm_stat, NULL, 0, 0);
	while(hmdev.hm_stat.code == 0)
		nulldev();

	hmdev.hm_VDD = NULL;
}
