/*
 * vc.c: version 1.2 of 3/8/85
 */
#ifdef SCCS
static char *sccsid = "@(#)vc.c	1.2";
#endif

#include "sys/types.h"
#include "sys/param.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/tty.h"
#include "sys/conf.h"
#include "sys/termio.h"
#include "sys/sysinfo.h"

#include "ais/viostatus.h"	/* vios definitions */
#include "ais/osiopkt.h"
#include "ais/viocmds.h"
#include "ais/piocmds.h"
#include "ais/aisuart.h"

#include "ais/vinfo.h"
#include "ais/vchar.h"

#define	OFF	0		/* state flags */
#define	DTRON	1		/* dtr on */
#define RQOUT	2		/* input request outstanding */


/* structures allocated in conf.c */
extern short vc_cnt;
extern struct tty vc_tty[];
extern struct vcdev vc_dev[];
extern TTY_STAT vc_stat[][3];


#define vc_istat(x) (vc_stat[x][0])
#define vc_ostat(x) (vc_stat[x][1])
#define vc_cstat(x) (vc_stat[x][2])


/* to do spl's right on interrupt ack */
extern	short	intflg;
short	vc_scan;

/* vios initiailization data */
char ttystr[] = "tty0";
char numstr[] = "0123456789abcdef";
char constr[] = "console";

/* error message strings */
char find_dev[] = "find device";
char in_throt[] = "input throttle";
char out_throt[] = "output throttle";
char in_baud[] = "input baud";
char out_baud[] = "output baud";

/* functions for interrupt completion */
int vcxint();
int vcrint();

static char vc_speeds[] = {
	0,		/* no baud hang up line */
	0,		/* 50 baud not supported */
	U_75,
	U_110,
	U_134,
	U_150,
	0,		/* 200 baud not supported */
	U_300,
	U_600,
	U_1200,
	U_1800,
	U_2400,
	U_4800,
	U_9600,
	U_19200,
	U_EXT1
};


/*
 * open a vitual character device
 * dev should contain only the minor device number
 */
vcopen(dev, flag)
register dev;
{
	register struct tty *tp;
	register struct vcdev *vu;
	register TTY_STAT *st;
	register int i;
	extern vcproc();

	if (dev >= vc_cnt) {
		u.u_error = ENXIO;
		return;
	}

	vu = &vc_dev[dev];
	if (vu->vc_VDD == NULL) {
		st = &vc_cstat(dev);

		/*
		 * generate device name, use ibuf for string temporary
		 * it must be at an address that is physical
		 */
		for(i=0;i<3;i++)
			vu->vc_ibuf[i] = ttystr[i];
		vu->vc_ibuf[3] = numstr[dev];

		/* VIOS CALL */
		vios_iorequest(FCODE(VINFO_FUNCTION,DEV_INFO,0),
			    0, 0, 0, 0,
			    vu->vc_ibuf,0,0,0,0,
			    st,NULL,0,0);
		while(st->st_stat.code == 0) nulldev();	/* wait till finished */

		if ( (st->st_stat.aux_stat2 == 0) || (st->st_stat.code != 1) ) {
			/* no virtual device for this unit number */
			printf("vcopen: %x returned status\n",st->st_stat.code);
			u.u_error = EUNATCH;
			printf("%s is not attached\n",vu->vc_ibuf);
			return;
		} else {
			/* found a virtual device */
			if (st->st_stat.aux_stat3 == VDTYPE)
			    vu->vc_VDD = (unsigned int)st->st_stat.aux_stat2;
			else
			    checkpoint("vcopen VDTYPE");
	    	}
	}

	/**************************************???
	if (vc_kmc[DZMDNO(dev)] == 0) {
		if (!(flag&FNDELAY))
		return;
	}
	???***************************************/
	tp = &vc_tty[dev];
	if ((tp->t_state&(ISOPEN|WOPEN)) == 0) {
		ttinit(tp);
		tp->t_proc = vcproc;
		/* tp->t_state |= EXTPROC; */
		vc_istat(dev).st_tty = tp;	/* init these */
		vc_ostat(dev).st_tty = tp;
		vc_cstat(dev).st_tty = tp;
		vcparam(dev);
	}
	spl5();
	if (tp->t_cflag&CLOCAL || vcmodem(dev, DTRON))
		tp->t_state |= CARR_ON;
	else
		tp->t_state &= ~CARR_ON;
	if (!(flag&FNDELAY))
	while ((tp->t_state&CARR_ON)==0) {
		tp->t_state |= WOPEN;
		sleep((caddr_t)&tp->t_canq, TTIPRI);
	}
	(*linesw[tp->t_line].l_open)(tp);
	spl0();
}

vcclose(dev)
{
	register struct tty *tp;

	tp = &vc_tty[dev];
	(*linesw[tp->t_line].l_close)(tp);
	if (tp->t_cflag&HUPCL && !(tp->t_state&ISOPEN))
		vcmodem(dev, OFF);
}

vcread(dev)
{
	register struct tty *tp;

	tp = &vc_tty[dev];
	(*linesw[tp->t_line].l_read)(tp);
}

vcwrite(dev)
{
	register struct tty *tp;

	tp = &vc_tty[dev];
	(*linesw[tp->t_line].l_write)(tp);
}

vcioctl(dev, cmd, arg, mode)
register dev;
{
	switch(cmd) {
	case TCDSET:
		break;

	/*
	 * Nothing, until more can be done here
	 * Current vios and unix implementation do not define ioctl 
	 */

	default:
		if (ttiocom(&vc_tty[dev], cmd, arg, mode))
			vcparam(dev);
	}
}

vcparam(dev)
{
	register struct tty *tp;
	register TTY_STAT *st;
	register struct vcdev *vu;
	register int flags;

	/* input and output pdd, should be same, is not checked  */
	int pdd;	
	int mode, speed;
	char *errstr;

	tp = &vc_tty[dev];
	vu = &vc_dev[dev];

	if (vc_scan==0) {
		vcscan();
		vc_scan++;
	}

	flags = tp->t_cflag;	
	if ((flags&CBAUD) == 0) {
		/* hang up line */
		vcmodem(dev, OFF);
		return;
	}


	/*
	 * set baud rate on both input and output vios channels
	 * to be the same
	 */
	st = &vc_cstat(dev);
	speed = vc_speeds[flags&CBAUD];

	vios_iorequest(FCODE(VINFO_FUNCTION,DEV_INFO,PRI_INP|FND_PDEV),
	    /* vu->vc_VDD, &vu->vc_info, sizeof(struct info), PHYS_BUFFER, */
		vu->vc_VDD, 0, 0, PHYS_BUFFER,
		NULL, 0, 0, 0, 0,
		st, NULL, 0, 0);
	while(st->st_stat.code == 0) nulldev();
	if (st->st_stat.code != V_SUCCESS) {
		errstr = find_dev;
		goto badctl;
	} else
		pdd = st->st_stat.aux_stat2;


	vios_iorequest(FCODE(DEVICE_FUNCTION,PDEV_CONTROL,INP_BAUD),
		pdd, 0, 0, 0,
		NULL, speed, 0, 0, 0,
		st, NULL, 0, 0);
	while(st->st_stat.code == 0) nulldev();
	if (st->st_stat.code != V_SUCCESS) {
		errstr = in_baud;
		goto badctl;
	}
	vios_iorequest(FCODE(DEVICE_FUNCTION,PDEV_CONTROL,INP_THROTTLE),
		pdd, 0, 0, 0,
		NULL, (tp->t_iflag & IXOFF)? XONXOFF : NULL, 0, 0, 0,
		st, NULL, 0, 0);
	while(st->st_stat.code == 0) nulldev();
	if (st->st_stat.code != V_SUCCESS) {
		errstr = in_throt;
		goto badctl;
	}


	vios_iorequest(FCODE(VINFO_FUNCTION,DEV_INFO,PRI_OUT|FND_PDEV),
	    /* vu->vc_VDD, &vu->vc_info, sizeof(struct info), PHYS_BUFFER, */
		vu->vc_VDD, 0, 0, PHYS_BUFFER,
		NULL, 0, 0, 0, 0,
		st, NULL, 0, 0);
	while(st->st_stat.code == 0) nulldev();
	if (st->st_stat.code != V_SUCCESS) {
		errstr = find_dev;
		goto badctl;
	} else
		pdd = st->st_stat.aux_stat2;

	vios_iorequest(FCODE(DEVICE_FUNCTION,PDEV_CONTROL,OUT_BAUD),
		pdd, 0, 0, 0,
		NULL, speed, 0, 0, 0,
		st, NULL, 0, 0);
	while(st->st_stat.code == 0) nulldev();
	if (st->st_stat.code != V_SUCCESS) {
		errstr = out_baud;
		goto badctl;
	}

	if (IXON & tp->t_iflag)
		mode = (IXANY & tp->t_iflag) ? XOFFXANY: XOFFXON;	
	else 
		mode = NULL;

	vios_iorequest(FCODE(DEVICE_FUNCTION,PDEV_CONTROL,OUT_THROTTLE),
		pdd, 0, 0, 0,
		NULL, mode, 0, 0, 0,
		st, NULL, 0, 0);
	while(st->st_stat.code == 0) nulldev();
	if (st->st_stat.code != V_SUCCESS) {
		errstr = out_throt;
		goto badctl;
	}

	return;


/******************************************
	if (flags&CREAD)
		lpr |= RCVENB;
	if (flags&CS6)
		lpr |= BITS6;
	if (flags&CS7)
		lpr |= BITS7;
	if (flags&PARENB) {
		lpr |= PENABLE;
		if (flags&PARODD)
			lpr |= OPAR;
	}
	if (flags&CSTOPB)
		lpr |= TWOSB;
*******************************************/

badctl:
	printf("vcparam: code=%d (%s)\n",st->st_stat.code,errstr);
	u.u_error = EINVAL;
}

