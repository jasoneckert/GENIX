/*
 * symbols.h: version 3.22 of 9/9/83
 * 
 */

/*
 * Copyright (C) 1982 by National Semiconductor Corporation
 *
 *	National Semiconductor Corporation
 *	2900 Semiconductor Drive
 *	Santa Clara, California 95051
 *
 *	All Rights Reserved
 *
 * This software is furnished under a license and may be used and copied only
 * in accordance with the terms of such license and with the inclusion of the
 * above copyright notice.  This software or any other copies thereof may not
 * be provided or otherwise made available to any other person.  No title to
 * or ownership of the software is hereby transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by National Semiconductor
 * Corporation.
 *
 * National Semiconductor Corporation assumes no responsibility for the use
 * or reliability of its software on equipment configurations that are not
 * supported by National Semiconductor Corporation.
 */

#ifdef export

int	symtotal=0,nsymbs=0;
int	glstringsiz=0;
char	*stringbuf=nil;
#ifdef	ON16K
char	*symname = "/tmp/meldXXXXXX";
#else	/* VAX, KERN or CUST */
char	*symname = "/tmp/meldXXXXXX";
#endif
char 	*getname(), *doname();

#else

extern int	symtotal,nsymbs;
extern int	glstringsiz;
extern char	*stringbuf;
extern char 	*symname;
extern file	*symfile;
extern char 	*getname(), *doname();
extern int	opensymfile(), outsymbol(), writesymbols(),
		next_sym(), do_symbols();

#endif
