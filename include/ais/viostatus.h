/*  viostatus.h -- Define VIOS command status codes
 *  copyright (c) American Information Systems Corporation
 *	Daniel Steinberg
 *	November, 1984
 *
 */

#ifndef VIOSTATUS
#define VIOSTATUS

/* Define I/O status return codes */

#define V_SUCCESS		 1	/* Successful I/O Completion */
#define V_NOT_STARTED		 0	/* Request not dequeued yet */
#define V_PENDING		-1	/* Request is waiting for something */
#define V_ALL_FAILURE		-2	/* Pool allocation failure */
#define V_DUP_NAME		-3	/* Duplicate device name */
#define V_BAD_PARAMS		-4	/* Bad request parameters */
#define V_ABORTED		-5	/* I/O request aborted */
#define V_NO_DEVICE		-6	/* Target device does not exist */
#define V_ILL_FUNCTION		-7	/* Illegal I/O function */
#define V_ILL_MODIFIER		-8	/* Illegal function modifier */
#define V_DEV_BUSY		-9	/* Device busy */
#define V_CIRCULAR		-10	/* Circular redirection */
#define V_CLASS_MISMATCH	-11	/* Class mismatch */
#define V_EOT			-12	/* End-of-transmission on device */
#define V_HARD_ERROR		-13	/* hardware error on device */
#define V_NO_UNITS		-14	/* no units for physical device open */
#define V_BAD_BUFFER		-15	/* bad buffer offset or address/size */
#define V_NO_READ		-16	/* device read not allowed */
#define V_NO_WRITE		-17	/* device write not allowed */
#define V_NO_CONTROL		-18	/* illegal device control function */
#define V_NOT_READY		-19	/* device not ready */
#define V_TIMEOUT		-20	/* request timeout */
#define V_DATA_OVERRUN		-21	/* data lost due to overrun */
#define V_EOF			-22	/* End-of-file encountered */
#define V_BREAK			-23	/* BREAK encountered */

#define V_IOP_BUSY		-64	/* no free IOP event semaphores */

#define V_SYS_WAIT		-65	/* Virtual OS wait condition */

/* define other VIOS global flags */
#define BLK_ADDR	1		/* Block-addressible */
#define REC_SEQ		2		/* Sequential records (& terminals) */
#define BUF_ADDR	3		/* Buffer-addressible */


#endif
