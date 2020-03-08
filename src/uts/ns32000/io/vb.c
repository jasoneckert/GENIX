/*
 * %M%: version %I% of %H%
 */
#ifdef SCCS
static char *sccsid = "%W%";
#endif

#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/errno.h"
#include "sys/systm.h"
#include "sys/buf.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/page.h"
#include "sys/iobuf.h"
#include "sys/filsys.h"
#include "sys/debug.h"

#include "ais/viostatus.h"	/* vios definitions */
#include "ais/osiopkt.h"
#include "ais/viocmds.h"

#include "ais/vinfo.h"
#include "ais/vblk.h"

/* Size, in bytes, of a 'physical' disk block */
#define DISKBLOCK (sizeof(struct filsys))


/* structures allocated in conf.c */
extern short vb_cnt;
extern short vb_rcnt;
extern short vb_qdepth;
extern struct vbdev vb_dev[];
extern struct request requests[];

/* vb debug flag */
short vbdebug = 0;

/* to do spl's right on interrupt ack */
extern	short	intflg;

/* kpte1 is needed for swap i/o */
extern seg_t **kpte1;

/*
 * vios initialization data
 * size reflected in NVBLKD in config.h
 */
char *namestr[] = {
	"root","swap","usr","boot",
	"ua","ub","uc","ud",
	"ue","uf","ug","uh",
	"ui","uj","uk","ul",
	"um","un","uo","up",
	"uq","ur","us","ut",
	"uu","uv","uw","ux",
	"uy","uz","man","bin",
	0
};



/* 
 * get a request block from the array of requests
 * and mark it busy
 */
struct request *rqalloc()
{
    register int i, j;
    register char *ptr;

    for (i=0; i<vb_rcnt; i++) {
	if(requests[i].q_isactv == NULL) {
	    for(j=0, ptr=(char*) &requests[i]; j<sizeof(struct request); j++)
		    *ptr++ = '\0';	
	    requests[i].q_isactv = 1;
	    return(&requests[i]);
	}
    }
    return(NULL);
}


/*
 * Initialize a virtual block device.
 * If the virtual device descriptor is null, try to get the
 *   relevant information about the device, reset reference
 *   counts and queues.
 */
vbopen(dev)
    dev_t   dev;
{
    register struct vbdev  *vu;

    /* the dev number always contains only the minor device number */
    vu = &vb_dev[dev];
	
    if (vu->vb_VDD == NULL) {

	/* initialize virtual device with information from vios */
	if ( getdevinfo( vu, dev ) ) {
		u.u_error = ENODEV;
		if (vbdebug)
		    printf("could not open vbdev(%x) through vios\n",dev);
		return;
	}
		

	if (vu->vb_VDD != NULL) {
	    vu->vb_nact = 0;
	    vu->vb_uact = 0;				/* DEBUG */
	    vu->vb_req = (struct request *) NULL;
	    vu->vb_tab.b_actf = NULL;
	    vu->vb_tab.b_actl = NULL;
	    if (dev == minor(swapdev)) 
		    nswap = vu->vb_size;
	} else {
	    u.u_error = ENODEV;
	    printf("dev %x %x VDD is null\n",		/* DEBUG */
		    major(dev), minor(dev));
	}
    }
}


/*
 * Close a block device
 */
vbclose(dev)
    dev_t dev;
{
}



/* Read/write routine for a buffer.  Finds the proper unit, range checks
 * arguments, and schedules the transfer.  Does not wait for the transfer
 * to complete.
 */
