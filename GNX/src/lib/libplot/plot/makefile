#
# makefile: version 1.5 of 11/8/83
# Unix Plot Library Source File
#
# @(#)makefile	1.5 (NSC) 11/8/83
#


.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/lib/libplot/plot
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
SRCS	= arc.c box.c circle.c close.c cont.c dot.c erase.c label.c line.c \
	  linmod.c move.c open.c point.c putsi.c space.c

all: plot.a 

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

plot.a:	${SRCS}
	${CC} ${CFLAGS} ${SRCS}
	${AR} rc plot.a *.o
	rm -f *.o

install:
	minstall -r plot.a ${RELDIR}/libplot.a
	${RANLIB} ${RELDIR}/libplot.a

clean:
	rm -f *.o plot.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile


