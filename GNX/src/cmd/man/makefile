#
# makefile: version 1.5 of 8/17/83
# Mesa Unix System Command Source File
#
# @(#)makefile	1.5 (NSC) 8/17/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/man
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= man.c man.sh

all:	man.x man.sh

install:
	minstall -s -x man.sh ${DESTDIR}/usr/bin/man
	minstall -s -x man.x ${DESTDIR}/usr/ucb/man

clean:
	rm -f man.x man.o

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.x:	$*.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@
