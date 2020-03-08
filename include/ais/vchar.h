/*
 * vchar.h
 * Various defintions for ais virtual character devices
 *	dgw 2/21/85
 */


/*
 * The structure of a virtual character device.
 * For a virtual block device we aren't especially worried about
 * the order of processing of requests, and we'll let the host
 * system figure out the best order to fill requests.
 * For a virtual character device we want all requests to
 * be filled in serial fashion, therefore we only allow
 * one outstanding output request per device.
 */
struct vcdev {
    unsigned int    vc_ikill;	/* vios packet number for killing */
   			 	/* an input request */
    unsigned int    vc_okill;	/* vios packet number for killing */
                                /* an output request */
    unsigned int    vc_ckill;	/* vios packet number for killing */
                                /* an ioctl request */
    char vc_ibuf[CLSIZE];	/* input buffer */
    unsigned int    vc_imode;	/* mode for current outstanding input request */
    unsigned int    vc_VDD;	/* virtual device descriptor */
    unsigned int    vc_state;	/* state of virtual tty */
    struct info	    vc_info;	/* buffer for get dev info */
};


typedef struct {
	PKT_IOSTATUS	st_stat;
	struct	tty	*st_tty;
	struct	vcdev	*st_vcdev;
} TTY_STAT;

