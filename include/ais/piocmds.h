/*  piocmds.h -- Define VIOS->IOP commands for Physical I/O
 *  copyright (c) American Information Systems Corporation
 *	Daniel Steinberg
 *	November, 1984
 *
 */

#ifndef PIOCMDS
#define PIOCMDS


/* Define Physical I/O command codes */

#define OPEN_CHANNEL		1	/* establish device unit connection */
#define CLOSE_CHANNEL		2	/* terminate device unit connection */
#define READ_CHANNEL		3	/* read from open channel */
#define WRITE_CHANNEL		4	/* write to open channel */
#define CONTROL_CHANNEL		5	/* issue i/o control functions */
#define CANCEL_IO		6	/* terminate in-progress i/o */
#define ALLOC_DATA		7	/* allocate from data page */
#define DEALL_DATA		8	/* release data page allocations */


/* Define Physical I/O auxilliary command codes */


/* Define Physical I/O command modifiers */

/* Define CREATEPD / OPEN_CHANNEL modifiers */
#define LOOKUP_PD	0	/* Lookup existing file */
/*	REM_ON_DEA	1	   Remove when attachments go to zero */
#define NEW_PD		2	/* Create new file */
#define WRITE_ACCESS	4	/* Allow writing to channel */
#define READ_ONLY	8	/* Set device read-only */
#define SHR_READ	16	/* Shared reading from channel */
#define SHR_WRITE	32	/* Shared writing to channel */


/* Define REMOVEPD / CLOSE_CHANNEL modifiers */
#define PRINT_PD	1	/* Spool before closing */
#define DELETE_PD	2	/* Delete file */

/* Define READ_CHANNEL modifiers */
#define ALLOC_BUF	16	/* Allocate data buffer before issuing read */

	/* REC_SEQ devices */
#define RAWMODE		0	/* return at least one character */
#define BLOCKMODE	1	/* return when buffer full */
#define RECORDMODE	2	/* return on buf full or non-printing char */
#define ECHO_RECORDMODE	3	/* same as RECORDMODE, but echo chars */

#define RS_MODES	3	/* mask for all record modes */

#define printable(c)	(isprint(c) || ((c) == ' '))


/* Define WRITE_CHANNEL modifiers */
#define DEALL_BUF	16	/* Deallocate data buffer after write done */

/* Define CONTROL_CHANNEL modifiers */
#define REWIND		 1
#define SKIP_RECORDS	 2
#define SKIP_FILES	 3
#define WRITE_EOF	 8
#define WRITE_EOT	 9
#define UNLOAD_MEDIA	14
#define FORMAT		15

#define INP_BAUD	16
#define OUT_BAUD	17
#define INP_THROTTLE	18
#define OUT_THROTTLE	19
#define INP_BUF		20
#define DTR_ON		21
#define DTR_OFF		22


		/* OUT_THROTTLE parameter 1 */
	/* REC_SEQ devices */
				/* normally, all input characters passed thru */
#define XOFFXANY	1	/* XOFF char stops output, any char restarts */
#define XOFFXON		2	/* XOFF / XON char pairs */


		/* INP_THROTTLE / INP_BUFFER parameter 1 */
	/* REC_SEQ devices */
				/* normally, new chars lost on data overrun */
#define RINGBUFFER	1	/* data overrun throws away oldest info */
#define XONXOFF		2	/* stop char @ 3/4 full; restart @ 1/2 empty */
#define DEVBUSY		3	/* set device busy @ 3/4; ready @ 1/2 */


		/* INP_BUFFER parameter 3 */
	/* REC_SEQ devices */

#define FLUSHBUFFER	1	/* flush existing buffer */
#define NEWBUFFER	2	/* allocate a buffer for buffered input */



/* Define CANCEL_IO modifiers */

/* Define ALLOC_DATA modifiers */
/* Define DEALL_DATA modifiers */


#endif
