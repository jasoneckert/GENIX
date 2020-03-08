#
# makefile: version 1.8 of 8/25/83
# Unix System Command Source File
#
# @(#)makefile	1.8 (NSC) 8/25/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/sed
DESTDIR	= /mesa
SCCSD	= sccs -d${SCCS}

CC	= nmcc

CEFLAGS =
CFLAGS	= -O -w -DSCCS ${CEFLAGS}

SRCS	= sed.h sed0.c sed1.c
OBJS	= sed0.o sed1.o

sed.x:	${OBJS}
	rm -f $@
	${CC} -o $@ ${OBJS}
	chmod 770 $@

install: sed.x
	minstall -s -x sed.x ${DESTDIR}/bin

clean:
	rm -f *.o sed.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

sed0.o: sed0.c sed.h
sed1.o: sed1.c sed.h

.c.o:	$*.c
	${CC} ${CFLAGS} -c $*.c
