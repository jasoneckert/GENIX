/*
 * scan.h: version 1.1 of 3/21/83
 * 
 *
 * @(#)scan.h	1.1	(National Semiconductor)	3/21/83
 */

/* Definitions for self-terminating scanning module */

#define	SCAN_SIZE	200	/* maximum line length */
#define	SCAN_ABORT	1000	/* setjmp return code for aborted command */
#define	SCAN_ERROR	1001	/* return code saying an error occurred */
#define	SCAN_EDIT	1002	/* return code saying editing occurred */
#define	SCAN_EOF	1003	/* return code saying no more input ready */

int	scaninit();		/* Initialize for reading */
int	scanchar();		/* Read next character */
int	scanbackup();		/* Put back last character */
int	scanabort();		/* Abort current command */
int	scaneof();		/* Say no more data for current command */

extern	char	scanbuffer[SCAN_SIZE+1];	/* buffer holding chars */
extern	char	*scanreadptr;			/* current read pointer */
extern	char	*scanwriteptr;			/* current write pointer */
extern	char	(*scanroutine)();		/* input read routine */
