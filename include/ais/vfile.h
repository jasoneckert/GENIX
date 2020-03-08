/*
 * vfile.h
 * Various structure definitions for ais virtual file transfer devices
 *	dss 3/21/85
 */

#define MAXFTPNAME 128		/* max number chars in Physical Device Name */

#define FT_FILEASGN	0x1000	/* assign Physical dev to Virtual dev */

/* virtual file device structure */
struct ftdev {
	PKT_IOSTATUS	ft_stat;	/* status from vios goes here */
	unsigned int	ft_VDD;		/* VDD for virtual file unit */
	unsigned int	ft_kill;	/* vios packet number for killing */
	unsigned int	ft_flag;	/* status flag */
};
