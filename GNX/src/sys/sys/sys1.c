/*
 * sys1.c: version 3.43 of 11/11/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)sys1.c	3.43 (NSC) 11/11/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/buf.h"
#include "../h/psr.h"
#include "../h/reg.h"
#include "../h/vm.h"
#include "../h/inode.h"
#include "../h/file.h"
#include "../h/mmu.h"
#include "../h/vlimit.h"
#include <wait.h>

extern	int	acct();
extern	int	maxuprc;	/* maximum processes allowed per user */
struct	vtimes	zvms;		/* zeroed structure, for initialization */

/*
 * exec system call, with and without environments.
 */
struct execa {
	char	*fname;
	char	**argp;
	char	**envp;
};

exec()
{
	((struct execa *)u.u_ap)->envp = NULL;
	exece();
}

exece()
{
	register nc;
	register char *cp;
	register char **rp;
	register struct execa *uap;
	int na, ne, bno, ucp, ap, c;
	int indir, uid, gid;
	char *sharg;
	struct inode *ip;
	char cfname[DIRSIZ];
	char cfarg[SHSIZE];
	char *data[((NCARGS+MCOFSET)/MCSIZE)+1];	/* arg storage */
	extern int schar();

	if ((ip = namei(uchar, 0)) == NULL)
		return;
	data[0] = 0;
	indir = 0;
	uid = u.u_uid;
	gid = u.u_gid;

	if (ip->i_mode & ISUID)
		uid = ip->i_uid;
	if (ip->i_mode & ISGID)
		gid = ip->i_gid;

again:
	if(access(ip, IEXEC))
		goto bad;
	if((u.u_procp->p_flag&STRC) && access(ip, IREAD))
		goto bad;
	if((ip->i_mode & IFMT) != IFREG ||
	   (ip->i_mode & (IEXEC|(IEXEC>>3)|(IEXEC>>6))) == 0) {
		u.u_error = EACCES;
		goto bad;
	}

	/*
	 * Read in first few bytess of file for segment sizes, ux_magic:
	 *	407 (OMAGIC) = plain executable
	 *	410 (NMAGIC) = RO text
	 *	413 (ZMAGIC) = demand paged RO text
	 * Also an ASCII line beginning with #! is
	 * the file name of a ``shell'' and arguments may be prepended
	 * to the argument list if given here.
	 *
	 * SHELL NAMES ARE LIMITED IN LENGTH.
	 *
	 * ONLY ONE ARGUMENT MAY BE PASSED TO THE SHELL FROM
	 * THE ASCII LINE.
	 */
	u.u_base = (caddr_t)&u.u_exdata;
	u.u_count = sizeof(u.u_exdata);
	u.u_offset = 0;
	u.u_segflg = 1;
	readi(ip);
	u.u_segflg = 0;
	if (u.u_error)
		goto bad;
	if (u.u_count > sizeof(u.u_exdata) - sizeof(u.u_exdata.Ux_A)
		&& u.u_exdata.ux_shell[0] != '#') {
		u.u_error = ENOEXEC;
		goto bad;
	}
	switch (u.u_exdata.ux_magic) {

	case OMAGIC:
		u.u_exdata.ux_data += u.u_exdata.ux_text;
		u.u_exdata.ux_text = 0;
		break;

	case NMAGIC:
	case ZMAGIC:
		if (u.u_exdata.ux_text == 0) {
			u.u_error = ENOEXEC;
			goto bad;
		}
		break;

	default:
		if (u.u_exdata.ux_shell[0] != '#' ||
		    u.u_exdata.ux_shell[1] != '!' ||
		    indir) {
			u.u_error = ENOEXEC;
			goto bad;
		}
		cp = &u.u_exdata.ux_shell[2];		/* skip "#!" */
		while (cp < &u.u_exdata.ux_shell[SHSIZE]) {
			if (*cp == '\t')
				*cp = ' ';
			else if (*cp == '\n') {
				*cp = '\0';
				break;
			}
			cp++;
		}
		if (*cp != '\0') {
			u.u_error = ENOEXEC;
			goto bad;
		}
		cp = &u.u_exdata.ux_shell[2];
		while (*cp == ' ')
			cp++;
		u.u_dirp = cp;
		while (*cp && *cp != ' ')
			cp++;
		sharg = NULL;
		if (*cp) {
			*cp++ = '\0';
			while (*cp == ' ')
				cp++;
			if (*cp) {
				bcopy((caddr_t)cp, (caddr_t)cfarg, SHSIZE);
				sharg = cfarg;
			}
		}
		bcopy((caddr_t)u.u_dbuf, (caddr_t)cfname, DIRSIZ);
		indir = 1;
		iput(ip);
		ip = namei(schar, 0);
		if (ip == NULL)
			return;
		goto again;
	}

	/*
	 * Collect arguments into the data pages.
	 */
	na = 0;
	ne = 0;
	nc = 0;
	uap = (struct execa *)u.u_ap;
	if (uap->argp) for (;;) {
		ap = NULL;
		if (indir && (na == 1 || na == 2 && sharg))
			ap = (int)uap->fname;
		else if (uap->argp) {
			ap = fuword((caddr_t)uap->argp);
			uap->argp++;
		}
		if (ap==NULL && uap->envp) {
			uap->argp = NULL;
			if ((ap = fuword((caddr_t)uap->envp)) == NULL)
				break;
			uap->envp++;
			ne++;
		}
		if (ap==NULL)
			break;
		na++;
		if(ap == -1)
			u.u_error = EFAULT;
		do {
			if (nc >= NCARGS-1)
				u.u_error = E2BIG;
			if (indir && na == 2 && sharg != NULL)
				c = *sharg++ & 0377;
			else if ((c = fubyte((caddr_t)ap++)) < 0)
				u.u_error = EFAULT;
			if (u.u_error)
				goto bad;
			if ((nc % MCSIZE) == 0) {
				cp = (char *) kallocpage(0);
				if (((int)cp) < 0) {
					u.u_error = ENOMEM;	/* swap full */
					goto bad;
				}
				data[nc/MCSIZE] = cp;
				data[(nc/MCSIZE)+1] = 0;
			}
			nc++;
			*cp++ = c;
		} while (c>0);
	}
	nc = (nc + NBPW-1) & ~(NBPW-1);
	if (indir)
		bcopy((caddr_t)cfname, (caddr_t)u.u_dbuf, DIRSIZ);
	getxfile(ip, nc + (na+4)*NBPW, uid, gid);
	if (u.u_error)
		goto bad;

	/*
	 * copy back arglist
	 */

	ucp = TOPUSER - nc - NBPW;
	ap = ucp - na*NBPW - 3*NBPW;
	u.u_ar0[RSP] = ap;
	(void) suword((caddr_t)ap, na-ne);	/* argc ... */
	rp = data; cp = *rp; bno = ucp;
	while(nc) {				/* copyout strings */
		c = (nc < MCSIZE ? nc : MCSIZE);
		copyout(cp,ucp,c);
		cp = *++rp;
		ucp += c;
		nc -= c;
	}
	rp = data; cp = *rp; ucp = bno;
	for (;;) {				/* generate arg pointers */
		ap += NBPW;
		if (na==ne) {
			suword((caddr_t)ap, 0);
			ap += NBPW;
		}
		if (--na < 0)
			break;
		(void) suword((caddr_t)ap, ucp);
		do {
			if (((int)cp & MCOFSET) == 0)
				cp = *rp++;
			ucp++;
		} while(*cp++);
	}
	(void) suword((caddr_t)ucp, 0);		/* terminate string space */
	(void) suword((caddr_t)ap, 0);		/* terminate pointer space */
	setregs();
bad:
	for (nc = 0; data[nc]; nc++) kfreepage(data[nc]);
	iput(ip);
}

