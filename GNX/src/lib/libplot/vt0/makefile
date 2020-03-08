#
# makefile: version 1.5 of 11/8/83
# Unix Plot Library Source File
#
# @(#)makefile	1.5 (NSC) 11/8/83
#


.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/lib/libplot/vt0
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AS	= nasm
AR	= ar
RANLIB	= nranlib

CEFLAGS =
CFLAGS	= -c -O -DSCCS ${CEFLAGS}
AFLAGS	=
SRCS	= arc.c box.c circle.c close.c dot.c erase.c frame.c label.c line.c \
	  linmod.c move.c open.c point.c space.c subr.c

all: vt0.a

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

vt0.a:	${SRCS}
	${CC} ${CFLAGS} ${SRCS}
	${AR} rc vt0.a *.o
	rm -f *.o

install: all
	minstall -r vt0.a ${RELDIR}/libvt0.a
	${RANLIB} ${RELDIR}/libvt0.a

clean:
	rm -f vt0.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

