/*
 * %M%: version %I% of %H%
 */
#ifdef SCCS
static char *sccsid = "%W%";
#endif

/*
 *	File transfer driver
 *	major device 6, minor device i is the ith file under consideration
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
#include "sys/file.h"
#include "sys/debug.h"

#include "ais/viostatus.h"	/* vios definitions */
#include "ais/osiopkt.h"
#include "ais/viocmds.h"
#include "ais/vfile.h"


int ftintr();

extern short intflg;

extern short ft_cnt;		/* number of ft devices allowed */
extern struct ftdev ft_dev[];	/* device structures defined in conf.c */
extern short ft_pnamlen;	/* max number of chars in Physical Dev name */
extern char ft_pname[];		/* temp storage for Phys Dev name in conf.c */

int ftbug = 0;

extern char numstr[];	/* "0123456789abcdef" */

static char ftname[] = "ft00";
#define set_name(x) (ftname[2] = numstr[((x)>>4) & 0x0f], \
			ftname[3] = numstr[(x) & 0x0f])

/* ftopen -- Open a 'file transfer' device
 *
 * Check for exclusive use and create a Virtual Device.
 * The actual Physical Device Name is supplied by an ioctl() call.
 */
ftopen(minordev, flag)
{
	register struct ftdev *fu;

	/* First, check that minor device number is ok */
	if (minordev >= ft_cnt) {
		u.u_error = ENXIO;
		return;
	}

	fu = &ft_dev[minordev];		/* ptr to table entry for this device */

	/* If the device is open (VDD not null), check for exclusive use */
	if (fu->ft_VDD != NULL) {
		if ((flag & FEXCL) || (fu->ft_flag & FEXCL)) {
		    u.u_error = EEXIST;
		}
		return;			/* if already open, just return */
	}

	/* make up virtual file name */
	set_name(minordev);

	/* create VD for the file */
	vios_iorequest(FCODE(VCTRL_FUNCTION,CREATEVD, 0),
			NULL, 0, 0, 0,
			ftname, REC_SEQ, 0 , 0 , 0,
			&fu->ft_stat, NULL, 0, 0);
	while(fu->ft_stat.code == 0)
		nulldev();
	if (fu->ft_stat.code < 0) {
		printf("ft: error creating Virtual Device: %s\n", ftname);
		u.u_error = ENXIO;
		return;
	}
	fu->ft_VDD = fu->ft_stat.aux_stat2;	/* save VDD */
	fu->ft_flag = flag;			/* save open flag */

}

/* ftioctl -- ft device i/o control routines
 *
 * cmd == FT_FILEASGN ::
 *   The VIOS is called to create a PDD for the named physical device
 *   and associate it with the VDD created by the open() above.
 *
 *   The 'flag' from the open() (now in fu->ft_flag) is used to determine
 *   how to access the Physical Device.  The 'arg' is the Physical Device
 *   name to access.  The 'flag' argument is ignored, at present.
 */
ftioctl(minordev, cmd, arg, flag)
{
	register struct ftdev *fu;
	register int pn;
	int pri;

	/* First, check that minor device number is ok */
	if (minordev >= ft_cnt) {
		u.u_error = ENXIO;
		return;
	}

	fu = &ft_dev[minordev];		/* ptr to table entry for this device */

	/* If the device is not open, error */
	if (fu->ft_VDD == NULL) {
		u.u_error = EBADF;
		return;
	}
	
	switch(cmd) {

	case FT_FILEASGN:
		if (fu->ft_flag & FT_FILEASGN) {
			u.u_error = EEXIST;
			return;
		}

		/* gather Physical Device name chars (truncate at limit) */
		/* "HOST:" should already have been prepended */

		for (pn = 0; pn < ft_pnamlen; pn++)
			if (! (ft_pname[pn] = fubyte(arg++)))
				break;
		ft_pname[pn] = '\0';	/* ensure termination/truncation */
		
		if (ftbug) {
			printf("\nft_pname= %s", ft_pname);
			asm("bpt");
			asm("nop");
		}

		/* set access flag:
		 *	 FAPPEND |  FWRITE ==> LOOKUP_PD | WRITE_ACCESS
		 *	 FAPPEND | !FWRITE ==> LOOKUP_PD | READ_ONLY
		 *	!FAPPEND |  FWRITE ==> NEW_PD | WRITE_ACCESS
		 *	!FAPPEND | !FWRITE ==> LOOKUP_PD | READ_ONLY | SHR_READ
		 */
		pn = (fu->ft_flag & FAPPEND) ? (LOOKUP_PD |
			((fu->ft_flag & FWRITE) ? WRITE_ACCESS : READ_ONLY)) :
				 ( (fu->ft_flag & FWRITE) ?
				     (NEW_PD | WRITE_ACCESS) :
				     (LOOKUP_PD | READ_ONLY | SHR_READ) );

		/* create PDD for the file */
		pri = spl6();

		vios_iorequest( FCODE(PCTRL_FUNCTION, CREATEPD, pn),
				NULL, 0, 0, 0,
				ft_pname, REC_SEQ, 132, 0, 0,
				&fu->ft_stat, ftintr, 0, 0);

		if (fu->ft_stat.code == 0) {
		    sleep((caddr_t)&fu->ft_stat, PRIBIO);
		}

		splx(pri);

		if (fu->ft_stat.code < 0) {
			u.u_error = EINVAL;
			return;
		}

		/* assign VDD to PDD */
		pn = ((fu->ft_flag & FREAD) ? PRI_INP : 0) |
			((fu->ft_flag & FWRITE) ? PRI_OUT : 0);
		vios_iorequest(FCODE(VCTRL_FUNCTION, ASSIGNVD, pn),
				fu->ft_VDD, 0, 0, 0,
				NULL, fu->ft_stat.aux_stat2, 0, 0, 0,
				&fu->ft_stat, NULL, 0, 0);
		while(fu->ft_stat.code == 0)
			nulldev();
		if (fu->ft_stat.code < 0) {
			u.u_error = EINVAL;
			return;
		}
		fu->ft_flag |= FT_FILEASGN;
		break;

	default:
		return;
	}
}


