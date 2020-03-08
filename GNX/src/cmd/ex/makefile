#
# makefile: version 1.13 of 8/17/83
# 
#
# @(#)makefile	1.13 (NSC) 8/17/83
#

.SUFFIXES: .o .c .s .x
VERSION	= 3.6
#
# Ex skeletal makefile for VAX VM/Unix version 7
#
# NB: This makefile doesn't indicate any dependencies on header files.
#
# Ex is very large - this version will not fit on PDP-11's without overlay
# software.  Things that can be turned off to save
# space include LISPCODE (-l flag, showmatch and lisp options), UCVISUAL
# (visual \ nonsense on upper case only terminals), CHDIR (the undocumented
# chdir command.)  CRYPT includes the code to edit encrypted files (the -x
# option, like ed.)  VMUNIX makes ex considerably larger, raising many limits
# and improving speed and simplicity of maintenance.  It is suitable only
# for a VAX or other large machine, and then probably only in a paged system.
#
# Don't define VFORK unless your system has the VFORK system call,
# which is like fork but the two processes have only one data space until the
# child execs. This speeds up ex by saving the memory copy.
#
# If your system expands tabs to 4 spaces you should -DTABS=4 below
#
SCCS	= /v/mesaux/src/cmd/ex
SCCSD	= sccs -d${SCCS}
RM	= -rm
STRIP	= strip

CC	= nmcc
BINDIR	= /v/mesaux/usr/ucb
LIBDIR	= /v/mesaux/usr/lib
XSTR	= /usr/ucb/xstr
MKSTR	= /usr/ucb/mkstr
CTAGS	= /usr/ucb/ctags
DEBUGFLAGS	= -DTRACE -g
NONDEBUGFLAGS	=
DEB	= ${NONDEBUGFLAGS}	# or ${DEBUGFLAGS} to debug
CEFLAGS =
CFLAGS	= -DTABS=8 -DVFORK ${DEB} -DSCCS ${CEFLAGS} #add back -DVMUNIX

AS	= nasm
AFLAGS	=
LIBES	= -ltermlib
CXREF	= /usr/ucb/cxref
INCLUDE	= /v/mesaux/include
PR	= pr
OBJS	= ex.o ex_addr.o ex_cmds.o ex_cmds2.o ex_cmdsub.o \
	ex_data.o ex_get.o ex_io.o ex_put.o ex_re.o \
	ex_set.o ex_subr.o ex_temp.o ex_tty.o ex_unix.o \
	ex_v.o ex_vadj.o ex_vget.o ex_vmain.o ex_voper.o \
	ex_vops.o ex_vops2.o ex_vops3.o ex_vput.o ex_vwind.o \
	printf.o bcopy.o strings.o
PROGS	= vi.x exrecover.x expreserve.x

HDRS	= ex.h ex_argv.h ex_re.h ex_temp.h ex_tty.h ex_tune.h ex_vars.h ex_vis.h
SFILES	= bcopy.s
SRC1	= ex.c ex_addr.c ex_cmds.c ex_cmds2.c ex_cmdsub.c
SRC2	= ex_data.c ex_get.c ex_io.c ex_put.c ex_re.c
SRC3	= ex_set.c ex_subr.c ex_temp.c ex_tty.c ex_unix.c
SRC4	= ex_v.c ex_vadj.c ex_vget.c ex_vmain.c ex_voper.c
SRC5	= ex_vops.c ex_vops2.c ex_vops3.c ex_vput.c ex_vwind.c
SRC6	= printf.c expreserve.c exrecover.c
SRCS	= ${HDRS} ${SFILES} ${SRC1} ${SRC2} ${SRC3} ${SRC4} ${SRC5} ${SRC6}
MISC	= makefile READ_ME rofix.ed
VGRIND	= csh /usr/ucb/vgrind
VHDR	= "Ex Version ${VERSION}"

foo:	${PROGS}

# install in standard place (/usr/ucb)
install: vi.x exrecover.x expreserve.x
	${RM} -f ${BINDIR}/ex
	${RM} -f ${BINDIR}/vi
	${RM} -f ${BINDIR}/view
	${RM} -f ${BINDIR}/edit
	${RM} -f ${BINDIR}/e
	minstall -s -x vi.x ${BINDIR}
