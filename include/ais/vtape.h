/*
 * vtape.h
 * Various structure definitions for ais virtual tape devices
 *	dgw 2/21/85
 */


/* virtual tape device structure */
struct vtdev {
	PKT_IOSTATUS	vt_stat;	/* status from vios goes here */
	unsigned int	vt_VDD;		/* VDD for virtual tape unit */
	unsigned char	vt_openf;	/* lock against multiple opens */
	unsigned char	vt_errflg;	/* error flag to abort requests */

	/* vios packet number for killing current request */
	unsigned int	vt_kill;

	/* note: vt_tab.b_active holds current state of tape unit */
	struct	iobuf	vt_tab;		/* buffer header for IO */

	struct	buf	vt_ctlbuf;	/* buffer for controlling operations */
	struct info	vt_info;
};

