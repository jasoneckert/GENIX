/*
 * rdwri.c: version 3.4 of 6/7/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)rdwri.c	3.4 (NSC) 6/7/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/inode.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/vlimit.h"

/*
 * Read the file corresponding to
 * the inode pointed at by the argument.
 * The actual read arguments are found
 * in the variables:
 *	u_base		core address for destination
 *	u_offset	byte offset in file
 *	u_count		number of bytes to read
 *	u_segflg	read to kernel/user/user I
 */
readi(ip)
register struct inode *ip;
{
	struct buf *bp;
	dev_t dev;
	daddr_t lbn, bn;
	off_t diff;
	register on, n;
	register type;

	if(u.u_count == 0)
		return;
	if(u.u_offset < 0) {
		u.u_error = EINVAL;
		return;
	}
	ip->i_flag |= IACC;
	dev = (dev_t)ip->i_un.i_rdev;
	type = ip->i_mode&IFMT;
	if (type==IFCHR) {
		return((*cdevsw[major(dev)].d_read)(dev));
	}

	do {
		lbn = bn = u.u_offset >> BSHIFT;
		on = u.u_offset & BMASK;
		n = MIN((unsigned)(BSIZE-on), u.u_count);
		if (type!=IFBLK) {
			diff = ip->i_size - u.u_offset;
			if (diff <= 0)
				return;
			if (diff < n)
				n = diff;
			bn = bmap(ip, bn, B_READ);
			if (u.u_error)
				return;
			dev = ip->i_dev;
		} else
			rablock = bn+1;
		if ((long)bn<0) {
			bp = geteblk();
			clrbuf(bp);
		} else if (ip->i_un.i_lastr+1==lbn)
			bp = breada(dev, bn, rablock);
		else
			bp = bread(dev, bn);
		ip->i_un.i_lastr = lbn;
		n = MIN(n, BSIZE-bp->b_resid);
		if (n!=0)
			iomove(bp->b_un.b_addr+on, n, B_READ);
		if (n+on==BSIZE || u.u_offset==ip->i_size) {
			if (ip->i_flag&IPIPE)
				bp->b_flags &= ~B_DELWRI;	/* cancel i/o */
			bp->b_flags |= B_AGE;
		}
		brelse(bp);
	} while(u.u_error==0 && u.u_count!=0 && n!=0);
}

/*
 * Write the file corresponding to
 * the inode pointed at by the argument.
 * The actual write arguments are found
 * in the variables:
 *	u_base		core address for source
 *	u_offset	byte offset in file
 *	u_count		number of bytes to write
 *	u_segflg	write to kernel/user/user I
 */
writei(ip)
register struct inode *ip;
{
	struct buf *bp;
	dev_t dev;
	daddr_t bn;
	register n, on;
	register type;

	if(u.u_offset < 0) {
		u.u_error = EINVAL;
		return;
	}
	dev = (dev_t)ip->i_un.i_rdev;
	type = ip->i_mode&IFMT;
	if (type==IFCHR) {
		ip->i_flag |= IUPD|ICHG;
		(*cdevsw[major(dev)].d_write)(dev);
		return;
	}
	if (u.u_count == 0)
		return;
	if ((ip->i_mode&IFMT)==IFREG &&
	     (u.u_offset + u.u_count > u.u_limit[LIM_FSIZE])) {
		psignal(u.u_procp, SIGXFSZ);
		u.u_error = EMFILE;
		return;
	}
	do {
		bn = u.u_offset >> BSHIFT;
		on = u.u_offset & BMASK;
		n = MIN((unsigned)(BSIZE-on), u.u_count);
		if (type!=IFBLK) {
			bn = bmap(ip, bn, B_WRITE);
			if((long)bn<0)
				return;
			dev = ip->i_dev;
		}
		if (n == BSIZE)
			bp = getblk(dev, bn);
		else if (bdevsw[major(dev)].d_tab->b_flags & B_TAPE) {
			bp = getblk(dev, bn);
			clrbuf(bp);
		} else
			bp = bread(dev, bn);
		iomove(bp->b_un.b_addr+on, n, B_WRITE);
		if(u.u_error != 0) {
			brelse(bp);
		} else {
			if ((ip->i_mode&IFMT) == IFDIR &&
			    ((struct direct *)(bp->b_un.b_addr+on))->d_ino == 0)
				/*
				 * Writing to clear a directory entry.
				 * Must insure the write occurs before
				 * the inode is freed, or may end up
				 * pointing at a new (different) file
				 * if inode is quickly allocated again
				 * and system crashes.
				 */
				bwrite(bp);
			else if (n+on==BSIZE && (ip->i_flag&IPIPE) == 0) {
				bp->b_flags |= B_AGE;
				bawrite(bp);
			} else
				bdwrite(bp);
		}
		if(u.u_offset > ip->i_size &&
		   (type==IFDIR || type==IFREG))
			ip->i_size = u.u_offset;
		ip->i_flag |= IUPD|ICHG;
	} while(u.u_error==0 && u.u_count!=0);
}


/*
 * Move n bytes at byte location
 * &bp->b_un.b_addr[o] to/from (flag) the
 * user/kernel area starting at u.base.
 * Update all the arguments by the number
 * of bytes moved.
 */
iomove(cp, n, flag)
register caddr_t cp;
register n;
{
	register t;

	if (n==0)
		return;
	if (u.u_segflg != 1) {
		if (flag==B_WRITE)
			t = copyin(u.u_base, (caddr_t)cp, n);
		else 
			t = copyout((caddr_t)cp, u.u_base, n);
		if (t) {
			u.u_error = EFAULT;
			return;
		}
	} else
		if (flag == B_WRITE)
			bcopy(u.u_base, (caddr_t)cp, n);
		else
			bcopy((caddr_t)cp, u.u_base, n);
	u.u_base += n;
	u.u_offset += n;
	u.u_count -= n;
}
