#
# makefile: version 1.21 of 9/11/83
# Mesa Unix C Library Makefile
#
# @(#)makefile	1.21 (NSC) 9/11/83
#

.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/libc/sys
SCCSD	= sccs -d${SCCS}

AS	= nasm
CC	= nmcc

CEFLAGS =
CFLAGS	= -O -F -DSCCS ${CEFLAGS}
AFLAGS	=

SRCS	= _exit.s abs.s abort.s access.s acct.s alarm.s \
	chdir.s chmod.s chown.s chroot.s close.s creat.s dup.s		\
	exec.s execl.s execle.s execv.s execve.s exit.s fork.s fstat.s	\
	fabs.s getcsw.s getgid.s gethostname.s getpagesize.s getpid.s \
	getuid.s ioctl.s\
	kill.s link.s lseek.s makdir.s mdate.s mknod.s mount.s nice.s	\
	open.s pause.s pipe.s prof.c ptrace.s read.s sbrk.s setgid.s	\
	sethostname.s setjmp.s setuid.s sigentry.s signal.s stat.s stime.s \
	sync.s syscall.s time.s times.s umask.s umount.s unlink.s utime.s \
	vfork.s vhangup.s vlimit.s vlock.s vmap.s vread.s vspy.s	\
	vtimes.s vwrite.s wait.s write.s

OBJS	= _exit.o abs.o abort.o access.o acct.o alarm.o \
	chdir.o chmod.o chown.o chroot.o close.o creat.o dup.o		\
	exec.o execl.o execle.o execv.o execve.o exit.o fork.o fstat.o	\
	fabs.o getcsw.o getgid.o gethostname.o getpagesize.o getpid.o	\
	getuid.o ioctl.o\
	kill.o link.o lseek.o makdir.o mdate.o mknod.o mount.o nice.o	\
	open.o pause.o pipe.o prof.o ptrace.o read.o sbrk.o setgid.o	\
	sethostname.o setjmp.o setuid.o sigentry.o signal.o stat.o stime.o \
	sync.o syscall.o time.o times.o umask.o umount.o unlink.o utime.o \
	vfork.o vhangup.o vlimit.o vlock.o vmap.o vread.o vspy.o	\
	vtimes.o vwrite.o wait.o write.o

all: ${OBJS}

clean:
	rm -f ${OBJS}

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:
	${CC} ${CFLAGS} -c $*.c
.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s
