
# @(#)makefile	1.3	8/29/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/libc/crt
SCCSD	= sccs -d${SCCS}

AS	= nasm
CC	= nmcc

CEFLAGS =
CFLAGS	= -O -F -DSCCS ${CEFLAGS}
AFLAGS	=

SRCS	= cerror.s mcount.s

OBJS	= cerror.o mcount.o

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
