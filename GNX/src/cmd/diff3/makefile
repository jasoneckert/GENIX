#
# makefile: version 1.8 of 8/15/83
# Unix System Command Source File
#
# @(#)makefile	1.8 (NSC) 8/15/83
#

.SUFFIXES: .c .o .x

SCCS	= /v/mesaux/src/cmd/diff3
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc
LD	= nmeld

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= diff3.c diff3.sh

OBJS	= diff3.x diff3.sh

diff3.x: diff3.c

all: ${OBJS}

install: ${OBJS}
	minstall -s -x diff3.x ${DESTDIR}/usr/lib
	minstall -x diff3.sh ${DESTDIR}/usr/bin

clean:
	rm -f diff3.x *.o

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:	*.c
	${CC} ${CFLAGS} -c $*.c

.o.x:	rm -f $*.x
	${LD} -o $*.x $*.o -lc
	chmod 770 $*.x

.c.x:	
	rm -f $*.x
	${CC} ${CFLAGS} -o $*.x $*.c
	chmod 770 $*.x
