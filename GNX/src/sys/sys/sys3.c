/*
 * sys3.c: version 3.12 of 5/26/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)sys3.c	3.12 (NSC) 5/26/83";
# endif


#include "../h/param.h"
#include "../h/systm.h"
#include "../h/mount.h"
#include "../h/ino.h"
#include "../h/reg.h"
#include "../h/buf.h"
#include "../h/filsys.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/inode.h"
#include "../h/file.h"
#include "../h/conf.h"
#include "../h/stat.h"

/*
 * the fstat system call.
 */
fstat()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		struct stat *sb;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	stat1(fp->f_inode, uap->sb, fp->f_flag&FPIPE? fp->f_un.f_offset: 0);
}

/*
 * the stat system call.
 */
stat()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		struct stat *sb;
	} *uap;

	uap = (struct a *)u.u_ap;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	stat1(ip, uap->sb, (off_t)0);
	iput(ip);
}

/*
 * The basic routine for fstat and stat:
 * get the inode and pass appropriate parts back.
 */
stat1(ip, ub, pipeadj)
register struct inode *ip;
struct stat *ub;
off_t pipeadj;
{
	register struct dinode *dp;
	register struct buf *bp;
	struct stat ds;

	iupdat(ip, &time, &time, 0);
	/*
	 * first copy from inode table
	 */
	ds.st_dev = ip->i_dev;
	ds.st_ino = ip->i_number;
	ds.st_mode = ip->i_mode;
	ds.st_nlink = ip->i_nlink;
	ds.st_uid = ip->i_uid;
	ds.st_gid = ip->i_gid;
	ds.st_rdev = (dev_t)ip->i_un.i_rdev;
	ds.st_size = ip->i_size - pipeadj;
	/*
	 * next the dates in the disk
	 */
	bp = bread(ip->i_dev, itod(ip->i_number));
	dp = bp->b_un.b_dino;
	dp += itoo(ip->i_number);
	ds.st_atime = dp->di_atime;
	ds.st_mtime = dp->di_mtime;
	ds.st_ctime = dp->di_ctime;
	brelse(bp);
	if (copyout((caddr_t)&ds, (caddr_t)ub, sizeof(ds)) < 0)
		u.u_error = EFAULT;
}

/*
 * the dup system call.
 */
dup()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		int	fdes2;
	} *uap;
	register i, m;

	uap = (struct a *)u.u_ap;
	m = uap->fdes & ~077;
	uap->fdes &= 077;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if ((m&0100) == 0) {
		if ((i = ufalloc()) < 0)
			return;
	} else {
		i = uap->fdes2;
		if (i<0 || i>=NOFILE) {
			u.u_error = EBADF;
			return;
		}
		u.u_r.r_val1 = i;
	}
	if (i!=uap->fdes) {
		if (u.u_ofile[i]!=NULL)
			closef(u.u_ofile[i]);
		u.u_ofile[i] = fp;
		fp->f_count++;
	}
}

extern dev_t getmdev();
/*
 * the mount system call.
 */
smount()
{
	dev_t dev;
	register struct inode *ip;
	register struct mount *mp;
	struct mount *smp;
	register struct filsys *fp;
	struct buf *bp;
	register struct a {
		char	*fspec;
		char	*freg;
		int	ronly;
	} *uap;
	register char *cp;

	uap = (struct a *)u.u_ap;
	dev = getmdev();
	if(u.u_error)
		return;
	u.u_dirp = (caddr_t)uap->freg;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	if(ip->i_count!=1 || (ip->i_mode&(IFBLK&IFCHR))!=0)
		goto out;
	smp = NULL;
	for(mp = mount; mp < mountNMOUNT; mp++) {
		if(mp->m_bufp != NULL) {
			if(dev == mp->m_dev)
				goto out;
		} else
		if(smp == NULL)
			smp = mp;
	}
	mp = smp;
	if(mp == NULL)
		goto out;
	(*bdevsw[major(dev)].d_open)(dev, !uap->ronly);
	if(u.u_error)
		goto out;
	bp = bread(dev, SUPERB);
	if(u.u_error) {
		brelse(bp);
		goto out1;
	}
	mp->m_inodp = ip;
	mp->m_dev = dev;
	bp->b_flags |= B_LOCKED;
	mp->m_bufp = bp;
	fp = bp->b_un.b_filsys;
	fp->s_ilock = 0;
	fp->s_flock = 0;
	fp->s_ronly = uap->ronly & 1;
	fp->s_nbehind = 0;
	fp->s_lasti = 1;
	u.u_dirp = uap->freg;
	for (cp = fp->s_fname; cp < &fp->s_fname[sizeof (fp->s_fname) - 1]; )
		if ((*cp++ = uchar()) == 0)
			u.u_dirp--;		/* get 0 again */
	*cp = 0;
	brelse(bp);
	ip->i_flag |= IMOUNT;
	prele(ip);
	return;

out:
	u.u_error = EBUSY;
out1:
	iput(ip);
}