/*
 * Read in and set up memory for executed file.
 */
getxfile(ip, nargc, uid, gid)
	register struct inode *ip;
{
	register unsigned long addr;
	register long pagadr;
	register unsigned ts;		/* text size */
	register unsigned ds;		/* data size */
	unsigned bs;			/* unitialized data size */
	unsigned ss;			/* stack size */

	ts = u.u_exdata.ux_text;
	ds = u.u_exdata.ux_data;
	bs = u.u_exdata.ux_bss;
	ss = SSIZE + btoc(nargc);

	/* See if the file is as large as it needs to be */
	if ((ts + ds + 1024) > ip->i_size) {
		u.u_error = EFAULT;
		return;
	}

	/* See if there is enough memory for the program */
	if (xchecksize(ts, ds, bs, ss)) {
		u.u_error = ENOMEM;	/* not enough */
		return;
	}

	/*
	 * allocate and clear core
	 * at this point, committed
	 * to the new image
	 */

	if (u.u_procp->p_flag & SVFORK) {	/* if was a vfork child */
		/* tell parent his new size */
		proc[u.u_spti].p_tdsize = u.u_tsize + u.u_dsize;
		setspti(u.u_procp - proc);	/* restore our true spt index */
		u.u_procp->p_flag &= ~SVFORK;
		wakeup((caddr_t)u.u_procp);	/* allow parent to continue */
	}

	/* attach to the text and data segments */
	if (xattach(u.u_spti, ip, ts, ds)) {
		return;			/* error reading text */
	}

	/* allocate stack */
	for (addr = ((TOPUSER-ss) & ~MCOFSET); addr < TOPUSER; addr += MCSIZE) {
		while ((pagadr = alloczeropage(u.u_spti,addr,PTY_DATA)) <= 0) {
			if (pagadr == 0) continue;
			swapkill(u.u_procp, 0);
			u.u_error = ENOMEM;
			return;
		}
		setmap(u.u_spti, addr, PTEMEM(pagadr, PG_RWRW));
		unlockaddr(pagadr);
	}

	/* no longer need any reserved memory */
	availpages += spt[u.u_spti].spt_reserved;
	spt[u.u_spti].spt_reserved = 0;

	/*
	 * set SUID/SGID protections, if no tracing
	 */
	if ((u.u_procp->p_flag&STRC)==0) {
		    if(u.u_uid != 0) {
			    u.u_uid = uid;
			    u.u_procp->p_uid = uid;
		    }
		    u.u_gid = gid;
	} else
		psignal(u.u_procp, SIGTRAP);
	u.u_tsize = ts;
	u.u_dsize = ds + bs;
	u.u_ssize = ss;
}

