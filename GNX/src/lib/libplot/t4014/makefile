#
# makefile: version 1.6 of 11/8/83
# Unix Plot Library Source File
#
# @(#)makefile	1.6 (NSC) 11/8/83
#


.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/lib/libplot/t4014
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
SRCS	= arc.c box.c circle.c close.c dot.c erase.c label.c line.c \
	  linemod.c move.c open.c point.c scale.c space.c subr.c

all: t4014.a

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

t4014.a:${SRCS}
	${CC} ${CFLAGS} ${SRCS}
	${AR} rc t4014.a *.o
	rm -f *.o

install: all
	minstall -r t4014.a ${RELDIR}/lib4014.a
	${RANLIB} ${RELDIR}/lib4014.a

clean:
	rm -f t4014.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

