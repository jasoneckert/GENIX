/*
 * lookup.h: version 3.21 of 8/25/83
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

#define htablesize    (200)
#define stablesize   (2000)

struct sy_export *lookup();
struct sy_name   *nlookup();

#ifdef export

struct sy_export *htable[htablesize] = {0};
struct sy_name   *ntable[htablesize] = {0};
struct sy_name   *namespace=nil,*namelist=nil;
char	*strnext=nil,*strlast=nil;

#else

extern struct sy_export *htable[];
extern struct sy_name   *ntable[];
extern struct sy_name	*namespace,*namelist;
extern char *strnext,*strlast;

#endif
