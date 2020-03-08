
# @(#)makefile	1.4	8/27/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.
#

.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/libc/csu
SCCSD	= sccs -d${SCCS}

AS	= nasm
CC	= nmcc

CEFLAGS =
CFLAGS	= -O -F -DSCCS ${CEFLAGS}
AFLAGS	=

SRCS	= crt0.s mcrt0.s

OBJS	= crt0.o mcrt0.o

all: ${OBJS}

clean:
	rm -f ${OBJS}

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:
	${CC} ${CFLAGS} -c $*.c
.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s
