#
# makefile: version 1.7 of 8/15/83
# Unix System Command Source File
#
# @(#)makefile	1.7 (NSC) 8/15/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/dc
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS 	= dc.h dc.c

all: dc.x

install:
	minstall -s -x dc.x ${DESTDIR}/usr/bin

clean:
	rm -f *.o dc.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

dc.c: dc.h

.c.x:	$*.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@
