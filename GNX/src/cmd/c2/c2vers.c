/* c2vers.c - c2 object code improver...
	      module containing just version string */

static char ver_string[] = 
	"@(#)NSC c2 optimizer version 2.56 created 8/23/83 (jima)\n;Copyright \
(c) 1983 National Semiconductor Corporation.  All rights reserved.";

char *version = (&ver_string[4]);  /* omit what(1) tag when printing it out */
