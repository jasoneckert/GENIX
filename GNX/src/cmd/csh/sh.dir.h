/*
 * sh.dir.h: version 3.1 of 12/13/82
 * Csh with job control
 *
 * @(#)sh.dir.h	3.1	(National Semiconductor)	12/13/82
 */

/*
 * Structure for entries in directory stack.
 */
struct	directory	{
	struct	directory *di_next;	/* next in loop */
	struct	directory *di_prev;	/* prev in loop */
	unsigned short *di_count;	/* refcount of processes */
	char	*di_name;		/* actual name */
};
struct directory *dcwd;		/* the one we are in now */