ftread(minordev)
{
	register int pri;
	register struct ftdev *fu;

	/* First, check that minor device number is ok */
	ASSERT(minordev < ft_cnt);

	fu = &ft_dev[minordev];		/* ptr to table entry for this device */

	/* If the device is not open, error */
	ASSERT(fu->ft_VDD != NULL);

	if (!(fu->ft_flag & FT_FILEASGN)) {
		u.u_error = EBADF;
		return;
	}

	if ( (u.u_count == 0) ||
		    (useracc(u.u_base, u.u_count, B_READ|B_PHYS) == NULL) ) {
	    u.u_error = EFAULT;
	    return;
	}

	pri = spl6();

	/* call VIOS to do the read */
	fu->ft_kill = vios_iorequest(
		FCODE(READ_FUNCTION, READ_DATA, 0),
		fu->ft_VDD,
		u.u_base, u.u_count, 
		ptob(u.u_procp->p_addr),
		NULL, 0, 0 , 0, 0,
		&fu->ft_stat, ftintr, 0, 0);

	sleep( (caddr_t)&fu->ft_stat, PRIBIO );
	splx(pri);

	if (fu->ft_stat.code == V_EOT || 
	     fu->ft_stat.code == V_EOF) {	/* EOF */
			return;
	}

	if (fu->ft_stat.code < 0) {
		u.u_error = EINVAL;
		return;
	}

	u.u_offset += fu->ft_stat.bcount;
	u.u_base += fu->ft_stat.bcount;
	u.u_count -= fu->ft_stat.bcount;
}

ftwrite(minordev)
{
	register int pri;
	register struct ftdev *fu;

	/* First, check that minor device number is ok */
	ASSERT(minordev < ft_cnt);

	fu = &ft_dev[minordev];		/* ptr to table entry for this device */

	/* If the device is not open, error */
	ASSERT(fu->ft_VDD != NULL);

	if (!(fu->ft_flag & FT_FILEASGN)) {
	    u.u_error = EBADF;
	    return;
	}

	if ( (u.u_count == 0) ||
		    (useracc(u.u_base, u.u_count, B_WRITE|B_PHYS) == NULL) ) {
	    u.u_error = EFAULT;
	    return;
	}

	pri = spl6();

	/* call VIOS to do the read */
	fu->ft_kill = vios_iorequest(
		FCODE(WRITE_FUNCTION, WRITE_DATA, 0),
		fu->ft_VDD,
		u.u_base, u.u_count, 
		ptob(u.u_procp->p_addr),
		NULL, 0, 0 , 0, 0,
		&fu->ft_stat, ftintr, 0, 0);

	sleep( (caddr_t)&fu->ft_stat, PRIBIO );
	splx(pri);

	if (fu->ft_stat.code < 0) {
		u.u_error = EINVAL;
		return;
	}

	u.u_offset += fu->ft_stat.bcount;
	u.u_base += fu->ft_stat.bcount;
	u.u_count -= fu->ft_stat.bcount;
}


ftintr(st)
    char *st;
{
	intflg = 10;
	wakeup((caddr_t)st);
	intflg = 0;
}


ftclose(minordev)
{
	register struct ftdev *fu;

	/* First, check that minor device number is ok */
	ASSERT(minordev < ft_cnt);

	fu = &ft_dev[minordev];		/* ptr to table entry for this device */

	/* If the device is not open, error */
	if (fu->ft_VDD == NULL) {
		printf("ft: Null VDD on close\n");
		u.u_error = EBADF;
		return;
	}

	/* remove the VD associated with the device */
	vios_iorequest( FCODE(VCTRL_FUNCTION, REMOVEVD, 0),
			fu->ft_VDD, 0, 0, 0,
			NULL, 0, 0, 0, 0,
			&fu->ft_stat, NULL, 0, 0);
	while(fu->ft_stat.code == 0)
		nulldev();

	fu->ft_VDD = NULL;
	fu->ft_flag = 0;
}
