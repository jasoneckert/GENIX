/*
 * @(#)assert.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

# ifndef NDEBUG
# define _assert(ex) {if (!(ex)){fprintf(stderr,"Assertion failed: file %s, line %d\n", __FILE__, __LINE__);exit(1);}}
# define assert(ex) {if (!(ex)){fprintf(stderr,"Assertion failed: file %s, line %d\n", __FILE__, __LINE__);exit(1);}}
# else
# define _assert(ex) ;
# define assert(ex) ;
# endif
