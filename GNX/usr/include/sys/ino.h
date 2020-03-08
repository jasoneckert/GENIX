/*
 * @(#)ino.h	3.2	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Inode structure as it appears on
 * a disk block.
 */
struct dinode
{
	unsigned short	di_mode;     	/* mode and type of file */
	short	di_nlink;    	/* number of links to file */
	short	di_uid;      	/* owner's user id */
	short	di_gid;      	/* owner's group id */
	off_t	di_size;     	/* number of bytes in file */
	char  	di_addr[40];	/* disk block addresses */
	time_t	di_atime;   	/* time last accessed */
	time_t	di_mtime;   	/* time last modified */
	time_t	di_ctime;   	/* time created */
};
#define	INOPB	16	/* 16 inodes per BSIZE block */
/*
 * the 40 address bytes:
 *	39 used; 13 addresses
 *	of 3 bytes each.
 */
