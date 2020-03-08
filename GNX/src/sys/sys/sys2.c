/*
 * sys2.c: version 3.8 of 8/27/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)sys2.c	3.8 (NSC) 8/27/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/reg.h"
#include "../h/file.h"
#include "../h/inode.h"
#include "../h/proc.h"

/*
 * read system call
 */
read()
{
	rdwr(FREAD);
}

/*
 * write system call
 */
write()
{
	rdwr(FWRITE);
}

/*
 * common code for read and write calls:
 * check permissions, set base, count, and offset,
 * and switch out to readi, writei, or pipe code.
 */
rdwr(mode)
register mode;
{
	register struct file *fp;
	register struct inode *ip;
	register struct a {
		int	fdes;
		char	*cbuf;
		unsigned count;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if((fp->f_flag&mode) == 0) {
		u.u_error = EBADF;
		return;
	}
	u.u_base = (caddr_t)uap->cbuf;
	u.u_count = uap->count;
	u.u_segflg = 0;
	if ((u.u_procp->p_flag&SNUSIG) && save(&u.u_qsav)) {
		if (u.u_count == uap->count)
			u.u_eosys = RESTARTSYS;
	} else
	if((fp->f_flag&FPIPE) != 0) {
		if(mode == FREAD)
			readp(fp);
		else
			writep(fp);
	} else {
		ip = fp->f_inode;
		u.u_offset = fp->f_un.f_offset;
		if((ip->i_mode&(IFCHR&IFBLK)) == 0)
			plock(ip);
		if(mode == FREAD)
			readi(ip);
		else
			writei(ip);
		if((ip->i_mode&(IFCHR&IFBLK)) == 0)
			prele(ip);
		fp->f_un.f_offset += uap->count-u.u_count;
	       }
	u.u_r.r_val1 = uap->count-u.u_count;
}

/*
 * open system call
 */
open()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	rwmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	open1(ip, ++uap->rwmode, 0);
}

/*
 * creat system call
 */
creat()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	ip = namei(uchar, 1);
	if(ip == NULL) {
		if(u.u_error)
			return;
		ip = maknode(uap->fmode&07777&(~ISVTX));
		if (ip==NULL)
			return;
		open1(ip, FWRITE, 2);
	} else
		open1(ip, FWRITE, 1);
}

/*
 * common code for open and creat.
 * Check permissions, allocate an open file structure,
 * and call the device open routine if any.
 */
open1(ip, mode, trf)
register struct inode *ip;
register mode;
{
	register struct file *fp;
	int i;

	if(trf != 2) {
		if(mode&FREAD)
			access(ip, IREAD);
		if(mode&FWRITE) {
			access(ip, IWRITE);
			if((ip->i_mode&IFMT) == IFDIR)
				u.u_error = EISDIR;
		}
	}
	if(u.u_error)
		goto out;
	if(trf == 1)
		itrunc(ip);
	prele(ip);
	if ((fp = falloc()) == NULL)
		goto out;
	fp->f_flag = mode&(FREAD|FWRITE);
	fp->f_inode = ip;
	i = u.u_r.r_val1;
	openi(ip, mode&FWRITE);
	if(u.u_error == 0)
		return;
	u.u_ofile[i] = NULL;
	fp->f_count--;

out:
	iput(ip);
}

/*
 * close system call
 */
close()
{
	register struct file *fp;
	register struct a {
		int	fdes;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	u.u_ofile[uap->fdes] = NULL;
	closef(fp);
}

/*
 * seek system call
 */
seek()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		off_t	off;
		int	sbase;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if(fp->f_flag&FPIPE) {
		u.u_error = ESPIPE;
		return;
	}
	if(uap->sbase == 1)
		uap->off += fp->f_un.f_offset;
	else if(uap->sbase == 2)
		uap->off += fp->f_inode->i_size;
	fp->f_un.f_offset = uap->off;
	u.u_r.r_off = uap->off;
}

/*
 * link system call
 */
link()
{
	register struct inode *ip, *xp;
	register struct a {
		char	*target;
		char	*linkname;
	} *uap;

	uap = (struct a *)u.u_ap;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	if((ip->i_mode&IFMT)==IFDIR && !suser())
		goto out1;
	
	ip->i_nlink++;
	ip->i_flag |= ICHG;
	iupdat(ip, &time, &time, 1);
	prele(ip);
	u.u_dirp = (caddr_t)uap->linkname;
	xp = namei(uchar, 1);
	if(xp != NULL) {
		u.u_error = EEXIST;
		iput(xp);
		goto out;
	}
	if (u.u_error)
		goto out;
	if(u.u_pdir->i_dev != ip->i_dev) {
		iput(u.u_pdir);
		u.u_error = EXDEV;
		goto out;
	}
	wdir(ip);

out:
	if (u.u_error) {
		ip->i_nlink--;
		ip->i_flag |= ICHG;
	}
out1:
	iput(ip);
}

/*
 * mknod system call
 */
mknod()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
		int	dev;
	} *uap;

	uap = (struct a *)u.u_ap;
	if(suser()) {
		ip = namei(uchar, 1);
		if(ip != NULL) {
			u.u_error = EEXIST;
			goto out;
		}
	}
	if(u.u_error)
		return;
	ip = maknode(uap->fmode);
	if (ip == NULL)
		return;
	if (uap->dev) {
		/*
		 * Want to be able to use this to make badblock
		 * inodes, so don't truncate the dev number.
		 */
		ip->i_un.i_rdev = uap->dev;
		ip->i_flag |= IACC|IUPD|ICHG;
	}

out:
	iput(ip);
}

/*
 * access system call
 */
saccess()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
	} *uap;
	register svuid, svgid;

	uap = (struct a *)u.u_ap;
	svuid = u.u_uid;
	svgid = u.u_gid;
	u.u_uid = u.u_ruid;
	u.u_gid = u.u_rgid;
	ip = namei(uchar, 0);
	if (ip != NULL) {
		if (uap->fmode&(IREAD>>6))
			access(ip, IREAD);
		if (uap->fmode&(IWRITE>>6))
			access(ip, IWRITE);
		if (uap->fmode&(IEXEC>>6))
			access(ip, IEXEC);
		iput(ip);
	}
	u.u_uid = svuid;
	u.u_gid = svgid;
}
