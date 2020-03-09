/*
 * @(#)mount.h	3.5	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Mount structure.
 * One allocated on every mount.
 * Used to find the super block.
 */
struct	mount
{
	dev_t	m_dev;		/* device mounted */
	struct buf *m_bufp;	/* pointer to superblock */
	struct inode *m_inodp;	/* pointer to mounted on inode */
};

#define SBSIZE	(sizeof(struct filsys))	/* superblock size */
# ifdef KERNEL
extern struct mount *mount;
extern struct mount *mountNMOUNT;
extern int nmount;
# endif