/*
 * Clear registers on exec
 */
setregs()
{
	register int (**rp)();
	int (**end)() = &u.u_signal[NSIG];
	register long sigmask;
	register struct user *up = &u;
	register i;
	register int *uregs;

	mtpr(ptb1, spt[up->u_spti].spt_pte1);
	u.u_sigcatch = SIG_DFL;
	for(rp = &up->u_signal[1], sigmask = 1L; rp < end; sigmask<<=1, rp++) {
		switch (*rp) {

		case SIG_IGN:
		case SIG_DFL:
		case SIG_HOLD:
			continue;

		default:
			/*
			 * Normal or deferring catch; revert to default.
			 */
			(void) spl6();
			*rp = SIG_DFL;
			if ((int)SIG_DFL & 1)
				up->u_procp->p_siga0 |= sigmask;
			else
				up->u_procp->p_siga0 &= ~sigmask;
			if ((int)SIG_DFL & 2)
				up->u_procp->p_siga1 |= sigmask;
			else
				up->u_procp->p_siga1 &= ~sigmask;
			(void) spl0();
			continue;
		}
	}
#ifdef nothing
	Eliminate the register clearing
	for(cp = &regloc[0]; cp < &regloc[8];) /* only zero general regs */
		up->u_ar0[*cp++] = 0;
#endif
#define	entrymod up->u_exdata.ux_entry_mod
	uregs = up->u_ar0;
	uregs[RPC] = up->u_exdata.ux_entry;
	uregs[RPSRMOD] = ((PSR_S|PSR_I|PSR_U) << 16) + (entrymod & 0xffff);
	/* stack pointer setup in exec to point to args.... */
	uregs[RFP] = uregs[RSP];
	for(i=0; i<NOFILE; i++) {
		if (up->u_pofile[i]&EXCLOSE) {
			closef(up->u_ofile[i]);
			up->u_ofile[i] = NULL;
			up->u_pofile[i] &= ~EXCLOSE;
		}
	}
	/*
	 * remember command name for accounting
	 */
	bcopy((caddr_t)up->u_dbuf, (caddr_t)up->u_comm, DIRSIZ);
	bcopy((caddr_t)up->u_dbuf, (caddr_t)up->u_procp->p_infoname, DIRSIZ);
}

/*
 * exit system call:
 * pass back caller's arg
 */
rexit()
{
	register struct a {
		int	rval;
	} *uap;

	uap = (struct a *)u.u_ap;
	exit((uap->rval & 0377) << 8);
}

/*
 * Release resources.
 * Save u. area for parent to look at.
 * Enter zombie state.
 * Wake up parent and init processes,
 * and dispose of children.
 */