# ifndef VMUNIX
	rm -f ${LIBDIR}/ex${VERSION}strings
	minstall ex${VERSION}strings ${LIBDIR}/ex${VERSION}strings
	chmod 666 ${LIBDIR}/ex${VERSION}strings
# endif
	ln ${BINDIR}/vi ${BINDIR}/edit
	ln ${BINDIR}/vi ${BINDIR}/e
	ln ${BINDIR}/vi ${BINDIR}/ex
	ln ${BINDIR}/vi ${BINDIR}/view
	chmod 1755 ${BINDIR}/ex
	minstall -s exrecover.x ${LIBDIR}/ex${VERSION}recover
	minstall -s expreserve.x ${LIBDIR}/ex${VERSION}preserve
	chmod 4755 ${LIBDIR}/ex${VERSION}recover ${LIBDIR}/ex${VERSION}preserve
# The following line normally fails.  This is OK.
#	-mkdir /v/mesaux/usr/preserve

clean:
#	If we dont have ex we cant make it so dont rm ex_vars.h
	${RM} -f ${PROGS} ex${VERSION}strings strings tags core errs trace
	${RM} -f *.o x*.[cs]

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS} rofix.ed makeoptions
${SRCS} rofix.ed makeoptions:
	${SCCSD} get $@

vi.x:	${OBJS}
	${RM} -f $@
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBES}
	chmod 770 $@

all:	${PROGS} tags

tags:	/tmp
	${CTAGS} -w ex.[hc] ex_*.[hc]

${OBJS}: ${HDRS}

# ex_vars.h: makeoptions
# 	csh makeoptions ${CFLAGS}

bcopy.o: bcopy.s
	${AS} ${AFLAGS} -o bcopy.o bcopy.s

# xstr: hands off!
strings.o: strings rofix.ed
	${XSTR}
	${CC} ${CFLAGS} -c -S xs.c
	ed - <rofix.ed xs.s
	${AS} -o strings.o xs.s
	rm xs.s
	
exrecover.x: exrecover.o
	${RM} -f $@
	${CC} ${CFLAGS} exrecover.o -o $@
	chmod 770 $@

exrecover.o: exrecover.c
	${CC} ${CFLAGS} -c exrecover.c

expreserve.x: expreserve.o
	${RM} -f $@
	${CC} ${CFLAGS} expreserve.o -o $@
	chmod 770 $@

expreserve.o:
	${CC} ${CFLAGS} -c expreserve.c

lint:
	lint ${CFLAGS} ex.c ex_?*.c
	lint ${CFLAGS} -u exrecover.c
	lint ${CFLAGS} expreserve.c

print:
	@${PR} READ* BUGS
	@${PR} makefile*
	@${PR} /etc/termcap
	@(size -l vi.x ; size *.o) | ${PR} -h sizes
	@${PR} -h errno.h ${INCLUDE}/errno.h
	@${PR} -h setjmp.h ${INCLUDE}/setjmp.h
	@${PR} -h sgtty.h ${INCLUDE}/sgtty.h
	@${PR} -h signal.h ${INCLUDE}/signal.h
	@${PR} -h sys/stat.h ${INCLUDE}/sys/stat.h
	@${PR} -h sys/types.h ${INCLUDE}/sys/types.h
	@ls -ls | ${PR}
	@${CXREF} *.c | ${PR} -h XREF
	@${PR} *.h *.c

vgrind:
	tee index < /dev/null
	${VGRIND} -h ${VHDR} ${HDRS}
	${VGRIND} -h ${VHDR} ${SRC1}
	${VGRIND} -h ${VHDR} ${SRC2}
	${VGRIND} -h ${VHDR} ${SRC3}
	${VGRIND} -h ${VHDR} ${SRC4}
	${VGRIND} -h ${VHDR} ${SRC5}
	${VGRIND} -h ${VHDR} ${SRC6}
	${VGRIND} -n -h ${VHDR} ${MISC}
	${VGRIND} -i -h ${VHDR} index

# Static dependencies.

# Handy dandy building rules.

.c.o:
# ifdef VMUNIX
#	${CC} -E ${CFLAGS} $*.c | ${XSTR} -c -
# else
	${MKSTR} - ex${VERSION}strings x $*.c
	${CC} -E ${CFLAGS} x$*.c | ${XSTR} -c -
	rm -f x$*.c
# endif
	${CC} ${CFLAGS} -c x.c 
	mv x.o $*.o

.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s

