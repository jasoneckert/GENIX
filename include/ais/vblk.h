/*
 * vblk.h
 * Various structure definitions for ais virtual block devices
 *	dgw 2/21/85
 */


/*
 * The structure of a virtual block device.
 */
struct  vbdev {
    struct request *vb_req;	/* list of queued requests */
    unsigned int vb_VDD;	/* virtual device descriptor */
    unsigned int vb_size;	/* size of virtual device in blocks */
    unsigned short vb_nact;	/* number of active requests to vios */
    unsigned short vb_uact;	/* DEBUG active requests from unix */
    struct  iobuf vb_tab;	/* head of queue for device */
};


/*
 * Virtual block device request 
 */
struct  request {
    PKT_IOSTATUS    q_stat;	/* returned status from vios goes here */
    struct  vbdev   *q_vu;	/* pointer to virtual device associated */
				/* with this request */
    struct  request *q_forw;	/* link to other active requests */
    struct  buf     *q_bp;	/* buffer associated with this request */
    unsigned int    q_kill;	/* vios packet number for killing request */
    struct  info    q_info;	/* info for vios vinfo calls */
    unsigned char   q_isactv;	/* nonzero if this is an active request */ 
};