exit(rv)
{
	register int i;
	register struct proc *p, *q;
	register struct file *f;
	register int x;

	p = u.u_procp;
	p->p_flag &= ~STRC;
	p->p_flag |= SWEXIT;
	p->p_clktim = 0;
	(void) spl6();
	if ((int)SIG_IGN & 1)
		p->p_siga0 = ~0L;
	else
		p->p_siga0 = 0L;
	if ((int)SIG_IGN & 2)
		p->p_siga1 = ~0L;
	else
		p->p_siga1 = 0L;
	(void) spl0();
	for(i=0; i<NSIG; i++)
		u.u_signal[i] = SIG_IGN;
	p->p_cpticks = 0;
	p->p_pctcpu = 0;
	if (p->p_flag & SVFORK) {		/* if was a vfork child */
		/* tell parent his new size */
		proc[u.u_spti].p_tdsize = u.u_tsize + u.u_dsize;
		setspti(p - proc);		/* restore our true spt index */
		p->p_flag &= ~SVFORK;
		wakeup((caddr_t)p);		/* let parent proceed */
	}
	clearptetable(u.u_spti);
	xblast(&spt[u.u_spti]);
	if (u.u_xspti >= 0) {
		if (--spt[u.u_xspti].spt_proccount == 0) {
			xdorm(u.u_xspti);
		}
	}
	u.u_xspti = -1;
	for(i=0; i<NOFILE; i++) {
		f = u.u_ofile[i];
		u.u_ofile[i] = NULL;
		closef(f);
	}
	plock(u.u_cdir);
	iput(u.u_cdir);
	if (u.u_rdir) {
		plock(u.u_rdir);
		iput(u.u_rdir);
	}
	u.u_limit[LIM_FSIZE] = INFINITY;  /* allow write to accounting file */
	acct();
	p->p_stat = SZOMB;
	noproc = 1;
	i = PIDHASH(p->p_pid);
	x = p - proc;
	if (pidhash[i] == x)
		pidhash[i] = p->p_idhash;
	else {
		for (i = pidhash[i]; i != 0; i = proc[i].p_idhash)
			if (proc[i].p_idhash == x) {
				proc[i].p_idhash = p->p_idhash;
				goto done;
			}
		panic("exit");
	}
	if (p->p_pid == 1)
		panic("init died");
done:
	p->xp_xstat = rv;
	p->xp_vm = u.u_vm;
	vmsadd(&p->xp_vm, &u.u_cvm);
	for(q = proc; q < procNPROC; q++)
		if(q->p_pptr == p) {
			q->p_pptr = &proc[1];
			q->p_ppid = 1;
			wakeup((caddr_t)&proc[1]);
			/*
			 * Traced processes are killed
			 * since their existence means someone is screwing up.
			 * Stopped processes are sent a hangup and a continue.
			 * This is designed to be ``safe'' for setuid
			 * processes since they must be willing to tolerate
			 * hangups anyways.
			 */
			if (q->p_flag&STRC) {
				q->p_flag &= ~STRC;
				psignal(q, SIGKILL);
			} else if (q->p_stat == SSTOP) {
				psignal(q, SIGHUP);
				psignal(q, SIGCONT);
			}
			/*
			 * Protect this process from future
			 * tty signals, clear TSTP/TTIN/TTOU if pending,
			 * and set SDETACH bit on procs.
			 */
			spgrp(q, -1);
		}
	wakeup((caddr_t)p->p_pptr);
	psignal(p->p_pptr, SIGCHLD);
	swtch();
}

/*
 * Wait system call.
 * Search for a terminated (zombie) child,
 * finally lay it to rest, and collect its status.
 * Look also for stopped (traced) children,
 * and pass back status from them.
 */
wait()
{
	struct	vtimes	vm;
	register struct a {
		int	*stat;
		int	opt;
		struct	vtimes	*vtimep;
	}	*uap;		/* pointer to args for this call */

	uap = (struct a *)u.u_ap;
	if (((int)uap->stat & 0x80000000) == 0) {
		wait1(0, (struct vtimes *)0);
		return;
	}
	wait1(uap->opt, &vm);
	if (u.u_error || (uap->vtimep == 0)) return;
	if (copyout((caddr_t)&vm, uap->vtimep, sizeof(struct vtimes)) < 0) {
		u.u_error = EFAULT;
		return;
	}
}

wait1(options,vp)
	register options;
	struct	vtimes	*vp;
{
	register f;
	register struct proc *p;

