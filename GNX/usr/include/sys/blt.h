/*
 * @(#)blt.h	3.2	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Maximum x and y coordinates in the pixel coordinate system:
 *	(0,0) is at the lower left corner of the display.
 */
# define XMAX	607
# define YMAX	799

typedef struct rectangle {
	int	x,
		y,
		bw,
		bh;
} rectangle;

typedef struct form {
	rectangle	r;
	char		*base;	/* Perhaps should be caddr_t... */
	int		xinc;
} form;

typedef enum bltops { mov, bic, or, and, xor } bltops;
