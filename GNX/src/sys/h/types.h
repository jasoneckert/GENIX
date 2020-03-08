/*
 * @(#)types.h	3.3	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Basic system types and major/minor device constructing/busting macros.
 */

# ifndef major	/* If already included, don't do it again... */

/* major part of a device */
#define	major(x)	((int)(((unsigned)(x)>>8)&0377))

/* minor part of a device */
#define	minor(x)	((int)((x)&0377))

/* make a device number */
#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;

/*typedef	struct	_physadr { int r[1]; } *physadr;*/
typedef	int	daddr_t;
typedef	char *	caddr_t;
typedef	u_short ino_t;
typedef	int	swblk_t;
typedef	int	size_t;
typedef	int	time_t;

/*	The kernel context of a user process. These are saved by save,
	and restored by resume. Three instances of this structure exist
	in the u. area, they are u.rsav, u.ssav, u.qsav...
*/

typedef struct kcontext {
	int	r7;
	int	r6;
	int	r5;
	int	r4;
	int	r3;
	int	sp;
	int	pc;
	int	mod;
	int	fp;
} label_t;

typedef	short	dev_t;
typedef	long	off_t;

# endif	/* major */