	f = 0;
loop:
	for(p = proc; p < procNPROC; p++)
	if(p->p_pptr == u.u_procp) {
		f++;
		if(p->p_stat == SZOMB) {
			u.u_r.r_val1 = p->p_pid;
			u.u_r.r_val2 = p->xp_xstat;
			p->xp_xstat = 0;
			if (vp) *vp = p->xp_vm;
			vmsadd(&u.u_cvm, &p->xp_vm);
			p->xp_vm = zvms;
			p->p_pptr = 0;
			p->p_siga0 = 0L;
			p->p_siga1 = 0L;
			p->p_cursig = 0;
			p->p_pid = 0;
			p->p_ppid = 0;
			p->p_pgrp = 0;
			p->p_sig = 0;
			p->p_flag = 0;
			p->p_wchan = 0;
			p->p_stat = NULL;
			return;
		}
		if (p->p_stat == SSTOP && (p->p_flag&SWTED)==0 &&
		    (p->p_flag&STRC
				|| options&WUNTRACED
		    			)) {
			p->p_flag |= SWTED;
			u.u_r.r_val1 = p->p_pid;
			u.u_r.r_val2 = (p->p_cursig<<8) | 0177;
			return;
		}
	}
	if(f) {
		if (options & WNOHANG) {
			u.u_r.r_val1 = 0;
			return;
		} else {
			if ((u.u_procp->p_flag&SNUSIG) && save(&u.u_qsav)) {
				u.u_eosys = RESTARTSYS;
				return;
			}
			sleep((caddr_t)u.u_procp, PWAIT);
			goto loop;
		}
	}
	u.u_error = ECHILD;
}

/*
 * fork system call.
 */
fork()
{
	fork1(0);
}

fork1(isvfork)
{
	register struct proc *p1, *p2;
	register a;

	a = 0;
	p2 = NULL;
	for(p1 = proc; p1 < procNPROC; p1++) {
		if (p1->p_stat==NULL && p2==NULL)
			p2 = p1;
		else {
			if (p1->p_uid==u.u_uid && p1->p_stat!=NULL)
				a++;
		}
	}
	/*
	 * Disallow if
	 *  No processes at all;
	 *  not su and too many procs owned (or in pgrp, if UCB_PGRP set);
	 *  or not su and would take last slot.
	 */
	if (p2==NULL || (u.u_uid!=0 && (p2==&proc[nproc-1] || a>=maxuprc))) {
		u.u_error = EAGAIN;
		return;
	}
	p1 = u.u_procp;
	if(newproc(isvfork)) {
		u.u_r.r_val1 = p1->p_pid;
		u.u_r.r_val2 = 1;
		u.u_start = time;
		u.u_vm = zvms;
		u.u_cvm = zvms;
		bcopy((caddr_t)u.u_comm, (caddr_t)p2->p_infoname, DIRSIZ);
		return;
	}
	u.u_r.r_val1 = p2->p_pid;
	u.u_r.r_val2 = 0;
}

/*
 * break system call.
 *  -- bad planning: "break" is a dirty word in C.
 */
sbreak()
{
	struct a {
		char	*nsiz;
	};

	register long	oldbreak;		/* old break address */
	register long	newbreak;		/* new break address */
	register long	datasize;		/* new data segment size */
	register long	stacksize;		/* current stack size */

	newbreak = (int) ((struct a *)u.u_ap)->nsiz;
	if (newbreak < (int) u.u_tsize) {	/* shrinking into text */
		u.u_error = EFAULT;
		return;
	}
	datasize = newbreak - u.u_tsize;
	oldbreak = u.u_tsize + u.u_dsize;
	if (oldbreak >= newbreak) {		/* shrinking memory */
		newbreak = (newbreak + MCOFSET) & ~MCOFSET;
		while (oldbreak >= newbreak) {
			clearmap(u.u_spti, oldbreak, PTLEVEL2);
			oldbreak -= MCSIZE;
		}
		u.u_dsize = datasize;
		return;
	}

	/* Growing memory.  Fail if no swap space or going above data limit */

	stacksize = TOPUSER - (u.u_ar0[RSP] & ~MCOFSET);
	if (stacksize < 0) stacksize = 0;
	if ((((unsigned)(datasize + stacksize)) > u.u_limit[LIM_DATA])
		|| ((newbreak - oldbreak + MCOFSET)/MCSIZE > availpages)) {
			u.u_error = ENOMEM;
			return;
	}
	u.u_dsize = datasize;
}
