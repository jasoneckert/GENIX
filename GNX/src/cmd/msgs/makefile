#
# makefile: version 1.3 of 8/17/83
# GENIX source command file
#
# @(#)makefile	1.3 (NSC) 8/17/83
#

.SUFFIXES: .o  .c  .x

SCCS	= /v/mesaux/src/cmd/msgs
DESTDIR = /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc
CEFLAGS	=
CFLAGS  = -O -DSCCS ${CEFLAGS}

SRCS	= msgs.c msgs.h

msgs.x: msgs.h

install:
	minstall -s -x msgs.x ${DESTDIR}/usr/ucb

clean:
	-rm -f msgs.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

# building rules

.c.x: $*.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c -ltermlib
	chmod 770 $@