/*
 * the umount system call.
 */
sumount()
{
	dev_t dev;
	register struct inode *ip;
	register struct mount *mp;
	struct buf *bp;
	int stillopen, flag;
	register struct a {
		char	*fspec;
	};

	dev = getmdev();
	if(u.u_error)
		return;
	xunmount(dev);
	update();
	for(mp = mount; mp < mountNMOUNT; mp++)
		if(mp->m_bufp != NULL && dev == mp->m_dev)
			goto found;
	u.u_error = EINVAL;
	return;

found:
	stillopen = 0;
	for(ip = inode; ip < inodeNINODE; ip++)
		if (ip->i_number != 0 && dev == ip->i_dev) {
			u.u_error = EBUSY;
			return;
		} else if (ip->i_number != 0 && (ip->i_mode&IFMT) == IFBLK &&
		    ip->i_un.i_rdev == dev)
			stillopen++;
	ip = mp->m_inodp;
	ip->i_flag &= ~IMOUNT;
	plock(ip);
	iput(ip);
	if ((bp = getblk(dev, SUPERB)) != mp->m_bufp)
		panic("umount");
	bp->b_flags &= ~B_LOCKED;
	flag = !bp->b_un.b_filsys->s_ronly;
	mp->m_bufp = NULL;
	brelse(bp);
	/* mpurge(mp - &mount[0]); we don't need to purge text maps */
	if (!stillopen) {
		(*bdevsw[major(dev)].d_close)(dev, flag);
		binval(dev);
	}
}

/*
 * Common code for mount and umount.
 * Check that the user's argument is a reasonable
 * thing on which to mount, and return the device number if so.
 */
dev_t
getmdev()
{
	register dev_t dev;
	register struct inode *ip;

	if (!suser())
		return(NODEV);
	ip = namei(uchar, 0);
	if(ip == NULL)
		return(NODEV);
	if((ip->i_mode&IFMT) != IFBLK)
		u.u_error = ENOTBLK;
	dev = (dev_t)ip->i_un.i_rdev;
	if(major(dev) >= nblkdev)
		u.u_error = ENXIO;
	iput(ip);
	return(dev);
}

/* The following calls are taken from version 4.2 */

char	hostname[32];		/* name of this node */
int	hostnamelen;		/* size of the name */

gethostname()
{
	register struct a {
		char	*hostname;
		int	len;
	} *uap = (struct a *)u.u_ap;
	register u_int len;

	len = uap->len;
	if (len > hostnamelen)
		len = hostnamelen;
	u.u_error = copyout((caddr_t)hostname, (caddr_t)uap->hostname, len);
}

sethostname()
{
	register struct a {
		char	*hostname;
		u_int	len;
	} *uap = (struct a *)u.u_ap;

	if (!suser())
		return;
	if (uap->len > sizeof (hostname) - 1) {
		u.u_error = EINVAL;
		return;
	}
	hostnamelen = uap->len;
	u.u_error = copyin((caddr_t)uap->hostname, hostname, uap->len);
	hostname[hostnamelen] = 0;
}


/* Return the page size */
getpagesize()
{
	u.u_r.r_val1 = MCSIZE;
}
