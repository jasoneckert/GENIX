/*  osiopkt.h -- Define VIOS<->Target Operating System I/O structures
 *  copyright (c) American Information Systems Corporation
 *	Daniel Steinberg
 *	November, 1984
 *
 */
#ifndef OSIOPKT
#define OSIOPKT

/* Define the i/o packet structures that are used by Target O.S. */

typedef struct
	{
	unsigned nc_type : 4;	/* general type of function */
	unsigned nc_code : 4;	/* specific function code */
	unsigned nc_mod  : 8;	/* modifier: extra information */
	}  PKT_FUNCTION;

typedef struct
	{
	short	code;		/* success/error code */
	short	aux_code;	/* reserved */				/*??*/
	int	bcount;		/* byte count of transfer */
	int	aux_stat2;	/* auxilliary status info */
	int	aux_stat3;	/* aux info */
	int	aux_stat4;	/* aux info */
	}  PKT_IOSTATUS;


typedef union
	{
	struct
	    {
	    unsigned  count : 14;	/* magnitude */
	    unsigned  scale : 2;	/* units */
	    }  tm;
	unsigned tmu;
	}  PKT_TIMEOUT;


/* define physical address buffer flag */
#define PHYS_BUFFER NULL


#endif
