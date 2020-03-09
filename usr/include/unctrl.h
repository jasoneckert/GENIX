/*
 * @(#)unctrl.h	1.3	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * unctrl.h
 *	This file is part of the curses package.
 */

extern char	*_unctrl[];

# define	unctrl(ch)	(_unctrl[(unsigned) ch])
