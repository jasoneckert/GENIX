/*
 * ieeio.h: version 2.2 of 11/12/82
 * Definitions for GPIB interface
 *
 * @(#)ieeio.h	2.2	(National Semiconductor)	11/12/82
 */

/* reply message values */
#define GOODSUM 0 
#define BADSUM -1 

/* gpib command request package */

#define BUFSIZE 5120 

struct ibcommand
{
	char	fcommand;	/* command byte */
	char	fname[80];	/* file name */
	int	i1st;		/* three integer fields */
	int	i2nd;
	int	i3rd;
	char	checkbyte;	/* check sum */
};

/* data block */

struct ibblock
{
	char	iopage[BUFSIZE];	/* transfer data */
	int	nbytes;		/* number of valid bytes in block */
	char	checkbyte;	/* check sum */
};
