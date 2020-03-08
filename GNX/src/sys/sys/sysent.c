/*
 * sysent.c: version 3.13 of 8/27/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)sysent.c	3.13 (NSC) 8/27/83";
# endif

#include "../h/param.h"
#include "../h/systm.h"

/*
 * This table is the switch used to transfer
 * to the appropriate routine for processing a system call.
 * Each row contains the number of arguments expected
 * and a pointer to the routine.
 */

int	alarm(),	chdir(),	chmod(),	chown();
int	chroot(),	close(),	creat(),	dup();
int	exec(),		exece(),	fork(),		fstat();
int	getgid(),	getpid(),	getuid(),	gtime();
int	ioctl(),	kill(),		link(),		mknod();
int	nice(),		nosys(),	nullsys(),	open();
int	pause(),	pipe(),		ptrace(),	read();
int	reboot(),	rexit(),	saccess(),	sbreak();
int	seek(),		setgid(),	setpgrp(),	setuid();
int	smount(),	ssig(),		stat(),		stime();
int	sumount(),	ftime(),	sync(),		times();
int	umask(),	unlink(),	utime(),	wait();
int	write();

int	vfork();	/* fork efficiently */
int	vhangup();	/* virtually "hangup" the current control terminal */
int	vlimit();	/* limit resource consumption */
int	vlock();	/* lock into memory */
int	vmap();		/* page map handling */
int	vspy();		/* spy on physical memory */
int	vtimes();	/* return process statistics */
int	sysacct();	/* turn accounting off/on */
int	sethostname(), gethostname();	/* from 4.2, set and read host name */
int	getpagesize();			/* from 4.2, return page size */

struct sysent sysent[] =
{
	0, nosys,			/*  0 = indir (MUST be nosys) */
	1, rexit,			/*  1 = exit */
	0, fork,			/*  2 = fork */
	3, read,			/*  3 = read */
	3, write,			/*  4 = write */
	2, open,			/*  5 = open */
	1, close,			/*  6 = close */
	3, wait,			/*  7 = wait (up to three args now) */
	2, creat,			/*  8 = creat */
	2, link,			/*  9 = link */
	1, unlink,			/* 10 = unlink */
	2, exec,			/* 11 = exec */
	1, chdir,			/* 12 = chdir */
	0, gtime,			/* 13 = time */
	3, mknod,			/* 14 = mknod */
	2, chmod,			/* 15 = chmod */
	3, chown,			/* 16 = chown; now 3 args */
	1, sbreak,			/* 17 = break */
	2, stat,			/* 18 = stat */
	3, seek,			/* 19 = seek; now 3 args */
	0, getpid,			/* 20 = getpid */
	3, smount,			/* 21 = mount */
	1, sumount,			/* 22 = umount */
	1, setuid,			/* 23 = setuid */
	0, getuid,			/* 24 = getuid */
	2, stime,			/* 25 = stime */
	4, ptrace,			/* 26 = ptrace */
	1, alarm,			/* 27 = alarm */
	2, fstat,			/* 28 = fstat */
	0, pause,			/* 29 = pause */
	2, utime,			/* 30 = utime */
	0, nullsys,			/* 31 = stty --- use ioctl */
	0, nullsys,			/* 32 = gtty --- use ioctl */
	2, saccess,			/* 33 = access */
	1, nice,			/* 34 = nice */
	1, ftime,			/* 35 = ftime; formerly sleep */
	0, sync,			/* 36 = sync */
	2, kill,			/* 37 = kill */
	3, vspy,			/* 38 = spy on physical memory */
	2, setpgrp,			/* 39 = setpgrp */
	0, nosys,			/* 40 = tell (obsolete) */
	2, dup,				/* 41 = dup */
	0, pipe,			/* 42 = pipe */
	1, times,			/* 43 = times */
	0, nullsys,			/* 44 = prof -- not implemented */
	0, nosys,			/* 45 = unused */
	1, setgid,			/* 46 = setgid */
	0, getgid,			/* 47 = getgid */
	2, ssig,			/* 48 = sig */
	2, vlock,			/* 49 = lock page in memory */
	0, nosys,			/* 50 = reserved for USG */
	1, sysacct,			/* 51 = turn accting off/on */
	0, nosys,			/* 52 = phys -- not implemented */
	0, nosys,			/* 53 = lock -- not implemented */
	3, ioctl,			/* 54 = ioctl */
	1, reboot,			/* 55 = reboot */
	0, nosys,			/* 56 = mpxchan -- banished */
	0, nosys,			/* 57 = reserved for USG */
	1, nosys,			/* 58 = local system calls  -- NA*/
	3, exece,			/* 59 = exece */
	1, umask,			/* 60 = umask */
	1, chroot,			/* 61 = chroot */
	0, vfork,			/* 62 = vfork */
	0, nosys,			/* 63 = used internally */
	2, vtimes,			/* 64 = vtimes */
	0, vhangup,			/* 65 = vhangup */
	2, vlimit,			/* 66 = vlimit */
	3, read,			/* 67 = vread (fake with read) */
	3, write,			/* 68 = vwrite (fake with write) */
	4, vmap,			/* 69 = vmap */
	2, sethostname,			/* 70 = sethostname */
	2, gethostname,			/* 71 = gethostname */
	0, getpagesize			/* 72 = getpagesize */
};

int sysentsize = sizeof(sysent) / sizeof(sysent[0]);	/* number of svcs */
