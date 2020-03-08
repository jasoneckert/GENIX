#
# makefile: version 1.8 of 8/17/83
# Unix System Command Source File
#
# @(#)makefile	1.8 (NSC) 8/17/83
#

.SUFFIXES: .o .c .S .x

SCCS	= /v/mesaux/src/cmd/ed
DESTDIR	= /mesa
SCCSD	= sccs -d${SCCS}

CC	= nmcc

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= ed.c

all: ed.x

install:
	minstall -s -x ed.x ${DESTDIR}/bin
	rm -f ${DESTDIR}/bin/e
	ln ${DESTDIR}/bin/ed ${DESTDIR}/bin/e

clean:
	rm -f ed.x ed.o

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.x:	$*.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@
