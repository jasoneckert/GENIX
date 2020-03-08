#
# makefile: version 1.9 of 11/8/83
# System III libPW makefile
#
# @(#)makefile	1.9 (NSC) 11/8/83
#

# WARNING!
#	To avoid naming conflicts between System III and 4.1 library
#	routines, some of the routines in this library have been renamed.
#	Any System III program making use of such routines must be
#	changed to use the new names.  Affected routines are:
#		index	->	PWindex
#
#	The routine alloca is not portable to the 16k and therefore is
#	not included in the 16k version of libPW.  Programs using it
#	should be rewritten to use xalloc/xfree.

.SUFFIXES: .o

DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib

# Source directories:
CSCCS	= /v/mesaux/src/lib/libPW
SSCCS	= /v/mesaux/src/lib/libPW

SSCCSD	= sccs -d${SSCCS}
CSCCSD	= sccs -d${CSCCS}

CC	= nmcc
AS	= nasm
RANLIB	= nranlib
CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS} 

INSTALL	= minstall
INSFLAGS= -r

CFILES	= \
	anystr.c bal.c curdir.c fdfopen.c giveup.c			\
	imatch.c PWindex.c lockit.c logname.c        patoi.c		\
	patol.c regcmp.c regex.c rename.c repeat.c repl.c satoi.c	\
	setsig.c sname.c strend.c substr.c trnslat.c userdir.c		\
	username.c verify.c any.c xalloc.c xcreat.c xlink.c		\
	xopen.c xpipe.c xunlink.c xwrite.c xmsg.c         		\
	cat.c dname.c fatal.c clean.c userexit.c zero.c zeropad.c

SFILES	= move.s

OFILES	= \
	anystr.o bal.o curdir.o fdfopen.o giveup.o			\
	imatch.o PWindex.o lockit.o logname.o move.o patoi.o		\
	patol.o regcmp.o regex.o rename.o repeat.o repl.o satoi.o	\
	setsig.o sname.o strend.o substr.o trnslat.o userdir.o		\
	username.o verify.o any.o xalloc.o xcreat.o xlink.o		\
	xopen.o xpipe.o xunlink.o xwrite.o xmsg.o         		\
	cat.o dname.o fatal.o clean.o userexit.o zero.o zeropad.o

all:	libPW.a
libPW.a: ${OFILES}
	rm -f libPW.a
	ar r libPW.a ${OFILES}

install: libPW.a
	${INSTALL} ${INSFLAGS} libPW.a ${RELDIR}

clean:
	rm -f ${OFILES} libPW.a

clobber: clean
	${SSCCSD} clean; ${SSCCSD} get makefile
	${CSCCSD} clean; ${SSCCSD} get makefile

sources: ${CFILES} ${SFILES}
${CFILES}:
	${CSCCSD} get $@
${SFILES}:
	${SSCCSD} get $@

.c.o:
	${CC} -c ${CFLAGS} $*.c

.s.o:
	${AS} -o $*.o $*.s
