#
# makefile: version 1.11 of 8/27/83
# 
#
# @(#)makefile	1.11 (NSC) 8/27/83
#

.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/lib/libm
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib
SCCSD	= sccs -d${SCCS}

AS	= nasm
CC	= nmcc
RANLIB	= nranlib

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}
AFLAGS	=

SRCS	= getfsr.s setfsr.s \
	asin.c atan.c hypot.c jn.c j0.c j1.c pow.c gamma.c \
	log.c sin.c sqrt.c tan.c tanh.c sinh.c erf.c exp.c floor.c

OBJS	= getfsr.o setfsr.o \
	asin.o atan.o hypot.o jn.o j0.o j1.o pow.o gamma.o \
	log.o sin.o sqrt.o tan.o tanh.o sinh.o erf.o exp.o floor.o

libm.a : ${OBJS}
	ar rc libm.a ${OBJS}

install: libm.a
	minstall -r libm.a ${RELDIR}

clean:
	rm -f ${OBJS} libm.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:
	${CC} ${CFLAGS} -c $*.c

.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s