vbstrategy(bp)
    register struct buf *bp;		/* IO operation to perform */
{
    register struct vbdev *vu;		/* device to do the IO */
    register int unit;
    int pri;


    unit = minor(bp->b_dev);
    bp->b_resid = bp->b_bcount;

    if ((unit >= vb_cnt) ||		/* check for errors */
	(bp->b_blkno < 0) ||
	( (unit != swapdev) && (bp->b_bcount & FsBMASK(bp->b_dev)) )) {
		printf("vberr: unit(%x) blkno(%x) bcount(%x)\n",
			unit,bp->b_blkno,bp->b_bcount);
		goto bad;
    }
    vu = &vb_dev[unit];		/* virtual device associated with buffer */
    vu->vb_uact++;		/* unix request count - DEBUG */
    if ((vu->vb_VDD) == 0) {
	printf("NULL vb\n");
	goto bad;
    }
    if ( (bp->b_blkno + (bp->b_bcount >> BPCSHIFT)) > vu->vb_size) {
	if (vbdebug)
	    printf("out of range on dev(%x)\n",bp->b_dev);
	goto bad;
    }


    /* queue buffer to device */
    pri = spl6();
	bp->av_forw = NULL;		/* insert at end of list */
	if ( (vu->vb_tab.b_actf) == NULL)
	    vu->vb_tab.b_actf = bp;
	else
	    vu->vb_tab.b_actl->av_forw = bp;
	vu->vb_tab.b_actl = bp;

	vbqueueio(vu);             /* queue request to vios, if possible */
    splx(pri);
    return;

bad:    
    bp->b_flags |= B_ERROR;
    vu->vb_uact--;		/* DEBUG */
    iodone(bp);
    if (bp->b_flags & B_WRITE)		/* tell user his write failed */
	u.u_error = ENXIO;
}



/* Routine to queue a virtual block device request to the vios.
 * If a device has not reached its queue limit and there is
 * an available vios request block then we start virtual IO.
 * If not, we simply return. When the queued IO is completed,
 * we'll get an interrupt and come here to check for more work.
 */
vbqueueio(vu)
    register struct vbdev *vu;        /* device to start IO for */
{
    register struct buf *bp;            /* IO transfer to perform */
    register struct request *rq;        /* vios request block */

    ASSERT(vu != NULL);

    /* if not too much queued, get next transfer, if any */
    if ( (vu->vb_nact >= vb_qdepth) ||
	 ((bp = vu->vb_tab.b_actf) == NULL) ||
	 ((rq = rqalloc()) == (struct request *)NULL)   )
		return;         /* nothing to do now, return */


    /* unlink this buffer from the device queue at vu */
    if (bp->av_forw == NULL)		/* if this is the last buffer */
	vu->vb_tab.b_actf = NULL;	/* zero the listhead */
    else {			/* if more buffers to relink */
	vu->vb_tab.b_actf = bp->av_forw;
    }

    /* rq->q_skip is initialized to zero by rqalloc() */
    rq->q_forw = vu->vb_req;		/* insert request into active list */
    vu->vb_req = rq;
    vu->vb_nact++;			/* bump active request count */
    rq->q_vu = vu;			/* fill in request block */
    rq->q_bp = bp;
    
    vbstart(vu, rq);			/* queue request to vios */
}


/*
 * Start a virtual IO operation. We are committed at this point;
 * remove a buffer describing virtual IO from the device's 
 * waiting queue and queue the request to the vios.
 */
vbstart(vu, rq)
    register struct vbdev *vu;		/* device for IO */
    register struct request *rq;        /* vios request block */
{
    register struct buf *bp;            /* IO transfer to perform */
    char isread;			/* read flag */
    int vbintr();

    ASSERT (vu != NULL);

    bp = rq->q_bp;		/* point to the current buffer */

    if (bp->b_flags & B_READ) isread = 1;
    else isread = 0;

    if ( bp->b_flags & B_PHYS ) {		/* physio */

	    /* 
	     * call the vios with enough information
	     * to do physio itself, i.e. a user virtual
	     * address, a size, and the user page table 
	     * to convert the virtual address to physical
	     */

	    /* user virtual address for transfer */
	    rq->q_info.i[0] = (int) bp->b_un.b_addr;

	    /* physcial address of level 0 page table of user */
	    rq->q_info.i[1] = (int) bp->b_seg;

	    /* VIOS CALL */
	    rq->q_kill = vios_iorequest(
		FCODE( (isread ? READ_FUNCTION : WRITE_FUNCTION),
		       (isread ? READ_DATA : WRITE_DATA),
		       0),
		vu->vb_VDD, rq->q_info.i[0], bp->b_bcount, rq->q_info.i[1],
		NULL, bp->b_blkno, 0, 0, 0,
		&(rq->q_stat), vbintr, 20, 0);

    } else {				/* normal bufferred I/O */
	
	/* VIOS CALL */
	rq->q_kill = vios_iorequest(
	    FCODE( (isread ? READ_FUNCTION : WRITE_FUNCTION),
		   (isread ? READ_DATA : WRITE_DATA),
		   0),
	    vu->vb_VDD, (svtopte(bp->b_un.b_addr)->pgi.pg_pte)&PFNMASK,
	    bp->b_bcount, PHYS_BUFFER, NULL, bp->b_blkno, 0, 0, 0,
	    &(rq->q_stat), vbintr, 16, 0);

    }
}


