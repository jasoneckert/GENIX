#
# makefile: version 1.8 of 8/16/83
# Unix System Command Source File
#
# @(#)makefile	1.8 (NSC) 8/16/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/ls
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= ls.c ucbls.c

all: ls.x ucbls.x

install:
	minstall -s -x ls.x /mesa/bin
	minstall -s ucbls.x ${DESTDIR}/usr/ucb/ls
	rm -f ${DESTDIR}/usr/ucb/l
	ln ${DESTDIR}/usr/ucb/ls ${DESTDIR}/usr/ucb/l
	cp ${DESTDIR}/usr/ucb/ls /mesa/bin/ucbls

clean:
	rm -f *.o ls.x ucbls.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.x:	$*.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@
