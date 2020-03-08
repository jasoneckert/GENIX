#
# makefile: version 1.10 of 7/14/83
# GENIX system makefile
#
# @(#)makefile	1.10 (NSC) 7/14/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/lib/libmp
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc
CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= gcd.c madd.c mdiv.c mout.c msqrt.c mult.c pow.c util.c
OBJS	= pow.o gcd.o msqrt.o mdiv.o mout.o mult.o madd.o util.o

all:	libmp.a
libmp.a: ${OBJS}
	ar cr libmp.a ${OBJS}

install: libmp.a
#	minstall -c libmp.a ${DESTDIR}/usr/lib
	minstall -r libmp.a ${RELDIR}

clean:
	rm -f *.o libmp.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:
	${CC} ${CFLAGS} -c $*.c
