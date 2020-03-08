/*  viocmds.h -- Define VIOS Virtual I/O command codes
 *  copyright (c) American Information Systems Corporation
 *	Daniel Steinberg
 *	November, 1984
 *
 */
#ifndef VIOCMDS
#define VIOCMDS

#include "piocmds.h"

/* Define the known I/O function types  [PKT_FUNCTION.f.u.nc_type]  */

#define VCTRL_FUNCTION  1	/* manipulate VIOS data structures */
#define PCTRL_FUNCTION	2	/* alter physical device data structures */
#define READ_FUNCTION	3	/* read data */
#define WRITE_FUNCTION	4	/* write data */
#define DEVICE_FUNCTION	5	/* perform device control */

#define AUX_FUNCTION	10	/* auxilliary i/o functions */

#define INTERNAL_FUNCTION 13	/* VIOS internal functions */
#define VINFO_FUNCTION	14	/* get information about VIOS */
#define DEBUG_FUNCTION	15	/* get debugging info about VIOS */


/* Define the known I/O function codes  [PKT_FUNCTION.f.u.nc_code]  */

		/* VCTRL_FUNCTION codes */
#define CREATEVD    1		/* create a virtual device descriptor */
#define REMOVEVD    2		/* remove a virtual device descriptor */
#define ASSIGNVD    3		/* assign devices to a vdd */

		/* PCTRL_FUNCTION codes */
#define CREATEPD    1		/* create a new physical device */
#define REMOVEPD    2		/* remove a physical device descriptor */
#define READ_BUFFER 15		/* establish input buffering */

		/* READ_FUNCTION codes */
#define READ_DATA   1		/* normal input function */

		/* WRITE_FUNCTION codes */
#define WRITE_DATA  1		/* normal output function */

		/* DEVICE_FUNCTION codes */
#define VDEV_CONTROL	1	/* Virtual Device control function */
#define PDEV_CONTROL	2	/* Physical Device control function */

		/* AUX_FUNCTION codes */
#define SET_TIMER   1		/* set an alarm */
#define GET_DATE    2		/* read time/date from TOD */
#define SET_DATE    3		/* set time/date in TOD */


		/* VINFO_FUNCTION codes */
#define DEV_INFO    1		/* get information on a particular device */


/* Define some I/O function modifiers */

		/* CREATEVD modifiers */
#define REM_ON_DEA	 1	/* remove device when attachments go to zero */

		/* ASSIGNVD modifiers */
#define PRI_INP		 1	/* primary input */
#define PRI_OUT		 2	/* primary output */
#define ERR_INP		 4	/* secondary input */
#define JOU_INP		 8	/* input journal */
#define JOU_OUT		16	/* output journal */
				/* eot input manipulated with push/pop */
#define PUSH_PRI_INP	32	/* push primary input */
#define POP_PRI_INP	64	/* pop primary input */

		/* CREATEPD modifiers */
/*	LOOKUP_PD	0	   lookup an existing device/file */
/*      REM_ON_DEA	1	   remove when attachments go to zero */
/*	NEW_PD		2	   create/allocate a new device */
/*	WRITE_ACCESS	4	   Access for write */
/*	READ_ONLY	8	   Set device read-only */
/*	SHR_READ	16	   Shared read access allowed */
/*	SHR_WRITE	32	   Shared write allowed */

		/* REMOVEPD modifiers */
/*	PRINT_PD	1	   Spool for printing */
/*	DELETE_PD	2	   Delete, if possible, after disconnect */


		/* VDEV_CONTROL modifiers */
/*	REWIND		 1	   Rewind device */
/*	SKIP_RECORDS	 2	   Record skip */
/*	SKIP_FILES	 3	   File skip */
/*	WRITE_EOF	 8	   Write End-of-File mark */
/*	WRITE_EOT	 9	   Write End-of-Tape mark */
/*	UNLOAD_MEDIA	14	   Unload device */
/*	FORMAT		15	   Format device */

		/* PDEV_CONTROL modifiers */
/*	INP_BAUD	16	   Input baud rate */
/*	OUT_BAUD	17	   Output baud rate */
/*	INP_THROTTLE	18	   Input flow control */
/*	OUT_THROTTLE	19	   Output flow control */
/*	INP_BUF		20	   Input buffer control */
/*	DTR_ON		21	   Turn DTR on */
/*	DTR_OFF		22	   Turn DTR off */


		/* OUT_THROTTLE parameter 1 */
	/* REC_SEQ devices */
				/* normally, all input characters passed thru */
/*	XOFFXANY	1	   XOFF char stops output, any char restarts */
/*	XOFFXON		2	   XOFF / XON char pairs */


		/* INP_THROTTLE / INP_BUFFER parameter 1 */
	/* REC_SEQ devices */
				/* normally, new chars lost on data overrun */
/*	RINGBUFFER	1	   data overrun throws away oldest info */
/*	XONXOFF		2	   stop char @ 3/4 full; restart @ 1/2 empty */
/*	DEVBUSY		3	   set device busy @ 3/4; ready @ 1/2 */


		/* INP_BUFFER parameter 3 */
	/* REC_SEQ devices */

/*	FLUSHBUFFER	1	   flush existing buffer */
/*	NEWBUFFER	2	   allocate a buffer for buffered input */


		/* READ_FUNCTION modifiers */
	/* REC_SEQ devices */
/*	RAWMODE		0	   return at least one character */
/*	BLOCKMODE	1	   return when buffer full */
/*	RECORDMODE	2	   return on buf full or non-printing char */
/*	ECHO_RECORDMODE	3	   same as RECORDMODE, but echo chars */


		/* SET_TIMER modifiers */
#define ONE_SHOT	0	/* wakeup at timeout */
#define PERIODIC	1	/* set free-running timer */


		/* DEV_INFO modifiers */
/* Only ONE of the following: */
/*				 0	specified virtual or physical device */
/*	#define PRI_INP		 1	primary input */
/*	#define PRI_OUT		 2	primary output */
/*	#define ERR_INP		 4	secondary input */
/*	#define JOU_INP		 8	input journal */
/*	#define JOU_OUT		16	output journal */
#define NXT_VDEV		5	/* next Virtual Device in list */
#define NXT_PDEV		6	/* next Physical Device in list */
#define FND_PDEV		128	/* find primary Physical Device unit */


/* Define some complete I/O functions  *** This depends on bit-field packing */

/* Define macro to construct Function from components */
	/* (Function code, Function aux, Modifier) */
#define FCODE(a,b,c) ((((((unsigned)c)<<4)+((unsigned)b))<<4)+((unsigned)a))

#define CREATE_VD  FCODE(VCTRL_FUNCTION, CREATEVD, 0)
#define REMOVE_VD  FCODE(VCTRL_FUNCTION, REMOVEVD, 0)
#define ASSIGN_VD  FCODE(VCTRL_FUNCTION, ASSIGNVD, 0)

#define CREATE_PD  FCODE(PCTRL_FUNCTION, CREATEPD, 0)
#define REMOVE_PD  FCODE(PCTRL_FUNCTION, REMOVEPD, 0)


/* Define the timeout interval units  [PKT_TIMEOUT.scale]  */

#define MSEC_TIMEOUT  0		/* milliseconds */
/* *** #define TICK_TIMEOUT  1 *** */		/* clock ticks */
#define  SEC_TIMEOUT  2		/* seconds */
#define  MIN_TIMEOUT  3		/* minutes */

/* define Virtual/Physical device flags */
#define VDTYPE 1
#define PDTYPE 0


#endif
