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

/*
 * prof.c: version 1.2 of 1/25/83
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)prof.c	1.2 (NSC) 1/25/83";
# endif


/* profil Unix call.  It is really more complicated because
   we did not implement the standard profil Unix call.  We
   implemented a function that is much simpler for the kernel
   that causes SIGPROF interrupts every clock tick rather than
   updating the table in the kernel.  This has several advantages:
   1) other forms of profiling may be used that take different
   kinds of data (e.g. stack trace), and 2) we do not lose
   clock ticks due to the profiling table not being in memory
 */
#include <signal.h>

static short *sbuf;
static int sscale;
static int stop_profile = 0;
static int sbufsize;
static int soffset;
static prof_catcher();

profil(buff, bufsiz, offset, scale)
  char *buff;
  int bufsiz, offset, scale;
 {
   if ((scale == 0) || (scale == 1)) {
       stop_profile = 1;
       return(0);
   }
   sscale = scale << 15;
   sbuf = (short *) buff;
   sbufsize = bufsiz/2;
   signal(SIGPROF, prof_catcher);
   return(0);
}

static int seccount;
static prof_catcher() {
  register long pc;
  register long scale;
  asm("movd 32(fp),r7");
  pc -= soffset;
  scale = sscale;
  asm("movd r6,r0");
  asm("meid r7,r0");
  asm("movd r1,r7");
  if ((pc >= 0) && (pc < sbufsize)) ++(sbuf[pc]);
  if (++seccount == 120) { seccount = 0;
	printf("120 clicks\n");
  }
  if (stop_profile == 0) signal(SIGPROF, prof_catcher);
}
