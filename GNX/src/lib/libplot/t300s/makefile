#
# makefile: version 1.5 of 11/8/83
# Unix Plot Library Source File
#
# @(#)makefile	1.5 (NSC) 11/8/83
#

.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/lib/libplot/t300s
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AR	= ar
RANLIB	= nranlib

CEFLAGS =
CFLAGS	= -c -O -DSCCS ${CEFLAGS}

SRCS	= arc.c box.c circle.c close.c con.h dot.c erase.c label.c line.c \
	  linmod.c move.c open.c point.c space.c subr.c

all: t300s.a

t300s.a:${SRCS}
	rm -f t300s.a
	${CC} ${CFLAGS} ${SRCS}
	${AR} rc t300s.a *.o
	rm -f *.o

install: all
	minstall -r t300s.a ${RELDIR}/lib300s.a
	${RANLIB} ${RELDIR}/lib300s.a

clean:
	rm -f t300s.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@
