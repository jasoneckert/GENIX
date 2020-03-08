/*  vioslink.h -- Define constants for VIOS<->Target Operating System linkage
 *  copyright (c) American Information Systems Corporation
 *	Daniel Steinberg
 *	November, 1984
 */

#ifndef VIOSLINK
#define VIOSLINK

/* Define Target Operating System SVC codes */
#define FASTSVCS	1	/* start of simple svcs */
#define USER_STATE_SVC	1	/* changing to user-state */
#define INT_ENABLE_SVC	2	/* enable completion interrupts */
#define INT_DISABLE_SVC	3	/* disable completion ints */
#define MEMCHECK_SVC	4	/* check memory bitmap */
#define CONSOLE_MSG_SVC	5	/* print asciz msg on tty00 */

#define SLOWSVCS	10	/* start of svcs with parameters */
#define SET_KTRAP_SVC	10	/* set vectors for traps from kernel */
#define SET_UTRAP_SVC	11	/* set vectors for traps from user */
#define VIO_REQUEST_SVC	12	/* VIOS I/O request */
#define LASTSVC		12	/* last valid svc code */

#define VIO_ARGS	14	/* number of arguments to VIO_REQUEST_SVC */


#endif
