/*
 * @(#)msgbuf.h	3.3	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */


#define	MSG_MAGIC	0x063060
#define	MSG_BSIZE	(MCSIZE - 2 * sizeof (long))
struct	msgbuf {
	long	msg_magic;
	long	msg_bufx;
	char	msg_bufc[MSG_BSIZE];
};
#ifdef KERNEL
struct	msgbuf msgbuf;
#endif