/* Interrupt routine for virtual devices.  Acknowledge the interrupt, check for
 * errors on the current operation, mark it done if necessary, and start
 * the next request.  Also check for a partially done transfer, and
 * continue with the next chunk if so.
 */
vbintr(st)
    register PKT_IOSTATUS *st;       /* pointer to vios return status */
{
    register struct vbdev *vu;		/* active device */
    register struct buf *bp;        	/* active transfer */
    register struct request *rq;	/* request block */
    register struct request **rqptr;	/* used for unlinking request block */
    int blkcnt;         /* sectors successfully transfered */

    intflg = 1;		/* doing interrupt routine work */

    rq = (struct request *)st;   /* initialize pointer from status block */
    vu = rq->q_vu;
 
    ASSERT(vu != NULL);
    
    bp = rq->q_bp;          /* buffer associated with this request */

    bp->b_resid -= st->bcount;

/* This piece of the transfer is complete (maybe with an error).  If there
 * was no error and there is more to be transfered, start the next piece.
 * Otherwise, terminate IO for this request and try to start a new one.
 */

    if (st->code != V_SUCCESS) {
	bp->b_flags |= B_ERROR;		/* set error flag */

        printf("vbdev %s error: dev %x blkno %x status %d\n",
            (bp->b_flags & B_READ) ? "read" : "write",
            bp->b_dev, bp->b_blkno, st->code);

    } else if (bp->b_resid > 0) {
	vbstart(vu, rq);
	intflg = 0;	/* finished interrupt routine work */
	return;     /* next chunk is started */
    }

    /*
     * All of this current buffer operation is now complete,
     *    with or without errors.
     * Remove request from active list ... for loop searches down the
     * chain of requests and links around the current one
     */
    for (rqptr = (struct request **) &(vu->vb_req);
	 (*rqptr) != NULL;
	 rqptr = &((*rqptr)->q_forw)) {
	    if (*rqptr == rq) {		/* if found this request */
		*rqptr = rq->q_forw;	/* link in next request */
		goto inact;		/* and skip next diagnostic */
	    }
    }
    checkpoint("vbrqlinks");		/* spurt out obscure nonsense */

inact:
    rq->q_isactv = 0;			/* mark inactive */
    vu->vb_nact--;			/* decrement active count for device */
    vu->vb_uact--;			/* DEBUG */

    bp->b_resid = 0;
    iodone(bp);				/* THIS MIGHT START SWAPPING I/O */
    if (vu->vb_tab.b_actf != NULL) {
        vbqueueio(vu);     /* requeue device if more io to do */
    }
    intflg = 0;	/* finished interrupt routine work */
}


/*
 * Routines to do raw IO for a unit.
 */
vbread(dev)            /* character read routine */
    dev_t dev;
{
    register int unit = minor(dev);

    if (unit >= vb_cnt) u.u_error = ENXIO;
    else if(physck(vb_dev[unit].vb_size, B_READ))
        physio(vbstrategy, 0, dev, B_READ);
}


vbwrite(dev)           /* character write routine */
    dev_t dev;
{
    register int unit = minor(dev);

    if (unit >= vb_cnt) u.u_error = ENXIO;
    else if(physck(vb_dev[unit].vb_size, B_WRITE))
        physio(vbstrategy, 0, dev, B_WRITE);
}




#ifdef notdef

/* 
 * Dump core to a the dump device 
 * and after a system crash
 */
vbdump(dev)
    dev_t dev;
{
    register struct vbdev *vu;		/* device to do the IO */
    long num;				/* number of sectors to write */
    int unit;
    extern int dumplo, totalpages;

    num = totalpages * MCPAGES;
    unit = minor(dev);
    if (unit >= vb_cnt) return(ENXIO);
    vu = &vb_dev[unit];
    if ((vu->vb_VDD) == 0) return(ENXIO);
    if (dumplo < 0)
        return(EINVAL);
    if ((dumplo + num) >= vu->vb_size)	/* if not enough space to dump all */
	num = vu->vb_size-dumplo;	/* memory, dump what fits */

    /* dump all of memory to dump device starting at 'disk' block dumplo */

    /* VIOS CALL */
    vios_iorequest(FCODE(WRITE_FUNCTION,WRITE_DATA,0),
			    vu->vb_VDD, 0, num*DISKBLOCK, PHYS_BUFFER,
			    NULL, dumplo, 0, 0, 0,
			    &viostatp, NULL, 0, 0);

    while (viostatp.code == 0) nulldev();	/* wait for completion */

    return(0);
}

#endif


/*
 * Get the device information from the vios for a
 *    particular device identified by the input parameter string
 * Find the VDD for the device identified by namestr
 * Search down the chain of virtual devices until
 *    we arrive at a physical device.
 * Use the info of the physical device, the number of blocks
 *    and the number of bytes per block, to calculate the number
 *    of blocks on the virtual device in its own blocking factor.
 */

getdevinfo( vu, dev )
register struct vbdev *vu;
dev_t dev;
{
	register struct request *rq;
	unsigned int nxtvdd;

	if ((rq = rqalloc()) == (struct request *)NULL) 
		goto badinfo;

	/* VIOS CALL */
	vios_iorequest(FCODE(VINFO_FUNCTION,DEV_INFO,0),
			NULL, &rq->q_info, sizeof(struct info), PHYS_BUFFER,
			namestr[dev], 0 ,0 ,0 ,0,
			&rq->q_stat, NULL, 0, 0);
	while(rq->q_stat.code == 0) nulldev();
	if (rq->q_stat.code != V_SUCCESS)
		goto badinfo;

	if (rq->q_stat.aux_stat3 == VDTYPE) {
		vu->vb_VDD = rq->q_stat.aux_stat2;
		if ((rq->q_info.i[0]) != DISKBLOCK) {
			printf("blocksize mismatch dev(%x)\n", dev);
		}
	}

	/* VIOS CALL */
	vios_iorequest(
		FCODE(VINFO_FUNCTION,DEV_INFO,(FND_PDEV|PRI_INP)),
		vu->vb_VDD, &rq->q_info, sizeof(struct info), PHYS_BUFFER,
		NULL, 0 ,0 ,0 ,0,
		&rq->q_stat, NULL, 0, 0);
	while(rq->q_stat.code == 0) nulldev();
	if ( (rq->q_stat.code != V_SUCCESS) ||
	     (rq->q_stat.aux_stat2 == NULL) )
		goto badinfo;

	if (rq->q_stat.aux_stat3 != PDTYPE) {
		printf("virtual device not assigned\n");
		goto badinfo;
	}

	vu->vb_size = (rq->q_info.i[0] * rq->q_info.i[1]) / DISKBLOCK;

	rq->q_isactv = 0;               /* mark rqblock inactive */
	return(0);			/* got info, everything ok */

badinfo:
	return(1);			/* bad */
}


checkpoint(str)
char *str;
{
	printf("checkpoint: %s\n",str);
	asm("bpt");
}
