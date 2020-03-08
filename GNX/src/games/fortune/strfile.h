/*
 * @(#)strfile.h	1.1	9/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

# define	MAXDELIMS	3

struct	strfile {		/* information table			*/
	int	str_numstr;		/* number of strings in the file */
	int	str_longlen;		/* length of longest string	*/
	int	str_shortlen;		/* length of shortest string	*/
	long	str_delims[MAXDELIMS];	/* delimiter markings		*/
	int	str_unused;		/* reserve space for later needs */
};

typedef struct strfile	STRFILE;