/*
 * Handle a receive interrupt from the vios
 * this occurs as result of an outstanding vios read request
 */
vcrint (st)
	register TTY_STAT *st;
{
	register struct tty *tp;
	register struct vcdev *vu;
	register int i;

	sysinfo.rcvint++;
	tp = st->st_tty;
	vu = &vc_dev[tp - vc_tty]; 
	vu->vc_state &= ~RQOUT;		/* input request done */

	if (tp == NULL) {
		printf("tp NULL in vcrint\n");
		return;
	}
	if (tp->t_rbuf.c_ptr == NULL)
		return;

	intflg = 2;		/* doing interrupt routine work */

	if (st->st_stat.code != V_SUCCESS) {
		printf("vcrint input err: %d\n", st->st_stat.code);
		intflg = 0;	/* finished interrupt routine work */
		return;
	}

	if (st->st_stat.aux_code == V_BREAK)
		(*linesw[tp->t_line].l_input)(tp, L_BREAK);
	else {
		for(i=0; i< st->st_stat.bcount; i++)
			tp->t_rbuf.c_ptr[i] = vu->vc_ibuf[i];
		tp->t_rbuf.c_count -= st->st_stat.bcount;
	}

	if (tp->t_state&ISOPEN) {
		(*linesw[tp->t_line].l_input)(tp, L_BUF);
		vcproc(tp, T_INPUT);
	}

	intflg = 0;	/* finished interrupt routine work */
}

vcxint(st)
	register TTY_STAT *st;
{
	register struct tty *tp;

	intflg = 3;		/* doing interrupt routine work */
	if (st->st_stat.code == V_SUCCESS) {
		sysinfo.xmtint++;
		tp = st->st_tty;
		tp->t_state &= ~BUSY;
		tp->t_tbuf.c_count -= st->st_stat.bcount;
		if(tp->t_tbuf.c_count != 0) 
			printf("vcxint: nozero resid\n");
		vcproc(tp, T_OUTPUT);
	} else 
		printf("vcxint: status=%x\n",st->st_stat.code);  
	intflg = 0;	/* finished interrupt routine work */
}

vcproc(tp, cmd)
register struct tty *tp;
{
	register dev;
	register struct vcdev *vu;
	int	sel4, sel6;

	dev = tp - vc_tty;
	sel4 = 0;
	vu = &vc_dev[dev];
	switch(cmd) {

	case T_WFLUSH:
		/* kill current output request */
		/* and restart another one */
		/* sel4 |= OFLUSH; */

	case T_RESUME:
		/* sel4 |= ORSME; */
		goto start;

	case T_INPUT:
		/* Pass new input buffer to vios */
		if (tp->t_rbuf.c_ptr != NULL) {
			    start_read(vu, tp, dev);
		}
		break;

	case T_OUTPUT:
	start:
		if (tp->t_state&(TIMEOUT|TTSTOP|BUSY))
			break;
		if (!(CPRES&(*linesw[tp->t_line].l_output)(tp)))
			break;
		else {
			/* VIOS CALL */
			vu->vc_okill = vios_iorequest(
				FCODE(WRITE_FUNCTION,WRITE_DATA,0),
				vu->vc_VDD, tp->t_tbuf.c_ptr,
				tp->t_tbuf.c_count,PHYS_BUFFER,
				NULL,0,0,0,0,
				&vc_ostat(dev),vcxint,8,0);
			tp->t_state |= BUSY;
		}
		break;

	case T_SUSPEND:
		/* call vios if needed and implemented */
		break;

	case T_BLOCK:
		tp->t_state |= TBLOCK;
		/* send stop character to sender via vios */
		/* sel4 |= SCHAR; */
		/* sel6 = CSTOP; */
		break;

	case T_RFLUSH:
		/* throw away input */
		/* kill current request and start another one */
		/* sel4 |= IFLUSH; */
		if (!(tp->t_state&TBLOCK))
			break;

	case T_UNBLOCK:
		tp->t_state &= ~TBLOCK;
		/* send resume character to sender via vios */
		/* sel4 |= SCHAR; */
		/* sel6 = CSTART; */
		break;

	case T_BREAK:
		/* call vios to do a brk */
		/* sel4 |= SBRK; */
		break;

	case T_PARM:
		vcparam(dev);
		break;
	}

	/* call vios to do ioctl on a virtual terminal
	 * prototype code:
	 *
	 * if (ioctl) {
	 *	VIOS CALL
	 * 	vios_ioctl(FCODE(DEVICE_FUNCTION,SET_TTY,mode),
	 *               vu->vc_VDD,
	 *               vu->vc_ibuf,0,
	 *               tp->t_ospeed,tp->t_ispeed,flags,0,0,
	 *               &vc_istat(dev),NULL,
	 *               0,0);
	 *
	 * while(*(&vc_istat(dev))->st_stat.code == 0) nulldev();
	 */
}

