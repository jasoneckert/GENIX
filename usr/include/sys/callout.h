/*
 * @(#)callout.h	1.4	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */


/*
 * The callout structure is for
 * a routine arranging
 * to be called by the clock interrupt
 * (clock.c) with a specified argument,
 * in a specified amount of time.
 * Used, for example, to time tab
 * delays on typewriters.
 */

struct	callout {
	int	c_time;		/* incremental time */
	caddr_t	c_arg;		/* argument to routine */
	int	(*c_func)();	/* routine */
	struct	callout *c_next;
};
#ifdef KERNEL
extern	struct	callout *callfree, *callout, calltodo;
extern	int	ncallout;
#endif
