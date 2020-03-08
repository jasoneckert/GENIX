#
# makefile: version 1.5 of 11/8/83
# Unix Plot Library Source File
#
# @(#)makefile	1.5 (NSC) 11/8/83
#


.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/lib/libplot/t450
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AS	= nasm
AR	= ar
RANLIB	= nranlib

CEFLAGS =
CFLAGS	= -c -DSCCS ${CEFLAGS}
AFLAGS	=
SRCS	= arc.c box.c circle.c close.c con.h dot.c erase.c label.c line.c \
	  linmod.c move.c open.c point.c space.c subr.c

all: t450.a

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

t450.a:	${SRCS}
	${CC} ${CFLAGS} ${SRCS}
	${AR} rc t450.a *.o
	rm -f *.o

install: all
	minstall -r t450.a ${RELDIR}/lib450.a
	${RANLIB} ${RELDIR}/lib450.a

clean:
	rm -f t450.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

