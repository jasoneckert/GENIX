#
# makefile: version 1.5 of 8/15/83
# Unix System Command Source File
#
# @(#)makefile	1.5 (NSC) 8/15/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/compact
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc

CEFLAGS	=
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= ccat.sh compact.c compact.h tree.c uncompact.c

all: compact.x uncompact.x ccat.sh

install: compact.x uncompact.x ccat.sh
	minstall -s -x compact.x uncompact.x ccat.sh ${DESTDIR}/usr/ucb
	chmod 755 ${DESTDIR}/usr/ucb/ccat

clean:
	rm -f *.x *.o

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

compact.x: compact.o tree.o
	rm -f $@
	${CC} ${CFLAGS} compact.o tree.o -o compact.x
	chmod 770 $@

uncompact.x: uncompact.o tree.o
	rm -f $@
	${CC} ${CFLAGS} uncompact.o tree.o -o uncompact.x
	chmod 770 $@

tree.c compact.c uncompact.c: compact.h


# Handy dandy building rules.

.c.o:
	${CC} ${CFLAGS} -c $*.c