vcmodem(dev, flag)
register int dev;
{
	register struct vcdev *vu;
	register TTY_STAT *st;
	int pdd;

	vu = &vc_dev[dev];
	st = &vc_cstat(dev);

	/* find the physical device to effect DTR */
	vios_iorequest(FCODE(VINFO_FUNCTION,DEV_INFO,PRI_INP|FND_PDEV),
		vu->vc_VDD, 0, 0, 0,
		NULL, 0, 0, 0, 0,
		st, NULL, 0, 0);
	while(st->st_stat.code == 0) nulldev();
	if (st->st_stat.code != V_SUCCESS) {
	    printf("vcmodem: couldn't get pdd, code=%d\n",st->st_stat.code);
	    goto vcmodout;
	} else
		pdd = st->st_stat.aux_stat2;

	vu->vc_ckill = vios_iorequest(
		FCODE(DEVICE_FUNCTION, PDEV_CONTROL,
			(flag==DTRON) ? DTR_ON : DTR_OFF),
		pdd, 0, 0, 0,
		NULL, 0, 0, 0, 0,
		st, NULL, 0, 0);
	while(st->st_stat.code == 0) nulldev();
	if (st->st_stat.code != V_SUCCESS) {
		printf("vcmodem: returned code = %d\n", st->st_stat.code);
	}

vcmodout:
	if (flag==DTRON)
		vc_dev[dev].vc_state |= DTRON;
	else
		vc_dev[dev].vc_state &= ~DTRON;

	return(flag);
}


vcscan()
{
	register i;
	register struct tty *tp;
	char	bit;

	for (i=0; i<vc_cnt; i++) {
		tp = &vc_tty[i];
		if (!(tp->t_state&(ISOPEN|WOPEN)))
			continue;
		if (tp->t_cflag&CLOCAL || (vc_dev[i].vc_state&DTRON) ) {
			if ((tp->t_state&CARR_ON)==0) {
				wakeup(&tp->t_canq);
				tp->t_state |= CARR_ON;
			}
		} else {
			if (tp->t_state&CARR_ON) {
				if (tp->t_state&ISOPEN) {
					signal(tp->t_pgrp, SIGHUP);
					tp->t_pgrp = 0;
					ttyflush(tp, (FREAD|FWRITE));
				}
				tp->t_state &= ~CARR_ON;
			}
		}
	}
	timeout(vcscan, 0, HZ);
}


/*
 * Start a read on a virtual tty.
 * Use the already initialized state of the tty
 * to pick the correct type of vios read request.
 * The driver should check before calling that a
 * read request is desired.
 */
start_read (vu, tp, dev)
    register struct vcdev *vu;
    register struct tty *tp;
    int dev;
{
    int mode;

    /* request outstanding don't start another */
    if(vu->vc_state & RQOUT) return;

    mode = RAWMODE;
    vu->vc_imode = mode;	/* set mode for new request */

    vu->vc_state |= RQOUT;
		/* VIOS CALL */
    vu->vc_ikill = vios_iorequest(FCODE(READ_FUNCTION,READ_DATA,mode),
					vu->vc_VDD,
					vu->vc_ibuf,CLSIZE,PHYS_BUFFER,
	    /* above line should be: tp->t_rbuf.c_ptr, tp->t_rbuf.c_size,*/
					NULL,0,0,0,0,
					&vc_istat(dev),vcrint,8,0);
}

