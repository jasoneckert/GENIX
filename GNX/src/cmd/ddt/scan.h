
/*
 * scan.h: version 1.1 of 11/17/82
 * 
 *
 * @(#)scan.h	1.1	(National Semiconductor)	11/17/82
 */

/* Definitions for self-terminating scanning module */

#define ESC 		0x1b	/* ESC is a special character appears as '$' */
#define	SCAN_SIZE	200	/* maximum line length */

int	scaninit();		/* Initialize for reading */
int	scanread();		/* Read next character */
int	scanbackup();		/* Put back last character */
int	scanabort();		/* Abort current command */

extern	char	scanbuffer[SCAN_SIZE+1];	/* buffer holding chars */
extern	char	*scanreadptr;			/* current read pointer */
extern	char	*scanwriteptr;			/* current write pointer */
extern	char	(*scanroutine)();		/* input read routine */
