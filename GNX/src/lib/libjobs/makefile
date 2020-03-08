#
# makefile: version 1.10 of 8/27/83
# 
#
# @(#)makefile	1.10 (NSC) 8/27/83
#


.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/lib/libjobs
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

SRCS	= getwd.c killpg.s setpgrp.s signal.s sigset.c wait3.s
OBJS	= getwd.o killpg.o setpgrp.o signal.o sigset.o wait3.o

all:	jobs.a
jobs.a: ${OBJS}
	${AR} cr jobs.a ${OBJS}

install: jobs.a
	minstall -r jobs.a ${RELDIR}/libjobs.a

clean:
	-rm -f ${OBJS} jobs.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

# Handy dandy building rules.

.c.o:
	${CC} ${CFLAGS} $*.c

.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s
